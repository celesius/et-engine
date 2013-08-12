/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/core/intrusiveptr.h>
#include <et/core/properties.h>

namespace et
{
	class Object : public Shared
	{
	public:
		ET_DECLARE_POINTER(Object)
		
	public:
		Object()
			{ }

		Object(const std::string& aName) :
			_name(aName) { }

		virtual ~Object()
			{ }

	public:
		ET_DECLARE_PROPERTY_GET_REF_SET_REF(std::string, name, setName)
	};

	class RenderContext;
	class ObjectsCache;

	class LoadableObject : public Object
	{
	public:
		ET_DECLARE_POINTER(LoadableObject)
		
	public:
		LoadableObject()
			{ }

		LoadableObject(const std::string& aName) :
			Object(aName) { }
		
		LoadableObject(const std::string& aName, const std::string& aOrigin) :
			Object(aName), _origin(aOrigin) { }

		bool canBeReloaded() const
			{ return _origin.size() > 0; }

		ET_DECLARE_PROPERTY_GET_REF_SET_REF(std::string, origin, setOrigin)
	};
	
	class ObjectLoader : public Shared
	{
	public:
		ET_DECLARE_POINTER(ObjectLoader)
		
	public:
		virtual void reloadObject(LoadableObject::Pointer, ObjectsCache&) = 0;
	};
}
