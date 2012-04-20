
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "BasicModel.h"
#include "Model.h"
#include "Triangle.h"

// Window (screen) dimensions
#define WindowWidth 400
#define WindowHeight 400

// World coordinates bounding box
#define XMinWorld -1
#define XMaxWorld 1
#define YMinWorld -1
#define YMaxWorld 1

// Camera is at origin looking down negative Z, so further away = smaller Z
#define MinZ -10000

using namespace std;

void init();
void test();
Triangle convertTriTo2D(Triangle);
Vector3 convertVertexTo2D(Vector3);
void rasterizeTriangle(Triangle);
Vector3 barycentricCoords(Vector3, Vector3, Vector3, Vector3);
void WriteTga(char* outfile);

float zbuffer[WindowWidth][WindowHeight];
float red[WindowWidth][WindowHeight];
float green[WindowWidth][WindowHeight];
float blue[WindowWidth][WindowHeight];

int main(int argc, char** argv)
{
	// Parse the model file
	string filename = argv[1];
	BasicModel model(filename);

	// Make an array of our Triangle structs
	Triangle* tris = new Triangle[model.TriangleStructs.size()];
	for (int i = 0; i < model.TriangleStructs.size(); ++i)
	{
		tris[i] = model.TriangleStructs[i];
	}

	// initialize the red, green, blue, and Z arrays
	init();

	// rasterize each triangle
	for (int i = 0; i < model.TriangleStructs.size(); ++i)
	{
		rasterizeTriangle(convertTriTo2D(tris[i]));
	}

	// Output the image
	WriteTga("test.tga");

	return 0;
}

/*
* Generate and rasterize a couple of test triangles.
*/
void test()
{
		// Create a test triangle.
	Vertex v1;
	Vertex v2;
	Vertex v3;
	Vector3 normal;

	// Create a red vertex at (-0.75, 0, -0.75)
	v1.position.x = -0.75;
	v1.position.y = 0;
	v1.position.z = -0.75;
	v1.rgb.x = 1;	// red
	v1.rgb.y = 0;	// green
	v1.rgb.z = 0;	// blue

	// Create a green vertex at (0, 0.5, 0)
	v2.position.x = 0;
	v2.position.y = 0.5;
	v2.position.z = 0;
	v2.rgb.x = 0;	// red
	v2.rgb.y = 1;	// green
	v2.rgb.z = 0;	// blue

	// Create a blue vertex at (0.5, 0, 0)
	v3.position.x = 0.5;
	v3.position.y = 0;
	v3.position.z = 0;
	v3.rgb.x = 0;	// red
	v3.rgb.y = 0;	// green
	v3.rgb.z = 1;	// blue

	normal.x = 0;
	normal.y = 0;
	normal.z = 1;

	// Create a test triangle that uses the vertices
	Triangle t;
	t.normal = normal;
	t.v1 = v1;
	t.v2 = v2;
	t.v3 = v3;

	Vertex v4;
	Vertex v5;
	Vertex v6;
	v4.position.x = -0.5;
	v4.position.y = 0.5;
	v4.position.z = -0.5;
	v4.rgb.x = 0;
	v4.rgb.y = 0;
	v4.rgb.z = 0.5;

	v5.position.x = -0.5;
	v5.position.y = -0.5;
	v5.position.z = -0.5;
	v5.rgb.x = 0;
	v5.rgb.y = 0;
	v5.rgb.z = 0.5;

	v6.position.x = 0.5;
	v6.position.y = -0.5;
	v6.position.z = -0.5;
	v6.rgb.x = 0;
	v6.rgb.y = 0;
	v6.rgb.z = 0.5;

	Triangle t2;
	t2.normal = normal;
	t2.v1 = v4;
	t2.v2 = v5;
	t2.v3 = v6;

	// Eventually we'll want to iterate through our list of triangles and 
	// call these two functions on each of them. For now, just use the test
	// triangle we made earlier.

	// Convert the test triangle to 2D
	Triangle converted = convertTriTo2D(t);

	// Rasterize the converted triangle
	rasterizeTriangle(converted);

	converted = convertTriTo2D(t2);
	rasterizeTriangle(converted);

	// Temporary testing code:
	// =================================================================
	/*
	ofstream outFile;
	outFile.open("rasterized.txt");

	// write some output to a text file so I can see something.
	// just put a '*' where there's at least *some* red.
	for (int x = 0; x < WindowWidth; ++x)
	{
		for (int y = 0; y < WindowHeight; ++y)
		{
			if (red[x][y] == 0)
				outFile << " ";
			else
				outFile << "*";
		}
		outFile << endl;
	}

	outFile.close();
	*/
	// =================================================================
}

