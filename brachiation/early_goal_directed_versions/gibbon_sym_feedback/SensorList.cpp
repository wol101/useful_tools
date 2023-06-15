// SensorList object - maintains an independent list of sensors
#include <vector>

#include <Sensor.h>

#include "SensorList.h"

SensorList::SensorList()
{
}

SensorList::~SensorList()
{
}

void SensorList::AddSensor(Sensor * sensor)
{
	sensorList.push_back(sensor);
}

Sensor *SensorList::FindByName(char *name)
{
	unsigned int i;
	unsigned int theSize = sensorList.size();

	for (i = 0; i < theSize; i++)
		if (strcmp(sensorList[i]->getName(), name) == 0)
			return sensorList[i];

	return 0;
}
