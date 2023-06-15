// program to read in the kinetics file produced by Objective and
// output a simple VRML animation

// would like to have used the non .h versions of the iostream include files but these throw
// up all sorts of errors - I suspect they just don't work properly on SGIs

#include <iostream.h>
#include <fstream.h> 
#include <string>
#include <vector>
#include <string.h>

#include "EulerAngles.h"  

// my classes
class Matrix3x3
{
public:
	double			m[3][3];
};
		
class Segment
{
public:
	std::string			name;
	std::vector<double>		x;
	std::vector<double>		y;
	std::vector<double>		z;
	std::vector<Matrix3x3>		r;
};

// function prototypes
bool ReadKineticsFile(istream &in, std::vector<Segment> &listOfSegments, std::vector<double> &theTimes);
bool WriteVRMLFile(ostream &out, std::vector<Segment> &listOfSegments, std::vector<double> &theTimes, istream &header);
void CalculateRotations(double r[3][3], double &x, double &y, double &z);

int main(int argc, char ** argv)
{
	std::vector<Segment>	listOfSegments;
	std::vector<double> listOfTimes;
	int i;
	ifstream inputFile;
	ifstream headerFile;
	ofstream outputFile;
	
	// do some simple stuff with command line arguments
	
	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--help") == 0)
		{
			// std::cerr << "Usage: kinetics_to_vrml -i input -o output -h header\n";
			exit(1);
		}
		
		if (strcmp(argv[i], "-i") == 0)
		{
			i++;
			if (i >= argc) exit(1);
			inputFile.open(argv[i]);
		}
		
		if (strcmp(argv[i], "-h") == 0)
		{
			i++;
			if (i >= argc) exit(1);
			headerFile.open(argv[i]);
		}
		
		if (strcmp(argv[i], "-o") == 0)
		{
			i++;
			if (i >= argc) exit(1);
			outputFile.open(argv[i]);
		}
	}
	
	if (ReadKineticsFile(inputFile, listOfSegments, listOfTimes)) exit(2);
	inputFile.close();
	
	if (WriteVRMLFile(outputFile, listOfSegments, listOfTimes, headerFile)) exit(3);
	outputFile.close();
	headerFile.close();
	
	exit(0);
}

// function to read a kinematics file

bool ReadKineticsFile(istream &in, std::vector<Segment> &listOfSegments, std::vector<double> &theTimes)
{
	char *buffer;
	char *ptr;
	Segment mySegment;
	const int kBufferSize = 100000; // create a reasonably sized buffer
	double x, y, z;
	Matrix3x3 r;
	int i, j, k;
	double myTime;

	try 
	{
		buffer = new char[kBufferSize];
	}
	catch (...) 
	{
		return true;
	}
	
	try
	{
		// read the first line
		in.getline(buffer, kBufferSize - 1);

		// parse the line
		ptr = strtok(buffer, "\t\n");
		// the first label is the time so skip
		ptr = strtok(NULL, "\t\n");
		while (ptr)
		{
			mySegment.name = ptr;
			listOfSegments.push_back(mySegment);
			// skip the next 12 labels
			for (i = 0; i < 12; i++) ptr = strtok(NULL, "\t\n");
		}
		
		while (in.good())
		{
			// read the time in the first column
			in >> myTime;
			if (in.good() == false) break;
			theTimes.push_back(myTime);
			
			// loop through the segments
			for (i = 0; i < listOfSegments.size(); i++)
			{
				in >> x >> y >> z;

                		for (j = 0; j < 3; j++)
                		{
                        		for (k = 0; k < 3; k++)
                        		{
                                		in >> r.m[j][k];
                        		}
                		}
				listOfSegments[i].x.push_back(x);
				listOfSegments[i].y.push_back(y);
				listOfSegments[i].z.push_back(z);
				listOfSegments[i].r.push_back(r);
			}
		}
				
		delete [] buffer;	
		return false;
	}
	catch (...)
	{
		delete [] buffer;
		return true;
	}
}

// function to produce a basic VRML animation file

