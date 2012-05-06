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

namespace et
{
	namespace gui
	{
		class GuiRenderer;

		enum ElementState
		{
			ElementState_Default,
			ElementState_Hovered,
			ElementState_Pressed,
			ElementState_Selected,
			ElementState_SelectedHovered,
			ElementState_SelectedPressed,
			ElementState_max
		};

		inline bool elementIsSelected(ElementState s)
			{ return (s >= ElementState_Selected) && (s < ElementState_max); }

		inline ElementState adjustElementState(ElementState s)
		{
			if (!input().canGetCurrentPointerInfo() && ((s == ElementState_Hovered) || (s == ElementState_SelectedHovered)))
				return static_cast<ElementState>(s - 1);
			else 
				return s;
		}

		enum ElementClass
		{
			ElementClass_2d,
			ElementClass_3d,
			ElementClass_max
		};

		enum ElementFlags
		{
			ElementFlag_RequiresKeyboard = 0x0001,
			ElementFlag_Dragable = 0x0002,
			ElementFlag_TransparentForPointer = 0x0004,
			ElementFlag_RenderTopmost = 0x0008
		};

		enum GuiRenderLayer 
		{
			GuiRenderLayer_Layer0,
			GuiRenderLayer_Layer1,
			GuiRenderLayer_max
		};

		struct ContentOffset
		{
			float left;
			float top;
			float right;
			float bottom;

			ContentOffset(float value = 0.0f) :
				left(value), top(value), right(value), bottom(value) { }

			ContentOffset(const vec2& values) :
				left(values.x), top(values.y), right(values.x), bottom(values.y) { }

			ContentOffset(float l, float t, float r, float b) :	
				left(l), top(t), right(r), bottom(b) { }

			const vec2 origin() const
				{ return vec2(left, top); }
		};

		struct ImageDescriptor
		{
			vec2 origin;
			vec2 size;
			ContentOffset contentOffset;

			ImageDescriptor() : 
				origin(0.0f), size(0.0f) { } 

			ImageDescriptor(const Texture& tex) :
				origin(0.0f), size(tex.valid() ? tex->sizeFloat() : vec2(0.0f)) { }

			ImageDescriptor(const Texture& tex, const ContentOffset& offset) : 
				origin(0.0f), size(tex.valid() ? tex->sizeFloat() : vec2(0.0f)), contentOffset(offset) { }

			ImageDescriptor(const vec2& aOrigin, const vec2& aSize, const ContentOffset& offset = ContentOffset()) : 
				origin(aOrigin), size(aSize), contentOffset(offset) { }

			inline vec2 centerPartTopLeft() const 
				{ return origin + contentOffset.origin(); }

			inline vec2 centerPartTopRight() const 
				{ return origin + vec2(size.x - contentOffset.right, contentOffset.top); }

			inline vec2 centerPartBottomLeft() const 
				{ return origin + vec2(contentOffset.left, size.y - contentOffset.bottom); }

			inline vec2 centerPartBottomRight() const 
				{ return origin + size - vec2(contentOffset.right, contentOffset.bottom); }

			inline rect rectangle() const
				{ return rect(origin, size); }
		};

		struct Image
		{
			Texture texture;
			ImageDescriptor descriptor;
			Image() { }
			Image(const Texture& t) : texture(t), descriptor(ImageDescriptor(t)) { }
			Image(const Texture& t, const ImageDescriptor& d) : texture(t), descriptor(d) { }
		};

		typedef std::vector<Image> ImageList;
		typedef std::map<std::string, Image> ImageMap;

		struct GuiVertex
		{
		public:
			GuiVertex() : 
				position(0.0f), texCoord(0.0f), color(0.0f) { }

			GuiVertex(const vec2& pos, const vec4& tc, const vec4& c = vec4(1.0f)) : 
				position(pos, 0.0f), texCoord(tc), color(c) { }

			GuiVertex(const vec3& pos, const vec4& tc, const vec4& c = vec4(1.0f)) : 
				position(pos), texCoord(tc), color(c) { }

		public:
			vec3 position;
			vec4 texCoord;
			vec4 color; 
		};

