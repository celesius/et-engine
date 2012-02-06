#include <et/apiobjects/textureloadingthread.h>
#include <et/resources/textureloader.h>
#include <et/app/invocation.h>

using namespace et;

TextureLoadingThread::TextureLoadingThread(TextureLoadingThreadDelegate* delegate) : Thread(false), _delegate(delegate)
{

}

TextureLoadingThread::~TextureLoadingThread()
{
	CriticalSectionScope lock(_requestsCriticalSection);

	while (_requests.size())
	{
		delete _requests.front();
		_requests.pop();
	}
}

TextureLoadingRequest* TextureLoadingThread::dequeRequest()
{
	CriticalSectionScope lock(_requestsCriticalSection);
	TextureLoadingRequest* result = 0;

	if (_requests.size()) 
	{
		result = _requests.front();
		_requests.pop();
	}
	
	return result;
}

ThreadResult TextureLoadingThread::main()
{
	while (running())
	{
		TextureLoadingRequest* req = dequeRequest();

		if (req)
		{
			*req->textureDescription = TextureLoader::load(req->fileName, req->screenScale);

			Invocation1 invocation;
			invocation.setTarget(_delegate, &TextureLoadingThreadDelegate::textureLoadingThreadDidLoadTextureData, req);
			invocation.invokeInMainRunLoop();
		}
		else
		{
			suspend();
		}
	}

	return 0;
}

void TextureLoadingThread::addRequest(const std::string& fileName, size_t scrScale, Texture texture, TextureLoaderDelegate* delegate)
{
	CriticalSectionScope lock(_requestsCriticalSection);
	_requests.push(new TextureLoadingRequest(fileName, scrScale, texture, delegate));

	if (!running())
		run();
	else
		resume();
}
