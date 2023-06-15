/*
 *  XMLContainerList.h
 *  MergeXML
 *
 *  Created by Bill Sellers on Mon Sep 19 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef XMLContainerList_h
#define XMLContainerList_h

namespace AsynchronousGA
{

class XMLContainer;

class XMLContainerList
{
   public:
         
   XMLContainerList();
   ~XMLContainerList();
   
   void Add(XMLContainer *object);
   XMLContainer *Get(int index);
   XMLContainer *Get(const char *name);
   int Find(const char *name);
   void Clear();
   
   protected:
   
   int mNumObjects;
   int mCurrentAllocation;
   int mAllocationIncrement;
   XMLContainer **mListObjects;
};

}
#endif
