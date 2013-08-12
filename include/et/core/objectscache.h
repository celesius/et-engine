/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <map>
#include <et/core/object.h>
#include <et/core/tools.h>
#include <et/threading/criticalsection.h>
#include <et/timers/timedobject.h>

namespace et
{
	class ObjectsCache : public TimedObject
	{
	public:
		ObjectsCache();
		~ObjectsCache();

		void manage(const LoadableObject::Pointer&, const ObjectLoader::Pointer& loader);
		void discard(const LoadableObject::Pointer& o);
		
		void clear();
		void flush();

		LoadableObject::Pointer find(const std::string& key);

		void startMonitoring();
		void stopMonitoring();
		
		void report();

	private:
		ET_DENY_COPY(ObjectsCache)

		unsigned long getFileProperty(const std::string& p);
		void performUpdate();
		void update(float t);

	private:
		struct ObjectProperty
		{
			LoadableObject::Pointer object;
			ObjectLoader::Pointer loader;
			uint64_t identifier;
			
			ObjectProperty() :
				identifier(0) { }
			
			ObjectProperty(LoadableObject::Pointer o, ObjectLoader::Pointer l, uint64_t i) :
				object(o), loader(l), identifier(i) { }
		};
		typedef std::map<const std::string, ObjectProperty> ObjectMap;

		CriticalSection _lock;
		ObjectMap _objects;
		float _updateTime;
	};
}
