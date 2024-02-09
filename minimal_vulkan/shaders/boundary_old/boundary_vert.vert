// Vertex shader (identical to 27_shader_depth.vert)
#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragNormal;

void main() {
    vec3 position = vec3(inPosition.x + gl_InstanceIndex * 0.5, inPosition.y, inPosition.z);
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(position, 1.0);

    fragPosition = inPosition;
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragNormal = inNormal;
}
