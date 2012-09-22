/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/app/application.h>
#include <et/gui/guibase.h>

using namespace et;
using namespace et::gui;

float et::gui::alignmentFactor(ElementAlignment a)
{
	static float alignmentValues[ElementAlignment_max] = { 0.0f, 0.5f, 1.0f };
	return alignmentValues[a];
}

ElementState et::gui::adjustElementState(ElementState s)
{
	if (!input().canGetCurrentPointerInfo() && ((s == ElementState_Hovered) || (s == ElementState_SelectedHovered)))
		return static_cast<ElementState>(s - 1);
	
	return s;
}

Element::Element(Element* parent) : ElementHierarchy(parent), tag(0),
	_enabled(true), _transformValid(false), _inverseTransformValid(false), _contentValid(false)
{
}

void Element::setParent(Element* element)
{
	ElementHierarchy::setParent(element);
	invalidateContent();
	invalidateTransform();
}

bool Element::enabled() const
{
	return _enabled;
}

void Element::setEnabled(bool enabled)
{
	_enabled = enabled;
}

void Element::invalidateContent()
{ 
	_contentValid = false; 

	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
		(*i)->invalidateContent();

	setInvalid();
}

void Element::invalidateTransform()
{ 
	_transformValid = false; 
	_inverseTransformValid = false;

	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
		(*i)->invalidateTransform();

	setInvalid();
}

void Element::addToRenderQueue(RenderContext*, GuiRenderer&) 
{
}

void Element::addToOverlayRenderQueue(RenderContext*, GuiRenderer&)
{
}

void Element::startUpdates(TimerPoolObject* timerPool)
{
	TimedObject::startUpdates(timerPool);
}

void Element::startUpdates()
{
	TimedObject::startUpdates();
}

TimerPool& Element::timerPool()
{
	return mainTimerPool();
}

const TimerPool& Element::timerPool() const
{
	return mainTimerPool();
}

void Element::layoutChildren()
{
	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
		(*i)->layout(size());
}

void Element::bringToFront(Element* c)
{
	ElementHierarchy::bringToFront(c);
	invalidateContent();
}

void Element::sendToBack(Element* c)
{
	ElementHierarchy::sendToBack(c);
	invalidateContent();
}
