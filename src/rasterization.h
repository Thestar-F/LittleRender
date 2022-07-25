#pragma once

#ifndef RASTERIZATION_H
#define RASTERIZATION_H

#include "shader.h"



void triangle(Shader& shader, int width, int height, mat<4, 3>& prj_tri,
              vector<double>& zbuffer, vector<double>& s_buffer, Texture& flash, vector<vec3>& color_buffer);
vec3 barycentric(const vec2 tri[3], const vec2 p);


void shadowMap(Matrix& Mat, mat<3, 3>& tri, int width, int height, vector<double>& s_buffer);

void WriteShadowMap(TGAImage& img, vector<double>& s_buffer);

#endif
