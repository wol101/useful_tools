// FacetedObjectList.h - maintains a list of faceted objects

#ifndef FacetedObjectList_h
#define FacetedObjectList_h

class FacetedObject;

class FacetedObjectList
{
   public:
         
   FacetedObjectList();
   ~FacetedObjectList();
   
   void Add(FacetedObject *object);
   FacetedObject *Get(int index);
   FacetedObject *Get(const char *name);
   int Delete(const char *name);
   int Find(const char *name);
   
   protected:
   
   int mNumObjects;
   int mCurrentAllocation;
   int mAllocationIncrement;
   FacetedObject **mListObjects;
};
   



#endif
