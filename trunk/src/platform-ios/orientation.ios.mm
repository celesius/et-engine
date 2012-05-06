#include <CoreMotion/CoreMotion.h>
#include <et/sensor/orientation.h>

namespace et
{
	class OrientationManagerPrivate : public EventReceiver 
	{
	public:
		OrientationManagerPrivate() : manager(0), accelEnabled(false), gyroEnabled(false), updating(false)
		{
            _oq = [[NSOperationQueue alloc] init];
            _motionManager = [[CMMotionManager alloc] init];
            _motionManager.deviceMotionUpdateInterval = 1.0f / 30.0f;
		}
        
        ~OrientationManagerPrivate()
        {
            [_motionManager release];
            [_oq release];
        }
        
        void update()
        {
            bool shouldSwitch = updating != (gyroEnabled || accelEnabled);
            if (!shouldSwitch) return;
            
            updating = gyroEnabled || accelEnabled;
            if (updating)
            {
                [_motionManager startDeviceMotionUpdatesToQueue:_oq withHandler:^(CMDeviceMotion *motion, NSError *error)
                 {
                     if (gyroEnabled)
                     {
                         et::GyroscopeData d;
                         d.rate.x = motion.rotationRate.x;
                         d.rate.y = motion.rotationRate.y;
                         d.rate.z = motion.rotationRate.z;
                         d.timestamp = motion.timestamp;
                         manager->gyroscopeDataUpdated.invokeInMainRunLoop(d);
                     }
                     
                     if (accelEnabled)
                     {
                         et::AccelerometerData d;
                         d.value.x = motion.gravity.x;
                         d.value.y = motion.gravity.y;
                         d.value.z = motion.gravity.z;
                         d.timestamp = motion.timestamp;
                         manager->accelerometerDataUpdated.invokeInMainRunLoop(d);
                     }
                 }];
            }
            else 
            {
                [_motionManager stopDeviceMotionUpdates];
            }
        }
        
		void setGyroEnabled(bool e)
		{
			if (gyroEnabled == e) return;	
			gyroEnabled = e;
            update();
		}
        
		void setAccelEnabled(bool e)
		{
			if (accelEnabled == e) return;	
			accelEnabled = e;
            update();
		}
        
	public:
        NSOperationQueue* _oq;
        CMMotionManager* _motionManager;
		OrientationManager* manager;
		
		bool accelEnabled;
		bool gyroEnabled;
        bool updating;
                
	};
}

using namespace et;

OrientationManager::OrientationManager() : _private(new OrientationManagerPrivate)
{ _private->manager = this; }

OrientationManager::~OrientationManager()
{ delete _private; }

void OrientationManager::setAccelerometerEnabled(bool e)
{ _private->setAccelEnabled(e); }

bool OrientationManager::accelerometerEnabled(bool e) const
{ return _private->accelEnabled; }

void OrientationManager::setGyroscopeEnabled(bool e)
{ _private->setGyroEnabled(e); }

bool OrientationManager::gyroscopeEnabled(bool e) const
{ return _private->gyroEnabled; }
