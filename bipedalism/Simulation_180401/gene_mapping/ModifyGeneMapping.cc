#include <iostream.h>
#include <fstream.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define NAME_SIZE 256

bool GetQuotedString(ifstream & in, char *string);

int main(int argc  , char ** argv)
{
	int size;
	int offset;
	int scale;
	char name[NAME_SIZE];
	
	if (argc != 4)
	{
		cerr << "Usage:\nModifyGeneMapping scale in_file out_file\n";
		return(1);
	}
	
	sscanf(argv[1], "%d", &scale);
        
	ifstream in(argv[2]);
	ofstream out(argv[3]);
	
	while (GetQuotedString(in, name))
	{
		in >> size >> offset;
		out << "\"" << name << "\"\t" << 
			size * scale << "\t" << offset * scale << "\n";
	}
	
	return(0);
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
