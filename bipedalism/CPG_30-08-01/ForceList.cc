// ForceList object - maintains an independent list of drawable forces

// Note, this object deletes the forces when it is destroyed. This
// may not be correct!

#include <vector>

#include <ExtendedForce.h>

#include "ForceList.h"

ForceList::ForceList()
{
	forceList = new std::vector < ExtendedForce * >;
}

ForceList::~ForceList()
{
// I shouldn't delete forces here
//	unsigned int i;
//	unsigned int theSize = forceList->size();

//	for (i = 0; i < theSize; i++)
//		delete(*forceList)[i];
	delete forceList;
}

void ForceList::AddForce(ExtendedForce * force)
{
	forceList->push_back(force);
}

void ForceList::draw(void)
{
	unsigned int i;
	unsigned int theSize = forceList->size();

	for (i = 0; i < theSize; i++)
		(*forceList)[i]->draw();

}

ExtendedForce *ForceList::FindByName(const char * const name)
{
	unsigned int i;
	unsigned int theSize = forceList->size();

	for (i = 0; i < theSize; i++)
		if (strcmp((*forceList)[i]->getName(), name) == 0)
			return (*forceList)[i];

	return 0;
}
