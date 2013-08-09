/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <map>
#include <et/core/intrusiveptr.h>

namespace et
{
	class ValueBase : public Shared
	{
	public:
		ET_DECLARE_POINTER(ValueBase)
						   
 	public:
		virtual ~ValueBase() { }
		
		virtual int valueClass() const = 0;
	};
	
	enum ValueClass
	{
		ValueClass_Float,
		ValueClass_Integer,
		ValueClass_String,
		ValueClass_Array,
		ValueClass_Dictionary,
	};
	
	template <typename T, ValueClass C>
	class Value : public ValueBase
	{
	public:
		typedef et::IntrusivePtr<Value<T, C>> Pointer;
		
	public:
		T content;
		
	public:
		Value() :
			content(T()) { }
		
		Value(const T& r) :
			content(r) { }

		int valueClass() const
			{ return C; }
	};
		
	template <typename T, ValueClass C>
	class ValuePointer : public Value<T, C>::Pointer
	{
	public:
		typedef T ValueType;
		
	public:
		ValuePointer() :
			Value<T, C>::Pointer(new Value<T, C>()) { }
		
		ValuePointer(const T& r) :
			Value<T, C>::Pointer(new Value<T, C>(r)) { }

		ValuePointer(const typename Value<T, C>::Pointer& p) :
			Value<T, C>::Pointer(p) { }

		ValuePointer(Value<T, C>* p) :
			Value<T, C>::Pointer(p) { }

		ValuePointer(ValueBase::Pointer p) :
			Value<T, C>::Pointer(p) { }
		
		const T& value() const
			{ return this->reference().content; }
	};
	
	typedef ValuePointer<double, ValueClass_Float> FloatValue;
	typedef ValuePointer<int, ValueClass_Integer> IntegerValue;
	
	class StringValue : public ValuePointer<std::string, ValueClass_String>
	{
	public:
		StringValue() :
			ValuePointer<std::string, ValueClass_String>() { }
		
		StringValue(const std::string& r) :
			ValuePointer<std::string, ValueClass_String>(r) { }

		StringValue(const Value<std::string, ValueClass_String>::Pointer& p) :
			ValuePointer<std::string, ValueClass_String>(p) { }
		
		StringValue(Value<std::string, ValueClass_String>* p) :
			ValuePointer<std::string, ValueClass_String>(p) { }
		
		StringValue(ValueBase::Pointer p) :
			ValuePointer<std::string, ValueClass_String>(p) { }
		
		size_t size() const
			{ return reference().content.size(); }

		bool empty() const
			{ return reference().content.empty(); }
	};
	
	class ArrayValue : public ValuePointer<std::vector<ValueBase::Pointer>, ValueClass_Array>
	{
	public:
		ArrayValue() :
			ValuePointer<ArrayValue::ValueType, ValueClass_Array>( ) { }
		
		ArrayValue(const ValuePointer<ArrayValue::ValueType, ValueClass_Array>& r) :
			ValuePointer<ArrayValue::ValueType, ValueClass_Array>(r) { }
		
		ArrayValue(const ArrayValue& r) :
			ValuePointer<ArrayValue::ValueType, ValueClass_Array>(r) { }
		
		ArrayValue(ArrayValue&& r) :
			ValuePointer<ArrayValue::ValueType, ValueClass_Array>(r) {	}
		
		ArrayValue(ValueBase::Pointer p) :
			ValuePointer<ArrayValue::ValueType, ValueClass_Array>(p) { }
		
		ArrayValue& operator = (const ArrayValue& r)
			{ reference().content = r->content; return *this; }
		
	public:
		void printContent() const;
	};
	
	class Dictionary : public ValuePointer<std::map<std::string, ValueBase::Pointer>, ValueClass_Dictionary>
	{
	public:
		Dictionary() :
			ValuePointer<Dictionary::ValueType, ValueClass_Dictionary>( ) { }
		
		Dictionary(const ValuePointer<Dictionary::ValueType, ValueClass_Dictionary>& r) :
			ValuePointer<Dictionary::ValueType, ValueClass_Dictionary>(r) { }

		Dictionary(const Dictionary& r) :
			ValuePointer<Dictionary::ValueType, ValueClass_Dictionary>(r) { }

