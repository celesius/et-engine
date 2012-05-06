/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#define ET_DECLARE_PROPERTY(T, name, setter) public: \
		T name() const \
			{ return _##name; } \
		void setter(const T& value) \
			{ _##name = value; } \
	private: \
		T _##name;

#define ET_DECLARE_PROPERTY_READONLY(T, name) public: \
		T name() const \
			{ return _##name; } \
	private: \
		T _##name;

#define ET_DECLARE_PROPERTY_READONLY_REF(T, name) public: \
		const T& name() const \
			{ return _##name; } \
	private: \
		T _##name;