#pragma once

namespace et
{
	class Flags
	{
	public:
		Flags(size_t flags = 0) : _flags(flags) { }

		inline size_t flags() const
			{ return _flags; }

		inline void setFlag(size_t flag)
			{ _flags = _flags | flag; }

		inline void removeFlag(size_t flag)
			{ _flags = _flags & (!flag); }

		inline bool hasFlag(size_t flag) const
			{ return (_flags & flag) == flag; }

		inline void setFlags(size_t value)
			{ _flags = value; }

	private:
		size_t _flags;
	};
}