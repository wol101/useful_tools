// Broadcaster class

#include <assert.h>

#include "Broadcaster.h"
#include "Listener.h"

Broadcaster::Broadcaster()
{
}

Broadcaster::~Broadcaster()
{
	for (unsigned int i = 0; i < mListenerList.size(); i++)
		mListenerList[i]->NotifyRemovedFromBroadCaster(this);
}


void Broadcaster::AddListener(Listener *inListener)
{
	mListenerList.push_back(inListener);
	inListener->NotifyAddedToBroadCaster(this);
}

void Broadcaster::RemoveListener(Listener *inListener)
{
	for (unsigned int i = 0; i < mListenerList.size(); i++)
	{
		if (mListenerList[i] == inListener)
		{
			mListenerList.erase(mListenerList.begin() + i);
			inListener->NotifyRemovedFromBroadCaster(this);
			return;
		}
	}
	assert(false); // should never get here
}

void Broadcaster::BroadcastMessage(int message, void *extraData)
{
	for (unsigned int i = 0; i < mListenerList.size(); i++)
	{
		mListenerList[i]->ListenToMessage(message, extraData);
	}
}
