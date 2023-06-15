// Listener class

#include <assert.h>

#include "Listener.h"
#include "Broadcaster.h"

Listener::Listener()
{
}

Listener::~Listener()
{
	unsigned int i = mBroadcasterList.size() - 1;
	while (i >= 0)
	{
		mBroadcasterList[i]->RemoveListener(this);
		i = mBroadcasterList.size() - 1;
	}
}


void Listener::NotifyAddedToBroadCaster(Broadcaster *inBroadcaster)
{
	mBroadcasterList.push_back(inBroadcaster);
}

void Listener::NotifyRemovedFromBroadCaster(Broadcaster *inBroadcaster)
{
	for (unsigned int i = 0; i < mBroadcasterList.size(); i++)
	{
		if (mBroadcasterList[i] == inBroadcaster)
		{
			mBroadcasterList.erase(mBroadcasterList.begin() + i);
			return;
		}
	}
	assert(false); // should never get here
}

