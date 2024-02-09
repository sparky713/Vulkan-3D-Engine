#version 450

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec4 fragPosition;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor.x, fragColor.y, fragColor.z, fragColor.w * texture(texSampler, fragTexCoord).w);
}
