/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/app/application.h>
#include <et/gui/scroll.h>
#include <et/gui/guirenderer.h>

using namespace et;
using namespace et::gui;

float deccelerationRate = 10.0f;
float accelerationRate = 0.5f;

Scroll::Scroll(Element2d* parent) : Element2d(parent), _offsetAnimator(0, 0, mainTimerPool()),
	_updateTime(0.0f), _pointerCaptured(false), _manualScrolling(false)
{
	_offsetAnimator.setDelegate(this);
	setFlag(ElementFlag_HandlesChildEvents);
	startUpdates();
}

void Scroll::addToRenderQueue(RenderContext*, GuiRenderer& r)
{
/*
	GuiVertexList vertices;
	r.createColorVertices(vertices, rect(vec2(0.0f), size()), vec4(1.0f, 0.5f, 0.25f, 0.25f), finalTransform(), GuiRenderLayer_Layer0);
	r.createColorVertices(vertices, rect(vec2(0.0f), size()), vec4(0.25f, 0.5f, 1.0f, 0.25f), Element2d::finalTransform(), GuiRenderLayer_Layer0);
	r.addVertices(vertices, Texture(), ElementClass_2d, GuiRenderLayer_Layer0);
*/ 
}

mat4 Scroll::finalTransform()
{
	mat4 e = Element2d::finalTransform();
	e[3] += vec4(_offset, 0.0f, 0.0f);
	return e;
}

mat4 Scroll::finalInverseTransform()
{
	return Element2d::finalTransform().inverse();
}

bool Scroll::pointerPressed(const PointerInputInfo& p)
{
	if (p.type == PointerType_General)
	{
		_previousPointer = p;
		_currentPointer = p;
		_manualScrolling = false;
		_pointerCaptured = false;
		_velocity = vec2(0.0f);
	}

	broadcastPressed(p);
	return true;
}

bool Scroll::pointerMoved(const PointerInputInfo& p)
{
	if (p.type == PointerType_General)
	{
		if (_manualScrolling)
		{
			applyOffset(p.pos - _currentPointer.pos);
		}
		else if (!_pointerCaptured)
		{
			_manualScrolling = true;
			_pointerCaptured = true;
			broadcastCancelled(p);
		}
		_previousPointer = _currentPointer;
		_currentPointer = p;
	}

	return true;
}

bool Scroll::pointerReleased(const PointerInputInfo& p)
{
	if (p.type == PointerType_General)
	{
		if (!_pointerCaptured)
			broadcastReleased(p);

		_pointerCaptured = false;
		_manualScrolling = false;
	}
	else 
	{
		broadcastReleased(p);
	}

	return true;
}

bool Scroll::pointerCancelled(const PointerInputInfo& p)
{
	_pointerCaptured = false;
	broadcastCancelled(p);
	return true;
}

void Scroll::invalidateChildren()
{
	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
	{
		(*i)->invalidateTransform();
		(*i)->invalidateContent();
	}
}

void Scroll::broadcastPressed(const PointerInputInfo& p)
{
	PointerInputInfo globalPos(p.type, Element2d::finalTransform() * p.pos, p.normalizedPos, p.scroll, p.id, p.timestamp);
	
	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
	{
		if ((*i)->containsPoint(globalPos.pos, globalPos.normalizedPos))
			(*i)->pointerPressed(PointerInputInfo(p.type, (*i)->positionInElement(globalPos.pos), globalPos.normalizedPos, p.scroll, p.id, p.timestamp));
	}
}

void Scroll::broadcastMoved(const PointerInputInfo& p)
{
	PointerInputInfo globalPos(p.type, Element2d::finalTransform() * p.pos, p.normalizedPos, p.scroll, p.id, p.timestamp);
	
	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
		(*i)->pointerMoved(PointerInputInfo(p.type, (*i)->positionInElement(globalPos.pos), globalPos.normalizedPos, p.scroll, p.id, p.timestamp));
}

void Scroll::broadcastReleased(const PointerInputInfo& p)
{
	PointerInputInfo globalPos(p.type, Element2d::finalTransform() * p.pos, p.normalizedPos, p.scroll, p.id, p.timestamp);
	
	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
		(*i)->pointerReleased(PointerInputInfo(p.type, (*i)->positionInElement(globalPos.pos), globalPos.normalizedPos, p.scroll, p.id, p.timestamp));
}

void Scroll::broadcastCancelled(const PointerInputInfo& p)
{
	PointerInputInfo globalPos(p.type, Element2d::finalTransform() * p.pos, p.normalizedPos, p.scroll, p.id, p.timestamp);
	
	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
		(*i)->pointerCancelled(PointerInputInfo(p.type, (*i)->positionInElement(globalPos.pos), globalPos.normalizedPos, p.scroll, p.id, p.timestamp));
}

bool Scroll::containsPoint(const vec2& p, const vec2& np)
{
	return Element2d::containsPoint(p, np);
}

void Scroll::update(float t)
{
	if (_updateTime == 0.0f)
		_updateTime = t;

	float deltaTime = t - _updateTime;

	_updateTime = t;

	if (_manualScrolling)
	{
		float dt = _currentPointer.timestamp - _previousPointer.timestamp;
		if (dt > 1.0e-2)
		{
			vec2 dp = _currentPointer.pos - _previousPointer.pos;
			_velocity = mix(_velocity, dp * (accelerationRate / dt), 0.5f);
		}
		return;
	}

	float dt = etMin(1.0f, deltaTime * deccelerationRate);
	_velocity -= _velocity * dt;

	if (_velocity.dotSelf() < 1.0f)
		_velocity = vec2(0.0f);

	vec2 dp = _velocity * deltaTime;
	if (dp.dotSelf() > 1.0e-6)
		applyOffset(dp);
}

void Scroll::setContentSize(const vec2& cs)
{
	_contentSize = cs;
}

void Scroll::adjustContentSize()
{
	vec2 size;
	
	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
	{
		Element* ptr = i->ptr();
		if (ptr->visible())
			size = maxv(size, ptr->origin() + ptr->size());
	}
	
	setContentSize(size);
}

void Scroll::applyOffset(const vec2& dOffset, float duration)
{
	setOffset(_offset + dOffset, duration);
}

void Scroll::setOffset(const vec2& aOffset, float duration)
{
	_offsetAnimator.cancelUpdates();
	
	if (duration == 0.0f)
	{
		internal_setOffset(aOffset);
	}
	else
	{
		_offsetAnimator.animate(&_offset, _offset, aOffset, duration);
	}
}

void Scroll::internal_setOffset(const vec2& o)
{
	_offset = o * vec2(0.0f, 1.0f);
	vec2 actualOffset = -_offset;
	
	if (actualOffset.y < 0.0f)
	{
		_offset.y = 0.0f;
		_velocity.y = 0.0f;
	}
	
	if (actualOffset.y + size().y > _contentSize.y)
	{
		_offset.y = -_contentSize.y + size().y;
		_velocity.y = 0.0f;
	}
	
	invalidateChildren();
}

void Scroll::animatorUpdated(BaseAnimator* a)
{
	if (a == &_offsetAnimator)
		internal_setOffset(_offset);
	
	Element2d::animatorUpdated(a);
}

void Scroll::animatorFinished(BaseAnimator* a)
{
	Element2d::animatorFinished(a);
}
