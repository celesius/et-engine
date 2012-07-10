/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <string>
#include <iostream>
#include <assert.h>
#include <et/scene3d/element.h>

using namespace et;
using namespace et::s3d;

Element::Element(const std::string& name, Element* parent) : ElementHierarchy(parent), 
	tag(0), _name(name), _active(true)
{
}

void Element::setParent(Element* p)
{
	invalidateTransform();
	ElementHierarchy::setParent(p);
}

void Element::invalidateTransform()
{
	ComponentTransformable::invalidateTransform();
	for (List::iterator i = children().begin(), e = children().end(); i != e; ++i)
		(*i)->invalidateTransform();
}

mat4 Element::finalTransform()
{
	if (!transformValid())
		_cachedFinalTransform = parent() ?  transform() * parent()->finalTransform() : transform();
	return _cachedFinalTransform;
}

bool Element::isKindOf(ElementType t) const
{
	return (t == ElementType_Any) || (type() == t);
}

Element::Pointer Element::childWithName(const std::string& name, ElementType ofType, bool assertFail)
{
	for (List::const_iterator i = children().begin(), e = children().end(); i != e; ++i)
	{
		Element::Pointer element = childWithNameCallback(name, *i, ofType);
		if (element.valid())
			return element;
	}

	if (assertFail)
	{
		std::cout << "Unable to find child with name: " + name << std::endl;
		assert("Unable to find child" && 0);
	}

	return Element::Pointer();
}

Element::List Element::childrenOfType(ElementType ofType)
{
	Element::List list;

	for (List::const_iterator i = children().begin(), e = children().end(); i != e; ++i)
		childrenOfTypeCallback(ofType, list, *i);

	return list;
}

Element::List Element::childrenHavingFlag(size_t flag)
{
	Element::List list;

	for (List::const_iterator i = children().begin(), e = children().end(); i != e; ++i)
		childrenHavingFlagCallback(flag, list, *i);

	return list;
}

Element::Pointer Element::childWithNameCallback(const std::string& name, Element::Pointer root, ElementType ofType)
{
	if (root->isKindOf(ofType) && (root->name() == name))
		return root;

	for (List::const_iterator i = root->children().begin(), e = root->children().end(); i != e; ++i)
	{
		Element::Pointer element = childWithNameCallback(name, *i, ofType);
		if (element.valid() && element->isKindOf(ofType))
			return element;
	}

	return Element::Pointer();
}

void Element::childrenOfTypeCallback(ElementType t, Element::List& list, Element::Pointer root)
{
	if (root->isKindOf(t))
		list.push_back(root);

	for (List::const_iterator i = root->children().begin(), e = root->children().end(); i != e; ++i)
		childrenOfTypeCallback(t, list, *i);
}

void Element::childrenHavingFlagCallback(size_t flag, Element::List& list, Element::Pointer root)
{
	if (root->hasFlag(flag))
		list.push_back(root);

	for (List::const_iterator i = root->children().begin(), e = root->children().end(); i != e; ++i)
		childrenHavingFlagCallback(flag, list, *i);
}

void Element::clear()
{
	removeChildren();
}

void Element::serializeGeneralParameters(std::ostream& stream)
{
	serializeString(stream, _name);
	serializeInt(stream, _active);
	serializeInt(stream, flags());
	serializeVector(stream, translation());
	serializeVector(stream, scale());
	serializeQuaternion(stream, orientation());
}

void Element::deserializeGeneralParameters(std::istream& stream)
{
	_name = deserializeString(stream);
	_active = deserializeInt(stream) != 0;
	setFlags(deserializeInt(stream));
	setTranslation(deserializeVector<vec3>(stream));
	setScale(deserializeVector<vec3>(stream));
	setOrientation(deserializeQuaternion(stream));
}

void Element::serializeChildren(std::ostream& stream)
{
	serializeInt(stream, children().size());
	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
	{
		serializeInt(stream, (*i)->type());
		(*i)->serialize(stream);
	}
}

void Element::deserializeChildren(std::istream& stream, ElementFactory* factory)
{
	size_t numChildren = deserializeInt(stream);
	for (size_t i = 0; i < numChildren; ++i)
	{
		size_t type = deserializeInt(stream);
		Element::Pointer child = factory->createElementOfType(type, (type == ElementType_Storage) ? 0 : this);
		child->deserialize(stream, factory);
	}
}

void Element::serialize(std::ostream&)
{
	std::cout << "Serialization method isn't defined for " << typeid(*this).name() << std::endl;
#if (ET_DEBUG)
	abort();
#endif
}

void Element::deserialize(std::istream&, ElementFactory*)
{
	std::cout << "Deserialization method isn't defined for " << typeid(*this).name() << std::endl;
#if (ET_DEBUG)
	abort();
#endif
}

void Element::setActive(bool active)
{
	_active = active;
}

bool Element::hasPropertyString(const std::string& s) const
{
	return std::find(_properites.begin(), _properites.end(), s) != _properites.end();
}