void init()
{
	for (int i = 0; i < WindowWidth; ++i)
	{
		for (int j = 0; j < WindowHeight; ++j)
		{
			zbuffer[i][j] = MinZ;
			red[i][j] = 0;
			green[i][j] = 0;
			blue[i][j] = 0;
		}
	}
}

/*
* Given a Triangle with vertices specified in world coordinates,
* convert the triangle to 2D (screen) coordinates.
*
* t: The triangle in world coordinates
* 
* returns: The triangle converted to screen coordinates
*/
Triangle convertTriTo2D(Triangle t)
{
	Triangle converted = t;

	// convert the vertices to screen space
	Vector3 convertedV1 = convertVertexTo2D(t.v1.position);
	Vector3 convertedV2 = convertVertexTo2D(t.v2.position);
	Vector3 convertedV3 = convertVertexTo2D(t.v3.position);
	converted.v1.position = convertedV1;
	converted.v2.position = convertedV2;
	converted.v3.position = convertedV3;

	// calculate bounding box for the triangle (in screen coordinates)
	converted.minX = min(convertedV1.x, min(convertedV2.x, convertedV3.x));
	converted.maxX = max(convertedV1.x, max(convertedV2.x, convertedV3.x));
	converted.minY = min(convertedV1.y, min(convertedV2.y, convertedV3.y));
	converted.maxY = max(convertedV1.y, max(convertedV2.y, convertedV3.y));

	return converted;
}

/*
* Convert the provided point from world coordinates to screen coordinates.
*
* coords: The vertex coordinates we want to convert
* 
* returns: The vertex converted to screen coordinates
*/
Vector3 convertVertexTo2D(Vector3 coords)
{
	Vector3 converted;

	converted.x = ((coords.x - XMinWorld) * WindowWidth) / (XMaxWorld - XMinWorld);
	converted.y = ((coords.y - YMinWorld) * WindowHeight) / (YMaxWorld - YMinWorld);

	// Z will be used later for depth interpolation and Z buffer tests
	converted.z = coords.z;	
	return converted;
}

/*
* Rasterize a triangle.
*
* t: The triangle to rasterize (should already be converted to screen coordinates)
*/
void rasterizeTriangle(Triangle t)
{
	Vector3 v1Color = t.v1.rgb;
	Vector3 v2Color = t.v2.rgb;
	Vector3 v3Color = t.v3.rgb;
	float v1Z = t.v1.position.z;
	float v2Z = t.v2.position.z;
	float v3Z = t.v3.position.z;

	// iterate over each point (pixel) in the triangle's bounding box
	for (int x = t.minX; x < t.maxX; ++x)
	{
		for (int y = t.minY; y < t.maxY; ++y)
		{
			Vertex p;
			p.position.x = x;
			p.position.y = y;

			// get barycentric coordinates for p (the current X/Y position)
			Vector3 baryCoords = barycentricCoords(t.v1.position, t.v2.position, t.v3.position, p.position);

			// Test the pixel to see if it's inside the triangle. All three
			// barycentric coordinates must be between 0 and 1 for the pixel
			// to be inside the triangle.
			if (baryCoords.x > 0 && baryCoords.x < 1		// check alpha
				&& baryCoords.y > 0 && baryCoords.y < 1		// check beta
				&& baryCoords.z > 0 && baryCoords.z < 1)	// check gamma
			{
				// linearly interpolate the point's color
				p.rgb.x = baryCoords.x*v1Color.x + baryCoords.y*v2Color.x + baryCoords.z*v3Color.x;	// red
				p.rgb.y = baryCoords.x*v1Color.y + baryCoords.y*v2Color.y + baryCoords.z*v3Color.y;	// green
				p.rgb.z = baryCoords.x*v1Color.z + baryCoords.y*v3Color.z + baryCoords.z*v3Color.z;	// blue

				// linearly interpolate the point's depth
				p.position.z = baryCoords.x*v1Z + baryCoords.y*v2Z + baryCoords.z*v3Z;

				// Z buffer test.
				// The camera is at the origin (0, 0, 0) looking down the negative Z axis.
				// This means closer to the camera = greater Z value.
				if (p.position.z > zbuffer[x][y])
				{
					// write the pixel's color components to our color arrays
					red[x][y] = p.rgb.x;
					green[x][y] = p.rgb.y;
					blue[x][y] = p.rgb.z;

					// update the Z buffer
					zbuffer[x][y] = p.position.z;
				}
			}
		}
	}
}

