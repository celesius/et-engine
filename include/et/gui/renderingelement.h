/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#pragma once

#include <et/core/intrusiveptr.h>
#include <et/device/rendercontext.h>
#include <et/apiobjects/vertexbuffer.h>
#include <et/gui/guibase.h>

namespace et
{
	namespace gui
	{
		class RenderingElement : public Shared
		{
		public:
			RenderingElement(RenderContext* rc, const std::string& name);
			void clear();

			const VertexArrayObject& vertexArrayObject();

		private:
			friend class GuiRenderer;

		private:
			RenderState& _rs;
			RenderChunkList _chunks;
			IndexArrayRef _indexArray;
			GuiVertexList _vertexList;

			VertexBuffer _vertexBuffer;
			IndexBuffer _indexBuffer;
			VertexArrayObject _vao;

			bool _changed;
		};

		typedef IntrusivePtr<RenderingElement> RenderingElementRef;
	}
}
