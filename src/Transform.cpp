#include <iostream>
#include "Transform.h"

const float Pi = 3.14159265359;

using std::cout;
using std::endl;

mat<4, 4> lookat(vec3 eye, vec3 center, vec3 up) {
	mat<4, 4> Matrix = mat<4, 4>::identity();
	mat<4, 4> Translate = mat<4, 4>::identity();
	vec3 z = (eye - center).normalize();
	vec3 x = cross(up, z).normalize();
	vec3 y = cross(z, x).normalize();
	for (int i = 0; i < 3; i++) {
		Matrix[0][i] = x[i];
		Matrix[1][i] = y[i];
		Matrix[2][i] = z[i];
		Translate[i][3] = -eye[i];
	}
	Matrix = Matrix * Translate;
	return Matrix;
}

//r = 1, l = -1; t = 1, b = -1;
mat<4, 4> orth_projection(float l, float r, float b, float t, float near, float far, float ratio) {
	mat<4, 4> Matrix = mat<4, 4>::identity();
	float diff = 1 / (far - near);
    Matrix[0][0] = 2.f / (r - l);
    Matrix[0][0] = Matrix[0][0] / ratio;
    Matrix[1][1] = 2.f / (t - b);
	Matrix[2][2] = -2 * diff;
	Matrix[2][3] = -(far + near) * diff;
	return Matrix;
}


//传入的fov是度数
mat<4, 4> pers_projection(float near, float far, float ratio, float fov) {
	mat<4, 4> Matrix = mat<4, 4>::identity();
	float radians = fov * Pi / 180;
	float cot = 1.f / tan(radians / 2);
	Matrix[0][0] = cot / ratio;
	Matrix[1][1] = cot;
	Matrix[2][2] = -(far + near) / (far - near);
	Matrix[2][3] = -2 * far * near / (far - near);
	Matrix[3][2] = -1.f;
	Matrix[3][3] = 0;
	return Matrix;
}



mat<4, 4> viewport(int width, int height) {
	mat<4, 4> Matrix = mat<4, 4>::identity();
	Matrix[0][0] = width / 2.f;
	Matrix[0][3] = width / 2.f;
	Matrix[1][1] = height / 2.f;
	Matrix[1][3] = height / 2.f;
    Matrix[2][2] = 127.5f;
    Matrix[2][3] = 127.5f;
	return Matrix;
}


void showMatrix(mat<4, 4> m) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			cout << m[i][j] << " ";
		cout << endl;
	}
}
