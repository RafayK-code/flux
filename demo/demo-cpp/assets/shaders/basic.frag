#version 450
layout (location = 0) in vec3 v_Color;
layout (location = 1) in vec2 v_TexCoord;

layout (location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform TimeUBO
{
    float time;
} ubo;

layout(set = 0, binding = 1) uniform sampler2D texSampler;

void main()
{
    /*
    // Normalize sine wave to [0,1]
    float t = 0.5 + 0.5 * sin(ubo.time);

    // Define two colors to interpolate between (you can customize these)
    vec3 colorA = vec3(1.0, 0.0, 0.0); // red
    vec3 colorB = vec3(0.0, 0.0, 1.0); // blue

    // Interpolate between colorA and colorB based on t
    vec3 interpolatedColor = mix(colorA, colorB, t);

    // Multiply input vertex color by interpolated color
    outColor = vec4(interpolatedColor, 1.0);
    */

    vec4 texColor = texture(texSampler, v_TexCoord);
    outColor = texColor;
}