#include <iostream.h>
#include <fstream.h>
#include <vector>

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
	
	cin >> numVertex >> numPoly;
	
	for (i = 0; i < numVertex; i++)
	{
		cin >> myVertex.x >>  myVertex.y >> myVertex.z >>
			myVertex.xn >> myVertex.yn >> myVertex.zn;
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
	
	for (i = 0; i < numVertex; i++)
	{
		cout << "v " << vertexList[i].x << " " 
			<< vertexList[i].y << " "
			<< vertexList[i].z << "\n";
	}
	
	for (i = 0; i < numPoly; i++)
	{
		cout << "f ";
		for (j = 0; j < polyList[i].vertex.size(); j++)
		{
			cout << polyList[i].vertex[j] << " ";
		}
		cout << "\n";
	}
	
	return 0;
}

	
	

	
	
