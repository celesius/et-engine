/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <vector>
#include <map>
#include <et/core/intrusiveptr.h>
#include <et/core/containers.h>

namespace et
{
	namespace plist
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
			virtual ~DictionaryEntryBase() { };
			virtual DictionaryEntryType type() const = 0;
		};
		typedef IntrusivePtr<DictionaryEntryBase> DictionaryEntryBaseRef;

		template <typename T>
		class DictionaryTypedEntry : public DictionaryEntryBase 
		{ 
		protected:
			T _value;
		};

		template <typename T>
		class DictionaryEntry : public DictionaryTypedEntry<T> { };

		typedef std::vector<DictionaryEntryBaseRef> Array;
		typedef std::map<std::string, DictionaryEntryBaseRef> Dictionary;

		/*
		 * Specialized classes
		 */
		template <>
		class DictionaryEntry<std::string> : public DictionaryTypedEntry<std::string>
		{
		public:
			DictionaryEntry(const std::string& s)
				{ _value = s; }

			DictionaryEntryType type() const 
				{ return DictionaryEntryType_String; }

			const std::string& value() const
				{ return _value; }
		};
		typedef DictionaryEntry<std::string> StringValue;
		typedef IntrusivePtr<StringValue> StringValueRef;

		template <>
		class DictionaryEntry<double> : public DictionaryTypedEntry<double>
		{
		public:
			DictionaryEntry(double v)
				{ _value = v; }

			DictionaryEntryType type() const 
				{ return DictionaryEntryType_Numeric; }

			double value() const 
				{ return _value; }
		};
		typedef DictionaryEntry<double> NumericValue;
		typedef IntrusivePtr<NumericValue> NumericValueRef;

		template <>
		class DictionaryEntry<bool> : public DictionaryTypedEntry<bool>
		{
		public:
			DictionaryEntry(bool b)
				{ _value = b; }

			DictionaryEntryType type() const 
				{ return DictionaryEntryType_Bool; }

			bool value() const
				{ return _value; }
		};
		typedef DictionaryEntry<bool> BoolValue;
		typedef IntrusivePtr<BoolValue> BoolValueRef;

		template <>
		class DictionaryEntry<Array> : public DictionaryTypedEntry<Array>
		{
		public:
			DictionaryEntryType type() const 
				{ return DictionaryEntryType_Array; }

			void push_back(const DictionaryEntryBaseRef& value)
				{ _value.push_back(value); }

			DictionaryEntryBaseRef objectAtIndex(size_t index)
				{ return index < _value.size() ? _value.at(index) : DictionaryEntryBaseRef(); }

			size_t size() const
				{ return _value.size(); }
		};
		typedef DictionaryEntry<Array> ArrayValue;
		typedef IntrusivePtr<ArrayValue> ArrayValueRef;

		template <>
		class DictionaryEntry<Dictionary> : public DictionaryTypedEntry<Dictionary>
		{
		public:
			typedef IntrusivePtr< DictionaryEntry<Dictionary> > Pointer;

		public:
			DictionaryEntryType type() const 
				{ return DictionaryEntryType_Dictionary; }

			void setValueForKey(const DictionaryEntryBaseRef& value, const std::string& key)
				{ _value[key] = value; }

			DictionaryEntryBaseRef valueForKey(const std::string& key)
			{
				Dictionary::iterator i = _value.find(key);
				return i == _value.end() ? DictionaryEntryBaseRef() : i->second;
			}

			Pointer dictionaryForKey(const std::string& key)
			{
				Dictionary::iterator i = _value.find(key);
				if ((i == _value.end()) || (i->second->type() != DictionaryEntryType_Dictionary))	
					return Pointer();
				return i->second;
			}

			ArrayValueRef arrayForKey(const std::string& key)
			{
				Dictionary::iterator i = _value.find(key);
				if ((i == _value.end()) || (i->second->type() != DictionaryEntryType_Array))	
					return ArrayValueRef();
				return i->second;
			}

			NumericValueRef numberForKey(const std::string& key)
			{
				Dictionary::iterator i = _value.find(key);
				if ((i == _value.end()) || (i->second->type() != DictionaryEntryType_Numeric))	
					return NumericValueRef();
				return i->second;
			}

			BoolValueRef boolForKey(const std::string& key)
			{
				Dictionary::iterator i = _value.find(key);
				if ((i == _value.end()) || (i->second->type() != DictionaryEntryType_Bool))	
					return BoolValueRef();
				return i->second;
			}

			StringValueRef stringForKey(const std::string& key)
			{
				Dictionary::iterator i = _value.find(key);
				if ((i == _value.end()) || (i->second->type() != DictionaryEntryType_String))	
					return StringValueRef();
				return i->second;
			}

			const Dictionary& dict() const
				{ return _value; }
		};
		typedef DictionaryEntry<Dictionary> DictionaryValue;
		typedef DictionaryEntry<Dictionary>::Pointer DictionaryValueRef;

		class Reader
		{
		public:
			DictionaryValueRef load(const std::string& filename);

		private:
			void parseBuffer(BinaryDataStorage& buffer, DictionaryValue* root);
			bool parseString(char*& ptr, DictionaryEntryBase** value);
			bool parseInteger(char*& ptr, DictionaryEntryBase** value);
			bool parseDictionary(DictionaryValue* owner, char*& ptr);
			bool parseArray(char*& ptr, DictionaryEntryBase** value);
			bool parseValue(char*& ptr, DictionaryEntryBase** value);
		};
	}
}