#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "utils.h"

typedef struct Vertex
{
	Vector3 position;	// X, Y, Z coordinates
	Vector3 rgb;		// <red, green, blue> values
} Vertex;

// Similar to Vertex but usable on GPU
typedef struct Vertex2
{
	VectorThree position;	// X, Y, Z coordinates
	VectorThree rgb;		// <red, green, blue> values
} Vertex2;

// C-style struct for triangle data.
typedef struct Triangle
{
	Vertex v1; // first vertex
	Vertex v2; // second vertex
	Vertex v3; // third vertex
	Vector3 normal; // normal for this face (calculated during file parsing)

	// bounding box
	float minX;
	float maxX;
	float minY;
	float maxY;
} Triangle;

#endif
