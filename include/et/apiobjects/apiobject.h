/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/opengl/opengl.h>
#include <et/core/intrusiveptr.h>

namespace et
{
	class APIObjectData : public Shared
	{
	public:
		int tag;

	public:
		static std::string uniqueName();

	public:
		APIObjectData() : tag(0), _name(APIObjectData::uniqueName())
			{ }

		APIObjectData(const std::string& aName) : tag(0), _name(aName.size() == 0 ? APIObjectData::uniqueName() : aName)
			{ }

		virtual ~APIObjectData() 
			{ }

		const std::string& name() const
			{ return _name; };

	protected:
		void setName(const std::string& name)
			{ _name = name; }

	private:
		std::string _name;
	};

}