		Dictionary(Dictionary&& r) :
			ValuePointer<Dictionary::ValueType, ValueClass_Dictionary>(r) {	}
		
		Dictionary(ValueBase::Pointer p) :
			ValuePointer<Dictionary::ValueType, ValueClass_Dictionary>(p) { }
		
		Dictionary& operator = (const Dictionary& r)
			{ reference().content = r->content; return *this; }
		
	public:
		void setStringForKey(const std::string& key, StringValue value)
			{ setValueForKey<StringValue, ValueClass_String>(key, value); }

		void setIntegerForKey(const std::string& key, IntegerValue value)
			{ setValueForKey<IntegerValue, ValueClass_Integer>(key, value); }
		
		void setArrayForKey(const std::string& key, ArrayValue value)
			{ setValueForKey<ArrayValue, ValueClass_Array>(key, value); }

		void setDictionaryForKey(const std::string& key, Dictionary value)
			{ setValueForKey<Dictionary, ValueClass_Dictionary>(key, value); }
		
	public:
		IntegerValue integerForKey(const std::string& key, IntegerValue def = IntegerValue()) const
			{ return valueForKey<IntegerValue::ValueType, ValueClass_Integer>(key, def); }
		
		IntegerValue integerForKeyPath(const std::vector<std::string>& key, IntegerValue def = IntegerValue()) const
			{ return valueForKeyPath<IntegerValue::ValueType, ValueClass_Integer>(key, def); }
		
		StringValue stringForKey(const std::string& key, StringValue def = StringValue()) const
			{ return valueForKey<StringValue::ValueType, ValueClass_String>(key, def); }
		
		StringValue stringForKeyPath(const std::vector<std::string>& key, StringValue def = StringValue()) const
			{ return valueForKeyPath<StringValue::ValueType, ValueClass_String>(key, def); }
		
		ArrayValue arrayForKey(const std::string& key, ArrayValue def = ArrayValue()) const
			{ return valueForKey<ArrayValue::ValueType, ValueClass_Array>(key, def); }
		
		ArrayValue arrayForKeyPath(const std::vector<std::string>& key, ArrayValue def = ArrayValue()) const
			{ return valueForKeyPath<ArrayValue::ValueType, ValueClass_Array>(key, def); }
		
		Dictionary dictionaryForKey(const std::string& key, Dictionary def = Dictionary()) const
			{ return Dictionary(valueForKey<Dictionary::ValueType, ValueClass_Dictionary>(key, def)); }
		
		Dictionary dictionaryForKeyPath(const std::vector<std::string>& key, Dictionary def = Dictionary()) const
			{ return Dictionary(valueForKeyPath<Dictionary::ValueType, ValueClass_Dictionary>(key, def)); }
		
		bool empty() const
			{ return reference().content.empty(); }
		
		void removeObjectForKey(const std::string& key)
			{ reference().content.erase(key); }
		
	public:
		void printContent() const;

	private:
		bool valueForKeyPathIsClassOf(const std::vector<std::string>& key, ValueClass) const;
		
		ValueBase::Pointer baseValueForKeyPath(const std::vector<std::string>& key) const;
		
		ValueBase::Pointer baseValueForKeyPathInHolder(const std::vector<std::string>& key,
			ValueBase::Pointer holder) const;
		
		template <typename T, ValueClass C>
		void setValueForKey(const std::string& key, const T& value)
			{ this->reference().content[key] = value; }
								
		template <typename T, ValueClass C>
		ValuePointer<T, C> valueForKeyPath(const std::vector<std::string>& key, ValuePointer<T, C> def) const
		{
			auto i = baseValueForKeyPath(key);
			if (i.invalid())
			{
				/*
				::printf("Value for key path {");
				for (auto& s : key) ::printf(" %s", s.c_str());
				::printf(" } not found.\n");
				*/
				return def;
			}
			
			if (i->valueClass() != C)
			{
				/*
				::printf("Value for key path {");
				for (auto& s : key) ::printf(" %s", s.c_str());
				::printf(" } found, but it contains different type.\n");
				 */
				return def;
			}
			
			return ValuePointer<T, C>(i);
		}
		
		template <typename T, ValueClass C>
		ValuePointer<T, C> valueForKey(const std::string& key, ValuePointer<T, C> def) const
			{ return valueForKeyPath(std::vector<std::string>(1, key), def); }
	};
}
