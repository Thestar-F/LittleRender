#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            vec3 v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "vt")) {
            iss >> trash;
            iss >> trash;
            vec2 v;
            for (int i = 0; i < 2; i++) iss >> v[i];
            tex_verts_.push_back(v);
        } else if (!line.compare(0, 2, "vn")) {
            iss >> trash;
            iss >> trash;
            vec3 v;
            for (int i = 0; i < 3; i++) iss >> v[i];
            normal_verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            std::vector<int> tf;
            std::vector<int> nf;
            int idx, tidx, nidx;
            iss >> trash;
            while (iss >> idx >> trash >> tidx >> trash >> nidx) {
                idx--; // in wavefront obj all indices start at 1, not zero
                tidx--;
                nidx--;
                f.push_back(idx);
                tf.push_back(tidx);
                nf.push_back(nidx);
            }
            faces_[0].push_back(f);
            faces_[1].push_back(tf);
            faces_[2].push_back(nf);
        }
    }
    //std::cerr << "# v# " << verts_.size() << " f# "  << faces_[0].size() << std::endl;
    //std::cerr << "# vt# " << tex_verts_.size() << " tf# " << faces_[1].size() << std::endl;
    //std::cerr << "# vn# " << normal_verts_.size() << " nf# " << faces_[2].size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_[0].size();
}

std::vector<int> Model::face(int idx) {
    return faces_[0][idx];
}

vec3 Model::vert(int i) {
    return verts_[i];
}



int Model::ntex_verts() {
    return (int)tex_verts_.size();
}


std::vector<int> Model::tex_face(int idx) {
    return faces_[1][idx];
}

vec2 Model::tex_vert(int i) {
    return tex_verts_[i];
}


int Model::normal_verts() {
    return (int)normal_verts_.size();
}


std::vector<int> Model::normal_face(int idx) {
    return faces_[2][idx];
}

vec3 Model::normal_vert(int i) {
    return normal_verts_[i];
}


