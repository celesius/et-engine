#include <et/app/application.h>
#include <et/gui/guibase.h>

using namespace et;
using namespace et::gui;

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

void Element::startUpdates(TimerPoolObject* timerPool)
{
	TimedObject::startUpdates(timerPool);
}

void Element::startUpdates()
{
	TimedObject::startUpdates(mainTimerPool().ptr());
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