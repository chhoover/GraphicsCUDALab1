#if !defined BASIC_MODEL_H
#define BASIC_MODEL_H

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include "Model.h"

#include "Triangle.h"

class BasicModel : virtual public Model
{
public:
   BasicModel(std::string filename);
   ~BasicModel();

   //stl vector to store all the triangles in the mesh
   std::vector<Tri *> Triangles;
   //stl vector to store all the vertices in the mesh
   std::vector<Vector3 *> Vertices;
   //stl vector to store all the vertices normals in the mesh
   std::vector<Vector3 *> VerticesNormals;

   // stores triangle data as C-style structs (for rasterization)
   std::vector<Triangle> TriangleStructs;

   void draw(float,float,float);
   void setLOD(int);
   void createTriangleStructs(float, float, float);

protected:
   GLuint id;

   Tri* parseTri(std::string line);
   void ReadFile(std::string filename);
   GLuint createDL();
   Vector3 normalizeVertexCoords(Vector3, float, float, float);
  
};

#endif
