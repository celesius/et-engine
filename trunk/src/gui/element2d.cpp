/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/gui/element2d.h>

using namespace et;
using namespace et::gui;

Element2D::Element2D(Element* parent) : Element(parent),
	_frame(0.0f, 0.0f, 0.0f, 0.0f), _scale(1.0f), _color(1.0f), _angle(0.0f), _pivotPoint(0.0f)
{
}

Element2D::Element2D(const rect& frame, Element* parent) : Element(parent),
	_frame(frame), _scale(1.0f), _color(1.0f), _angle(0.0f), _pivotPoint(0.0f)
{
}

Element2D::~Element2D()
{
	for (AnimatorList::iterator i = _animations.begin(), e = _animations.end(); i != e; ++i)
		(*i)->destroy();
}

const vec2& Element2D::size() const
{ 
	return _frame.size(); 
}

const vec4 Element2D::color() const
{
	return vec4(_color.xyz(), finalAlpha());
}

const vec2& Element2D::position() const
{
	return _frame.origin(); 
}

const rect& Element2D::frame() const
{
	return _frame;
}

const vec2& Element2D::scale() const
{
	return _scale;
}

float Element2D::angle() const
{
	return _angle;
}

float Element2D::alpha() const
{
	return finalAlpha();
}

bool Element2D::visible() const
{
	return _color.w > 0.0f;
}

BaseAnimator* Element2D::setAngle(float angle, float duration)
{
	if (duration == 0.0f)
	{
		_angle = angle;
		invalidateTransform();
		return 0;
	}
	else 
	{
		BaseAnimator* a = new FloatAnimator(this, &_angle, _angle, angle, duration, ElementAnimatedProperty_Angle, timerPool());
		_animations.push_back(a);
		return a;
	}
}

BaseAnimator* Element2D::rotate(float angle, float duration)
{
	return setAngle(_angle + angle, duration);
}

BaseAnimator* Element2D::setScale(const vec2& scale, float duration)
{
	if (duration == 0.0f)
	{
		_scale = scale;
		invalidateTransform();
		return 0;
	}
	else 
	{
		BaseAnimator* a = new Vector2Animator(this, &_scale, _scale, scale, duration, ElementAnimatedProperty_Scale, timerPool());
		_animations.push_back(a);
		return a;
	}
}


BaseAnimator* Element2D::setColor(const vec4& color, float duration) 
{ 
	if (duration == 0.0f)
	{
		_color = color; 
		invalidateContent(); 
		return 0;
	}
	else 
	{
		BaseAnimator* a = new Vector4Animator(this, &_color, _color, color, duration, ElementAnimatedProperty_Color, timerPool());
		_animations.push_back(a);
		return a;
	}
}

BaseAnimator* Element2D::setAlpha(const float alpha, float duration) 
{ 
	if (_color.w == alpha) return 0;

	if (duration == 0.0f)
	{
		_color.w = alpha; 
		invalidateContent(); 
		return 0;
	}
	else 
	{
		BaseAnimator* a = new FloatAnimator(this, &_color.w, _color.w, alpha, duration, ElementAnimatedProperty_Color, timerPool());
		_animations.push_back(a);
		return a;
	}
}

BaseAnimator* Element2D::setFrame(const rect& r, float duration)
{
	if (duration == 0.0f)
	{
		_frame = r;
		invalidateTransform();
		invalidateContent(); 
		return 0;
	}
	else 
	{
		BaseAnimator* a = new RectAnimator(this, &_frame, _frame, r, duration, ElementAnimatedProperty_Frame, timerPool());
		_animations.push_back(a);
		return a;
	}
}

BaseAnimator* Element2D::setFrame(float x, float y, float width, float height, float duration)
{
	return setFrame(rect(x, y, width, height), duration);
}

BaseAnimator* Element2D::setFrame(const vec2& origin, const vec2& size, float duration)
{
	return setFrame(rect(origin, size), duration);
}

BaseAnimator* Element2D::setPosition(const vec2& p, float duration) 
{ 
	return setFrame(p.x, p.y, _frame.width, _frame.height, duration);
}

BaseAnimator* Element2D::setSize(const vec2& s, float duration) 
{ 
	return setFrame(_frame.left, _frame.top, s.x, s.y, duration);
}

BaseAnimator* Element2D::setPosition(float x, float y, float duration) 
{ 
	return setPosition(vec2(x, y), duration); 
}

BaseAnimator* Element2D::setSize(float w, float h, float duration) 
{ 
	return setSize(vec2(w, h), duration); 
}

BaseAnimator* Element2D::setVisible(bool vis, float duration)
{
	if (visible() == vis) return 0;
	return setAlpha( (vis ? 1.0f : 0.0f) , duration);
}

mat4 Element2D::finalTransform() 
{
	if (!transformValid())
		buildFinalTransform();

	return _finalTransform;
}

void Element2D::buildFinalTransform()
{ 
	_finalTransform = translationMatrix(vec3(offset(), 0.0f)) * 
		transform2DMatrix(_angle, _scale, _frame.origin()) * parentFinalTransform(); 

	setTransformValid(true);
}

float Element2D::finalAlpha() const
{
	return parent() ? parent()->finalAlpha() * _color.w : _color.w;
}

void Element2D::animatorUpdated(BaseAnimator* a)
{
	ElementAnimatedPropery prop = static_cast<ElementAnimatedPropery>(a->tag);

	bool isFrame = prop == ElementAnimatedProperty_Frame;
	
	if (isFrame || (prop == ElementAnimatedProperty_Angle) || (prop == ElementAnimatedProperty_Scale))
		invalidateTransform();

	if (isFrame || (prop == ElementAnimatedProperty_Color))
		invalidateContent();
}

void Element2D::animatorFinished(BaseAnimator* a)
{
	if (_animations.size())
	{
		AnimatorList::iterator i = find(_animations.begin(), _animations.end(), a);
		if (i != _animations.end())
			_animations.erase(i);
	}

	elementAnimationFinished.invoke(this, ElementAnimatedPropery(a->tag));
	a->destroy();
}

bool Element2D::containsPoint(const vec2& p, const vec2&) 
{
	vec2 tp = finalInverseTransform() * p;
	return (tp.x >= 0.0f) && (tp.y >= 0.0f) && (tp.x < _frame.width) && (tp.y < _frame.height);
}

bool Element2D::containLocalPoint(const vec2& p)
{
	return (p.x >= 0.0f) && (p.y >= 0.0f) && (p.x < _frame.width) && (p.y < _frame.height);
}

void Element2D::setPivotPoint(const vec2& p, bool preservePosition)
{
	_pivotPoint = p;

	if (preservePosition)
		_frame.setOrigin(_frame.origin() - offset());
}

vec2 Element2D::offset() const
{
	return -_frame.size() * _pivotPoint;
}

vec2 Element2D::origin() const
{
	return _frame.origin() + offset();
}

mat4 Element2D::finalInverseTransform()
{
	if (!inverseTransformValid())
	{
		_finalInverseTransform = finalTransform().inverse();
		setIverseTransformValid(true);
	}

	return _finalInverseTransform;
}

vec2 Element2D::positionInElement(const vec2& p)
{
	return finalInverseTransform() * p;
}
