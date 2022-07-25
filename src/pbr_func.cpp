#include "pbr_func.h"


const float Pi = 3.14159265359;


float DistriGGX(float NdotH, float roughness){
    float r2 = roughness * roughness;

    float NdotH2 = NdotH * NdotH;

    float nom = r2;
    float denom = NdotH2 * (r2 - 1.f) + 1.f;
    denom = Pi * denom * denom;

    return nom / denom;
}



float GeoSchlickGGX(float NdotV, float roughness){
    float r = roughness + 1.f;
    float k = (r * r) / 8.f;

    float nom = NdotV;
    float denom = NdotV * (1.f - k) + k;

    return nom / denom;
}



float GeoSmith(float NdotV, float NdotL, float roughness){
    float ggx1 = GeoSchlickGGX(NdotV, roughness);
    float ggx2 = GeoSchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}



vec3 FnelSchlick(const vec3& color, float cosTheta, float metalness){
    vec3 F0(0.04, 0.04, 0.04);
    float alpha = 1 - metalness;
    float beta = metalness;
    for(int i = 0; i < 3; i++)
        F0[i] = alpha * F0[i] + beta * color[i];

    return F0 + (vec3(1.0, 1.0, 1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

