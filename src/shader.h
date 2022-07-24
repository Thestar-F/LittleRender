#pragma once

#ifndef SHADER_H
#define SHADER_H

#include "geometry.h"
#include "tgaimage.h"
#include "pbr_func.h"
#include <limits>
#include <cfloat>

using namespace std;

extern const int SampleN;



struct Texture {
    TGAImage Diff;
    TGAImage Meta;
    TGAImage Norm;
    TGAImage Roug;
    TGAImage Emis;
};

struct Matrix {
	mat<4, 4> w2c;
	mat<4, 4> c2p;
	mat<4, 4> vp;
	mat<4, 4> toSmap;
};



inline TGAColor sample2D(TGAImage& img, vec2& uv) {
	return img.get(uv[0] * img.get_width(), uv[1] * img.get_height());
}


void ACES(vec3& c);


class Shader {
public:
	Matrix M;
	vec3 light_cam;
    vec3 light_color;
	mat<2, 3> varing_uv;
	mat<3, 3> varing_tri;
	mat<3, 3> varing_nrm;
	

    Shader(vec3 light, Matrix& m, const vec3& color);
	void vertex(mat<3,3>& tri, mat<3,3>& nrm, mat<2,3>& uv, mat<4,3>& prj_tri);
	bool fragment(const vec3 bar, vec3& color, Texture& flash, vector<double>& s_buffer, int width);
};




#endif
