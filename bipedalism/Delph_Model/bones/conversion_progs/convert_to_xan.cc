#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <vector>
#include <assert.h>

void swap(double &x, double &y);

struct Vertex
{
	double x;
	double y;
	double z;
	double xn;
	double yn;
	double zn;
};

struct Poly
{
	std::vector<int> vertex;
};

int main(int argc  , char ** argv)
{
	int numVertex;
	int numPoly;
	int i, j, k;
	std::vector<Vertex> vertexList;
	std::vector<Poly> polyList;
	Vertex myVertex;
	Poly myPoly;
	int vertexCount;
	bool swapZYFlag = false;
	bool swapYXFlag = false;
	bool flipXFlag = false;
	bool flipYFlag = false;
	bool flipOrder = false;
	double xOff = 0;
	double yOff = 0;
	double zOff = 0;
	
        for (i = 1; i < argc; i++)
        {
                if (strcmp(argv[i], "--swapYX") == 0)
                        swapYXFlag = true;
                
                if (strcmp(argv[i], "--swapZY") == 0)
                        swapZYFlag = true;
                
                if (strcmp(argv[i], "--flipX") == 0)
                        flipXFlag = true;
                
                if (strcmp(argv[i], "--flipY") == 0)
                        flipYFlag = true;
                
                if (strcmp(argv[i], "--flipOrder") == 0)
                        flipOrder = true;
                
                if (strcmp(argv[i], "--offsetX") == 0)
		{
			i++;
			sscanf(argv[i], "%lf", &xOff);
		}
                 
                if (strcmp(argv[i], "--offsetY") == 0)
		{
			i++;
			sscanf(argv[i], "%lf", &yOff);
		}
                
                if (strcmp(argv[i], "--offsetZ") == 0)
		{
			i++;
			sscanf(argv[i], "%lf", &zOff);
		}
       }
	
	// read file
	
	cin >> numVertex >> numPoly;
	
	for (i = 0; i < numVertex; i++)
	{
		cin >> myVertex.x >>  myVertex.y >> myVertex.z >>
			myVertex.xn >> myVertex.yn >> myVertex.zn;

		if (swapYXFlag) // note swapping happens before flipping
			swap(myVertex.y, myVertex.x);

		if (swapZYFlag) // note swapping happens before flipping
			swap(myVertex.z, myVertex.y);

		if (flipXFlag)
			myVertex.x = -myVertex.x;

		if (flipYFlag)
			myVertex.y = -myVertex.y;
		
		myVertex.x += xOff;
		myVertex.y += yOff;
		myVertex.z += zOff;
		
		vertexList.push_back(myVertex);
	}
	
	for (i = 0; i < numPoly; i++)
	{
		myPoly.vertex.clear();
		cin >> vertexCount;
		for (j = 0; j < vertexCount; j++)
		{
			cin >> k;
			myPoly.vertex.push_back(k);
		}
		polyList.push_back(myPoly);
	}
	
	// write file
	
	// create a dummy header
	
	cout << " 0.000 0.000 0.000\n";
	cout << " 0.000 0.200 0.200\n";
	cout << " 0.000 1.000 1.000\n";
	cout << " 0.000 0.500 0.500\n";
	cout << " 10.000\n";
	cout << "  1.000\n";
	cout << "\n 1.0 1.0 1.0\n\n";
	
	// now write my data
	cout << numVertex << " " << numPoly << "\n";
	
	for (i = 0; i < numVertex; i++)
	{
		cout << vertexList[i].x << " " 
			<< vertexList[i].y << " "
			<< vertexList[i].z << "\n";
	}
	
	for (i = 0; i < numPoly; i++)
	{
		cout << polyList[i].vertex.size() << " ";
	}
	cout << "\n";
	
	for (i = 0; i < numPoly; i++)
	{
                if (flipOrder)
                {
		        for (j = polyList[i].vertex.size() - 1; j >= 0; j--)
		        {
			        k = polyList[i].vertex[j] - 1;
			        assert(k >= 0 && k < numVertex);
			        cout << k << " ";
		        }
		        cout << "\n";
                }
                else
                {
		        for (j = 0; j < polyList[i].vertex.size(); j++)
		        {
			        k = polyList[i].vertex[j] - 1;
			        assert(k >= 0 && k < numVertex);
			        cout << k << " ";
		        }
		        cout << "\n";
                }
	}
	
	return 0;
}

void swap(double &x, double &y)
{
	double t = x;
	x = y;
	y = t;
}	
	

	
	
