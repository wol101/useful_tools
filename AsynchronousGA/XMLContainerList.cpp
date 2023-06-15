/*
 *  XMLContainerList.cpp
 *  MergeXML
 *
 *  Created by Bill Sellers on Mon Sep 19 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */
 
#include <string.h>

#include "XMLContainerList.h"
#include "XMLContainer.h"

namespace AsynchronousGA
{

extern int gMergeXMLVerboseLevel;

// initialise list
XMLContainerList::XMLContainerList()
{
   mNumObjects = 0;
   mAllocationIncrement = 1000;
   mCurrentAllocation = mAllocationIncrement;
   mListObjects = new XMLContainer *[mCurrentAllocation];
}

// delete list
XMLContainerList::~XMLContainerList()
{
   for (int i = 0; i < mNumObjects; i++)
      delete mListObjects[i];
   delete [] mListObjects;
}

// clear list
// note, list allocation is left unchanged
void XMLContainerList::Clear()
{
   for (int i = 0; i < mNumObjects; i++)
      delete mListObjects[i];
   mNumObjects = 0;
}
   
// add an object
void XMLContainerList::Add(XMLContainer *object)
{
   int i;
   XMLContainer **oldList;
   
   if (mNumObjects == mCurrentAllocation)
   {
      mCurrentAllocation += mAllocationIncrement;
      oldList = mListObjects;
      mListObjects = new XMLContainer *[mCurrentAllocation];
      for (i = 0; i < mNumObjects; i++)
         mListObjects[i] = oldList[i];
      delete [] oldList;
   }
   mListObjects[mNumObjects] = object;
   mNumObjects++;
}

// retrieve an object by number
XMLContainer *XMLContainerList::Get(int index)
{
   if (index < 0 || index >= mNumObjects) return 0;
   
   return mListObjects[index];
}

// retrieve an object by name
XMLContainer *XMLContainerList::Get(const char *name)
{
   int index = Find(name);
   return Get(index);
}

// Find an object by name
// returns -1 if object name not found
int XMLContainerList::Find(const char *name)
{
   int i;
   for (i = 0; i < mNumObjects; i++)
   {
      if (strcmp(name, mListObjects[i]->GetName()) == 0) return i;
   }
   
   return -1;
}

}

