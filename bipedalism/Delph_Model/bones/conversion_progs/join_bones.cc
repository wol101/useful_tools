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
	int numVertex1, numVertex2;
	int numPoly1, numPoly2;
	int i, j, k;
	std::vector<Vertex> vertexList;
	std::vector<Poly> polyList;
	Vertex myVertex;
	Poly myPoly;
	int vertexCount;
	ifstream in1(argv[1]);
	ifstream in2(argv[2]);
	
	// read file 1
	
	in1 >> numVertex1 >> numPoly1;
	
	for (i = 0; i < numVertex1; i++)
	{
		in1 >> myVertex.x >>  myVertex.y >> myVertex.z >>
			myVertex.xn >> myVertex.yn >> myVertex.zn;
		
		vertexList.push_back(myVertex);
	}
	
	for (i = 0; i < numPoly1; i++)
	{
		myPoly.vertex.clear();
		in1 >> vertexCount;
		for (j = 0; j < vertexCount; j++)
		{
			in1 >> k;
			myPoly.vertex.push_back(k);
		}
		polyList.push_back(myPoly);
	}
	in1.close();
	
	// read file 2
	
	in2 >> numVertex2 >> numPoly2;
	
	for (i = 0; i < numVertex2; i++)
	{
		in2 >> myVertex.x >>  myVertex.y >> myVertex.z >>
			myVertex.xn >> myVertex.yn >> myVertex.zn;
		
		vertexList.push_back(myVertex);
	}
	
	for (i = 0; i < numPoly2; i++)
	{
		myPoly.vertex.clear();
		in2 >> vertexCount;
		for (j = 0; j < vertexCount; j++)
		{
			in2 >> k;
			k = k + numVertex1;
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

	
	

	
	
