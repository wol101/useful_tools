// GAUtils.cpp - a few useful routines for the GA stuff

#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>
#include <string.h>

#include "GAUtils.h"

bool ReadParameter(ifstream & inp, char *m, int &i)
{
	char buffer[NAME_SIZE];

	try
	{
		inp.seekg(0);

		do
		{
			inp >> buffer;
			if (buffer[0] == 0)
				return true;
		}
		while (strcmp(m, buffer) != 0);

		inp >> i;

		return false;
	}
	catch (...)
	{
		return false;
	}
}

bool ReadParameter(ifstream & inp, char *m, double &i)
{
	char buffer[NAME_SIZE];

	try
	{
		inp.seekg(0);

		do
		{
			inp >> buffer;
			if (buffer[0] == 0)
				return true;
		}
		while (strcmp(m, buffer) != 0);

		inp >> i;

		return false;
	}
	catch (...)
	{
		return false;
	}
}

bool GetQuotedString(ifstream & in, char *string)
{
	try
	{
		if (in.getline(string, NAME_SIZE, '\042'))
		{
			if (in.getline(string, NAME_SIZE, '\042'))
			{
				if (strlen(string) > 0)
				{
					//cerr << "Got name: " << name << "\r";
					return true;
				}
			}
		}
		return false;
	}
	catch (...)
	{
		return false;
	}
}
