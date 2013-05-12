/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

namespace et
{
	enum DictionaryEntryType
	{
		DictionaryEntryType_String,
		DictionaryEntryType_Numeric,
		DictionaryEntryType_Bool,
		DictionaryEntryType_Array,
		DictionaryEntryType_Dictionary,
	};

	/*
	 * Base classes
	 */
	class DictionaryEntryBase : public Shared
	{
	public:
		typedef IntrusivePtr<DictionaryEntryBase> Pointer;
		
	public:
		virtual ~DictionaryEntryBase() { };
		virtual DictionaryEntryType type() const = 0;
	};

	template <typename T>
	class DictionaryTypedEntry : public DictionaryEntryBase
	{
	protected:
		T _value;
	};

	template <typename T>
	class DictionaryEntry : public DictionaryTypedEntry<T> { };

	typedef std::vector<DictionaryEntryBase::Pointer> Array;
	typedef std::map<std::string, DictionaryEntryBase::Pointer> Dictionary;

	/*
	 * Specialized classes
	 */
	template <>
	class DictionaryEntry<std::string> : public DictionaryTypedEntry<std::string>
	{
	public:
		typedef IntrusivePtr<DictionaryEntry<std::string>> Pointer;

	public:
		DictionaryEntry(const std::string& s)
			{ _value = s; }

		DictionaryEntryType type() const
			{ return DictionaryEntryType_String; }

		const std::string& value() const
			{ return _value; }
	};

	template <>
	class DictionaryEntry<double> : public DictionaryTypedEntry<double>
	{
	public:
		typedef IntrusivePtr<DictionaryEntry<double>> Pointer;
		
	public:
		DictionaryEntry(double v)
			{ _value = v; }

		DictionaryEntryType type() const
			{ return DictionaryEntryType_Numeric; }

		double value() const
			{ return _value; }
	};

	template <>
	class DictionaryEntry<bool> : public DictionaryTypedEntry<bool>
	{
	public:
		typedef IntrusivePtr<DictionaryEntry<bool>> Pointer;
	public:
		DictionaryEntry(bool b)
			{ _value = b; }

		DictionaryEntryType type() const
			{ return DictionaryEntryType_Bool; }

		bool value() const
			{ return _value; }
	};

	template <>
	class DictionaryEntry<Array> : public DictionaryTypedEntry<Array>
	{
	public:
		DictionaryEntryType type() const
			{ return DictionaryEntryType_Array; }

		void push_back(const DictionaryEntryBase::Pointer& value)
			{ _value.push_back(value); }

		DictionaryEntryBase::Pointer objectAtIndex(size_t index)
		{ return index < _value.size() ? _value.at(index) : DictionaryEntryBase::Pointer(); }

		size_t size() const
			{ return _value.size(); }
	};

	template <>
	class DictionaryEntry<Dictionary> : public DictionaryTypedEntry<Dictionary>
	{
	public:
		typedef IntrusivePtr< DictionaryEntry<Dictionary> > Pointer;

	public:
		DictionaryEntryType type() const
			{ return DictionaryEntryType_Dictionary; }

		void setValueForKey(const DictionaryEntryBase::Pointer& value, const std::string& key)
			{ _value[key] = value; }

		DictionaryEntryBase::Pointer valueForKey(const std::string& key)
		{
			auto i = _value.find(key);
			return i == _value.end() ? DictionaryEntryBase::Pointer() : i->second;
		}

		DictionaryEntry<Dictionary>::Pointer dictionaryForKey(const std::string& key)
		{
			auto i = _value.find(key);

			if ((i == _value.end()) || (i->second->type() != DictionaryEntryType_Dictionary))
				return DictionaryEntry<Dictionary>::Pointer();
			
			return i->second;
		}

		DictionaryEntry<Array>::Pointer arrayForKey(const std::string& key)
		{
			auto i = _value.find(key);

			if ((i == _value.end()) || (i->second->type() != DictionaryEntryType_Array))
				return DictionaryEntry<Array>::Pointer();

			return i->second;
		}

		DictionaryEntry<double>::Pointer numberForKey(const std::string& key)
		{
			auto i = _value.find(key);
			
			if ((i == _value.end()) || (i->second->type() != DictionaryEntryType_Numeric))
				return DictionaryEntry<double>::Pointer();

			return i->second;
		}

		DictionaryEntry<bool>::Pointer boolForKey(const std::string& key)
		{
			auto i = _value.find(key);

			if ((i == _value.end()) || (i->second->type() != DictionaryEntryType_Bool))
				return DictionaryEntry<bool>::Pointer();
			
			return i->second;
		}

		DictionaryEntry<std::string>::Pointer stringForKey(const std::string& key)
		{
			auto i = _value.find(key);
			
			if ((i == _value.end()) || (i->second->type() != DictionaryEntryType_String))
				return DictionaryEntry<std::string>::Pointer();

			return i->second;
		}

		const Dictionary& dictionary() const
			{ return _value; }
	};
}
