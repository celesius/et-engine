/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <et/core/dictionary.h>

using namespace et;

void Dictionary::printContent() const
{
	log::info("<");
	printDictionary(*this, "\t");
	log::info(">");
}

void Dictionary::printArray(ArrayValue arr, const std::string& tabs) const
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

void Dictionary::printDictionary(Dictionary dict, const std::string& tabs) const
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
