/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

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