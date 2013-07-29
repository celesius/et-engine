/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <typeinfo>
#include <et/core/tools.h>
#include <et/core/dictionary.h>

using namespace et;

void printDictionary(Dictionary dict, const std::string& tabs);
void printArray(ArrayValue arr, const std::string& tabs);

void Dictionary::printContent() const
{
	log::info("<");
	printDictionary(*this, "\t");
	log::info(">");
}

void ArrayValue::printContent() const
{
	log::info("{");
	printArray(*this, "\t");
	log::info("}");
}

ValueBase::Pointer Dictionary::baseValueForKeyPathInHolder(const std::vector<std::string>& path,
	ValueBase::Pointer holder) const
{
	if (holder->valueClass() == ValueClass_Dictionary)
	{
		Dictionary dictionary(holder);
		auto value = dictionary->content.find(path.front());
		
		if (value == dictionary->content.end())
			return ValueBase::Pointer();
		
		return (path.size() == 1) ? value->second : baseValueForKeyPathInHolder(
			std::vector<std::string>(path.begin() + 1, path.end()), value->second);
	}
	else if (holder->valueClass() == ValueClass_Array)
	{
		size_t index = strToInt(path.front());
		ArrayValue array(holder);
		
		if (index >= array->content.size())
			return ValueBase::Pointer();
		
		ValueBase::Pointer value = array->content.at(index);
		
		return (path.size() == 1) ? value : baseValueForKeyPathInHolder(
			std::vector<std::string>(path.begin() + 1, path.end()), value);
	}
	else if (holder->valueClass() == ValueClass_String)
	{
		StringValue string(holder);
		log::warning("Trying to extract subvalue `%s` from string `%s`", path.front().c_str(),
			string->content.c_str());
	}
	else if (holder->valueClass() == ValueClass_Numeric)
	{
		NumericValue number(holder);
		log::warning("Trying to extract subvalue `%s` from number %g.", path.front().c_str(),
			number->content);
	}
	else
	{
		assert(false);
	}
	
	return ValueBase::Pointer();
}

ValueBase::Pointer Dictionary::baseValueForKeyPath(const std::vector<std::string>& path) const
{
	assert(path.size() > 0);
	return baseValueForKeyPathInHolder(path, *this);
}

bool Dictionary::valueForKeyPathIsClassOf(const std::vector<std::string>& key, ValueClass c) const
{
	auto v = baseValueForKeyPath(key);
	return v.valid() && (v->valueClass() == c);
}

/*
 * Service functions
 */
void printArray(ArrayValue arr, const std::string& tabs)
{
	for (auto i : arr->content)
	{
		if (i->valueClass() == ValueClass_Numeric)
		{
			NumericValue val = i;
			log::info("%s%g", tabs.c_str(), val->content);
		}
		else if (i->valueClass() == ValueClass_String)
		{
			StringValue val = i;
			log::info("%s%s", tabs.c_str(), val->content.c_str());
		}
		else if (i->valueClass() == ValueClass_Array)
		{
			log::info("%s{", tabs.c_str());
			printArray(i, tabs + "\t");
			log::info("%s}", tabs.c_str());
		}
		else if (i->valueClass() == ValueClass_Dictionary)
		{
			Dictionary val = i;
			log::info("%s<", tabs.c_str());
			printDictionary(val, tabs + "\t");
			log::info("%s>", tabs.c_str());
		}
	}
}

void printDictionary(Dictionary dict, const std::string& tabs)
{
	for (auto i : dict->content)
	{
		if (i.second->valueClass() == ValueClass_Numeric)
		{
			NumericValue val = i.second;
			log::info("%s%s = %g", tabs.c_str(), i.first.c_str(), val->content);
		}
		else if (i.second->valueClass() == ValueClass_String)
		{
			StringValue val = i.second;
			log::info("%s%s = %s", tabs.c_str(), i.first.c_str(), val->content.c_str());
		}
		else if (i.second->valueClass() == ValueClass_Array)
		{
			ArrayValue val = i.second;
			log::info("%s%s =\n%s{", tabs.c_str(), i.first.c_str(), tabs.c_str());
			printArray(val, tabs + "\t");
			log::info("%s}", tabs.c_str());
		}
		else if (i.second->valueClass() == ValueClass_Dictionary)
		{
			Dictionary val = i.second;
			log::info("%s%s =\n%s<", tabs.c_str(), i.first.c_str(), tabs.c_str());
			printDictionary(val, tabs + "\t");
			log::info("%s>", tabs.c_str());
		}
	}
}

