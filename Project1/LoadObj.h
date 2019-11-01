
#ifndef  _LOADOBJ_H_  
#define  _LOADOBJ_H_  


#include<algorithm>
#include <math.h>  
#include <iostream>  
#include <fstream>  
#include <string>  
#include <vector>  

using namespace std;

#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z*(v).z)    
#define norm2(v)   dot(v,v)        // norm2 = squared length of vector    
#define norm(v)    sqrt(norm2(v))  // norm = length of vector    
#define d(u,v)     norm(u-v)       // distance = norm of difference    

// Obj loader  
struct TriangleFace
{
	int v[3]; // vertex indices  
	float n[3];
};

class float3
{
public:
	float x;
	float y;
	float z;
public:
	float3(){ x = 0; y = 0; z = 0; }
	float3(float mx, float my, float mz){ x = mx; y = my; z = mz; }
	~float3(){}

	float3 operator+(float3);
	float3 operator-(float3);

	friend float3 operator*(float m, float3 f3)
	{
		return float3(f3.x*m, f3.y*m, f3.z*m);
	}
	friend float3 operator*(float3 f3, float m)
	{
		return float3(f3.x*m, f3.y*m, f3.z*m);
	}

	float3 operator=(float3);

	float3& operator += (float3);

};

float3 float3::operator +(float3 m)
{
	float3 result;
	result.x = x + m.x;
	result.y = y + m.y;
	result.z = z + m.z;
	return result;
}

float3 float3::operator - (float3 m)
{
	float3 result;
	result.x = x - m.x;
	result.y = y - m.y;
	result.z = z - m.z;
	return result;
}

float3 float3::operator =(float3 f3)
{
	x = f3.x;
	y = f3.y;
	z = f3.z;
	return float3(x, y, z);
}

struct TriangleMesh
{
	vector<float3> verts;
	vector<TriangleFace> faces;
};

TriangleMesh mesh;

int total_number_of_triangles = 0;

float3 normal(float3 a, float3 b, float3 c){
	float3 vec1 = b - a;
	float3 vec2 = c - a;
	float3 res = float3(vec1.y * vec2.z - vec2.y * vec1.z, vec1.z * vec2.x - vec2.z * vec1.x, vec1.x * vec2.y - vec2.x * vec1.y);
	return res;
}

void loadObj(const std::string filename, TriangleMesh &mesh);

void loadObj(const std::string filename, TriangleMesh &mesh)
{
	std::ifstream in(filename.c_str());

	if (!in.good())
	{
		cout << "ERROR: loading obj:(" << filename << ") file is not good" << "\n";
		exit(0);
	}

	char buffer[256], str[255];
	float f1, f2, f3;

	while (!in.getline(buffer, 255).eof())
	{
		buffer[255] = '\0';

		sscanf_s(buffer, "%s", str, 255);

		// reading a vertex  
		if (buffer[0] == 'v' && (buffer[1] == ' ' || buffer[1] == 32))
		{
			if (sscanf_s(buffer, "v %f %f %f", &f1, &f2, &f3) == 3)
			{
				mesh.verts.push_back(float3(f1, f2, f3));
			}
			else
			{
				cout << "ERROR: vertex not in wanted format in OBJLoader" << "\n";
				exit(-1);
			}
		}
		// reading FaceMtls   
		else if (buffer[0] == 'f' && (buffer[1] == ' ' || buffer[1] == 32))
		{
			TriangleFace f;
			int nt = sscanf_s(buffer, "f %d %d %d", &f.v[0], &f.v[1], &f.v[2]);
			if (nt != 3)
			{
				cout << "ERROR: I don't know the format of that FaceMtl" << "\n";
				exit(-1);
			}

			float3 x = mesh.verts[f.v[0]-1];
			float3 y = mesh.verts[f.v[1]-1];
			float3 z = mesh.verts[f.v[2]-1];
			float3 n = normal(x, y, z);
			f.n[0] = n.x;
			f.n[1] = n.y;
			f.n[2] = n.z;
			mesh.faces.push_back(f);
		}
	}
}


#endif  