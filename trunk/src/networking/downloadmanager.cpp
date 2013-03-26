/*
 * This file is part of `et engine`
 * Copyright 2009-2013 by Sergey Reznik
 * Please, do not modify content without approval.
 *
 */

#include <queue>
#include <libcurl/curl.h>
#include <et/threading/thread.h>
#include <et/networking/downloadmanager.h>

using namespace et;

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream)
	{ return fwrite(ptr, size, nmemb, stream); }

class DownloadThread : public Thread
{
public:
	DownloadThread(DownloadManager* owner) :
		Thread(false), _owner(owner) { }
	~DownloadThread();
	
	ThreadResult main();
	
	void pushRequest(DownloadRequest::Pointer);
	
private:
	DownloadRequest::Pointer dequeRequest();
	void processRequest(DownloadRequest::Pointer);
	
private:
	DownloadManager* _owner;
	
	CriticalSection _csModifying;
	std::queue<DownloadRequest::Pointer> _queue;
};

class et::DownloadManagerPrivate
{
public:
	DownloadManager* owner;
	DownloadThread* thread;
	
public:
	DownloadManagerPrivate(DownloadManager* o) :
		owner(o), thread(new DownloadThread(o)) { }
	
	~DownloadManagerPrivate()
	{
		thread->terminate();
		delete thread;
	}
};

/**
 *
 * DownloadManager
 *
 */

DownloadManager::DownloadManager()
{
	_private = new DownloadManagerPrivate(this);
}

DownloadManager::~DownloadManager()
{
	delete _private;
}

DownloadRequest::Pointer DownloadManager::downloadFile(const std::string& url, const std::string& destination)
{
	DownloadRequest::Pointer request(new DownloadRequest(url, destination));
	_private->thread->pushRequest(request);
	return request;
}

/**
 *
 * DownloadThread
 *
 */
ThreadResult DownloadThread::main()
{
	while (running())
	{
		DownloadRequest::Pointer req = dequeRequest();
		
		if (req.valid())
		{
			processRequest(req);
		}
		else
		{
			suspend();
		}
	}
	
	return 0;
}

DownloadThread::~DownloadThread()
{
	CriticalSectionScope lock(_csModifying);
	
	while (_queue.size())
		_queue.pop();
}

void DownloadThread::pushRequest(DownloadRequest::Pointer req)
{
	CriticalSectionScope lock(_csModifying);
	_queue.push(req);
	
	if (running())
		resume();
	else
		run();
}

DownloadRequest::Pointer DownloadThread::dequeRequest()
{
	CriticalSectionScope lock(_csModifying);
	DownloadRequest::Pointer result;
	if (_queue.size())
	{
		result = _queue.front();
		_queue.pop();
	}
	return result;
}

void DownloadThread::processRequest(DownloadRequest::Pointer request)
{
    CURL* curl = curl_easy_init();
    FILE* fp = fopen(request->destination().c_str(), "wb");
	curl_easy_setopt(curl, CURLOPT_URL, request->url().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	fclose(fp);
	
	_owner->downloadCompleted.invokeInMainRunLoop(request);
}

/**
 *
 * DownloadRequest
 *
 */
DownloadRequest::DownloadRequest(const std::string& u, const std::string& d) :
	_url(u), _destination(d) { }
