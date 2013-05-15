/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/core/objectscache.h>
#include <et/app/applicationnotifier.h>

using namespace et;

ObjectsCache::ObjectsCache() : _updateTime(0.0f)
{
}

ObjectsCache::~ObjectsCache()
{
	clear();
}

void ObjectsCache::manage(const LoadableObject::Pointer& o)
{
	if (o.valid())
	{
		CriticalSectionScope lock(_lock);
		_objects[o->origin()].first = o;
		_objects[o->origin()].second = getFileProperty(o->origin());
	}
}

LoadableObject::Pointer ObjectsCache::find(const std::string& key)
{
	CriticalSectionScope lock(_lock);
	auto i = _objects.find(key);
	return (i == _objects.end()) ? LoadableObject::Pointer() : i->second.first;
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
		if (i->second.first->atomicCounterValue() == 1)
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

unsigned long ObjectsCache::getFileProperty(const std::string& p)
{
	return getFileDate(p);
}

void ObjectsCache::performUpdate()
{
	ET_ITERATE(_objects, auto&, p, 
	{
		if (p.second.first->canBeReloaded())
		{
			unsigned long newProp = getFileProperty(p.first);
			if (newProp != p.second.second)
			{
				log::info("Updated (%lu -> %lu): %s", p.second.second, newProp, p.first.c_str());
				_objects[p.first].first->reload(p.first, ApplicationNotifier().accessRenderContext(), *this);
				p.second.second = newProp;
			}
		}
	})
}
