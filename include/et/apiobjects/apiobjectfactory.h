/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

namespace et
{
	class RenderContext;

	class APIObjectFactory
	{
	protected:
		APIObjectFactory(RenderContext* rc) : _rc(rc)
			{ }

		virtual ~APIObjectFactory()
			{ }

		RenderContext* renderContext()
			{ return _rc; }

	private:
		RenderContext* _rc;
	};
}