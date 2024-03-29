/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/core/et.h>
#include <et/core/objectscache.h>

#include <et/timers/notifytimer.h>
#include <et/app/events.h>

#include <et/rendering/rendercontextparams.h>
#include <et/rendering/renderer.h>
#include <et/rendering/renderstate.h>

#include <et/apiobjects/programfactory.h>
#include <et/apiobjects/texturefactory.h>
#include <et/apiobjects/framebufferfactory.h>
#include <et/apiobjects/vertexbufferfactory.h>

namespace et
{
	class Application;
	class ApplicationNotifier;
	struct RenderingInfo
	{
		size_t averageFramePerSecond;
		size_t averageDIPPerSecond;
		size_t averagePolygonsPerSecond;
		
		uint64_t averageFrameTimeInMicroseconds;
		
		RenderingInfo() :
			averageFramePerSecond(0), averageDIPPerSecond(0), averagePolygonsPerSecond(0),
			averageFrameTimeInMicroseconds(0) { }
	};

	class RenderContextPrivate;
	class RenderContextNotifier;
	
	class RenderContext : public EventReceiver
	{
	public:
		RenderContext(const RenderContextParameters& params, Application* app);
		~RenderContext();

		void init();
		bool valid();
		
		const RenderContextParameters& parameters() const
			{ return _params; }

		const vec2& size() const
			{ return _renderState.mainViewportSizeFloat(); }

		const vec2i& sizei() const
			{ return _renderState.mainViewportSize(); }

		size_t screenScaleFactor() const
			{ return _screenScaleFactor; }

		RenderState& renderState()
			{ return _renderState; }

		Renderer* renderer()
			{ return _renderer.ptr(); }

		ProgramFactory& programFactory()
			{ return _programFactory.reference(); }

		TextureFactory& textureFactory()
			{ return _textureFactory.reference(); }

		FramebufferFactory& framebufferFactory()
			{ return _framebufferFactory.reference(); }

		VertexBufferFactory& vertexBufferFactory()
			{ return _vertexBufferFactory.reference(); }

		size_t lastFPSValue() const
			{ return _info.averageFramePerSecond; }

		void beginRender();
		void endRender();

		size_t renderingContextHandle();

	public:
		ET_DECLARE_EVENT1(renderingInfoUpdated, const RenderingInfo&)
		ET_DECLARE_EVENT1(screenScaleFactorChanged, size_t)

	private:
		RenderContext(RenderContext*) : _private(0), _app(0), 
			_programFactory(0), _textureFactory(0), _framebufferFactory(0), _vertexBufferFactory(0),
			_screenScaleFactor(1) { }

		RenderContext* operator = (const RenderContext*) 
			{ return this; }

		void onFPSTimerExpired(NotifyTimer* t);
		void resized(const vec2i&);
		void updateScreenScale(const vec2i& screenSize);

	private:
		friend class RenderContextPrivate;
		friend class RenderContextNotifier;
		friend class ApplicationNotifier;

		RenderContextPrivate* _private;

		RenderContextParameters _params;

		Application* _app;
		NotifyTimer _fpsTimer;
		RenderingInfo _info;

		RenderState _renderState;

		AutoPtr<ProgramFactory> _programFactory;
		AutoPtr<TextureFactory> _textureFactory;
		AutoPtr<FramebufferFactory> _framebufferFactory;
		AutoPtr<VertexBufferFactory> _vertexBufferFactory;

		AutoPtr<Renderer> _renderer;
		
		size_t _screenScaleFactor;
	};

}