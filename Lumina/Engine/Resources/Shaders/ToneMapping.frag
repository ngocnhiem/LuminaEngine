#version 450 core
#pragma shader_stage(fragment)

#include "Includes/Common.glsl"

layout(set = 0, binding = 0) uniform sampler2D uHDRSceneColor;
layout(set = 0, binding = 1) uniform sampler2D uBayerDither;

layout(push_constant) uniform PushConstants
{
    float Exposure;
} PC;

layout(location = 0) in vec2 vTexCoord;
layout(location = 0) out vec4 oFragColor;

vec3 ACESFilm(vec3 x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 Uncharted2Tonemap(vec3 x)
{
    const float A = 0.15;
    const float B = 0.50;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.02;
    const float F = 0.30;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

void main()
{
    vec3 hdrColor = texture(uHDRSceneColor, vTexCoord).rgb;
    
    float exposure = 1.0;
    hdrColor *= exposure;

    // Tonemap
    vec3 toneMappedColor = ACESFilm(hdrColor);

    // Gamma correction
    toneMappedColor = pow(toneMappedColor, vec3(1.0 / 2.2));
    
    toneMappedColor += vec4(texture(uBayerDither, gl_FragCoord.xy / 8.0) / 32.0 - (1.0 / 256.0)).rgb;
    
    oFragColor = vec4(toneMappedColor, 1.0);
}