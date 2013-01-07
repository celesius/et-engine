/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/singleton.h>
#include <et/core/tools.h>
#include <et/app/events.h>
#include <et/app/runloop.h>
#include <et/app/appevironment.h>
#include <et/app/applicationdelegate.h>
#include <et/rendering/rendercontext.h>

namespace et
{
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

		size_t renderingContextHandle() const
			{ return _renderingContextHandle; }

		AppEnvironment& environment()
			{ return _env; }

		const std::string& launchParameter(size_t i) const
			{ return (i >= _parameters.size()) ? _emptyParamter : _parameters.at(i); }

		size_t launchParamtersCount() const
			{ return _parameters.size(); }

		const ApplicationIdentifier& identifier() const 
			{ return _identifier; }

		bool running() const 
			{ return _running; }

		bool active() const 
			{ return _active; }

		size_t memoryUsage() const;
		float cpuLoad() const;

		void setTitle(const std::string& s);
		void setFrameRateLimit(size_t value);

		void alert(const std::string& title, const std::string& message, AlertType type = AlertType_Information);
		
	private:
		friend class RenderContext;

		RenderContext* renderContext() 
			{ return _renderContext; }

		void performRendering();
		void setActive(bool active);
		void contextResized(const vec2i& size);
		
		void platformInit();
		void platformFinalize();
		void platformActivate();
		void platformDeactivate();
		
		int platformRun();
		
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
		ApplicationIdentifier _identifier;
		RenderContext* _renderContext;
		AppEnvironment _env;
		RunLoop _runLoop;

		std::string _emptyParamter;
		StringList _parameters;

		int _exitCode;
		
		size_t _renderingContextHandle;
		uint64_t _lastQueuedTimeMSec;
		uint64_t _fpsLimitMSec;
		uint64_t _fpsLimitMSecFractPart;

		volatile bool _running;
		volatile bool _active;
	};

	inline Application& application()
		{ return Application::instance(); }

	inline RunLoop& mainRunLoop()
		{ return Application::instance().runLoop(); }

	inline TimerPool& mainTimerPool()
		{ return Application::instance().runLoop()->mainTimerPool(); }
}