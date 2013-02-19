/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/platform-android/nativeactivity.h>
#include <et/app/application.h>

using namespace et;

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

static android_app* _sharedApplication = nullptr;

static void handleCommand(android_app* app, int32_t cmd)
{
    switch (cmd)
	{
        case APP_CMD_INIT_WINDOW:
		{
			LOGI("handleCommand: APP_CMD_INIT_WINDOW");
			break;
		}
			
        case APP_CMD_TERM_WINDOW:
		{
			LOGI("handleCommand: APP_CMD_TERM_WINDOW");
			break;
		}
			
        case APP_CMD_LOST_FOCUS:
		{
			LOGI("handleCommand: APP_CMD_LOST_FOCUS");
			break;
		}
			
		default:
			LOGI("handleCommand: ANOTHER COMMAND (%d)", cmd);
    }
}

static int32_t handleInput(android_app* app, AInputEvent* event)
{
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		LOGI("handleInput: AINPUT_EVENT_TYPE_MOTION");
        return 1;
    }
	else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
	{
		LOGI("handleInput: AINPUT_EVENT_TYPE_KEY");
    }
	
    return 0;
}

void et::initSharedAndroidApplication(android_app* state)
{
	assert(_sharedApplication == nullptr);
	_sharedApplication = state;
    _sharedApplication->onAppCmd = handleCommand;
    _sharedApplication->onInputEvent = handleInput;
}

void Application::loaded()
{
	LOGI("Application::loaded()");
	
	_lastQueuedTimeMSec = queryTimeMSec();
	_runLoop.update(_lastQueuedTimeMSec);
	
	RenderContextParameters parameters;
	delegate()->setRenderContextParameters(parameters);
	
	_renderContext = new RenderContext(parameters, this);
			
	enterRunLoop();
}

void Application::enterRunLoop()
{
	assert(_sharedApplication != nullptr);
	
	LOGI("Application::enterRunLoop()");
	app_dummy();
	
	_active = true;
	_running = true;
	
	delegate()->applicationDidLoad(_renderContext);
	
	_renderContext->init();
	
	while (1)
	{
        int ident = 0;
		
		do
		{
			int events = 0;
			android_poll_source* source = nullptr;
			ident = ALooper_pollAll(0, nullptr, &events, (void**)&source);
			if (ident >= 0)
				source->process(_sharedApplication, source);
		}
		while (ident >= 0);
		
		idle();
    }
}

void Application::quit(int exitCode)
{
}

void Application::setTitle(const std::string &s)
{
}

void Application::alert(const std::string&, const std::string&, AlertType)
{	
}

void Application::platformInit()
{
	LOGI("Application::platformInit()");
	_env.updateDocumentsFolder(_identifier);
}

int Application::platformRun()
{
	LOGI("Application::platformRun()");
	
	loaded();
	
	return 0;
}

void Application::platformFinalize()
{
	LOGI("Application::platformFinalize()");
	delete _delegate, _delegate = nullptr;
	delete _renderContext, _renderContext = nullptr;
}

void Application::platformActivate()
{
	LOGI("Application::platformActivate()");
}

void Application::platformDeactivate()
{
	LOGI("Application::platformDeactivate()");
}
