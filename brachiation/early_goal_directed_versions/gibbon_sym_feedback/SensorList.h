// SensorList object - maintains an independent list of drawable sensors

#pragma once

class Sensor;
#include <vector>

class SensorList 
{ 
public:

	SensorList();
	~SensorList();

	void AddSensor(Sensor * sensor);
	Sensor *FindByName(char *name);
	int GetSize(void) { return sensorList.size(); };
	Sensor *GetEntry(int i) { return sensorList[i];};

protected:

	 std::vector < Sensor * > sensorList;
};

