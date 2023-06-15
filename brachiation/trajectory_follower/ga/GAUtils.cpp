// GAUtils.cpp - a few useful routines for the GA stuff

#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "GAUtils.h"

extern char **environ;

// slow but sure parameter reading from stream inp
// char is the parameter name
// i is the integer value to read
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

// slow but sure parameter reading
// char is the parameter name
// i is the double value to read
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

// read the next "quoted string" in a stream
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

// put this in in case I need more control over the system() call
int my_system (char *command) 
{
	pid_t pid;
	int status;

	if (command == 0) return 1;
	
	pid = fork();
	
	if (pid == -1) return -1;
	
	if (pid == 0) // we are the child process
	{
		char *argv[4];
		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = command;
		argv[3] = 0;
		execve("/bin/sh", argv, environ);
		exit(127);
	}
	
	// only get here if we are the parent process
	do 
	{
		if (waitpid(pid, &status, 0) == -1) 
		{
			if (errno != EINTR) return -1;
		}
		else return status;
	} while(1);
}
