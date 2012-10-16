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
			ManagerPrivate() : device(0), context(0)
				{ }

			ALCdevice* device;
			ALCcontext* context;
		};
    }
}

using namespace et;
using namespace et::audio;

/*
 * Manager implementation
 */

Manager::Manager() : _private(new ManagerPrivate)
{
	const char* defaultDeviceSpecifier = alcGetString(0, ALC_DEFAULT_DEVICE_SPECIFIER);
	std::cout << "OpenAL device: " << defaultDeviceSpecifier;

	_private->device = alcOpenDevice(defaultDeviceSpecifier);
	assert(_private->device);

	vec2i version;
	alcGetIntegerv(_private->device, ALC_MAJOR_VERSION, sizeof(int), &version.x);
	alcGetIntegerv(_private->device, ALC_MINOR_VERSION, sizeof(int), &version.y);

	std::cout << ", version: " << version << std::endl;

	_private->context = alcCreateContext(_private->device, 0);
	assert(_private->device);

	ALboolean success = alcMakeContextCurrent(_private->context);
	assert(success);

	vec3 nullVector;
	alListenerfv(AL_POSITION, nullVector.data());
	checkErrors();

    alListenerfv(AL_VELOCITY, nullVector.data());
	checkErrors();

	float orientation[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
	alListenerfv(AL_ORIENTATION, orientation);
	checkErrors();
}

Manager::~Manager()
{
	alcMakeContextCurrent(0);
	alcDestroyContext(_private->context);
	alcCloseDevice(_private->device);
	delete _private;
}

bool Manager::checkErrors()
{
	ALenum error = alcGetError(_private->device);

	if (error != ALC_NO_ERROR)
	{
		const char* message = alcGetString(_private->device, error);
		std::cout << "OpenAL ALC error: " << message << std::endl;
		assert(false);
		return true;
	}

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		const char* message = alGetString(error);
		std::cout << "OpenAL error: " << message << std::endl;
		assert(false);
		return true;
	}

	return false;
}

Track::Pointer Manager::loadTrack(const std::string& fileName)
{
	return Track::Pointer(new Track(fileName));
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
	alGenBuffers(1, &_private->buffer);
	manager().checkErrors();

	Description::Pointer desc = loadFile(fileName);
	if (desc.valid())
	{
		alBufferData(_private->buffer, desc->format, desc->data.data(), desc->data.dataSize(), desc->sampleRate);
		manager().checkErrors();
	}
}

Track::~Track()
{
	alDeleteBuffers(1, &_private->buffer);
	delete _private;
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
	manager().checkErrors();

	delete _private;
}

void Player::init()
{
	vec3 nullVector;

	alGenSources(1, &_private->source);
	manager().checkErrors();

	alSourcef(_private->source, AL_PITCH, 1.0f);
	manager().checkErrors();

	alSourcef(_private->source, AL_GAIN, 1.0f);
	manager().checkErrors();

	alSourcefv(_private->source, AL_POSITION, nullVector.data());
	manager().checkErrors();

	alSourcefv(_private->source, AL_VELOCITY, nullVector.data());
	manager().checkErrors();
}

void Player::play(bool looped)
{
	alSourcei(_private->source, AL_LOOPING, looped ? AL_TRUE : AL_FALSE);
	manager().checkErrors();

	alSourcePlay(_private->source);
	manager().checkErrors();
}

void Player::play(Track::Pointer track, bool looped)
{
	linkTrack(track);
	play(looped);
}

void Player::pause()
{
	alSourcePause(_private->source);
	manager().checkErrors();
}

void Player::stop()
{
	alSourceStop(_private->source);
	manager().checkErrors();
}

void Player::linkTrack(Track::Pointer track)
{
	if (_currentTrack == track) return;

	_currentTrack = track;

//	alSourceQueueBuffers(_private->source, 1, &track->_private->buffer);
	alSourcei(_private->source, AL_BUFFER, track->_private->buffer);
	manager().checkErrors();
}
