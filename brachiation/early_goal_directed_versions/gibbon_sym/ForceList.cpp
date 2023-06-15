// ForceList object - maintains an independent list of drawable forces
#include <vector>

#include <dmForce.hpp>

#include "ForceList.h"

ForceList::ForceList()
{
	forceList = new std::vector < dmForce * >;
}

ForceList::~ForceList()
{
	unsigned int i;
	unsigned int theSize = forceList->size();

	for (i = 0; i < theSize; i++)
		delete(*forceList)[i];
	delete forceList;
}

void ForceList::AddForce(dmForce * force)
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

dmForce *ForceList::FindByName(char *name)
{
	unsigned int i;
	unsigned int theSize = forceList->size();

	for (i = 0; i < theSize; i++)
		if (strcmp((*forceList)[i]->getName(), name) == 0)
			return (*forceList)[i];

	return 0;
}
