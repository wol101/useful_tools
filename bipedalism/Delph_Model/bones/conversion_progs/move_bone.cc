#include <iostream.h>
#include <fstream.h>
#include <vector>
#include <stdio.h>
#include <string.h>

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
	double xOff = 0;
	double yOff = 0;
	double zOff = 0;
	
	sscanf(argv[1], "%lf", &xOff);
	sscanf(argv[2], "%lf", &yOff);
	sscanf(argv[3], "%lf", &zOff);		
		
	// read file
	
	cin >> numVertex >> numPoly;
	
	for (i = 0; i < numVertex; i++)
	{
		cin >> myVertex.x >>  myVertex.y >> myVertex.z >>
			myVertex.xn >> myVertex.yn >> myVertex.zn;
		
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
	
	cout << vertexList.size() << " " << polyList.size() << "\n";
	
	for (i = 0; i < vertexList.size(); i++) // dummy normals
	{
		cout << vertexList[i].x << " " 
			<< vertexList[i].y << " "
			<< vertexList[i].z << " 0 0 0\n";
	}
	
	for (i = 0; i < polyList.size(); i++)
	{
		cout << polyList[i].vertex.size() << " ";
		for (j = 0; j < polyList[i].vertex.size(); j++)
		{
			cout << polyList[i].vertex[j] << " ";
		}
		cout << "\n";
	}
	
	return 0;
}

	
	

	
	
