/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/platform-android/nativeactivity.h>
#include <et/input/input.h>
#include <et/app/application.h>
#include <et/app/applicationnotifier.h>

static android_app* _sharedApplication = nullptr;

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

/**
 *
 * Native Activity entry point, handlers and extern declaration of "main" function
 *
 */

extern int main(int, char*[]);

void handleCommand(android_app* app, int32_t cmd);
int32_t handleInput(android_app* app, AInputEvent* event);

void android_main(android_app* state)
{
	app_dummy();

	_sharedApplication = state;
    _sharedApplication->onAppCmd = handleCommand;
    _sharedApplication->onInputEvent = handleInput;

	et::application();

	main(0, 0);

	exit(0);
}

using namespace et;

static ApplicationNotifier sharedApplicationNotifier;
static Input::PointerInputSource sharedPointerInput;

#define THIS_CASE(A) case A: { LOGI("handleCommand:" #A); break; }
#define THIS_MOTION_CASE(A) case A: { LOGI("motion:" #A); break; }

void handleCommand(android_app* app, int32_t cmd)
{
    switch (cmd)
	{
		case APP_CMD_START:
		{
			LOGI("APP_CMD_START");
			break;
		};

		case APP_CMD_GAINED_FOCUS:
		case APP_CMD_RESUME:
		{
			LOGI("APP_CMD_GAINED_FOCUS or APP_CMD_RESUME, userData: %X", app->userData);
//			sharedApplicationNotifier.notifyActivated();
			break;
		}

		case APP_CMD_LOST_FOCUS:
		case APP_CMD_PAUSE:
		{
			LOGI("APP_CMD_LOST_FOCUS or APP_CMD_PAUSE");
			sharedApplicationNotifier.notifyDeactivated();
			
			break;
		}

		case APP_CMD_DESTROY:
		{
			LOGI("APP_CMD_DESTROY");
			break;
		}

		case APP_CMD_INIT_WINDOW:
		{
			LOGI("APP_CMD_INIT_WINDOW");
			sharedApplicationNotifier.notifyLoaded();
			break;
		}

		THIS_CASE(APP_CMD_INPUT_CHANGED)
		THIS_CASE(APP_CMD_TERM_WINDOW)
		THIS_CASE(APP_CMD_WINDOW_RESIZED)
		THIS_CASE(APP_CMD_WINDOW_REDRAW_NEEDED)
        THIS_CASE(APP_CMD_CONTENT_RECT_CHANGED)
        THIS_CASE(APP_CMD_CONFIG_CHANGED)
        THIS_CASE(APP_CMD_LOW_MEMORY)
        THIS_CASE(APP_CMD_SAVE_STATE)
		THIS_CASE(APP_CMD_STOP)
				  
		default:
			LOGI("WARNING!!! handleCommand: ANOTHER COMMAND (%d)", cmd);
    }
}

int32_t handleInput(android_app* app, AInputEvent* event)
{
	int32_t eventType = AInputEvent_getType(event);

    if (eventType == AINPUT_EVENT_TYPE_KEY)
	{
		int32_t action = AKeyEvent_getAction(event);
		int32_t keyCode = AKeyEvent_getKeyCode(event);

		if (action == AKEY_EVENT_ACTION_DOWN)
		{
			LOGI("AKEY_EVENT_ACTION_DOWN: %d", keyCode);
		}
		else if (action == AKEY_EVENT_ACTION_UP)
		{
			LOGI("AKEY_EVENT_ACTION_UP: %d", keyCode);
			if (keyCode == 4)
				application().quit(0);
		}
		else if (action == AKEY_EVENT_ACTION_MULTIPLE)
		{
			LOGI("AKEY_EVENT_ACTION_MULTIPLE");
		}
		else
		{
			LOGI("WARNING!!! AINPUT_EVENT_TYPE_KEY, action: %d", action);
		}
    }
	else if (eventType == AINPUT_EVENT_TYPE_MOTION)
	{
		int32_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
		int32_t numPointers = AMotionEvent_getPointerCount(event);

		switch (action)
		{
			case AMOTION_EVENT_ACTION_MOVE:
				return 1;

			THIS_MOTION_CASE(AMOTION_EVENT_ACTION_DOWN)
			THIS_MOTION_CASE(AMOTION_EVENT_ACTION_UP)
			THIS_MOTION_CASE(AMOTION_EVENT_ACTION_CANCEL)
			THIS_MOTION_CASE(AMOTION_EVENT_ACTION_OUTSIDE)
			THIS_MOTION_CASE(AMOTION_EVENT_ACTION_POINTER_DOWN)
			THIS_MOTION_CASE(AMOTION_EVENT_ACTION_POINTER_UP)
				
			default:
				LOGI("WARNING!!! AINPUT_EVENT_TYPE_MOTION, action: %d", action);
				break;
		}

		for (int32_t i = 0; i < numPointers; ++i)
		{
			int32_t pid = AMotionEvent_getPointerId(event, i);
			float x = AMotionEvent_getX(event, i);
			float y = AMotionEvent_getY(event, i);
			LOGI("{ %d : %f, %f}", pid, x, y);
		}
    }
	
    return 1;
}

void processEvents()
{
	int ident = 0;
	do
	{
		int events = 0;

		android_poll_source* source = nullptr;
		ident = ALooper_pollAll(0, nullptr, &events, (void**)(&source));

		if (ident >= 0)
			source->process(_sharedApplication, source);
	}
	while (ident >= 0);
}

void Application::loaded()
{
	LOGI("Application::loaded()");

	_lastQueuedTimeMSec = queryTimeMSec();
	_runLoop.update(_lastQueuedTimeMSec);
	
	RenderContextParameters parameters;
	delegate()->setRenderContextParameters(parameters);
	
	_renderContext = new RenderContext(parameters, this);
	_renderContext->init();

	delegate()->applicationDidLoad(_renderContext);
}

void Application::enterRunLoop()
{
	LOGI("Application::enterRunLoop()");
	assert(_sharedApplication != nullptr);

	_active = false;
	_running = false;

	while (_sharedApplication->destroyRequested == 0)
	{
		processEvents();

		if (_active)
			idle();
    }
}

void Application::quit(int exitCode)
{
	LOGI("Application::quit()");
	
	ANativeActivity_finish(_sharedApplication->activity);
	_running = false;
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
	_renderingContextHandle = reinterpret_cast<size_t>(_sharedApplication);
	
	LOGI("Application::platformRun()");
	enterRunLoop();
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
	
	if (!_running)
		processEvents();
}

void Application::platformDeactivate()
{
	LOGI("Application::platformDeactivate()");
}
