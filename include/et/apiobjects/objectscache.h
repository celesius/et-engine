/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <map>
#include <et/core/tools.h>
#include <et/threading/criticalsection.h>
#include <et/timers/timedobject.h>
#include <et/apiobjects/apiobject.h>

namespace et
{
	class ObjectsCache : public TimedObject
	{
	public:
		ObjectsCache();
		~ObjectsCache();

		void manage(const APIObject::Pointer& o);
		void discard(const APIObject::Pointer& o);
		void clear();
		
		void flush();

		APIObject::Pointer find(const std::string& key);

		void startMonitoring();
		void stopMonitoring();

	private:
		ET_DENY_COPY(ObjectsCache)

		unsigned long getFileProperty(const std::string& p);
		void performUpdate();
		void update(float t);

	private:
		typedef std::pair<APIObject::Pointer, unsigned long> ObjectProperty;
		typedef std::map<const std::string, ObjectProperty> ObjectMap;

		CriticalSection _lock;
		ObjectMap _objects;
		float _updateTime;
	};

	// backward compatibility
	typedef ObjectsCache TextureCache;
}
