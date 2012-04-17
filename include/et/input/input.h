#pragma once

#include <et/core/tools.h>
#include <et/core/singleton.h>
#include <et/app/events.h>

namespace et
{

#define MAX_KEYS    256

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

	enum PointerType
	{
		PointerType_None,

		PointerType_General,
		PointerType_RightButton,
		PointerType_MiddleButton,

		PointerType_max
	};

	struct PointerInputInfo
	{
		size_t id;
		float timestamp;
		vec2 pos;
		vec2 normalizedPos;
		short scroll;
		PointerType type;
		char tag;

		PointerInputInfo() :
			id(0), timestamp(0), scroll(0), type(PointerType_None), tag(0) { }
		
		PointerInputInfo(const PointerInputInfo& p) : 
			id(p.id), timestamp(p.timestamp), pos(p.pos), normalizedPos(p.normalizedPos), scroll(p.scroll), 
			type(p.type), tag(p.tag) { }

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
		
		PointerInputInfo(PointerInputInfo&& r) : id(r.id), timestamp(r.timestamp), pos(r.pos),
			normalizedPos(r.normalizedPos), scroll(r.scroll), type(r.type), tag(r.tag) { }
		
		PointerInputInfo(PointerType t, const vec2& p, const vec2& np, short aScroll, long aId, float time) : 
			id(aId), timestamp(time), pos(p), normalizedPos(np), scroll(aScroll), type(t), tag(0) { }

	};

	class Input : public Singleton<Input>
	{
	public:
		class KeyboardInputSource;
		class PointerInputSource;

		bool isKeyPressed(short key) const
			{ return _keys[key]; }

		bool isPointerPressed(PointerType type) const;

		bool canGetCurrentPointerInfo() const;
		PointerInputInfo currentPointer() const;

	public:
		ET_DECLARE_EVENT1(keyPressed, unsigned char)  
		ET_DECLARE_EVENT1(charEntered, unsigned char)
		ET_DECLARE_EVENT1(keyReleased, unsigned char)  

		ET_DECLARE_EVENT1(pointerPressed, PointerInputInfo)
		ET_DECLARE_EVENT1(pointerMoved, PointerInputInfo)
		ET_DECLARE_EVENT1(pointerReleased, PointerInputInfo)
		ET_DECLARE_EVENT1(pointerCancelled, PointerInputInfo)
		ET_DECLARE_EVENT1(pointerScrolled, PointerInputInfo)

	private:
		Input();
		ET_SINGLETON_COPY_DENY(Input)

		friend class KeyboardInputSource;
		friend class PointerInputSource;

		void pushKeyboardInputAction(unsigned char key, InputAction action);
		void pushPointerInputAction(const PointerInputInfo& info, InputAction action);

		void addPointerInfo(const PointerInputInfo& info);
		void updatePointerInfo(const PointerInputInfo& info);
		void removePointerInfo(const PointerInputInfo& info);

	private:
		typedef std::vector<PointerInputInfo> PointerInputInfoList;

		bool _keys[MAX_KEYS];
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
		virtual void pointerPressed(const PointerInputInfo& info)
			{ Input::instance().pushPointerInputAction(info, InputAction_PointerPressed); }

		virtual void pointerMoved(const PointerInputInfo& info)
			{ Input::instance().pushPointerInputAction(info, InputAction_PointerMoved); }

		virtual void pointerReleased(const PointerInputInfo& info)
			{ Input::instance().pushPointerInputAction(info, InputAction_PointerReleased); }

		virtual void pointerCancelled(const PointerInputInfo& info)
            { Input::instance().pushPointerInputAction(info, InputAction_PointerCancelled); }
        
		virtual void pointerScrolled(const PointerInputInfo& info)
			{ Input::instance().pushPointerInputAction(info, InputAction_PointerScrolled); }
	};

	class InputHandler : virtual public EventReceiver
	{
	protected:
		InputHandler();
		virtual ~InputHandler();

		void connectInputEvents();

	protected:
		virtual void onPointerPressed(et::PointerInputInfo) { }
		virtual void onPointerMoved(et::PointerInputInfo) { }
		virtual void onPointerReleased(et::PointerInputInfo) { }
		virtual void onPointerCancelled(et::PointerInputInfo) { }
		virtual void onPointerScrolled(et::PointerInputInfo) { }
		virtual void onKeyPressed(unsigned char) { }
		virtual void onCharEnterer(unsigned char) { }
		virtual void onKeyReleased(unsigned char) { }
	};

	inline Input& input() { return Input::instance(); }
}
