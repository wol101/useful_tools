// some useful routines

#include <math.h>
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

// needed for environ variable
extern char **environ;

#include "Util.h"

#define SWAP(a,b) {double temp=(a);(a)=(b);(b)=temp;}

// INVERSION OF A MATRIX WITH GAUSS-JORDAN'S METHOD
// modified from Numerical Recipes (gausj.c) and fixed at 3 by 3
// returns 0 if successful

// matrix order is:

/*

( a[0][0] a[1][0] a[2][0] )
( a[0][1] a[1][1] a[2][1] )
( a[0][2] a[1][2] a[2][2] )

*/

int
 Util::Gaussj3x3(double a[3][3])
{
	int indxc[3], indxr[3], ipiv[3];
	int i, icol = 0, irow = 0, j, k, l, ll;
	double big, dum, pivinv;

	for (j = 0; j < 3; j++)
		ipiv[j] = 0;
	for (i = 0; i < 3; i++)
	{
		big = 0.0;
		for (j = 0; j < 3; j++)
			if (ipiv[j] != 1)
				for (k = 0; k < 3; k++)
				{
					if (ipiv[k] == 0)
					{
						if (fabs(a[j][k]) >= big)
						{
							big = fabs(a[j][k]);
							irow = j;
							icol = k;
						}
					} else if (ipiv[k] > 1)
						return 1;
				}
		++(ipiv[icol]);
		if (irow != icol)
		{
			for (l = 0; l < 3; l++)
				SWAP(a[irow][l], a[icol][l]);
		}
		indxr[i] = irow;
		indxc[i] = icol;
		if (a[icol][icol] == 0.0)
			return 1;
		pivinv = 1.0 / a[icol][icol];
		a[icol][icol] = 1.0;
		for (l = 0; l < 3; l++)
			a[icol][l] *= pivinv;
		for (ll = 0; ll < 3; ll++)
			if (ll != icol)
			{
				dum = a[ll][icol];
				a[ll][icol] = 0.0;
				for (l = 0; l < 3; l++)
					a[ll][l] -= a[icol][l] * dum;
			}
	}
	for (l = 2; l >= 0; l--)
	{
		if (indxr[l] != indxc[l])
			for (k = 0; k < 3; k++)
				SWAP(a[k][indxr[l]], a[k][indxc[l]]);
	}
	// successful
	return 0;
}

#undef SWAP

bool ReadParameter(ifstream & inp, char *m, int &i)
{
	char buffer[NAME_SIZE];

	try
	{
		inp.seekg(0);

		do
		{
			inp >> buffer;
			if (inp.eof()) return true;
		}
		while (strcmp(m, buffer) != 0);

		inp >> i;
		if (inp.eof()) return true;

		return false;
	}
	catch (...)
	{
		return true;
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
			if (inp.eof()) return true;
		}
		while (strcmp(m, buffer) != 0);

		inp >> i;
		if (inp.eof()) return true;

		return false;
	}
	catch (...)
	{
		return true;
	}
}

bool ReadParameter(ifstream & inp, char *m, char *i)
{
	char buffer[NAME_SIZE];

	try
	{
		inp.seekg(0);

		do
		{
			inp >> buffer;
			if (inp.eof()) return true;
		}
		while (strcmp(m, buffer) != 0);

		inp >> i;
		if (inp.eof()) return true;

		return false;
	}
	catch (...)
	{
		return true;
	}
}

bool GetQuotedString(ifstream & in, char *string)
{
	try
	{
		if (in.getline(string, NAME_SIZE, '\042'))
		{
			//if (in.eof()) return true;
			if (in.getline(string, NAME_SIZE, '\042'))
			{
				//if (in.eof()) return true;
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
		return true;
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
