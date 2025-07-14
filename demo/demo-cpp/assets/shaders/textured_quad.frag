#version 450
layout (location = 0) in vec4 v_Color;
layout (location = 1) in vec2 v_TexCoord;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 1) uniform sampler2D u_Texture;

void main()
{
    vec4 texColor = texture(u_Texture, v_TexCoord);
    outColor = texColor * v_Color;
}