// Broadcaster class

#pragma once

#include <vector>

class Listener;	

class Broadcaster
{

public:
	Broadcaster();
	~Broadcaster();

	virtual void AddListener(Listener *inListener);
	virtual void RemoveListener(Listener *inListener);
	
	virtual void BroadcastMessage(int message, void *extraData);

protected:
		
	std::vector<Listener *>		mListenerList;

};
	
	
	
	
	