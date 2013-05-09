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

namespace et
{
	class ObjectsCacheBase : public TimedObject
	{
	public:
		ObjectsCacheBase()
			{ }
		
	private:
		ET_DENY_COPY(ObjectsCacheBase)
	};

	class ObjectsCache : public ObjectsCacheBase
	{
	public:
		ObjectsCache() : _updateTime(0.0f)
			{ }

		~ObjectsCache()
			{ clear(); }

		void manage(const APIObject::Pointer& o)
		{
			if (o.valid())
			{
				CriticalSectionScope lock(_lock);
				_objects[o->origin()] = o;
				_properties[o->origin()] = getFileProperty(o->origin());
			}
		}
		
		APIObject::Pointer find(const std::string& key)
		{
			CriticalSectionScope lock(_lock);
			auto i = _objects.find(key);
			return (i == _objects.end()) ? APIObject::Pointer() : i->second;
		}
		
		void discard(const APIObject::Pointer& o)
		{
			if (o.valid())
			{
				CriticalSectionScope lock(_lock);
				_objects.erase(o->origin());
				_properties.erase(o->origin());
			}
		}

		void clear()
		{
			CriticalSectionScope lock(_lock);
			_objects.clear();
			_properties.clear();
		}
		
		void flush()
		{
			CriticalSectionScope lock(_lock);
			auto i = _objects.begin();
			while (i != _objects.end())
			{
				if (i->second->atomicCounterValue() == 1)
				{
					auto toErase = i++;
					_properties.erase(i->first);
					_objects.erase(toErase);
				}
				else
				{
					++i;
				}
			}
		}

		void startMonitoring()
		{
			startUpdates();
		}

		void stopMonitoring()
		{
			cancelUpdates();
		}

		void update(float t)
		{
			if (_updateTime == 0.0f)
				_updateTime = t;
			float dt = t - _updateTime;

			if (dt > 1.0f)
			{
				performUpdate();
				_updateTime = t;
			}
		}

	private:
		ET_DENY_COPY(ObjectsCache)

		unsigned long getFileProperty(const std::string& p)
			{ return getFileDate(p); }

		void performUpdate()
		{
			for (auto& p : _properties)
			{
				unsigned long newProp = getFileProperty(p.first);
				if (newProp != p.second)
				{
					_objects[p.first]->reload(p.first);
					p.second = newProp;
				}
			}
		}
		
	private:
		typedef std::map<const std::string, APIObject::Pointer> ObjectMap;
		typedef std::map<const std::string, unsigned long> ObjectPropertyMap;

		CriticalSection _lock;
		ObjectMap _objects;
		ObjectPropertyMap _properties;
		float _updateTime;
	};
}
