/*
* This file is part of `et engine`
* Copyright 2009-2012 by Sergey Reznik
* Please, do not modify contents without approval.
*
*/

#pragma once

#include <map>

#include <et/core/flags.h>
#include <et/core/hierarchy.h>
#include <et/app/events.h>
#include <et/timers/animator.h>
#include <et/input/input.h>
#include <et/rendering/rendercontext.h>
#include <et/gui/guibaseconst.h>
#include <et/gui/guibaseclasses.h>

namespace et
{
	namespace gui
	{
		class Element;
		class Layout;
		typedef Hierarchy<Element> ElementHierarchy;
		class Element : public ElementHierarchy, public Flags, public EventReceiver, public TimedObject, public AnimatorDelegate
		{
		public:
			typedef IntrusivePtr<Element> Pointer;

			union 
			{
				size_t tag;
				int tag_i;
			};

		public:
			Element(Element* parent);

			void setParent(Element* element);

			void invalidateTransform();
			void invalidateContent();

			virtual void processMessage(const GuiMessage&) 
				{ }

			virtual void addToRenderQueue(RenderContext*, GuiRenderer&);
			virtual void addToOverlayRenderQueue(RenderContext*, GuiRenderer&);

			virtual bool pointerPressed(const PointerInputInfo&)
				{ return !hasFlag(ElementFlag_TransparentForPointer); }

			virtual bool pointerMoved(const PointerInputInfo&)
				{ return !hasFlag(ElementFlag_TransparentForPointer); }

			virtual bool pointerReleased(const PointerInputInfo&)
				{ return !hasFlag(ElementFlag_TransparentForPointer); }

			virtual bool pointerScrolled(const PointerInputInfo&)
				{ return !hasFlag(ElementFlag_TransparentForPointer); }

			virtual bool pointerCancelled(const PointerInputInfo&)
				{ return !hasFlag(ElementFlag_TransparentForPointer); }

			virtual void pointerEntered(const PointerInputInfo&) { }
			virtual void pointerLeaved(const PointerInputInfo&) { }

			virtual bool capturesPointer() const { return true; }
			virtual bool containLocalPoint(const vec2&) { return false; }
			virtual vec2 positionInElement(const vec2& p) { return p; }

			virtual bool enabled() const;
			virtual void setEnabled(bool enabled);

			virtual bool visible() const
				{ return true; }

			virtual float finalAlpha() const 
				{ return 1.0f; }

			virtual mat4 finalTransform() 
				{ return IDENTITY_MATRIX; }

			virtual mat4 finalInverseTransform() 
				{ return IDENTITY_MATRIX; }

			virtual void layout(const vec2&) 
				{ layoutChildren(); }

			virtual void setFocus() { };
			virtual void resignFocus(Element*) { };

			virtual void didAppear() { }
			virtual void didDisappear() { }
			virtual void willAppear() { }
			virtual void willDisappear() { }

			void bringToFront(Element* c);
			void sendToBack(Element* c);

			/*
			* Required Methods
			*/
			virtual ElementClass elementClass() const = 0;
			virtual const vec2& position() const = 0;
			virtual vec2 origin() const = 0;
			virtual const vec2& size() const = 0;
			virtual void setPosition(const vec2& p, float duration = 0.0f) = 0;
			virtual bool containsPoint(const vec2&, const vec2&) = 0;

			/*
			* Events
			*/
			ET_DECLARE_EVENT2(dragStarted, Element*, const ElementDragInfo&)
			ET_DECLARE_EVENT2(dragged, Element*, const ElementDragInfo&)
			ET_DECLARE_EVENT2(dragFinished, Element*, const ElementDragInfo&)

		protected:
			void setContentValid()
				{ _contentValid = true; }

			bool contentValid() const
				{ return _contentValid; }

			bool transformValid() const
				{ return _transformValid; }

			bool inverseTransformValid()
				{ return _inverseTransformValid; }

			virtual void setInvalid()
				{ if (parent()) parent()->setInvalid(); }

			void setTransformValid(bool v) 
				{ _transformValid = v; }

			void setIverseTransformValid(bool v)
				{ _inverseTransformValid = v; }

			virtual mat4 parentFinalTransform()
				{ return parent() ? parent()->finalTransform() : IDENTITY_MATRIX; }

			virtual void animatorUpdated(BaseAnimator*) 
				{ /* virtual */ }

			virtual void animatorFinished(BaseAnimator*) 
				{ /* virtual */ }

			virtual Layout* owner()
				{ return parent() ? parent()->owner() : 0; }

			void startUpdates();
			TimerPool& timerPool();
			const TimerPool& timerPool() const;

			void layoutChildren();

		private:
			friend class Hierarchy<Element>;

			Element(const Element&) : 
				ElementHierarchy(0) { }

			Element& operator = (const Element&)
				{ return *this; }

			void startUpdates(TimerPoolObject* timerPool);

		private:
			bool _enabled;
			bool _transformValid;
			bool _inverseTransformValid;
			bool _contentValid;
		};

		inline bool elementIsSelected(ElementState s)
			{ return (s >= ElementState_Selected) && (s < ElementState_max); }

		ElementState adjustElementState(ElementState s);
		float alignmentFactor(ElementAlignment a);
	}
}
