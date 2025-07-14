#version 450
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;

layout (location = 0) out vec3 v_FragPos;
layout (location = 1) out vec3 v_FragNormal;

layout (set = 0, binding = 0) uniform u_Camera
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec3 viewPos;
} camera;

layout (set = 1, binding = 0) uniform u_Model
{
    mat4 model;
    mat4 normalMatrix;
} model;

void main()
{
    vec4 worldPos = model.model * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;
    
    // Transform normal to world space
    v_FragNormal = normalize(mat3(model.normalMatrix) * a_Normal);
    
    // Final position
    gl_Position = camera.projectionMatrix * camera.viewMatrix * worldPos;
}