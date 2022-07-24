#pragma once

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "geometry.h"

mat<4, 4> lookat(vec3 eye, vec3 center, vec3 up);
mat<4, 4> orth_projection(float l, float r, float b, float t, float near, float far, float ratio);
mat<4, 4> pers_projection(float near, float far, float ratio, float fov);
mat<4, 4> viewport(int width, int height);

void showMatrix(mat<4, 4> m);

#endif