		typedef GuiVertex* GuiVertexPointer;
		typedef DataStorage<GuiVertex> GuiVertexList;

		struct RenderChunk
		{
			size_t first;
			size_t count;
			Texture layers[GuiRenderLayer_max];
			ElementClass elementClass;

			RenderChunk(size_t f, size_t cnt, const Texture& l0, const Texture& l1, ElementClass c) : 
				first(f), count(cnt), elementClass(c)
			{ 
				layers[GuiRenderLayer_Layer0] = l0;
				layers[GuiRenderLayer_Layer1] = l1;
			}
		};

		typedef std::list<RenderChunk> RenderChunkList;

		typedef Animator<vec2> Vector2Animator;
		typedef Animator<vec3> Vector3Animator;
		typedef Animator<vec4> Vector4Animator;
		typedef Animator<mat4> MatrixAnimator;
		typedef Animator<rect> RectAnimator;

		struct ElementDragInfo
		{
			vec2 currentPosition;
			vec2 initialPosition;
			vec2 normalizedPointerPosition;
			ElementDragInfo(const vec2& c, const vec2& i, const vec2& npp) : 
				currentPosition(c), initialPosition(i), normalizedPointerPosition(npp) { }
		};

		struct GuiMessage
		{
			enum Type
			{
				Type_None = 0x0000,
				Type_TextInput = 0x0001,

				Type_User = 0xFFFF
			};

			union Parameter
			{
				unsigned int uintValue;
				int intValue;
				unsigned short ushortValues[2];
				short shortValues[2];
				unsigned char ucharValues[4];
				char charValues[4];
			};

			size_t type;
			Parameter p1;
			Parameter p2;

			GuiMessage(size_t t, int param) : type(t)
			{ 
				p1.intValue = param;
				p2.intValue = 0;
			}
		};

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

			virtual bool pointerPressed(const PointerInputInfo&)
				{ return !hasFlag(ElementFlag_TransparentForPointer); }

			virtual bool pointerMoved(const PointerInputInfo&)
				{ return !hasFlag(ElementFlag_TransparentForPointer); }

			virtual bool pointerReleased(const PointerInputInfo&)
				{ return !hasFlag(ElementFlag_TransparentForPointer); }

			virtual bool pointerScrolled(const PointerInputInfo&)
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

			virtual void setFocus() { };
			virtual void resignFocus(Element*) { };

			virtual void layout(const vec2&) 
				{ layoutChildren(); }

			virtual void didAppear() { }
			virtual void didDisappear() { }
			virtual void willAppear() { }
			virtual void willDisappear() { }

			/*
			 * Required Methods
			 */
			virtual ElementClass elementClass() const = 0;
			virtual const vec2& position() const = 0;
			virtual vec2 origin() const = 0;
			virtual const vec2& size() const = 0;
			virtual BaseAnimator* setPosition(const vec2& p, float duration = 0.0f) = 0;
			virtual bool containPoint(const vec2&, const vec2&) = 0;

			/*
			 * Events
			 */
			ET_DECLARE_EVENT2(dragStarted, Element*, const ElementDragInfo&)
			ET_DECLARE_EVENT2(dragged, Element*, const ElementDragInfo&)
			ET_DECLARE_EVENT2(dragFinished, Element*, const ElementDragInfo&)

		protected:
			inline void setContentValid()
				{ _contentValid = true; }

			inline bool contentValid() const
				{ return _contentValid; }

			inline bool transformValid() const
				{ return _transformValid; }

			inline bool inverseTransformValid()
				{ return _inverseTransformValid; }

			virtual void setInvalid()
				{ if (parent()) parent()->setInvalid(); }

			inline void setTransformValid(bool v) 
				{ _transformValid = v; }

			inline void setIverseTransformValid(bool v)
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

			Element(const Element&) : ElementHierarchy(0) { }
			inline Element& operator = (const Element&) { return *this; }

			void startUpdates(TimerPoolObject* timerPool);

		private:
			bool _enabled;
			bool _transformValid;
			bool _inverseTransformValid;
			bool _contentValid;
		};

	}
}