/*
* Calculate the barycentric coordinates for a point with respect to the provided
* vertex positions.
*
* v1, v2, v3: The triangle's vertices
* p: The point to get the barycentric coordinates for (may be inside or outside the triangle)
*
* returns: p's corresponding barycentric coordinates as a Vector3 struct. (x = alpha, y = beta, z = gamma)
*/
Vector3 barycentricCoords(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 p)
{
	// NOTE: these formulas found at http://crackthecode.us/barycentric/barycentric_coordinates.html
	float denom = (v1.x*v2.y) - (v1.x*v3.y) - (v2.x*v1.y) + (v2.x*v3.y) + (v3.x*v1.y) - (v3.x*v2.y);

	// ((X4 * Y2) - (X4 * Y3) - (X2 * Y4) + (X2 * Y3) + (X3 * Y4) - (X3 * Y2)) 
	float alpha = ((p.x*v2.y) - (p.x*v3.y) - (v2.x*p.y) + (v2.x*v3.y) + (v3.x*p.y) - (v3.x*v2.y)) / denom;

	// ((X1 * Y4) - (X1 * Y3) - (X4 * Y1) + (X4 * Y3) + (X3 * Y1) - (X3 * Y4)) 
	float beta = ((v1.x*p.y) - (v1.x*v3.y) - (p.x*v1.y) + (p.x*v3.y) + (v3.x*v1.y) - (v3.x*p.y)) / denom;

	// ((X1 * Y2) - (X1 * Y4) - (X2 * Y1) + (X2 * Y4) + (X4 * Y1) - (X4 * Y2))
	float gamma = ((v1.x*v2.y) - (v1.x*p.y) - (v2.x*v1.y) + (v2.x*p.y) + (p.x*v1.y) - (p.x*v2.y)) / denom;

	Vector3 baryCoords;
	baryCoords.x = alpha;
	baryCoords.y = beta;
	baryCoords.z = gamma;

	return baryCoords;
}

void WriteTga(char *outfile)
{
    FILE *fp = fopen(outfile, "wb"); // originally was just "w"
    if (fp == NULL)
    {
        perror("ERROR: Image::WriteTga() failed to open file for writing!\n");
        exit(EXIT_FAILURE);
    }
    
    // write 24-bit uncompressed targa header
    // thanks to Paul Bourke (http://local.wasp.uwa.edu.au/~pbourke/dataformats/tga/)
    putc(0, fp);
    putc(0, fp);
    
    putc(2, fp); // type is uncompressed RGB
    
    putc(0, fp);
    putc(0, fp);
    putc(0, fp);
    putc(0, fp);
    putc(0, fp);
    
    putc(0, fp); // x origin, low byte
    putc(0, fp); // x origin, high byte
    
    putc(0, fp); // y origin, low byte
    putc(0, fp); // y origin, high byte

    putc(WindowWidth & 0xff, fp); // width, low byte
    putc((WindowWidth & 0xff00) >> 8, fp); // width, high byte

    putc(WindowHeight & 0xff, fp); // height, low byte
    putc((WindowHeight & 0xff00) >> 8, fp); // height, high byte

    putc(24, fp); // 24-bit color depth

    putc(0, fp);

    // write the raw pixel data in groups of 3 bytes (BGR order)
    for (int y = 0; y < WindowHeight; y++)
    {
        for (int x = 0; x < WindowWidth; x++)
        {
            // if color scaling is on, scale 0.0 -> _max as a 0 -> 255 unsigned byte
            unsigned char rbyte, gbyte, bbyte;
            double r = (red[x][y] > 1.0) ? 1.0 : red[x][y];
            double g = (green[x][y] > 1.0) ? 1.0 : green[x][y];
            double b = (blue[x][y] > 1.0) ? 1.0 : blue[x][y];
            rbyte = (unsigned char)(r * 255);
            gbyte = (unsigned char)(g * 255);
            bbyte = (unsigned char)(b * 255);

            putc(bbyte, fp);
            putc(gbyte, fp);
            putc(rbyte, fp);
        }
    }

    fclose(fp);
}