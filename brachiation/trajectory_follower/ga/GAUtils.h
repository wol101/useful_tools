// GAUtils.h - a few useful routines for the GA stuff

#ifndef __GAUTILS_H__
#define __GAUTILS_H__

#include <fstream.h>

const int NAME_SIZE = 256;

bool GetQuotedString(ifstream & in, char *string);
inline bool OutRange(double v, double l, double h);
bool ReadParameter(ifstream & inp, char *m, int &i);
bool ReadParameter(ifstream & inp, char *m, double &i);
int my_system (char *command);

inline bool OutRange(double v, double l, double h)
{
	if (v < l)
		return true;
	if (v > h)
		return true;
	return false;
}


#endif						 // __GAUTILS_H__
