/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/core/et.h>

namespace et
{ 

	template <class t_class>
	class Singleton
	{
	public:
		static t_class& instance()
		{
			static t_class _instance;
			return _instance;
		}

	protected: 
		Singleton() { };
		Singleton(const Singleton&) { };
		Singleton& operator = (const Singleton&) { };
	};

#define ET_SINGLETON_CONSTRUCTORS(t) private:	\
	t() { }										\
	t(const t&) { }								\
	t& operator = (const t&) { return *this; }	\
	friend class et::Singleton<t>;

#define ET_SINGLETON_COPY_DENY(t) private:		\
	ET_DENY_COPY(t)								\
	friend class et::Singleton<t>;

}