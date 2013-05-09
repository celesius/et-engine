/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/core/intrusiveptr.h>
#include <et/core/properties.h>
#include <et/opengl/opengl.h>

namespace et
{
	std::string uniqueObjectName();

	class RenderContext;
	class ObjectsCache;
	
	class APIObject : public Shared
	{
	public:
		typedef IntrusivePtr<APIObject> Pointer;
		
	public:
		int tag;

	public:
		APIObject() :
			tag(0), _objectName(uniqueObjectName()) { }

		APIObject(const std::string& aName) :
			tag(0), _objectName(aName.empty() ? uniqueObjectName() : aName) { }
		
		APIObject(const std::string& aName, const std::string& aOrigin) :
			tag(0), _objectName(aName.empty() ? uniqueObjectName() : aName), _origin(aOrigin) { }

		virtual void reload(const std::string&, RenderContext*, ObjectsCache&)
			{ }

		virtual ~APIObject()
			{ }

		ET_DECLARE_PROPERTY_GET_REF_SET_REF(std::string, objectName, setObjectName)
		ET_DECLARE_PROPERTY_GET_REF_SET_REF(std::string, origin, setOrigin)
	};

}