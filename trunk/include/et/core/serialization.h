#include <string>
#include <ostream>
#include <iostream>

#include <et/core/containers.h>
#include <et/geometry/geometry.h>

namespace et
{
	inline void serializeInt(std::ostream& stream, int value)
	{
		stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
	}

	inline int deserializeInt(std::istream& stream)
	{
		int value = 0;
		stream.read(reinterpret_cast<char*>(&value), sizeof(value)); 
		return value;
	}

	inline void serializeString(std::ostream& stream, const std::string& s)
	{
		serializeInt(stream, s.size());
		if (s.size())
			stream.write(s.c_str(), s.size());
	}

	inline std::string deserializeString(std::istream& stream)
	{
		int size = deserializeInt(stream);
		DataStorage<char> value(size + 1, 0);
		stream.read(value.binary(), size);
		return std::string(value.binary());
	}

	inline void serializeFloat(std::ostream& stream, float value)
	{
		stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
	}

	inline float deserializeFloat(std::istream& stream)
	{
		float value = 0;
		stream.read(reinterpret_cast<char*>(&value), sizeof(value)); 
		return value;
	}

	template <typename T>
	inline void serializeVector(std::ostream& stream, const T& value)
		{ stream.write(value.binary(), sizeof(value)); }

	template <typename T>
	inline T deserializeVector(std::istream& stream)
	{
		T value;
		stream.read(value.binary(), sizeof(value)); 
		return value;
	}

	inline void serializeQuaternion(std::ostream& stream, const quaternion& value)
	{
		serializeFloat(stream, value.scalar);
		serializeVector(stream, value.vector);
	}

	inline quaternion deserializeQuaternion(std::istream& stream)
	{
		quaternion result;
		result.scalar = deserializeFloat(stream);
		result.vector = deserializeVector<vec3>(stream);
		return result;
	}

	inline void serializeMatrix(std::ostream& stream, const mat4& value)
	{
		stream.write(value.binary(), sizeof(value));
	}

	inline mat4 deserializeMatrix(std::istream& stream)
	{
		mat4 value;
		stream.read(value.binary(), sizeof(value));
		return value;
	}

}