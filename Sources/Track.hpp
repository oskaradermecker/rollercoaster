#pragma once

#ifndef TRACK_H
#define TRACK_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


/*
various varibles
*/
extern float t;
extern float dT;
extern int CpLIndex, pLIndex;

// Structures

/* represents one control point along the spline */
struct point {
	double x;
	double y;
	double z;
};

struct myPoint {
	point position;
	point normal;
	point tangent;
	point biNormal;
};

extern struct myPoint *pointsList;
extern point p1, p2, p3, p4;
extern point up;

bool loadCurve(const char *path, std::vector < glm::vec3 > & out_vertices);
void CreatePointList(std::vector < glm::vec3 > Vec_vertices);
void setControlPoints(int j, std::vector < glm::vec3 > Vec_vertices, int size);
point CalculatePosition(float u);
point CalculateTangent(float u);
void StoreCurrentPoint(point position, point tangent);
point normalize(point vector);
point crossProduct(point p1, point p2);
double dotProduct(point p1, point p2);
double anglex(point vector1, point vector2);
double angley(point vector1, point vector2);
double anglez(point vector1, point vector2);
std::vector<GLfloat> GenerateTrack(const char *path);

#endif