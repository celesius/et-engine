//
//  FacebookSharing.h
//  PentaPuzzle
//
//  Created by Sergey Reznik on 7/18/12.
//
//

#pragma once

#include <et/core/debug.h>
#include <et/core/singleton.h>

namespace et
{
	class ChartBoostProxyPrivate;
	class ChartBoostProxy : public et::Singleton<ChartBoostProxy>
	{
	public:
		ChartBoostProxy();
		
		void start(const std::string& identifier, const std::string& signature);
		
		void show();
		void show(const std::string& tag);
		void showMoreGames();
		
	private:
		ChartBoostProxyPrivate* _private;
	};
}