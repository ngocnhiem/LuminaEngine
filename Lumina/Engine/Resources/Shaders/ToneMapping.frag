#version 450 core
#pragma shader_stage(fragment)

#include "Includes/Common.glsl"

layout(set = 0, binding = 0) uniform sampler2D uHDRSceneColor;

layout(push_constant) uniform PushConstants
{
    float Exposure;
    float Time;
};

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

vec3 ScreenSpaceDither(vec2 vScreenPos)
{
    // Iestyn's RGB dither (7 asm instructions) from Portal 2 X360, slightly modified.
    vec3 vDither = vec3(dot(vec2(171.0, 231.0), vScreenPos.xy + Time));
    vDither.rgb = fract(vDither.rgb / vec3(103.0, 71.0, 97.0)) - vec3(0.5, 0.5, 0.5);
    return (vDither.rgb / 255.0) * 0.375;
}

void main()
{
    vec3 hdrColor = texture(uHDRSceneColor, vTexCoord).rgb;
    
    hdrColor *= Exposure;

    // Tonemap
    vec3 toneMappedColor = ACESFilm(hdrColor);

    // Gamma correction
    toneMappedColor = pow(toneMappedColor, vec3(1.0 / 2.2));
    
    toneMappedColor += ScreenSpaceDither(gl_FragCoord.xy);
    
    oFragColor = vec4(toneMappedColor, 1.0);
}