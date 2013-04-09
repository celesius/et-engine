/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/gui/element2d.h>

namespace et
{
    namespace gui
    {
        class Slider : public Element2d
        {
        public:
            typedef IntrusivePtr<Slider> Pointer;
			
		public:
            Slider(Element2d* parent);
			
            void setMinValue(float min)
                { _min = min; }
			
            float minValue()
                { return _min; }
			
            void setMaxValue(float max)
                { _max = max; }
			
            float maxValue()
                { return _max; }
			
            float value() const;
            void setValue(float value);
			
            void setFillingLineWidth(float fillingLineWidth)
                { _fillingLineWidth = fillingLineWidth; }
			
            void setImages(Image background, Image fillingLine, Image handle);

            bool capturesPointer() const
                { return true; }

            ET_DECLARE_EVENT1(valueChanged, Slider*);

        private:
            void addToRenderQueue(RenderContext*, GuiRenderer&);
            void buildVertices(RenderContext*, GuiRenderer&);

            bool pointerPressed(const PointerInputInfo&);
            bool pointerMoved(const PointerInputInfo&);
            bool pointerReleased(const PointerInputInfo&);
            bool pointerCancelled(const PointerInputInfo&);

        private:
            GuiVertexList _backgroundVertices;
            GuiVertexList _fillingLineVertices;
            GuiVertexList _handleVertices;

        private:
            Image _backgroundImage;
            Image _fillingLineImage;
            Image _handleImage;
			
            float _min;
            float _max;
            float _value;
            float _fillingLineWidth;
			
            bool _selected;
        };
    }
}
