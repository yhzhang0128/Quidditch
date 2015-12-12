#ifndef _VERTEX_3D_
#define _VERTEX_3D_

#include <cmath>

#define PI 3.1415926535898
#define NEAR_ZERO_ESP 0.0000001

struct Vertex3D{
	double x, y, z;
	Vertex3D()	{}
	Vertex3D(const Vertex3D &other){
		x = other.x; y = other.y; z = other.z;
	}
	Vertex3D(double xx, double yy, double zz) :
		x(xx), y(yy), z(zz)	{}

	double length(){
		return sqrt(squareLength());
	}

	double squareLength(){
		return x*x + y*y + z*z;
	}

	Vertex3D scale(double len){
		double ratio = len / sqrt(squareLength());
		x *= ratio; y *= ratio; z *= ratio;
		return *this;
	}

	Vertex3D normalize(){
		double len = sqrt(squareLength());
		x /= len; y /= len; z /= len;
		return *this;
	}

	float dot(const Vertex3D &oth){
		return x * oth.x + y * oth.y + z * oth.z;
	}

	Vertex3D cross(const Vertex3D &oth){
		//Vec3(f[1]*v.f[2] - f[2]*v.f[1], f[2]*v.f[0] - f[0]*v.f[2], f[0]*v.f[1] - f[1]*v.f[0]);
		return Vertex3D(y * oth.z - z * oth.y, z * oth.x - x * oth.z, x * oth.y - y * oth.x);
	}


	Vertex3D operator = (const Vertex3D &right){
		x = right.x; y = right.y; z = right.z;
		return *this;
	}

	void operator *= (const double &ratio){
		x *= ratio;
		y *= ratio;
		z *= ratio;
	}

	void operator += (const Vertex3D& oth){
		x += oth.x;
		y += oth.y;
		z += oth.z;
	}
};

double vertexDist(const Vertex3D& A, const Vertex3D& B){
	return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y) + (A.z - B.z) * (A.z - B.z));
}

Vertex3D operator + (const Vertex3D &left, const Vertex3D &right){
	return Vertex3D(left.x + right.x, left.y + right.y, left.z + right.z);
}
Vertex3D operator - (const Vertex3D &left, const Vertex3D &right){
	return Vertex3D(left.x - right.x, left.y - right.y, left.z - right.z);
}

Vertex3D operator * (const Vertex3D &left, float right){
	return Vertex3D(left.x * right, left.y * right, left.z * right);
}
Vertex3D operator / (const Vertex3D &left, float right){
	return Vertex3D(left.x / right, left.y / right, left.z / right);
}

#endif