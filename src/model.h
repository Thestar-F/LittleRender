#pragma once

#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<vec3> verts_;
	std::vector<std::vector<int> > faces_[3];
	
	std::vector<vec2> tex_verts_;

	std::vector<vec3> normal_verts_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	vec3 vert(int i);
	std::vector<int> face(int idx);

	int ntex_verts();
	vec2 tex_vert(int i);
	std::vector<int> tex_face(int idx);

	int normal_verts();
	vec3 normal_vert(int i);
	std::vector<int> normal_face(int idx);


};

#endif //__MODEL_H__
