// ControllerList object - maintains an independent list of controllers
#include <vector>

#include <Controller.h>

#include "ControllerList.h"

ControllerList::ControllerList()
{
	controllerList = new std::vector < Controller * >;
}

ControllerList::~ControllerList()
{
	unsigned int i;
	unsigned int theSize = controllerList->size();

	for (i = 0; i < theSize; i++)
		delete(*controllerList)[i];
	delete controllerList;
}

void ControllerList::AddController(Controller * controller)
{
	controllerList->push_back(controller);
}

Controller *ControllerList::FindByName(const char * const name)
{
	unsigned int i;
	unsigned int theSize = controllerList->size();

	for (i = 0; i < theSize; i++)
		if (strcmp((*controllerList)[i]->getName(), name) == 0)
			return (*controllerList)[i];

	return 0;
}
