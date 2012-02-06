#pragma once

#include <et/core/classes.h>

namespace et
{

enum DeviceType
{
 DeviceType_PC,
 DeviceType_Mac,
 DeviceType_iPhone,
 DeviceType_iPhone4,
 DeviceType_iPad,
 DeviceType_Max
};

class Device : public Singleton<Device>
{
 ET_SINGLETON_COPY_DENY(Device);
 public:
  Device();

  inline DeviceType type() const { return _type; }

 private:
  DeviceType _type;
};

inline Device& currentDevice() { return Device::instance(); }

 
}