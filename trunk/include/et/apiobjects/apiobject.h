#pragma once

#include <string>
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
		inline void setName(const std::string& name)
			{ _name = name; }

	private:
		std::string _name;
	};

}