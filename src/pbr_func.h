#ifndef PBR_FUNC_H
#define PBR_FUNC_H
#include "geometry.h"


float DistriGGX(float NdotH, float roughness);


float GeoSchlickGGX(float NdotV, float roughness);
float GeoSmith(float NdotV, float NdotL, float roughness);

vec3 FnelSchlick(const vec3& color, float cosTheta, float metalness);


#endif // PBR_FUNC_H
