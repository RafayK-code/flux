#version 450
layout (location = 0) in vec3 v_FragPos;
layout (location = 1) in vec3 v_FragNormal;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform u_Camera
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec3 viewPos;
} camera;

layout(set = 0, binding = 1) uniform u_Light {
    vec3 lightPos;
    vec3 lightColor;
    float lightIntensity;
} light;

// Material properties
layout(set = 1, binding = 1) uniform u_Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;

void main()
{
    vec3 normal = normalize(v_FragNormal);

    vec3 lightDir = normalize(light.lightPos - v_FragPos);
    vec3 viewDir = normalize(camera.viewPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 ambient = material.ambient;
    
    // Diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = material.diffuse * diff * light.lightColor;
    
    // Specular component
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.lightColor;
    
    // Combine all components
    vec3 result = (ambient + diffuse + specular) * light.lightIntensity;

    outColor = vec4(result, 1.0);
}