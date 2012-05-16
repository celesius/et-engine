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

Scroll::Scroll(Element2D* parent) : Element2D(parent)
{
	setFlag(ElementFlag_HandlesChildEvents);
}

void Scroll::addToRenderQueue(RenderContext*, GuiRenderer& r)
{
	GuiVertexList vertices;
	r.createColorVertices(vertices, rect(vec2(0.0f), size()), vec4(1.0f, 0.5f, 0.25f, 0.75f), finalTransform(), GuiRenderLayer_Layer1);
	r.createColorVertices(vertices, rect(vec2(0.0f), size()), vec4(0.25f, 0.5f, 1.0f, 0.75f), Element2D::finalTransform(), GuiRenderLayer_Layer1);
	r.addVertices(vertices, Texture(), ElementClass_2d, GuiRenderLayer_Layer0);
}

mat4 Scroll::finalTransform()
{
	mat4 e = Element2D::finalTransform();
	e[3] += vec4(_offset, 0.0f, 0.0f);
	return e;
}

mat4 Scroll::finalInverseTransform()
{
	return Element2D::finalTransform().inverse();
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
		_offset += dp;
		_dragPoint = p;
		
		invalidateChildren();
		broadcastCanceled(p);
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
		broadcastCanceled(p);
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
		(*i)->invalidateTransform();
}

void Scroll::broadcastPressed(const PointerInputInfo& p)
{
	PointerInputInfo globalPos(p.type, Element2D::finalTransform() * p.pos, p.normalizedPos, p.scroll, p.id, p.timestamp);
	
	std::cout << "Original point: " << p.pos << ", global: " << globalPos.pos << std::endl;
	
	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
	{
		if ((*i)->containsPoint(globalPos.pos, globalPos.normalizedPos))
			(*i)->pointerPressed(PointerInputInfo(p.type, (*i)->positionInElement(globalPos.pos), globalPos.normalizedPos, p.scroll, p.id, p.timestamp));
	}
}

void Scroll::broadcastReleased(const PointerInputInfo& p)
{
	PointerInputInfo globalPos(p.type, Element2D::finalTransform() * p.pos, p.normalizedPos, p.scroll, p.id, p.timestamp);
	
	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
	{
		if ((*i)->containsPoint(globalPos.pos, globalPos.normalizedPos))
			(*i)->pointerReleased(PointerInputInfo(p.type, (*i)->positionInElement(globalPos.pos), globalPos.normalizedPos, p.scroll, p.id, p.timestamp));
	}
}

void Scroll::broadcastCanceled(const PointerInputInfo& p)
{
	PointerInputInfo globalPos(p.type, Element2D::finalTransform() * p.pos, p.normalizedPos, p.scroll, p.id, p.timestamp);
	
	for (Element::List::iterator i = children().begin(), e = children().end(); i != e; ++i)
		(*i)->pointerCanceled(PointerInputInfo(p.type, (*i)->positionInElement(globalPos.pos), globalPos.normalizedPos, p.scroll, p.id, p.timestamp));
}

bool Scroll::containsPoint(const vec2& p, const vec2& np)
{
	return true;
}