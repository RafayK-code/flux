#version 450
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;

layout (location = 0) out vec3 v_Color;

layout (set = 0, binding = 0) uniform u_ViewProjection
{
    mat4 viewProjectionMatrix;
} vp;

layout (set = 0, binding = 1) uniform u_Model
{
    mat4 model;
} m;

void main()
{
    v_Color = a_Color;
    gl_Position = vp.viewProjectionMatrix * m.model * vec4(a_Position, 1.0);
}