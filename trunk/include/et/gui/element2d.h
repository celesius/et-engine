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

		class Element2D : public Element
		{
		public:
			typedef IntrusivePtr<Element2D> Pointer;

		public:
			Element2D(Element* parent);
			Element2D(const rect& frame, Element* parent);

			virtual ~Element2D();

			virtual ElementClass elementClass() const 
				{ return ElementClass_2d; };

			const vec2& size() const;
			const vec4 color() const;
			const vec2& position() const;
			const vec2& scale() const;
			const rect& frame() const;

			vec2 origin() const;
			vec2 offset() const;

			float angle() const;
			float alpha() const;
			bool visible() const;

			/*
			 * Setters
			 */
			virtual BaseAnimator* setAngle(float angle, float duration = 0.0f);
			virtual BaseAnimator* setScale(const vec2& scale, float duration = 0.0f);
			virtual BaseAnimator* setColor(const vec4& color, float duration = 0.0f);
			virtual BaseAnimator* setFrame(const rect& r, float duration = 0.0f);
			virtual BaseAnimator* setFrame(const vec2& origin, const vec2& size, float duration = 0.0f);

			inline const vec2& pivotPoint() const 
				{ return _pivotPoint; }

			virtual void setPivotPoint(const vec2& p, bool preservePosition = true);

			/*
			 * Convenience
			 */
			BaseAnimator* setPosition(const vec2& p, float duration = 0.0f);
			BaseAnimator* setAlpha(const float alpha, float duration = 0.0f);
			BaseAnimator* setFrame(float x, float y, float width, float height, float duration = 0.0f);
			BaseAnimator* setPosition(float x, float y, float duration = 0.0f);
			BaseAnimator* setSize(const vec2& s, float duration = 0.0f);
			BaseAnimator* setSize(float w, float h, float duration = 0.0f);
			BaseAnimator* rotate(float angle, float duration = 0.0f);
			BaseAnimator* setVisible(bool visible, float duration = 0.0f);

			virtual bool containPoint(const vec2& p, const vec2&);
			virtual bool containLocalPoint(const vec2& p);

			mat4 finalTransform();
			mat4 finalInverseTransform();
			vec2 positionInElement(const vec2& p);

			ET_DECLARE_EVENT2(elementAnimationFinished, Element2D*, ElementAnimatedPropery)

		protected:
			void buildFinalTransform();

			mat4 transform();
			float finalAlpha() const;

			void animatorUpdated(BaseAnimator*);
			void animatorFinished(BaseAnimator*);

		private:
			AnimatorList _animations;
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