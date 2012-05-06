/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/singleton.h>
#include <et/app/appevironment.h>
#include <et/app/events.h>
#include <et/app/runloop.h>
#include <et/rendering/rendercontext.h>

namespace et
{
	class IApplicationDelegate : virtual public EventReceiver
	{
	public:
		virtual ~IApplicationDelegate() { }

		virtual void setRenderContextParameters(et::RenderContextParameters&) { }
		virtual void applicationDidLoad(et::RenderContext*) { }
		virtual void applicationWillTerminate() { }

		virtual void applicationWillResizeContext(const et::vec2i&) { }

		virtual void applicationWillActivate() { }
		virtual void applicationWillDeactivate() { }

		virtual void render(et::RenderContext*) { }
		virtual void idle(float) { }
	};

	class ApplicationNotifier;
	class Application : public Singleton<Application>,  public EventReceiver
	{
	public:
		enum AlertType
		{
			AlertType_Information,
			AlertType_Warning,
			AlertType_Error
		};

	public: 
		int run(int argc, char* argv[]);
		void quit(int exitCode = 0);

		IApplicationDelegate* delegate();
		static IApplicationDelegate* initApplicationDelegate();

		RunLoop& runLoop()
			{ return _runLoop; }

		void alert(const std::string& title, const std::string& message, AlertType type = AlertType_Information);

		void setFPSLimit(size_t value);
		void setCPULoadLimit(bool enable) 
			{ _cpuLimit = enable; }

		size_t renderingContextHandle() const
			{ return _renderingContextHandle; }

		AppEnvironment& environment()
			{ return _env; }

		const std::string& launchParameter(size_t i) const
			{ return (i >= _parameters.size()) ? _emptyParamter : _parameters.at(i); }

		size_t launchParamtersCount() const
			{ return _parameters.size(); }

	protected:
		friend class RenderContext;

		inline RenderContext* renderContext() 
			{ return _renderContext; }

		void performRendering();
		void setActive(bool active);
		void contextResized(const vec2i& size);

	private:
		friend class ApplicationNotifier;
		
		Application();
		~Application();

		ET_SINGLETON_COPY_DENY(Application)

		void enterRunLoop();
		void idle();
		void updateTimers(float dt);
		
		void loaded();

	private:
		static IApplicationDelegate* _delegate;
		RenderContext* _renderContext;
		AppEnvironment _env;
		RunLoop _runLoop;

		std::string _emptyParamter;
		StringList _parameters;

		int _exitCode;
		size_t _renderingContextHandle;
		float _lastQueuedTime;
		float _fpsLimit;

		volatile bool _running;
		volatile bool _active;

		bool _fpsLimitEnabled;
		bool _cpuLimit;
	};

	inline Application& application()
		{ return Application::instance(); }

	inline RunLoop& mainRunLoop()
		{ return Application::instance().runLoop(); }

	inline TimerPool& mainTimerPool()
		{ return Application::instance().runLoop()->mainTimerPool(); }
}