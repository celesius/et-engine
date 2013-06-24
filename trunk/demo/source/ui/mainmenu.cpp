//
//  MainMenu.cpp
//  ios
//
//  Created by Sergey Reznik on 07.02.13.
//  Copyright (c) 2013 Sergey Reznik. All rights reserved.
//

#include "mainmenu.h"

using namespace et;
using namespace demo;

MainMenuLayout::MainMenuLayout(et::RenderContext*, ResourceManager& resourceManager)
{
	_title = resourceManager.label("Hello World!", this);
	_title->setPivotPoint(vec2(0.5f));
}

void MainMenuLayout::layout(const et::vec2& sz)
{
	_title->setPosition(0.5f * sz);
}