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
	class APIObjectData : public Shared
	{
	public:
		int tag;

	public:
		static std::string uniqueName();

	public:
		APIObjectData() :
			tag(0), _name(APIObjectData::uniqueName()) { }

		APIObjectData(const std::string& aName) :
			tag(0), _name(aName.empty() ? APIObjectData::uniqueName() : aName) { }

		virtual ~APIObjectData()
			{ }

		ET_DECLARE_PROPERTY_GET_REF_SET_REF(std::string, name, setName)
	};

}