/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/core/objectscache.h>
#include <et/app/applicationnotifier.h>

using namespace et;

ObjectsCache::ObjectsCache() :
	_updateTime(0.0f)
{
}

ObjectsCache::~ObjectsCache()
{
	clear();
}

void ObjectsCache::manage(const LoadableObject::Pointer& o, const ObjectLoader::Pointer& loader)
{
	if (o.valid())
	{
		CriticalSectionScope lock(_lock);
		auto last = _objects.insert(std::make_pair(o->origin(), ObjectProperty(o, loader)));
		last.first->second.identifiers[o->origin()] = getFileProperty(o->origin());
		for (auto& s : o->distributedOrigins())
			last.first->second.identifiers[s] = getFileProperty(s);
	}
}

LoadableObject::Pointer ObjectsCache::find(const std::string& key)
{
	CriticalSectionScope lock(_lock);
	auto i = _objects.find(key);
	return (i == _objects.end()) ? LoadableObject::Pointer() : i->second.object;
}

void ObjectsCache::discard(const LoadableObject::Pointer& o)
{
	if (o.valid())
	{
		CriticalSectionScope lock(_lock);
		_objects.erase(o->origin());
	}
}

void ObjectsCache::clear()
{
	CriticalSectionScope lock(_lock);
	_objects.clear();
}

void ObjectsCache::flush()
{
	CriticalSectionScope lock(_lock);
	auto i = _objects.begin();
	while (i != _objects.end())
	{
		if (i->second.object->atomicCounterValue() == 1)
		{
			auto toErase = i++;
			_objects.erase(toErase);
		}
		else
		{
			++i;
		}
	}
}

void ObjectsCache::startMonitoring()
{
	startUpdates();
}

void ObjectsCache::stopMonitoring()
{
	cancelUpdates();
}

void ObjectsCache::update(float t)
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

int64_t ObjectsCache::getFileProperty(const std::string& p)
{
	return getFileDate(p);
}

void ObjectsCache::performUpdate()
{
	ObjectsCache& cache = *this;
	
	for (auto& p : _objects)
	{
		if (p.second.loader.valid() && p.second.object->canBeReloaded())
		{
			bool shouldReload = false;
			
			for (auto i : p.second.identifiers)
			{
				int64_t prop = getFileProperty(i.first);
				if (prop != i.second)
				{
					i.second = prop;
					shouldReload = true;
				}
			}
			
			if (shouldReload)
			{
				log::info("[ObjectsCache] Object updated: %s", p.first.c_str());
				p.second.loader->reloadObject(p.second.object, cache);
			}
		}
	}
}

void ObjectsCache::report()
{
	log::info("[ObjectsCache] Contains %lu objects", _objects.size());
}
