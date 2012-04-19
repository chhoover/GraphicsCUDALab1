#include "BasicModel.h"
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

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

   for (int i = 0; i < Triangles.size(); ++i)
   {
		Tri* t = Triangles[i];

		// Vertex labels in the file start at 1, but vector uses 0-based indices.
		// Vertices are listed in order of label, though, so <vector pos> + 1 = label.
		Vector3 v1 = *(Vertices.at(t->v1 - 1));
		Vector3 v2 = (*Vertices.at(t->v2 - 1));
		Vector3 v3 = (*Vertices.at(t->v3 - 1));
			 
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

/*
GLuint BasicModel::createDL()
{
   // Create the list's ID
   GLuint modelid;
   modelid = glGenLists(1);


   // start list
   glNewList(modelid,GL_COMPILE);

   // call the function that contains the rendering commands
   //draw the bunny
   for(unsigned int j = 0; j < Triangles.size(); j++) 
   {
      drawTria(Triangles[j]);
   }

   // endList
   glEndList();

   return modelid;
}
*/

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
      t->color = Vector3(1.0,0.0,0.0);
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

//drawing routine to draw triangles as wireframe
void BasicModel::drawTria(Tri* t) 
{
   glPushMatrix();
      // Average the three vectors to determine their centers
      float xCenter = (Vertices[t->v1 - 1]->x + Vertices[t->v2 - 1]->x + Vertices[t->v3 - 1]->x)/3.0; 
      float yCenter = (Vertices[t->v1 - 1]->y + Vertices[t->v2 - 1]->y + Vertices[t->v3 - 1]->y)/3.0;
      float zCenter = (Vertices[t->v1 - 1]->z + Vertices[t->v2 - 1]->z + Vertices[t->v3 - 1]->z)/3.0;

      glBegin(GL_TRIANGLE_FAN);

         //glColor3f(t->color.x,t->color.y,t->color.z);
         //note that the vertices are indexed starting at 0, but the triangles
         //index them starting from 1, so we must offset by -1!!!

         float norm = VerticesNormals[t->v1 - 1]->length();
         glNormal3f(VerticesNormals[t->v1 - 1]->x/norm * 1.0f/max_extent,
                    VerticesNormals[t->v1 - 1]->y/norm * 1.0f/max_extent,
                    VerticesNormals[t->v1 - 1]->z/norm * 1.0f/max_extent);

         glVertex3f( Vertices[t->v1 - 1]->x, 
                     Vertices[t->v1 - 1]->y,
                     Vertices[t->v1 - 1]->z);

      
         norm = VerticesNormals[t->v2 - 1]->length();
         glNormal3f(VerticesNormals[t->v2 - 1]->x/norm * 1.0f/max_extent,
                    VerticesNormals[t->v2 - 1]->y/norm * 1.0f/max_extent,
                    VerticesNormals[t->v2 - 1]->z/norm * 1.0f/max_extent);
      
         glVertex3f( Vertices[t->v2 - 1]->x, 
                     Vertices[t->v2 - 1]->y,
                     Vertices[t->v2 - 1]->z);

      
         norm = VerticesNormals[t->v3 - 1]->length();
         glNormal3f(VerticesNormals[t->v3 - 1]->x/norm * 1.0f/max_extent,
                    VerticesNormals[t->v3 - 1]->y/norm * 1.0f/max_extent,
                    VerticesNormals[t->v3 - 1]->z/norm * 1.0f/max_extent);
      
         glVertex3f( Vertices[t->v3 - 1]->x, 
                     Vertices[t->v3 - 1]->y,
                     Vertices[t->v3 - 1]->z);

      glEnd();
   glPopMatrix();
}

void BasicModel::draw(float rx, float ry, float rz)
{
   glCallList(id);
}

void BasicModel::setLOD(int lvl)
{
}