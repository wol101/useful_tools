// quick and dirty conversion program that ignores colours and scale factors

#include <stdio.h>
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
	std::vector<int> vertexCountList;
	std::vector<Poly> polyList;
	Vertex myVertex;
	Poly myPoly;
	int vertexCount;
        double dummy;
        
        // skip the unused items at the beginning
        for (i = 0; i < 17; i++) cin >> dummy;
	
	cin >> numVertex >> numPoly;
	
	for (i = 0; i < numVertex; i++)
	{
		cin >> myVertex.x >>  myVertex.y >> myVertex.z ;
		vertexList.push_back(myVertex);
	}
        
	for (i = 0; i < numPoly; i++)
	{
		cin >> vertexCount;
                vertexCountList.push_back(vertexCount);
              }
	
	for (i = 0; i < numPoly; i++)
	{
		myPoly.vertex.clear();
		for (j = 0; j < vertexCountList[i]; j++)
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
                     	if (j != polyList[i].vertex.size() - 1)
				cout << polyList[i].vertex[j] + 1 << " ";
                        else
				cout << polyList[i].vertex[j] + 1;
		}
		cout << "\n";
	}
	
	return 0;
}

	
	

	
	
