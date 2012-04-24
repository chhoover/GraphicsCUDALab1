#include "BasicModel.h"
#include <iostream>
#include <fstream>
#include <string>

#include "Triangle.h"

using namespace std;

BasicModel::BasicModel(string filename)
{
   max_x = max_y = max_z = 1.1754E-38F;
   min_x = min_y = min_z = 1.1754E+38F;
   center.x = 0.0;
   center.y = 0.0;
   center.z = 0.0;

   ReadFile(filename);

   max_extent = max_x - min_x;
   if (max_y - min_y > max_extent) 
   {
      max_extent = max_y - min_y;
   }
   center.x = center.x/Vertices.size();
   center.y = center.y/Vertices.size();
   center.z = center.z/Vertices.size();

   // adjust min/max x and y so that they'll be correct
   // when the bunny is centered at the origin
   min_x = min_x + (0 - center.x);
   max_x = max_x + (0 - center.x);
   min_y = min_y + (0 - center.y);
   max_y = max_y + (0 - center.y);

   //id = createDL();
}

BasicModel::~BasicModel()
{

   // Delete vector<Vector3 *> Vertices
   for ( vector<Vector3 *>::iterator i = Vertices.begin(); 
         i != Vertices.end(); 
         i++)
   {
      Vector3* temp = *i;
      delete temp; // delete all objects newed into the vector
   }

   // Delete vector<Vector3 *> VerticesNormals
   for ( vector<Vector3 *>::iterator i = VerticesNormals.begin(); 
         i != VerticesNormals.end(); 
         i++)
   {
      Vector3* temp = *i;
      delete temp; // delete all objects newed into the vector
   }

   // Delete vector<Tri *> Triangles
   for ( vector<Tri *>::iterator i = Triangles.begin(); 
         i != Triangles.end(); 
         i++)
   {
      Tri* temp = *i;
      delete temp; // delete all objects newed into the vector
   }
}

void BasicModel::createTriangleStructs(float xOffset, float yOffset, float scaleFactor)
{
	TriangleStructs.clear();

	for (int i = 0; i < Triangles.size(); ++i)
	{
		Tri* t = Triangles[i];

		// Vertex labels in the file start at 1, but vector uses 0-based indices.
		// Vertices are listed in order of label, though, so <vector pos> + 1 = label.
		Vector3 v1 = normalizeVertexCoords(*Vertices.at(t->v1 - 1), xOffset, yOffset, scaleFactor);
		Vector3 v2 = normalizeVertexCoords(*Vertices.at(t->v2 - 1), xOffset, yOffset, scaleFactor);
		Vector3 v3 = normalizeVertexCoords(*Vertices.at(t->v3 - 1), xOffset, yOffset, scaleFactor);
			 
		Triangle triangle;
			 
		triangle.normal = t->normal;
		triangle.v1.position = v1;
		triangle.v1.rgb = t->color;
			 
		triangle.v2.position = v2;
		triangle.v2.rgb = t->color;

		triangle.v3.position = v3;
		triangle.v3.rgb = t->color;

		TriangleStructs.push_back(triangle);
	}
}

//open the file for reading
void BasicModel::ReadFile(string filename) 
{
   string line;
   streampos linestart;

   ifstream infile(filename.c_str());
   Vector3 *v = 0;
   if (infile.is_open())  
   {
      while (!infile.eof())
      {
         int start;

         linestart = infile.tellg();
         getline(infile,line);

         // Ignore commented lines
         if (line.find("#") == 0)
            continue;

         if (!(start = line.find("Vertex"))) // we found a vertex
         {
            v = parseCoords(line);

            //house keeping to display in center of the scene
            center.x += v->x;
            center.y += v->y;
            center.z += v->z;

            if (v->x > max_x) max_x = v->x; 
            if (v->x < min_x) min_x = v->x;

            if (v->y > max_y) max_y = v->y; 
            if (v->y < min_y) min_y = v->y;

            if (v->z > max_z) max_z = v->z; 
            if (v->z < min_z) min_z = v->z;

            Vertices.push_back(v);
            VerticesNormals.push_back(new Vector3(0.0,0.0,0.0));
         }
         else if (!(start = line.find("Face"))) // we found a face
         {
			 Triangles.push_back(parseTri(line));
         }
      }
      infile.close();
   } 
   else
   {
      throw("Could not open file " /*<< filename << endl*/);
   }
}


BasicModel::Tri* BasicModel::parseTri(string line)
{
   Tri* t = new Tri();

   size_t start;

   start = line.find("  ");

   start += 2;
   line = line.substr(start);
   t->v1 = static_cast<int>(parseFloat(line));

   start = line.find(" ");
   line = line.substr(++start);
   t->v2 = static_cast<int>(parseFloat(line));

   start = line.find(" ");
   line = line.substr(++start);
   t->v3 = static_cast<int>(parseFloat(line));
   
   if ((start = line.find("{")) != string::npos) // there is a color
   {
      start = line.find("rgb=(");
      start += 5; // rgb=( is 5 characters

      line = line.substr(start);
      t->color.x = parseFloat(line);

      start = line.find(" ");
      line = line.substr(start);
      t->color.y = parseFloat(line);

      start = line.find(" ");
      line = line.substr(start);
      t->color.z = parseFloat(line);
   }
   else
   {
      t->color = Vector3(1.0,1.0,1.0);
   }
         

   //Calculate the normal for this triangle
   Vector3* v1 = Vertices[t->v1 - 1];
   Vector3* v2 = Vertices[t->v2 - 1];
   Vector3* v3 = Vertices[t->v3 - 1];

   Vector3 vCp1(v2->x - v1->x,v2->y - v1->y,v2->z - v1->z);
   Vector3 vCp2(v3->x - v1->x,v3->y - v1->y,v3->z - v1->z);
   Vector3 normalV = vCp1.crossP(vCp2);

   float normalizingFactor = sqrtf(normalV.dotP(normalV));
   normalV.x = normalV.x/normalizingFactor;
   normalV.y = normalV.y/normalizingFactor;
   normalV.z = normalV.z/normalizingFactor;

   t->normal = normalV;

   v1 = VerticesNormals[t->v1 - 1];
   v2 = VerticesNormals[t->v2 - 1];
   v3 = VerticesNormals[t->v3 - 1];

   v1->x += normalV.x;
   v1->y += normalV.y;
   v1->z += normalV.z;

   v2->x += normalV.x;
   v2->y += normalV.y;
   v2->z += normalV.z;

   v3->x += normalV.x;
   v3->y += normalV.y;
   v3->z += normalV.z;

   return t;
}

void BasicModel::draw(float rx, float ry, float rz)
{
   //glCallList(id);
}

void BasicModel::setLOD(int lvl)
{
}

Vector3 BasicModel::normalizeVertexCoords(Vector3 v, float xOffset, float yOffset, float scaleFactor)
{
	Vector3 normalizedVertex;

	// adjust the vertex's position so that the bunny's
	// center is at the origin
	normalizedVertex.x = v.x + (0 - center.x);
	normalizedVertex.y = v.y + (0 - center.y);
	normalizedVertex.z = v.z;

	// now scale the vertex's coordinates so the bunny will fill
	// more of the screen.
	normalizedVertex.x *= scaleFactor;
	normalizedVertex.y *= scaleFactor;

	// finally, shift the scaled coordinates by the specified offsets.
	normalizedVertex.x += xOffset;
	normalizedVertex.y += yOffset;

	return normalizedVertex;
}