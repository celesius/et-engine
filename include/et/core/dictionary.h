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
	
	class StringValue : public ValuePointer<std::string, ValueClass_String>
	{
	public:
		StringValue() :
			ValuePointer<std::string, ValueClass_String>() { }
		
		StringValue(const std::string& r) :
			ValuePointer<std::string, ValueClass_String>(r) { }

		StringValue(const char* r) :
			ValuePointer<std::string, ValueClass_String>(r) { }
		
		StringValue(const typename Value<std::string, ValueClass_String>::Pointer& p) :
			ValuePointer<std::string, ValueClass_String>(p) { }
		
		StringValue(Value<std::string, ValueClass_String>* p) :
			ValuePointer<std::string, ValueClass_String>(p) { }
		
		StringValue(ValueBase::Pointer p) :
			ValuePointer<std::string, ValueClass_String>(p) { }
		
		operator std::string ()
			{ return reference().content; }
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

		void setNumberForKey(const std::string& key, NumericValue value)
			{ setValueForKey<NumericValue, ValueClass_Numeric>(key, value); }
		
		void setArrayForKey(const std::string& key, ArrayValue value)
			{ setValueForKey<ArrayValue, ValueClass_Array>(key, value); }

		void setDictionaryForKey(const std::string& key, Dictionary value)
			{ setValueForKey<Dictionary, ValueClass_Dictionary>(key, value); }
		
	public:
		NumericValue numberForKey(const std::string& key, NumericValue def = NumericValue()) const
			{ return valueForKey<NumericValue::ValueType, ValueClass_Numeric>(key, def); }
		
		NumericValue numberForKeyPath(const std::vector<std::string>& key, NumericValue def = NumericValue()) const
			{ return valueForKeyPath<NumericValue::ValueType, ValueClass_Numeric>(key, def); }
		
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
		
		bool emptry() const
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
			return (i.valid() && (i->valueClass() == C)) ? ValuePointer<T, C>(i) : ValuePointer<T, C>(def);
		}
		
		template <typename T, ValueClass C>
		ValuePointer<T, C> valueForKey(const std::string& key, ValuePointer<T, C> def) const
			{ return valueForKeyPath({key}, def); }
	};
}
