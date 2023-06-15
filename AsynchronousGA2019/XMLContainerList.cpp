/*
 *  XMLContainerList.cpp
 *  MergeXML
 *
 *  Created by Bill Sellers on Mon Sep 19 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

#include <string.h>

#include "XMLContainer.h"
#include "XMLContainerList.h"

// initialise list
XMLContainerList::XMLContainerList()
{
}

// clear list
// note, list allocation is left unchanged
void XMLContainerList::Clear()
{
    mListObjects.clear();
}

// add an object
void XMLContainerList::Add(std::unique_ptr<XMLContainer> object)
{
    mListObjects.push_back(std::move(object));
}

// retrieve an object by number
XMLContainer *XMLContainerList::Get(int index)
{
   if (index < 0 || index >= int(mListObjects.size())) return nullptr;

   return mListObjects[size_t(index)].get();
}

// retrieve an object by name
XMLContainer *XMLContainerList::Get(const std::string &name)
{
   int index = Find(name);
   return Get(index);
}

// Find an object by name
// returns -1 if object name not found
int XMLContainerList::Find(const std::string &name)
{
   for (size_t i = 0; i < mListObjects.size(); i++)
   {
       if (mListObjects[i]->GetName() == name) return int(i);
   }
   return -1;
}


