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
		ValueClass_Numeric,
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
	
	typedef ValuePointer<double, ValueClass_Numeric> NumericValue;
	typedef ValuePointer<std::string, ValueClass_String> StringValue;
	typedef ValuePointer<std::vector<ValueBase::Pointer>, ValueClass_Array> ArrayValue;
	
	class Dictionary : public ValuePointer<std::map<std::string, ValueBase::Pointer>, ValueClass_Dictionary>
	{
	public:
		Dictionary() :
			ValuePointer<Dictionary::ValueType, ValueClass_Dictionary>(
			new Value<Dictionary::ValueType, ValueClass_Dictionary>) { }
		
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

		void setNumberForKey(const std::string& key, NumericValue value)
			{ setValueForKey<NumericValue, ValueClass_Numeric>(key, value); }
		
		void setArrayForKey(const std::string& key, ArrayValue value)
			{ setValueForKey<ArrayValue, ValueClass_Array>(key, value); }

		void setDictionaryForKey(const std::string& key, Dictionary value)
			{ setValueForKey<Dictionary, ValueClass_Dictionary>(key, value); }
		
	public:
		NumericValue numberForKey(const std::string& key) const
			{ return valueForKey<NumericValue::ValueType, ValueClass_Numeric>(key); }

		StringValue stringForKey(const std::string& key) const
			{ return valueForKey<StringValue::ValueType, ValueClass_String>(key); }

		ArrayValue arrayForKey(const std::string& key) const
			{ return valueForKey<ArrayValue::ValueType, ValueClass_Array>(key); }
		
		Dictionary dictionaryForKey(const std::string& key) const
			{ return Dictionary(valueForKey<Dictionary::ValueType, ValueClass_Dictionary>(key)); }
		
	public:
		void printContent() const;

	private:
		template <typename T, ValueClass C>
		void setValueForKey(const std::string& key, const T& value)
			{ this->reference().content[key] = value; }
				
		template <typename T, ValueClass C>
		ValuePointer<T, C> valueForKey(const std::string& key) const
		{
			auto i = reference().content.find(key);
			
			if (i == reference().content.end())
				return ValuePointer<T, C>();
			
			typename Value<T, C>::Pointer ptr = i->second;
			assert(i->second->valueClass() == C);
			return ValuePointer<T, C>(ptr.ptr());
		}
		
	private:
		void printArray(ArrayValue, const std::string&) const;
		void printDictionary(Dictionary, const std::string&) const;
	};
}
