/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#pragma once

#include <et/core/properties.h>
#include <et/core/singleton.h>
#include <et/app/events.h>

namespace et
{
	class DownloadManager;
	class DownloadManagerPrivate;
	
	class DownloadRequest : public Shared
	{
	public:
		typedef IntrusivePtr<DownloadRequest> Pointer;
		
	public:
		ET_DECLARE_PROPERTY_READONLY(std::string, url)
		ET_DECLARE_PROPERTY_READONLY(std::string, destination)
		
	private:
		friend class DownloadManager;
		DownloadRequest(const std::string&, const std::string&);
	};
	
	class DownloadManager : public et::Singleton<DownloadManager>
	{
	public:
		DownloadRequest::Pointer downloadFile(const std::string& url, const std::string& destination);
		
	public:
		ET_DECLARE_EVENT1(downloadCompleted, DownloadRequest::Pointer);
		
	private:
		DownloadManager();
		~DownloadManager();
		
		ET_SINGLETON_COPY_DENY(DownloadManager)
		
	private:
		DownloadManagerPrivate* _private;
	};
}