bool WriteVRMLFile(ostream &out, std::vector<Segment> &listOfSegments, std::vector<double> &theTimes, istream &header)
{
	int i, j;
	double x, y, z;
	char lineBuffer[256]; // small line buffer
	
	try
	{
		if (header.good())
		{
			while (header.good())
			{
				header.getline(lineBuffer, sizeof(lineBuffer) - 1);
				out << lineBuffer << "\n";
			}
		}

		// produce the clock
		out << "DEF Clock TimeSensor {\n";
		out << "\tstartTime " << theTimes[1] << "\n";
		out << "\tstopTime 0 \n";
		out << "\tloop TRUE\n"; 
		out << "\tcycleInterval " << theTimes[theTimes.size() - 1] << "\n";
		out << "}\n\n";

                // loop through the links adding extra rotation steps               
                for (i = 0; i < listOfSegments.size(); i++)
                {
                        out << "DEF " << listOfSegments[i].name << "_T Transform {\n";
                        out << "\ttranslation 0.0 0.0 0.0\n";
                        out << "\tchildren DEF " << listOfSegments[i].name << "_Z Transform {\n";
                        out << "\t\trotation 0.0 0.0 1.0 0.0\n";
                        out << "\t\tchildren DEF " << listOfSegments[i].name << "_Y Transform {\n";
                        out << "\t\t\trotation 0.0 1.0 0.0 0.0\n";
                        out << "\t\t\tchildren DEF " << listOfSegments[i].name << "_X Transform {\n";
                        out << "\t\t\t\trotation 1.0 0.0 0.0 0.0\n";
                        out << "\t\t\t\tchildren [ \n";
			out << "\t\t\t\t\t" << listOfSegments[i].name << " {} \n";
			out << "\t\t\t\t]\n";
                        out << "\t\t\t}\n";
                        out << "\t\t}\n";
                        out << "\t}\n";
                        out << "}\n\n";
                }
		// produce the position interpolators

		for (i = 0; i < listOfSegments.size(); i++)
		{
			out << "DEF " << listOfSegments[i].name << "_position PositionInterpolator {\n";
			out << "\t key [\n";
			for (j = 0; j < theTimes.size(); j++) 
			{
				if (j == 0) out << "\t\t";
				out << (double) j / theTimes.size();
				if (j != theTimes.size() - 1) 
				{
					out << ", ";
					if (j % 20 == 19) out << "\n\t\t";
				}
				else out << "\n";
			}
			out << "\t]\n";
			out << "\t keyValue [\n";
			for (j = 0; j < theTimes.size(); j++)
			{
				if (j == 0) out << "\t\t";
				out << listOfSegments[i].x[j] << " " 
						<< listOfSegments[i].y[j] << " " 
						<< listOfSegments[i].z[j];
				if (j != theTimes.size() - 1) 
				{
					out << ", ";
					if (j % 10 == 9) out << "\n\t\t";
				}
				else out << "\n";
			}				
			out << "\t]\n";
			out << "}\n\n";
		}
		
		// produce the various rotation interpolators
		
		// X first
		for (i = 0; i < listOfSegments.size(); i++)
		{
			out << "DEF " << listOfSegments[i].name << "_rot_X OrientationInterpolator {\n";
			out << "\t key [\n";
			for (j = 0; j < theTimes.size(); j++) 
			{
				if (j == 0) out << "\t\t";
				out << (double) j / theTimes.size();
				if (j != theTimes.size() - 1) 
				{
					out << ", ";
					if (j % 20 == 19) out << "\n\t\t";
				}
				else out << "\n";
				
			}
			out << "\t]\n";
			out << "\t keyValue [\n";
			for (j = 0; j < theTimes.size(); j++)
			{
				CalculateRotations(listOfSegments[i].r[j].m, x, y, z);
				if (j == 0) out << "\t\t";
				out << "1.0 0.0 0.0 " << x;
				if (j != theTimes.size() - 1) 
				{
					out << ", ";
					if (j % 10 == 9) out << "\n\t\t";
				}
				else out << "\n";
			}				
			out << "\t]\n";
			out << "}\n\n";
		}

		// Then Y
		for (i = 0; i < listOfSegments.size(); i++)
		{
			out << "DEF " << listOfSegments[i].name << "_rot_Y OrientationInterpolator {\n";
			out << "\t key [\n";
			for (j = 0; j < theTimes.size(); j++) 
			{
				if (j == 0) out << "\t\t";
				out << (double) j / theTimes.size();
				if (j != theTimes.size() - 1) 
				{
					out << ", ";
					if (j % 20 == 19) out << "\n\t\t";
				}
				else out << "\n";
				
			}
			out << "\t]\n";
			out << "\t keyValue [\n";
			for (j = 0; j < theTimes.size(); j++)
			{
				CalculateRotations(listOfSegments[i].r[j].m, x, y, z);
				if (j == 0) out << "\t\t";
				out << "0.0 1.0 0.0 " << y;
				if (j != theTimes.size() - 1) 
				{
					out << ", ";
					if (j % 10 == 9) out << "\n\t\t";
				}
				else out << "\n";
			}				
			out << "\t]\n";
			out << "}\n\n";
		}

		// finally Z
		for (i = 0; i < listOfSegments.size(); i++)
		{
			out << "DEF " << listOfSegments[i].name << "_rot_Z OrientationInterpolator {\n";
			out << "\t key [\n";
			for (j = 0; j < theTimes.size(); j++) 
			{
				if (j == 0) out << "\t\t";
				out << (double) j / theTimes.size();
				if (j != theTimes.size() - 1) 
				{
					out << ", ";
					if (j % 20 == 19) out << "\n\t\t";
				}
				else out << "\n";
				
			}
			out << "\t]\n";
			out << "\t keyValue [\n";
			for (j = 0; j < theTimes.size(); j++)
			{
				CalculateRotations(listOfSegments[i].r[j].m, x, y, z);
				if (j == 0) out << "\t\t";
				out << "0.0 0.0 1.0 " << z;
				if (j != theTimes.size() - 1) 
				{
					out << ", ";
					if (j % 10 == 9) out << "\n\t\t";
				}
				else out << "\n";
			}				
			out << "\t]\n";
			out << "}\n\n";
		}

		// link the clock to the animators
		for (i = 0; i < listOfSegments.size(); i++)
		{
			out << "ROUTE Clock.fraction_changed TO " << 
					listOfSegments[i].name << 
					"_position.set_fraction\n";
			out << "ROUTE " << listOfSegments[i].name << 
					"_position.value_changed TO " <<
					listOfSegments[i].name <<
					"_T.set_translation\n";
			out << "ROUTE Clock.fraction_changed TO " << 
					listOfSegments[i].name << 
					"_rot_X.set_fraction\n";
			out << "ROUTE " << listOfSegments[i].name << 
					"_rot_X.value_changed TO " <<
					listOfSegments[i].name <<
					"_X.set_rotation\n";
			out << "ROUTE Clock.fraction_changed TO " << 
					listOfSegments[i].name << 
					"_rot_Y.set_fraction\n";
			out << "ROUTE " << listOfSegments[i].name << 
					"_rot_Y.value_changed TO " <<
					listOfSegments[i].name <<
					"_Y.set_rotation\n";
			out << "ROUTE Clock.fraction_changed TO " << 
					listOfSegments[i].name << 
					"_rot_Z.set_fraction\n";
			out << "ROUTE " << listOfSegments[i].name << 
					"_rot_Z.value_changed TO " <<
					listOfSegments[i].name <<
					"_Z.set_rotation\n\n";
		}

		return false;
	}
	catch (...)
	{
		return true;
	}
}

// function to decompose the overall rotation matrix into 3 rotations
// about separate axes (X first, then Y and finally Z)
// this has to be done numerically because there won't be an exact answer
// due to rounding errors

void CalculateRotations(double r[3][3], double &x, double &y, double &z)
{
	int i, j;
	HMatrix myHMatrix = {1, 0, 0, 0, 
			     0, 1, 0, 0,
			     0, 0, 1, 0,
			     0, 0, 0, 1};
	EulerAngles myEulerAngles;
	
	// set the values in the Homegeneous Matrix
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			myHMatrix[i][j] = r[i][j];
		}
	}
	
	// Calculate the Euler angles
	myEulerAngles = Eul_FromHMatrix(myHMatrix, EulOrdXYZs);
	
	x = myEulerAngles.x;
	y = myEulerAngles.y;
	z = myEulerAngles.z;
}	
