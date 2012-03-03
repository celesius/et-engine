#pragma once

namespace et
{
	class Flags
	{
	public:
		Flags(size_t flags = 0) : _flags(flags) { }

		size_t flags() const
			{ return _flags; }

		void setFlag(size_t flag)
			{ _flags = _flags | flag; }

		void removeFlag(size_t flag)
			{ _flags = _flags & (~flag); }

		bool hasFlag(size_t flag) const
			{ return (_flags & flag) == flag; }

		void setFlags(size_t value)
			{ _flags = value; }

		void toggleFlag(size_t value) 
		{ 
			if (hasFlag(value)) 
				removeFlag(value);
			else
				setFlag(value);
		}

	private:
		size_t _flags;
	};
}