/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

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
	t(const t&) { }								\
	t& operator = (const t&) { return *this; }	\
	friend class et::Singleton<t>;

}