/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

inline bool shouldRemoveConnection(EventConnectionBase* b)
	{ return b->removed(); }

/*
* Event0Connection
*/

template <typename T>
Event0Connection<T>::Event0Connection(T* receiver, void(T::*func)()) :  
	_receiverMethod(func), _receiver(receiver)
{
}

/*
* Event0
*/

template <typename R>
void Event0::connect(R* receiver, void (R::*receiverMethod)())
{
	for (ConnectionList::iterator i = _connections.begin(), e = _connections.end(); i != e; ++i)
	{
		if ((*i)->receiver() == receiver) 
		{
			(*i)->setRemoved(false);
			return;
		}
	}

	_connections.push_back( new Event0Connection<R>(receiver, receiverMethod) );
	receiver->eventConnected(this);
}

template <typename R>
void Event0::disconnect(R* receiver)
{
	receiverDisconnected(receiver);
	receiver->eventDisconnected(this);
}

/*
* Event1Connection
*/

template <typename ReceiverType, typename ArgType>
Event1Connection<ReceiverType, ArgType>::Event1Connection(ReceiverType* receiver, void(ReceiverType::*func)(ArgType)) :  
	_receiverMethod(func), _receiver(receiver)
{
}

/*
* Event1
*/
template <typename ArgType>
Event1<ArgType>::Event1() : _invoking(false)
{
}

template <typename ArgType>
Event1<ArgType>::~Event1()
{
	for (typename ConnectionList::iterator i = _connections.begin(), e = _connections.end(); i != e; ++i)
	{
		Event1ConnectionBase<ArgType>* connection = (*i);

		if (connection->receiver())
		{
			if (!connection->removed())
				connection->receiver()->eventDisconnected(this);

			delete connection;
		}
	}
}

template <typename ArgType>
template <typename ReceiverType>
inline void Event1<ArgType>::connect(ReceiverType* receiver, void (ReceiverType::*receiverMethod)(ArgType))
{
	for (typename ConnectionList::iterator i = _connections.begin(), e = _connections.end(); i != e; ++i)
	{
		if (((*i)->receiver() == receiver))
		{
			(*i)->setRemoved(false);
			return;
		}
	}

	_connections.push_back(new Event1Connection<ReceiverType, ArgType>(receiver, receiverMethod));
	receiver->eventConnected(this);
}

template <typename ArgType>
inline void Event1<ArgType>::connect(Event1& e)
{
	if (&e != this)
		_connections.push_back(&e);
}

template <typename ArgType>
template <typename ReceiverType>
inline void Event1<ArgType>::disconnect(ReceiverType* receiver)
{
	receiverDisconnected(receiver);
	receiver->eventDisconnected(this);
}

template <typename ArgType>
inline void Event1<ArgType>::receiverDisconnected(EventReceiver* r)
{ 
	typename ConnectionList::iterator i = _connections.begin();
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

template <typename ArgType>
inline void Event1<ArgType>::cleanup()
{
	typename ConnectionList::iterator i = remove_if(_connections.begin(), _connections.end(), shouldRemoveConnection);
	if (i != _connections.end())
		_connections.erase(i, _connections.end());
}

template <typename ArgType>
inline void Event1<ArgType>::invoke(ArgType arg)
{
	cleanup();

	typename ConnectionList::iterator i = _connections.begin();
	while (i != _connections.end())
	{
		(*i)->invoke(arg);
		++i;
	}
}

template <typename ArgType>
inline void Event1<ArgType>::invokeInMainRunLoop(ArgType arg, float delay)
{
	cleanup();

	typename ConnectionList::iterator i = _connections.begin();
	while (i != _connections.end())
	{
		(*i)->invokeInMainRunLoop(arg, delay);
		++i;
	}
}

/*
* Event2Connection
*/

template <typename ReceiverType, typename Arg1Type, typename Arg2Type>
Event2Connection<ReceiverType, Arg1Type, Arg2Type>::Event2Connection(ReceiverType* receiver, void(ReceiverType::*func)(Arg1Type, Arg2Type)) :  
	_receiverMethod(func), _receiver(receiver)
{
}

/*
* Event2
*/
template <typename Arg1Type, typename Arg2Type>
Event2<Arg1Type, Arg2Type>::Event2() : _invoking(false)
{
}

template <typename Arg1Type, typename Arg2Type>
Event2<Arg1Type, Arg2Type>::~Event2()
{
	for (typename ConnectionList::iterator i = _connections.begin(), e = _connections.end(); i != e; ++i)
	{
		Event2ConnectionBase<Arg1Type, Arg2Type>* connection = (*i);

		if (connection->receiver())
		{
			if (!connection->removed())
				connection->receiver()->eventDisconnected(this);

			delete connection;
		}
	}
}

template <typename Arg1Type, typename Arg2Type>
template <typename ReceiverType>
inline void Event2<Arg1Type, Arg2Type>::connect(ReceiverType* receiver, void (ReceiverType::*receiverMethod)(Arg1Type, Arg2Type))
{
	for (typename ConnectionList::iterator i = _connections.begin(), e = _connections.end(); i != e; ++i)
	{
		if (((*i)->receiver() == receiver))
		{
			(*i)->setRemoved(false);
			return;
		}
	}

	_connections.push_back( new Event2Connection<ReceiverType, Arg1Type, Arg2Type>(receiver, receiverMethod) );
	receiver->eventConnected(this);
}

template <typename Arg1Type, typename Arg2Type>
inline void Event2<Arg1Type, Arg2Type>::connect(Event2<Arg1Type, Arg2Type>& e)
{
	if (&e != this)
		_connections.push_back(&e);
}

template <typename Arg1Type, typename Arg2Type>
template <typename ReceiverType>
inline void Event2<Arg1Type, Arg2Type>::disconnect(ReceiverType* receiver)
{
	receiverDisconnected(receiver);
	receiver->eventDisconnected(this);
}

template <typename Arg1Type, typename Arg2Type>
inline void Event2<Arg1Type, Arg2Type>::receiverDisconnected(EventReceiver* r)
{ 
	typename ConnectionList::iterator i = _connections.begin();
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

template <typename Arg1Type, typename Arg2Type>
inline void Event2<Arg1Type, Arg2Type>::invoke(Arg1Type a1, Arg2Type a2)
{
	typename ConnectionList::iterator i = remove_if(_connections.begin(), _connections.end(), shouldRemoveConnection);

	if (i != _connections.end())
		_connections.erase(i, _connections.end());

	i = _connections.begin();
	while (i != _connections.end())
	{
		(*i)->invoke(a1, a2);
		if (_connections.size() == 0) return;

		++i;
	}
}

template <typename Arg1Type, typename Arg2Type>
inline void Event2<Arg1Type, Arg2Type>::invokeInMainRunLoop(Arg1Type a1, Arg2Type a2, float delay)
{
	typename ConnectionList::iterator i = remove_if(_connections.begin(), _connections.end(), shouldRemoveConnection);
	
	if (i != _connections.end())
		_connections.erase(i, _connections.end());
	
	i = _connections.begin();
	while (i != _connections.end())
	{
		(*i)->invokeInMainRunLoop(a1, a2, delay);
		if (_connections.size() == 0) return;
		
		++i;
	}
}