#include "shader.h"


const float Pi = 3.14159265359;



void ACES(vec3& c) {
	float A = 2.51f;
	float B = 0.03f;
	float C = 2.43f;
	float D = 0.59f;
	float E = 0.14f;

	float alpha[3];

    for (int i = 0; i < 3; i++){
        alpha[i] = (float)c[i] / 255;
    }




    for(int i = 0; i < 3; i++){
        alpha[i] = (alpha[i] * (A * alpha[i] + B)) / (alpha[i] * (C * alpha[i] + D) + E);
    }

	
	for (int i : {0, 1, 2})
        c[i] = alpha[i] * 255;
}



Shader::Shader(vec3 light, Matrix& m, const vec3& color) {
	M = m;
	light_cam = proj<3>(M.w2c * embed<4>(light, 0.)).normalize();
    light_color = color;
}


void Shader::vertex(mat<3, 3>& tri, mat<3, 3>& nrm, mat<2, 3>& uv, mat<4, 3>& prj_tri) {
	varing_uv = uv;

    mat<4, 4> Mat = (M.w2c * M.mod).invert_transpose();
	for (int i = 0; i < 3; i++) {
		varing_nrm.set_col(i, proj<3>(Mat * embed<4>(nrm.col(i), 0.)));
	}

	for (int i = 0; i < 3; i++) {
        prj_tri.set_col(i, M.w2c * M.mod * embed<4>(tri.col(i)));
		varing_tri.set_col(i, proj<3>(prj_tri.col(i)));
	}

	prj_tri = M.c2p * prj_tri;
}


bool Shader::fragment(const vec3 bar, vec3& color, Texture& flash, vector<double>& s_buffer, int width) {
	vec3 bn = (varing_nrm * bar).normalize();
	vec2 uv = varing_uv * bar;
    uv.x = fmod(uv.x, 1.0);
    uv.y = fmod(uv.y, 1.0);

	vec3 tri = varing_tri * bar;

    vec4 stri = M.toSmap * embed<4>(tri);
    int idx = int(stri[0]) + int(stri[1]) * width;
    float shadow = 0.3f;
    if(idx < int(s_buffer.size()))
        shadow += 0.7f * (stri[2] < s_buffer[idx] + 1.f);



    mat<3,3> A = mat<3,3>::identity();
    vec3 frow = varing_tri.col(1) - varing_tri.col(0);
    vec3 srow = varing_tri.col(2) - varing_tri.col(0);
    for(int i = 0; i < 3; i++){
        A[0][i] = frow[i];
        A[1][i] = srow[i];
        A[2][i] = bn[i];
    }
	
	mat<3, 3> AI = A.invert_transpose();

	vec3 tan_x = (AI * vec3(varing_uv[0][1] - varing_uv[0][0], varing_uv[0][2] - varing_uv[0][0], 0)).normalize();
	vec3 tan_y = (AI * vec3(varing_uv[1][1] - varing_uv[1][0], varing_uv[1][2] - varing_uv[1][0], 0)).normalize();

    mat<3,3> T2C = mat<3,3>::identity();
    for(int i = 0; i < 3; i++){
        T2C[0][i] = tan_x[i];
        T2C[1][i] = tan_y[i];
        T2C[2][i] = bn[i];
    }
    T2C = T2C.transpose();
	

    TGAColor nrmColor = sample2D(flash.Norm, uv);

	vec3 n;
	for (int i = 0; i < 3; i++)
        n[i] = (2.f * nrmColor[2 - i] / 255 - 1.f);

	

	n = (T2C * n).normalize();


    //---------------------------------------------------------------------------------//

    //Blinn-Phong

//    vec3 ambient(10, 10, 10);
//    TGAColor DiffuseColor = sample2D(flash.Diff, uv);
//    vec3 dcolor(DiffuseColor[2], DiffuseColor[1], DiffuseColor[0]);


//    vec3 viewDir = (vec3(-tri.x, -tri.y, -tri.z)).normalize();
//    vec3 h = light_cam + viewDir;
//    float hlen = sqrt(h.x * h.x + h.y * h.y + h.z * h.z);
//    h = h / hlen;



//    float NdotH = max(n * h, 0.0);
//    float NdotL = max(n * light_cam, 0.0);

//    float diff = NdotL;
//    float spec = pow(NdotH, 32);

//    TGAColor Ecolor = sample2D(flash.Emis, uv);
//    vec3 Le(Ecolor[2], Ecolor[1], Ecolor[0]);

//    for(int i = 0; i < 3; i++)
//        color[i] = dcolor[i] * (0.1 + (diff + spec) * shadow) + Le[i];




    //PBR

    vec3 radiance = light_color;


    vec3 viewDir = (vec3(-tri.x, -tri.y, -tri.z)).normalize();
    vec3 h = light_cam + viewDir;
    float hlen = sqrt(h.x * h.x + h.y * h.y + h.z * h.z);
    h = h / hlen;

    float NdotH = max(n * h, 0.0);
    float NdotV = max(n * viewDir, 0.0);
    float NdotL = max(n * light_cam, 0.0);

    float roughness = sample2D(flash.Roug, uv)[0] / 255.f;
    float metalness = sample2D(flash.Meta, uv)[0] / 255.f;


    float ao = sample2D(flash.Occl, uv)[0] / 255.f;


    TGAColor DiffuseColor = sample2D(flash.Diff, uv);

    vec3 dcolor(DiffuseColor[2], DiffuseColor[1], DiffuseColor[0]);

    vec3 albedo = 4.5 * dcolor / 255;

    float D = DistriGGX(NdotH, roughness);
    float G = GeoSmith(NdotV, NdotL, roughness);
    vec3 F = FnelSchlick(albedo, max(h * viewDir, 0.0), metalness);


    vec3 ks = F;
    vec3 kd = vec3(1.0, 1.0, 1.0) - ks;
    kd = kd * (1.0 - metalness);


    vec3 norminator = D * G * F;
    float denorminator = 4.0 * max((viewDir * n), 0.0) * max((light_cam * n), 0.0) + 0.01;
    vec3 specular = norminator / denorminator;



    vec3 coe = cordot(kd,albedo) / Pi + specular;


    vec3 L0 = cordot(coe, radiance) * NdotL;



    TGAColor Ecolor = sample2D(flash.Emis, uv);
    vec3 Le(Ecolor[2], Ecolor[1], Ecolor[0]);



    vec3 ambient(10, 10, 10);
    ambient = cordot(ambient, albedo);


    color = (ambient  + L0 * shadow) * ao + Le;


	return false;
}




