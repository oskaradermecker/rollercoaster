#define _CRT_SECURE_NO_DEPRECATE

#include "Track.hpp"

#include <vector>

/*
various varibles
*/
float t = 0.0000;
float dT = 0.001;
point p1, p2, p3, p4;
int CpLIndex = 0, pLIndex = 0;
struct myPoint *pointsList;
point up;

////////////////Functions/////////////////////

/*
Loads the curve from the text file
*/
bool loadCurve(const char *path, std::vector < glm::vec3 > & out_vertices) {

	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}
	while (1) {

		char lineHeader[128]; // read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.
		if (strcmp(lineHeader, "p") == 0) {
			glm::vec3 point1;
			glm::vec3 point2;
			glm::vec3 point3;
			fscanf(file, "%f %f %f %f %f %f %f %f %f \n", &point1.x, &point1.y, &point1.z, &point2.x, &point2.y, &point2.z, &point3.x, &point3.y, &point3.z);
			out_vertices.push_back(point1);
			out_vertices.push_back(point2);
			out_vertices.push_back(point3);
		}
		else if (strcmp(lineHeader, "n") == 0) {
			break;
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	fclose(file);
	return true;
}

/*
Reads the point file and generates the list of points
*/
std::vector<GLfloat> GenerateTrack(const char *path) {

	std::vector < glm::vec3 > Vec_vertices_i;
	std::vector<GLfloat> Vec_vertices;

	bool res = loadCurve(path, Vec_vertices_i);
	if (!res) {
		exit(1);
	}

	pointsList = (struct myPoint *)malloc(((1 / dT) * Vec_vertices_i.size() / 3) * sizeof(struct myPoint));
	CreatePointList(Vec_vertices_i);

	for (int j = 0; j < pLIndex; j++) {
		Vec_vertices.push_back(GLfloat(pointsList[j].position.x));
		Vec_vertices.push_back(GLfloat(pointsList[j].position.y));
		Vec_vertices.push_back(GLfloat(pointsList[j].position.z));
	}

	return Vec_vertices;
}

/*
Calculates the position and tangent of every point of the curve and stores them in a struct.
*/
void CreatePointList(std::vector < glm::vec3 > Vec_vertices) {

	int size = Vec_vertices.size();
	for (int j = 1; j < size; j = 3 + j) {
		setControlPoints(j, Vec_vertices, size);
		while (t >= 0 && t < 1) {
			point drawPoint = CalculatePosition(t);
			point tangentPoint = CalculateTangent(t);
			StoreCurrentPoint(drawPoint, tangentPoint);
			t += dT;
			pLIndex++;
		}
		t = 0;
	}
}

/*
Sets the control points to calculate the point's position & tangent.
*/
void setControlPoints(int j, std::vector < glm::vec3 > Vec_vertices, int size) {

	p1.x = Vec_vertices[j].x;
	p1.y = Vec_vertices[j].y;
	p1.z = Vec_vertices[j].z;

	p2.x = Vec_vertices[j + 1].x;
	p2.y = Vec_vertices[j + 1].y;
	p2.z = Vec_vertices[j + 1].z;

	if (j <= size - 3) {
		p3.x = Vec_vertices[j + 2].x;
		p3.y = Vec_vertices[j + 2].y;
		p3.z = Vec_vertices[j + 2].z;

		p4.x = Vec_vertices[j + 3].x;
		p4.y = Vec_vertices[j + 3].y;
		p4.z = Vec_vertices[j + 3].z;
	}
	else {
		p3.x = Vec_vertices[0].x;
		p3.y = Vec_vertices[0].y;
		p3.z = Vec_vertices[0].z;

		p4.x = Vec_vertices[1].x;
		p4.y = Vec_vertices[1].y;
		p4.z = Vec_vertices[1].z;
	}

}

/*
Calculates the point's position following a Bezier Curve according to a small increment (u). Interpolate between two points with a uPlus increment.
*/
point CalculatePosition(float t) {
	point p;
	p.x = pow((1 - t), 3) * p1.x + 3 * t * pow((1 - t), 2) * p2.x + 3 * (1 - t) * pow(t, 2)* p3.x + pow(t, 3)* p4.x;
	p.y = pow((1 - t), 3) * p1.y + 3 * t * pow((1 - t), 2) * p2.y + 3 * (1 - t) * pow(t, 2)* p3.y + pow(t, 3)* p4.y;
	p.z = pow((1 - t), 3) * p1.z + 3 * t * pow((1 - t), 2) * p2.z + 3 * (1 - t) * pow(t, 2)* p3.z + pow(t, 3)* p4.z;
	return p;
}

/*
Calculates the point's tangent following a Bezier Curve according to a small increment (u).
*/
point CalculateTangent(float t) {
	point p;
	p.x = 3 * (p2.x - p1.x) * pow((1 - t), 2) + 6 * (p3.x - p2.x) * (1 - t) * t + 3 * (p4.x - p3.x) * pow(t, 2);
	p.y = 3 * (p2.y - p1.y) * pow((1 - t), 2) + 6 * (p3.y - p2.y) * (1 - t) * t + 3 * (p4.y - p3.y) * pow(t, 2);
	p.z = 3 * (p2.z - p1.z) * pow((1 - t), 2) + 6 * (p3.z - p2.z) * (1 - t) * t + 3 * (p4.z - p3.z) * pow(t, 2);
	return p;
}

/*
stores the current point's position and tangent in the point list and calculates the corresponding normals and binormals.
*/
void StoreCurrentPoint(point position, point tangent) {

	//Position
	pointsList[pLIndex].position.x = position.x;
	pointsList[pLIndex].position.y = position.y;
	pointsList[pLIndex].position.z = position.z;

	//Tangent
	pointsList[pLIndex].tangent.x = tangent.x;
	pointsList[pLIndex].tangent.y = tangent.y;
	pointsList[pLIndex].tangent.z = tangent.z;
	pointsList[pLIndex].tangent = normalize(pointsList[pLIndex].tangent); //normalise tangent	
	/*
	//Normal
	if (pLIndex == 0) { //Consider binormal of first vertex is in y
		pointsList[pLIndex].biNormal.x = 0;
		pointsList[pLIndex].biNormal.y = 1;
		pointsList[pLIndex].biNormal.z = 0;
		pointsList[pLIndex].biNormal = normalize(pointsList[pLIndex].biNormal);
	}
	else { //biNormal[i] = Tangent[i] x Normal[i-1]
		pointsList[pLIndex].biNormal.x = crossProduct(tangent, pointsList[pLIndex - 1].normal).x;
		pointsList[pLIndex].biNormal.y = crossProduct(tangent, pointsList[pLIndex - 1].normal).y + 0.3;
		pointsList[pLIndex].biNormal.z = crossProduct(tangent, pointsList[pLIndex - 1].normal).z;
		pointsList[pLIndex].biNormal = normalize(pointsList[pLIndex].biNormal);
	}

	//Binormal
	pointsList[pLIndex].normal.x = crossProduct(pointsList[pLIndex].biNormal, tangent).x;
	pointsList[pLIndex].normal.y = crossProduct(pointsList[pLIndex].biNormal, tangent).y;
	pointsList[pLIndex].normal.z = crossProduct(pointsList[pLIndex].biNormal, tangent).z;
	pointsList[pLIndex].normal = normalize(pointsList[pLIndex].normal);
	*/
	if (pLIndex == 0) {
		up.x = 0; up.y = 1; up.z = 0;
	}
	pointsList[pLIndex].normal.x = crossProduct(tangent, up).x;
	pointsList[pLIndex].normal.y = crossProduct(tangent, up).y;
	pointsList[pLIndex].normal.z = crossProduct(tangent, up).z;
	pointsList[pLIndex].normal = normalize(pointsList[pLIndex].normal);

	pointsList[pLIndex].biNormal.x = crossProduct(pointsList[pLIndex].normal, tangent).x;
	pointsList[pLIndex].biNormal.y = crossProduct(pointsList[pLIndex].normal, tangent).y;
	pointsList[pLIndex].biNormal.z = crossProduct(pointsList[pLIndex].normal, tangent).z;
	pointsList[pLIndex].biNormal = normalize(pointsList[pLIndex].biNormal);
}

/*
function for normalizing a vector
*/
point normalize(point vector) {
	point p;
	float sum = sqrt((vector.x * vector.x + vector.y * vector.y + vector.z * vector.z));

	p.x = vector.x / sum;
	p.y = vector.y / sum;
	p.z = vector.z / sum;
	return p;
}

/*
computes the cross product of two vectors
*/
point crossProduct(point p1, point p2) {
	point p;
	p.x = p1.y * p2.z - p1.z * p2.y;
	p.y = p1.z * p2.x - p1.x * p2.z;
	p.z = p1.x * p2.y - p1.y * p2.x;
	return p;
}

/*
computes the dot product of two vectors
*/
double dotProduct(point p1, point p2) {

	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

/*
function for calculating the angle between two vectors
*/
double anglex(point vector1, point vector2) {//, point v1Origin, point v2Origin) {

	float pi = 3.1415926535897932384;
	double dot = vector1.y*vector2.y + vector1.z*vector2.z;//vecP1.y*vecP2.y + vecP1.z*vecP2.z;     //dot product between[x1, y1] and [x2, y2]
	double det = vector1.y*vector2.z - vector1.z*vector2.y;//vecP1.y*vecP2.z - vecP1.z*vecP2.y;     //determinant
	double angle = atan2(-det, -dot) + pi;  //atan2(-y, -x) + 180°

											//std::cout << glm::degrees(float(angle)) << std::endl;
	return angle;

}

/*
function for calculating the angle between two vectors
*/
double angley(point vector1, point vector2) {

	float pi = 3.1415926535897932384;
	double dot = vector1.z*vector2.z + vector1.x*vector2.x;     //dot product between[x1, y1] and [x2, y2]
	double det = vector1.z*vector2.x - vector1.x*vector2.z;     //determinant
	double angle = atan2(-det, -dot) + pi;  //atan2(-y, -x) + 180°

	return angle;
}

/*
function for calculating the angle between two vectors
*/
double anglez(point vector1, point vector2) {

	float pi = 3.1415926535897932384;
	double dot = vector1.x*vector2.x + vector1.y*vector2.y;     //dot product between[x1, y1] and [x2, y2]
	double det = vector1.x*vector2.y - vector1.y*vector2.x;     //determinant
	double angle = atan2(-det, -dot) + pi;  //atan2(-y, -x) + 180°

	return angle;
}