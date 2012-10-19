/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <iostream>
#include <assert.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <et/geometry/geometry.h>
#include <et/core/tools.h>
#include <et/core/containers.h>
#include <et/sound/sound.h>

#if (ET_PLATFORM_WIN)
#	pragma comment(lib, "OpenAL32")
#endif

namespace et
{
    namespace audio
    {
        class TrackPrivate
        {
		public:     
			TrackPrivate() : buffer(0) 
				{ }

			ALuint buffer;
        };
        
        class PlayerPrivate
        {
		public:     
			PlayerPrivate() : source(0)
				{ }

			ALuint source;
		};

		class ManagerPrivate
		{
		public:
			ManagerPrivate() : context(0)
				{ }

			ALCcontext* context;
		};
    }
}

using namespace et;
using namespace et::audio;

/*
 * Manager implementation
 */

static ALCdevice* sharedDevice = 0;

void checkOpenALErrorEx(const char* caller, const char* sourceFile, const char* lineNumber, const char* tag)
{
	ALenum error = alcGetError(sharedDevice);
	if (error != ALC_NO_ERROR)
	{
		const char* message = alcGetString(sharedDevice, error);
        printf("OpenAL ALC error: %s\n%s[%s]: %s\n", message, sourceFile, lineNumber, tag);
        fflush(stdout);
	}
    
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		const char* message = alGetString(error);
        printf("OpenAL error: %s\n%s[%s]: %s\n", message, sourceFile, lineNumber, tag);
        fflush(stdout);
	}
}

#if (ET_DEBUG)
#   define checkOpenALError(tag) checkOpenALErrorEx(ET_CALL_FUNCTION, __FILE__, ET_TOCONSTCHAR(__LINE__), tag)
#else
#   define checkOpenALError(tag)
#endif

Manager::Manager() : _private(new ManagerPrivate)
{
	const char* defaultDeviceSpecifier = alcGetString(0, ALC_DEFAULT_DEVICE_SPECIFIER);
	std::cout << "OpenAL device: " << defaultDeviceSpecifier;

	sharedDevice = alcOpenDevice(defaultDeviceSpecifier);
	assert(sharedDevice);
    
	vec2i version;
	alcGetIntegerv(sharedDevice, ALC_MAJOR_VERSION, sizeof(int), &version.x);
	alcGetIntegerv(sharedDevice, ALC_MINOR_VERSION, sizeof(int), &version.y);
    
	std::cout << ", version: " << version << std::endl;

	_private->context = alcCreateContext(sharedDevice, 0);
	assert(sharedDevice);
    
	ALboolean success = alcMakeContextCurrent(_private->context);
	assert(success); (void)success;

	vec3 nullVector;
	alListenerfv(AL_POSITION, nullVector.data());
	checkOpenALError("alListenerfv(AL_POSITION, ...");

    alListenerfv(AL_VELOCITY, nullVector.data());
	checkOpenALError("alListenerfv(AL_VELOCITY, ...");

	float orientation[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
	alListenerfv(AL_ORIENTATION, orientation);
	checkOpenALError("alListenerfv(AL_ORIENTATION, ...");
}

Manager::~Manager()
{
	alcMakeContextCurrent(0);
	alcDestroyContext(_private->context);
	alcCloseDevice(sharedDevice);
	delete _private;
    
    sharedDevice = 0;
}

Track::Pointer Manager::loadTrack(const std::string& fileName)
{
	return Track::Pointer(new Track(fileName));
}

Track::Pointer Manager::genTrack(Description::Pointer desc)
{
	return Track::Pointer(new Track(desc));
}

Player::Pointer Manager::genPlayer(Track::Pointer track)
{
	assert(track.valid());
	return Player::Pointer(new Player(track));
}

Player::Pointer Manager::genPlayer()
{
	return Player::Pointer(new Player);
}

/*
 * Track implementation
 */ 

Track::Track(const std::string& fileName) : _private(new TrackPrivate)
{
	init(loadFile(fileName));
}

Track::Track(Description::Pointer desc) : _private(new TrackPrivate)
{
	init(desc);
}

Track::~Track()
{
	alDeleteBuffers(1, &_private->buffer);
	delete _private;
}

void Track::init(Description::Pointer data)
{
	alGenBuffers(1, &_private->buffer);
    checkOpenALError("alGenBuffers");

	if (data.invalid()) return;

	alBufferData(_private->buffer, data->format, data->data.data(), data->data.dataSize(), data->sampleRate);
    checkOpenALError("alBufferData");
}

/*
 * Player imlementation
 */

Player::Player() : 
	_private(new PlayerPrivate)
{
	init();
}

Player::Player(Track::Pointer track) : 
	_private(new PlayerPrivate)
{
	init();
	linkTrack(track);
}

Player::~Player()
{
	stop();

	alDeleteSources(1, &_private->source);
    checkOpenALError("alDeleteSources");

	delete _private;
}

void Player::init()
{
	vec3 nullVector;

	alGenSources(1, &_private->source);
    checkOpenALError("alGenSources");

	alSourcef(_private->source, AL_PITCH, 1.0f);
    checkOpenALError("alSourcef(..., AL_PITCH, ...)");

	alSourcef(_private->source, AL_GAIN, 1.0f);
    checkOpenALError("alSourcef(..., AL_GAIN, ...)");

	alSourcefv(_private->source, AL_POSITION, nullVector.data());
    checkOpenALError("alSourcefv(..., AL_POSITION, ...)");

	alSourcefv(_private->source, AL_VELOCITY, nullVector.data());
    checkOpenALError("alSourcefv(..., AL_VELOCITY, ...)");
}

void Player::play(bool looped)
{
	alSourcei(_private->source, AL_LOOPING, looped ? AL_TRUE : AL_FALSE);
    checkOpenALError("alSourcei(..., AL_LOOPING, ...)");

	alSourcePlay(_private->source);
    checkOpenALError("alSourcePlay");
}

void Player::play(Track::Pointer track, bool looped)
{
	linkTrack(track);
	play(looped);
}

void Player::pause()
{
	alSourcePause(_private->source);
    checkOpenALError("alSourcePause");
}

void Player::stop()
{
	alSourceStop(_private->source);
    checkOpenALError("alSourceStop");
}

void Player::rewind()
{
    alSourceRewind(_private->source);
    checkOpenALError("alSourceRewind");
}

void Player::linkTrack(Track::Pointer track)
{
	if (_currentTrack == track) return;

    stop();
    
	_currentTrack = track;
	alSourcei(_private->source, AL_BUFFER, track->_private->buffer);
    checkOpenALError("alSourcei(.., AL_BUFFER, ...)");
}

void Player::setVolume(float value)
{
	alSourcef(_private->source, AL_GAIN, value);
    checkOpenALError("alSourcei(.., AL_GAIN, ...)");
}