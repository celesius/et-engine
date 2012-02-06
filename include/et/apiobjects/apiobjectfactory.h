#pragma once

namespace et
{
	class RenderContext;

	class APIObjectFactory
	{
	protected:
		APIObjectFactory(RenderContext* rc) : _rc(rc) { }
		inline RenderContext* renderContext() { return _rc; }

	private:
		RenderContext* _rc;
	};
}