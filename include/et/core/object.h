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
		typedef IntrusivePtr<Object> Pointer;
		
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
		typedef IntrusivePtr<LoadableObject> Pointer;
		
	public:
		LoadableObject()
			{ }

		LoadableObject(const std::string& aName) :
			Object(aName) { }
		
		LoadableObject(const std::string& aName, const std::string& aOrigin) :
			Object(aName), _origin(aOrigin) { }

		virtual void reload(const std::string&, RenderContext*, ObjectsCache&)
			{ }

		bool canBeReloaded() const
			{ return _origin.size() > 0; }

		ET_DECLARE_PROPERTY_GET_REF_SET_REF(std::string, origin, setOrigin)
	};
}
