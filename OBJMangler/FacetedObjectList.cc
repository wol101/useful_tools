// FacetedObjectList.cc - maintains a list of faceted objects

#include <string.h>

#include "FacetedObjectList.h"
#include "FacetedObject.h"

// initialise list
FacetedObjectList::FacetedObjectList()
{
   mNumObjects = 0;
   mAllocationIncrement = 100;
   mCurrentAllocation = mAllocationIncrement;
   mListObjects = new FacetedObject *[mCurrentAllocation];
}

// delete list
FacetedObjectList::~FacetedObjectList()
{
   for (int i = 0; i < mNumObjects; i++)
      delete mListObjects[i];
   delete [] mListObjects;
}
   
// add an object
void FacetedObjectList::Add(FacetedObject *object)
{
   int i;
   FacetedObject **oldList;
   
   if (mNumObjects == mCurrentAllocation)
   {
      mCurrentAllocation += mAllocationIncrement;
      oldList = mListObjects;
      mListObjects = new FacetedObject *[mCurrentAllocation];
      for (i = 0; i < mNumObjects; i++)
         mListObjects[i] = oldList[i];
      delete [] oldList;
   }
   mListObjects[mNumObjects] = object;
   mNumObjects++;
}

// retrieve an object by number
FacetedObject *FacetedObjectList::Get(int index)
{
   if (index < 0 || index >= mNumObjects) return 0;
   
   return mListObjects[index];
}

// retrieve an object by name
FacetedObject *FacetedObjectList::Get(const char *name)
{
    int index = Find(name);
    return Get(index);
}

// delete an object by name
// returns -1 if object name not found
int FacetedObjectList::Delete(const char *name)
{
    int i;
    int index = Find(name);
    if (index == -1) return -1;
    
    delete mListObjects[index];
    for (i = index + 1; i < mNumObjects; i++)
        mListObjects[i - 1] = mListObjects[i];
    mNumObjects--;
    return 0;
}

// Find an object by name
// returns -1 if object name not found
int FacetedObjectList::Find(const char *name)
{
   int i;
   for (i = 0; i < mNumObjects; i++)
   {
      if (strcmp(name, mListObjects[i]->GetName()) == 0) return i;
   }
   
   return -1;
}

