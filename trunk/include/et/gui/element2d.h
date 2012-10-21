/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/gui/guibase.h>

namespace et
{
	namespace gui
	{
		enum ElementAnimatedPropery
		{
			ElementAnimatedProperty_None,
			ElementAnimatedProperty_Angle,
			ElementAnimatedProperty_Scale,
			ElementAnimatedProperty_Color,
			ElementAnimatedProperty_Frame,
			ElementAnimatedProperty_max
		};

		class Element2d : public Element
		{
		public:
			typedef IntrusivePtr<Element2d> Pointer;

		public:
			Element2d(Element* parent);
			Element2d(const rect& frame, Element* parent);

			virtual ~Element2d();

			virtual ElementClass elementClass() const 
				{ return ElementClass_2d; };

			const vec2& size() const;
			const vec4 color() const;
			const vec2& position() const;
			const vec2& scale() const;
			const rect& frame() const;
			const vec2& pivotPoint() const;

			vec2 origin() const;
			vec2 offset() const;

			float angle() const;
			float alpha() const;
			bool visible() const;

			/*
			 * Setters
			 */
			virtual void setAngle(float angle, float duration = 0.0f);
			virtual void setScale(const vec2& scale, float duration = 0.0f);
			virtual void setColor(const vec4& color, float duration = 0.0f);
			virtual void setFrame(const rect& r, float duration = 0.0f);
			virtual void setFrame(const vec2& origin, const vec2& size, float duration = 0.0f);
			virtual void setPivotPoint(const vec2& p, bool preservePosition = true);

			/*
			 * Convenience
			 */
			void setPosition(const vec2& p, float duration = 0.0f);
			void setAlpha(const float alpha, float duration = 0.0f);
			void setFrame(float x, float y, float width, float height, float duration = 0.0f);
			void setPosition(float x, float y, float duration = 0.0f);
			void setSize(const vec2& s, float duration = 0.0f);
			void setSize(float w, float h, float duration = 0.0f);
			void rotate(float angle, float duration = 0.0f);
			void setVisible(bool visible, float duration = 0.0f);

			virtual bool containsPoint(const vec2& p, const vec2&);
			virtual bool containLocalPoint(const vec2& p);

			mat4 finalTransform();
			mat4 finalInverseTransform();
			vec2 positionInElement(const vec2& p);

			ET_DECLARE_EVENT2(elementAnimationFinished, Element2d*, ElementAnimatedPropery)

		protected:
			void buildFinalTransform();

			mat4 transform();
			float finalAlpha() const;

			void animatorUpdated(BaseAnimator*);
			void animatorFinished(BaseAnimator*);

		private:
			RectAnimator _frameAnimator;
			Vector4Animator _colorAnimator;
			Vector2Animator _scaleAnimator;
			FloatAnimator _angleAnimator;

			mat4 _finalTransform;
			mat4 _finalInverseTransform;
			rect _frame;
			vec2 _scale;
			vec4 _color;
			vec2 _pivotPoint;
			float _angle;
		};
	}
}
