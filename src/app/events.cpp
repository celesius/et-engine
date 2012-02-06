#include <et/app/events.h>

using namespace et;

/*
* EventReceiver
*/

void EventReceiver::eventConnected(Event* e)
{
	_events.push_back(e); 
}

EventReceiver::~EventReceiver() 
{
	for (EventList::iterator i = _events.begin(), e = _events.end(); i != e; ++i)
		(*i)->receiverDisconnected(this);
}

void EventReceiver::eventDisconnected(Event* e)
{
	EventList::iterator i = std::find(_events.begin(), _events.end(), e);
	if (i != _events.end())
		_events.erase(i);
}

/*
 * Event(0)
 */

Event0::Event0() : _invoking(false)
{
}

Event0::~Event0()
{
	for (ConnectionList::iterator i = _connections.begin(), e = _connections.end(); i != e; ++i)
	{
		Event0ConnectionBase* connection = (*i);

		if (connection->receiver())
		{
			if (!connection->removed())
				connection->receiver()->eventDisconnected(this);

			delete connection;
		}
	}
}

void Event0::connect(Event0& e)
{
	if (&e != this)
		_connections.push_back(&e);
}

void Event0::cleanup()
{
	ConnectionList::iterator i = remove_if(_connections.begin(), _connections.end(), shouldRemoveConnection);
	if (i != _connections.end())
		_connections.erase(i, _connections.end());
}

void Event0::invoke()
{
	cleanup();

	_invoking = true;

	ConnectionList::iterator i = _connections.begin();
	while (i != _connections.end())
	{
		(*i)->invoke();
		++i;
	}

	_invoking = false;
}

void Event0::invokeInMainRunLoop(float delay)
{
	cleanup();

	for (ConnectionList::iterator i = _connections.begin(), e = _connections.end(); i != e; ++i)
		(*i)->invokeInMainRunLoop(delay);
}

void Event0::receiverDisconnected(EventReceiver* r)
{ 
	ConnectionList::iterator i = _connections.begin();
	while (i != _connections.end())
	{
		if (r == (*i)->receiver())
		{
			if (_invoking)
			{
				(*i)->remove();
				++i;
			}
			else
			{
				delete (*i);
				i = _connections.erase(i);
			}
		}
		else 
		{
			++i;
		}
	}
}