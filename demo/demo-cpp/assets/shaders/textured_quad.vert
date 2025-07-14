#version 450
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord;

layout (location = 0) out vec4 v_Color;
layout (location = 1) out vec2 v_TexCoord;

layout (set = 0, binding = 0) uniform u_ViewProjection
{
    mat4 viewProjectionMatrix;
} vp;

void main()
{
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    gl_Position = vp.viewProjectionMatrix * vec4(a_Position, 1.0);
}