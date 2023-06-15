// Listener class - virtual class, ListenToMessage needs to be defined

// this is currently fragile since it doesn't maintain a list of broadcasters

#pragma once

#include <vector>

class Broadcaster;	

class Listener
{

public:
	Listener();
	~Listener();
	
	virtual void NotifyAddedToBroadCaster(Broadcaster *inBroadcaster);
	virtual void NotifyRemovedFromBroadCaster(Broadcaster *inBroadcaster);
	
	virtual void ListenToMessage(int message, void *extraData) = 0;
	
protected:
	
	std::vector<Broadcaster *>	mBroadcasterList;
};

