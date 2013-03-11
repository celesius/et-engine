/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <map>
#include <et/core/tools.h>
#include <et/core/singleton.h>
#include <et/core/containers.h>
#include <et/geometry/geometry.h>
#include <et/app/events.h>

namespace et
{

	enum InputAction
	{
		InputAction_KeyDown,
		InputAction_KeyUp,
		InputAction_Char,
		InputAction_PointerPressed,
		InputAction_PointerMoved,
		InputAction_PointerReleased,
        InputAction_PointerCancelled,
		InputAction_PointerScrolled
	};

	enum PointerTypeMask
	{
		PointerType_None = 0x00,
		PointerType_General = 0x01,
		PointerType_RightButton = 0x02,
		PointerType_MiddleButton = 0x04,
	};

	typedef size_t PointerType;

	struct PointerInputInfo
	{
		size_t id;
		float timestamp;
		vec2 pos;
		vec2 normalizedPos;
		vec2 scroll;
		PointerType type;
		char tag;

		PointerInputInfo() :
			id(0), timestamp(0), scroll(0), type(PointerType_None), tag(0) { }
		
		PointerInputInfo(const PointerInputInfo& p) : 
			id(p.id), timestamp(p.timestamp), pos(p.pos), normalizedPos(p.normalizedPos), scroll(p.scroll), 
			type(p.type), tag(p.tag) { }

		PointerInputInfo(PointerInputInfo&& r) : id(r.id), timestamp(r.timestamp), pos(r.pos),
			normalizedPos(r.normalizedPos), scroll(r.scroll), type(r.type), tag(r.tag) { }
		
		PointerInputInfo(PointerType t, const vec2& p, const vec2& np, const vec2& aScroll,
			long aId, float time) : id(aId), timestamp(time), pos(p), normalizedPos(np),
			scroll(aScroll), type(t), tag(0) { }

		PointerInputInfo& operator = (const PointerInputInfo& p)
		{
			id = p.id;
			timestamp = p.timestamp;
			pos = p.pos;
			normalizedPos = p.normalizedPos;
			scroll = p.scroll;
			type = p.type;
			tag = p.tag;
			return *this;
		}
	};

	enum GestureTypeMask
	{
		GestureTypeMask_Zoom = 0x01,
		GestureTypeMask_Rotate = 0x02,
		GestureTypeMask_Swipe = 0x04
	};

	struct GestureInputInfo
	{
		/*
		 * x, y - swipe
		 * z - zoom
		 * w - rotation
		 */
		vec4 values;
		size_t mask;

		GestureInputInfo() :
			values(0.0f), mask(0) { }

		GestureInputInfo(size_t m, float v) : mask(m)
		{
			if ((m & GestureTypeMask_Zoom) == GestureTypeMask_Zoom)
				values.z = v;
			else if ((m & GestureTypeMask_Rotate) == GestureTypeMask_Rotate)
				values.w = v;
		}

		GestureInputInfo(size_t m, float x, float y) :
			values(x, y, 0.0f, 0.0f), mask(m) { }
	};

	class Input : public Singleton<Input>
	{
	public:
		class KeyboardInputSource;
		class PointerInputSource;

		bool isKeyPressed(size_t key) const
			{ return _pressedKeys.count(key) > 0; }
				
		bool isPointerPressed(PointerType type) const;

		bool canGetCurrentPointerInfo() const;
		PointerInputInfo currentPointer() const;
		
		void activateSoftwareKeyboard();
		void deactivateSoftwareKeyboard();

	public:
		ET_DECLARE_EVENT1(keyPressed, size_t)  
		ET_DECLARE_EVENT1(charEntered, size_t)
		ET_DECLARE_EVENT1(keyReleased, size_t)  

		ET_DECLARE_EVENT1(pointerPressed, PointerInputInfo)
		ET_DECLARE_EVENT1(pointerMoved, PointerInputInfo)
		ET_DECLARE_EVENT1(pointerReleased, PointerInputInfo)
		ET_DECLARE_EVENT1(pointerCancelled, PointerInputInfo)
		ET_DECLARE_EVENT1(pointerScrolled, PointerInputInfo)

		ET_DECLARE_EVENT1(gesturePerformed, GestureInputInfo)

	private:
		Input();
		ET_SINGLETON_COPY_DENY(Input)

		friend class KeyboardInputSource;
		friend class PointerInputSource;

		void pushKeyboardInputAction(size_t key, InputAction action);
		void pushPointerInputAction(const PointerInputInfo& info, InputAction action);
		void pushGestureInputAction(const GestureInputInfo&);

		void addPointerInfo(const PointerInputInfo& info);
		void updatePointerInfo(const PointerInputInfo& info);
		void removePointerInfo(const PointerInputInfo& info);

	private:
		typedef std::vector<PointerInputInfo> PointerInputInfoList;
		typedef std::map<size_t, size_t> KeysMap;
		
		KeysMap _pressedKeys;
		PointerInputInfoList _pointers;
	};

	class Input::KeyboardInputSource 
	{
	public:
		virtual void keyPressed(unsigned char key)
			{ Input::instance().pushKeyboardInputAction(key, InputAction_KeyDown); }

		virtual void charEntered(unsigned char key)
			{ Input::instance().pushKeyboardInputAction(key, InputAction_Char); }

		virtual void keyReleased(unsigned char key )
			{ Input::instance().pushKeyboardInputAction(key, InputAction_KeyUp); }
	};

	class Input::PointerInputSource 
	{
	public:
		void pointerPressed(const PointerInputInfo& info)
			{ Input::instance().pushPointerInputAction(info, InputAction_PointerPressed); }

		void pointerMoved(const PointerInputInfo& info)
			{ Input::instance().pushPointerInputAction(info, InputAction_PointerMoved); }

		void pointerReleased(const PointerInputInfo& info)
			{ Input::instance().pushPointerInputAction(info, InputAction_PointerReleased); }

		void pointerCancelled(const PointerInputInfo& info)
            { Input::instance().pushPointerInputAction(info, InputAction_PointerCancelled); }
        
		void pointerScrolled(const PointerInputInfo& info)
			{ Input::instance().pushPointerInputAction(info, InputAction_PointerScrolled); }

		void gesturePerformed(const GestureInputInfo& info)
			{ Input::instance().pushGestureInputAction(info); }
	};

	class InputHandler : virtual public EventReceiver
	{
	protected:
		InputHandler(bool connect = true);
		virtual ~InputHandler();

		void connectInputEvents();

	protected:
		virtual void onPointerPressed(et::PointerInputInfo) { }
		virtual void onPointerMoved(et::PointerInputInfo) { }
		virtual void onPointerReleased(et::PointerInputInfo) { }
		virtual void onPointerCancelled(et::PointerInputInfo) { }
		virtual void onPointerScrolled(et::PointerInputInfo) { }
		
		virtual void onKeyPressed(size_t) { }
		virtual void onCharEnterer(size_t) { }
		virtual void onKeyReleased(size_t) { }

		virtual void onGesturePerformed(et::GestureInputInfo) { }
	};

	inline Input& input()
		{ return Input::instance(); }

	std::ostream& operator << (std::ostream&, const PointerInputInfo&);
}
