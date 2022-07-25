#include "widget.h"

#include <QApplication>
#include <iostream>

#include "Transform.h"
#include "rasterization.h"
#include "model.h"


using namespace std;


const int Width = 1000;
const int Height = 1000;
const float WH_ratio = (float)Width / Height;

const int SampleN = 4;

vec3 light(10, 10, 10);
vec3 center(0, 0, 0);
vec3 eye(0, 0, 4);
vec3 up(0, 1.0, 0);

vec3 light_color(255, 255, 255);



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w(Width, Height, SampleN);

    TGAImage ShadowMap(Width, Height, TGAImage::RGB);

    shared_ptr<Model> model = make_shared<Model>("D:/c++/GUI-MyRender/MyRender/assets/helmet/helmet.obj");


    TGAImage Diffuse;
    Diffuse.read_tga_file("D:/c++/GUI-MyRender/MyRender/assets/helmet/helmet_basecolor.tga");
    Diffuse.flip_vertically();

    TGAImage Metallic;
    Metallic.read_tga_file("D:/c++/GUI-MyRender/MyRender/assets/helmet/helmet_metalness.tga");
    Metallic.flip_vertically();

    TGAImage Normal;
    Normal.read_tga_file("D:/c++/GUI-MyRender/MyRender/assets/helmet/helmet_normal.tga");
    Normal.flip_vertically();

    TGAImage Roughness;
    Roughness.read_tga_file("D:/c++/GUI-MyRender/MyRender/assets/helmet/helmet_roughness.tga");
    Roughness.flip_vertically();

    TGAImage Emission;
    Emission.read_tga_file("D:/c++/GUI-MyRender/MyRender/assets/helmet/helmet_emission.tga");
    Emission.flip_vertically();

    TGAImage Occlusion;
    Occlusion.read_tga_file("D:/c++/GUI-MyRender/MyRender/assets/helmet/helmet_occlusion.tga");
    Occlusion.flip_vertically();




    Texture flash;
    flash.Diff = Diffuse;
    flash.Meta = Metallic;
    flash.Norm = Normal;
    flash.Roug= Roughness;
    flash.Emis = Emission;
    flash.Occl = Occlusion;


    vector<double> zbuffer(SampleN * Width * Height + SampleN * Width, DBL_MAX);
    vector<double> s_buffer(Width * Height + Width , DBL_MAX);



    Matrix M;
    M.w2c = lookat(eye, center, up);
    M.c2p = pers_projection(0.01, 10, WH_ratio, 45);
    M.vp = viewport(Width, Height);


    //Model Matrix
    mat<4, 4> mod = mat<4, 4>::identity();
    mod[1][1] = 0;
    mod[1][2] = -1;
    mod[2][1] = 1;
    mod[2][2] = 0;
    M.mod = mod;



    Matrix Shadow_M;
    Shadow_M.w2c = lookat(light, center, up);
    Shadow_M.c2p = orth_projection(-2, 2, -2, 2, 0.01, 10, WH_ratio);
    Shadow_M.vp = viewport(Width, Height);
    Shadow_M.mod = mod;

    mat<4, 4> W2L = Shadow_M.vp * Shadow_M.c2p * Shadow_M.w2c * Shadow_M.mod;
    mat<4, 4> C2L = W2L * (M.w2c * M.mod).invert();
    M.toSmap = C2L;

    Shader shader(light, M, light_color);


    for (int i = 0; i < model->nfaces(); i++) {


        mat<3, 3> tri = mat<3, 3>::identity();


        auto face = model->face(i);


        for (int j = 0; j < 3; j++) {
            tri.set_col(j, model->vert(face[j]));
        }


        shadowMap(Shadow_M, tri,  Width, Height, s_buffer);
    }

    for (int i = 0; i < model->nfaces(); i++) {


        mat<3, 3> tri = mat<3, 3>::identity();
        mat<3, 3> nrm = mat<3, 3>::identity();
        mat<2, 3> uv = mat<2, 3>::identity();

        auto face = model->face(i);
        auto nface = model->normal_face(i);
        auto tface = model->tex_face(i);

        for (int j = 0; j < 3; j++) {
            tri.set_col(j, model->vert(face[j]));
            nrm.set_col(j, model->normal_vert(nface[j]));
            uv.set_col(j, model->tex_vert(tface[j]));
        }

        mat<4, 3> prj_tri = mat<4, 3>::identity();
        shader.vertex(tri, nrm, uv, prj_tri);
        triangle(shader, Width, Height, prj_tri, zbuffer, s_buffer, flash, w.color_buffer);
    }

    WriteShadowMap(ShadowMap, s_buffer);
    ShadowMap.flip_vertically();
    ShadowMap.write_tga_file("D:/c++/GUI-MyRender/MyRender/ShadowMap/ShadowMap.tga");

    w.show();



    return a.exec();

}
