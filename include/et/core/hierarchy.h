/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <list>
#include <et/core/object.h>

namespace et
{
	template <typename T>
	class Hierarchy : public Object
	{
	public:
		typedef IntrusivePtr<T> BasePointer;
		typedef std::list<BasePointer> List;
		
	public:
		Hierarchy(T* parent);
		virtual ~Hierarchy();

		virtual void setParent(T* p);

		T* parent();
		const T* parent() const;

		List& children();
		const List& children() const;

		virtual void bringToFront(T* c);
		virtual void sendToBack(T* c);

	protected:
		void removeChildren();

	private:
		bool addChild(T* c);
		bool removeChild(T* c);

	private:
		void pushChild(T* c);
		void removeParent();

	private:
		T* _parent;
		List _children;
	};

	template <typename T>
	Hierarchy<T>::Hierarchy(T* parent) : _parent(parent)
	{
		if (_parent)
			_parent->pushChild(static_cast<T*>(this));
	}

	template <typename T>
	Hierarchy<T>::~Hierarchy()
	{
		for (auto& i : _children)
			i->removeParent();
	}

	template <typename T>
	void Hierarchy<T>::setParent(T* p)
	{
		retain();

		if (_parent)
			_parent->removeChild(static_cast<T*>(this));

		_parent = p;

		if (_parent)
			_parent->addChild(static_cast<T*>(this));

		release();
	}

	template <typename T>
	void Hierarchy<T>::pushChild(T* c)
	{
		_children.push_back(typename Hierarchy<T>::BasePointer(c));
	}

	template <typename T>
	bool Hierarchy<T>::addChild(T* c)
	{
		if (c == this) return false;

		bool notFound = true;
		for (auto i = _children.begin(), e = _children.end(); i != e; ++i)
		{
			if (*i == c)
			{
				notFound = false;
				break;
			}
		}

		if (notFound)
			pushChild(c);

		return notFound;
	}

	template <typename T>
	bool Hierarchy<T>::removeChild(T* c)
	{
		bool found = false;
		
		for (auto i = _children.begin(), e = _children.end(); i != e; ++i)
		{
			if (*i == c)
			{
				_children.erase(i);
				found = true;
				break;
			}
		}


		return found;
	}

	template <typename T>
	void Hierarchy<T>::bringToFront(T* c)
	{
		for (auto i = _children.begin(), e = _children.end(); i != e; ++i)
		{
			if (*i == c)
			{
				_children.erase(i);
				_children.push_back(typename Hierarchy<T>::BasePointer(c));
				break;
			}
		}
	}

	template <typename T>
	void Hierarchy<T>::sendToBack(T* c)
	{
		for (auto i = _children.begin(), e = _children.end(); i != e; ++i)
		{
			if (*i == c)
			{
				_children.erase(i);
				_children.push_front(typename Hierarchy<T>::BasePointer(c));
				break;
			}
		}
	}

	template <typename T>
	void Hierarchy<T>::removeChildren()
	{
		_children.clear();
	}

	template <typename T>
	inline T* Hierarchy<T>::parent()
	{
		return _parent; 
	}

	template <typename T>
	inline const T* Hierarchy<T>::parent() const
	{ 
		return _parent; 
	}

	template <typename T>
	inline const typename Hierarchy<T>::List& Hierarchy<T>::children() const
	{
		return _children; 
	}

	template <typename T>
	inline typename Hierarchy<T>::List& Hierarchy<T>::children()
	{
		return _children; 
	}

	template <typename T>
	void Hierarchy<T>::removeParent()
	{
		_parent = 0;
	}

}