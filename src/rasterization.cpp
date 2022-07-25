#include "rasterization.h"

vec2 offset[4]{
    vec2(-0.25, 0.25),
    vec2(-0.25, -0.25),
    vec2(0.25, 0.25),
    vec2(0.25, -0.25)
};


vec3 barycentric(const vec2 tri[3], const vec2 p) {
	vec3 a(tri[1][0] - tri[0][0], tri[2][0] - tri[0][0], tri[0][0] - p[0]);
	vec3 b(tri[1][1] - tri[0][1], tri[2][1] - tri[0][1], tri[0][1] - p[1]);
	vec3 c = cross(a, b);

	if (std::abs(c.z) < 1)
		return vec3(-1, 1, 1);

	return vec3(1. - (c.x + c.y) / c.z, c.x / c.z, c.y / c.z);
}





void triangle(Shader& shader, int width, int height, mat<4, 3>& prj_tri,
              vector<double>& zbuffer, vector<double>& s_buffer, Texture& flash, vector<vec3>& color_buffer) {


	vec4 ptr[3];
    vec4 Ptr[3];
	for (int i = 0; i < 3; i++) {
        Ptr[i] = shader.M.vp * prj_tri.col(i);
        ptr[i] = Ptr[i] / Ptr[i][3];
	}

    vec4 cwprj[3];
    for(int i = 0; i < 3; i++)
        cwprj[i] = prj_tri.col(i) / prj_tri.col(i)[3];

	vec2 ptr2[3];
	for (int i = 0; i < 3; i++)
		ptr2[i] = proj<2>(ptr[i]);

	vec2 boxmin( std::numeric_limits<double>::max(),  std::numeric_limits<double>::max());
	vec2 boxmax(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max());
    vec2 clamp(width - 1, height - 1);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			boxmin[j] = std::max(0.,       std::min(boxmin[j], ptr2[i][j]));
			boxmax[j] = std::min(clamp[j], std::max(boxmax[j], ptr2[i][j]));
		}
	}

	for (int x = (int)boxmin.x; x <= (int)boxmax.x; x++) {
		for (int y = (int)boxmin.y; y <= (int)boxmax.y; y++) {

			vec2 center(x + 0.5, y + 0.5);
			bool cal = false;
			vec3 color(0, 0, 0);

			for (int i = 0; i < SampleN; i++) {
				vec2 p = center + offset[i];
				
				vec3 bar_screen = barycentric(ptr2, p);
				if (bar_screen.x < 0 || bar_screen.y < 0 || bar_screen.z < 0)
					continue;
				
				vec3 bar_clip =
                vec3(bar_screen.x / cwprj[0][2], bar_screen.y / cwprj[1][2], bar_screen.z / cwprj[2][2]);
				bar_clip = bar_clip / (bar_clip.x + bar_clip.y + bar_clip.z);
				
                double frag_z = vec3(ptr[0][2], ptr[1][2], ptr[2][2]) * bar_clip;
                int idx = SampleN * (p.x + p.y * width) + i;
				

                if(frag_z < 0.0 || frag_z > 255.0)
                    continue;


				if (frag_z > zbuffer[idx])
					continue;
				
				if (!cal) {
					vec3 bar_center = barycentric(ptr2, center);
                    if (shader.fragment(bar_center, color, flash, s_buffer, width)) {
						break;
					}
					cal = true;
				}
				color_buffer[idx] = color;
				zbuffer[idx] = frag_z;
			}
			
		}
	}

}






void shadowMap(Matrix& Mat, mat<3, 3>& tri, int width, int height, vector<double>& s_buffer) {
	
    mat<4, 4> W2V = Mat.vp * Mat.c2p * Mat.w2c * Mat.mod;
    mat<4, 4> W2P = Mat.c2p * Mat.w2c * Mat.mod;

    vec4 prj[3];
    for(int i = 0; i < 3; i++){
        prj[i] = W2P * embed<4>(tri.col(i));
    }



	vec4 ptr[3];
	for (int i = 0; i < 3; i++) {
        ptr[i] = W2V * embed<4>(tri.col(i));
	}


	vec2 ptr2[3];
	for (int i = 0; i < 3; i++)
		ptr2[i] = proj<2>(ptr[i]);

	vec2 boxmin(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	vec2 boxmax(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max());
	vec2 clamp(width - 1, height - 1);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			boxmin[j] = std::max(0., std::min(boxmin[j], ptr2[i][j]));
			boxmax[j] = std::min(clamp[j], std::max(boxmax[j], ptr2[i][j]));
		}
	}


	for (int x = (int)boxmin.x; x <= (int)boxmax.x; x++) {
		for (int y = (int)boxmin.y; y <= (int)boxmax.y; y++) {
            vec2 center(x, y);
            vec3 bar_screen = barycentric(ptr2, center);
            if (bar_screen.x < 0 || bar_screen.y < 0 || bar_screen.z < 0)
                continue;

            vec3 bar_clip =
            vec3(bar_screen.x / prj[0][2], bar_screen.y / prj[1][2], bar_screen.z / prj[2][2]);
            bar_clip = bar_clip / (bar_clip.x + bar_clip.y + bar_clip.z);

            double frag_z = vec3(ptr[0][2], ptr[1][2], ptr[2][2]) * bar_clip;
            int idx = center.x + center.y * width;
            if(frag_z > s_buffer[idx])
                continue;
            s_buffer[idx] = frag_z;
		}
	}

}


void WriteShadowMap(TGAImage& img, vector<double>& s_buffer){
    for(int i = 0; i < img.get_width(); i++){
        for(int j = 0; j < img.get_height(); j++){
            int depth = (int)(s_buffer[i + j * img.get_height()] + 0.5);
            TGAColor color(depth, depth, depth);
            img.set(i, j, color);
        }
    }
}


