/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/geometry/rectplacer.h>

using namespace et;

RectPlacer::RectPlacer(const vec2i& contextSize, bool addSpace) : _contextSize(contextSize), _addSpace(addSpace)
{

}

bool RectPlacer::place(const vec2i& size, rect& placedPosition)
{
	int w = size.x;
	int h = size.y;

	int xOffset = 0;
	int yOffset = 0;

	if (_addSpace)
	{
		if (w < _contextSize.x - 1)
		{
			w++;
			xOffset = 1;
		}

		if (h < _contextSize.y - 1)
		{
			h++;
			yOffset = 1;
		}
	}

	placedPosition = rect(vec2(0.0f), vec2(static_cast<float>(w), static_cast<float>(h)));

	if (_placedItems.size() == 0)
	{
		_placedItems.push_back(placedPosition);
		return true;
	}
	
	for (RectList::iterator i = _placedItems.begin(), e = _placedItems.end(); i != e; ++i)
	{
		placedPosition.setOrigin(i->origin() + vec2(i->size().x, 0.0f));

		bool placed = (placedPosition.origin().x + w <= _contextSize.x) && (placedPosition.origin().y + h <= _contextSize.y);
		if (placed)
		{
			for (RectList::iterator ii = _placedItems.begin(); ii != e; ++ii)
			{
				if ((ii != i) && ii->intersects(placedPosition))
				{
					placed = false;
					break;
				}
			}
		}

		if (placed)
		{
			_placedItems.push_back(placedPosition);
			return true;
		}

		placedPosition.setOrigin(i->origin() + vec2(0.0f, i->size().y));
		placed = (placedPosition.origin().x + w <= _contextSize.x) && (placedPosition.origin().y + h <= _contextSize.y);
		if (placed)
		{
			for (RectList::iterator ii = _placedItems.begin(); ii != e; ++ii)
			{
				if ((ii != i) && ii->intersects(placedPosition))
				{
					placed = false;
					break;
				}
			}
		}

		if (placed) 
		{
			_placedItems.push_back(placedPosition);
			return true;
		}
	}

	return false;
}
