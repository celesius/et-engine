/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/gui/scroll.h>
#include <et/gui/guirenderer.h>

using namespace et;
using namespace et::gui;

float deccelerationRate = 10.0f;
float accelerationRate = 0.5f;

Scroll::Scroll(Element2d* parent) : Element2d(parent), _updateTime(0.0f)
{
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
		_dragging = false;
	
	broadcastPressed(p);
	return true;
}

bool Scroll::pointerMoved(const PointerInputInfo& p)
{
	if (_dragging)
	{
		vec2 dp = p.pos - _dragPoint.pos;
		float dt = etMax(0.01f, p.timestamp - _dragPoint.timestamp);
		_velocity += accelerationRate * (dp / dt);
		_velocity.x = 0.0f;
		_dragPoint = p;
		
		invalidateChildren();
		broadcastCancelled(p);
	}
	else if (p.type == PointerType_General)
	{
		_dragging = true;
		_dragPoint = p;
	}
	
	return true;
}

bool Scroll::pointerReleased(const PointerInputInfo& p)
{
	if (_dragging)
	{
		broadcastCancelled(p);
		_dragging = false;
	}
	else 
	{
		broadcastReleased(p);
	}
	
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

void Scroll::broadcastReleased(const PointerInputInfo& p)
{
	PointerInputInfo globalPos(p.type, Element2d::finalTransform() * p.pos, p.normalizedPos, p.scroll, p.id, p.timestamp);
	
	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
	{
		if ((*i)->containsPoint(globalPos.pos, globalPos.normalizedPos))
			(*i)->pointerReleased(PointerInputInfo(p.type, (*i)->positionInElement(globalPos.pos), globalPos.normalizedPos, p.scroll, p.id, p.timestamp));
	}
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
	float dt = (_updateTime == 0.0f) ? 0.0f : (t - _updateTime);
	_updateTime = t;
	
	float dv = etMin(1.0f, dt * deccelerationRate);
	_velocity -= dv * _velocity;
	
	vec2 dp = _velocity * dt;
	if (dp.dotSelf() > 1.0e-6)
	{
		updateOffset(dp);
	}
}

void Scroll::setContentSize(const vec2& cs)
{
	std::cout << "Content size: " << cs << std::endl;
	_contentSize = cs;
}

void Scroll::adjustContentSize()
{
	if (children().size() == 0)
	{
		setContentSize(vec2(0.0f));
		return;
	}
	
	vec2 size = children().front()->origin() + children().front()->size();
	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
		size = maxv(size, (*i)->origin() + (*i)->size());
	
	setContentSize(size);
}

void Scroll::updateOffset(const vec2& dOffset)
{
	_offset += dOffset;
	vec2 actualOffset = -_offset;
	
	if (actualOffset.y < 0.0f)
		_offset.y = 0.0f;
	
	if (actualOffset.y + size().y > _contentSize.y)
		_offset.y = -_contentSize.y + size().y;
/*
	if (actualOffset.x < 0.0f)
		_offset.x = 0.0f;
	
	if (actualOffset.x + size().x > _contentSize.x)
		_offset.x = -_contentSize.x + size().x;
*/	
	invalidateChildren();
}