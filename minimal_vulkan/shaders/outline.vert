#version 450

layout (location = 0) in vec4 inPos;
layout (location = 3) in vec3 inNormal;

/*
layout (binding = 0) uniform UBO {
	mat4 projection;
	mat4 model;
	vec4 lightPos;
	float outlineWidth;
} ubo;
*/

// sally:
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
    //float outlineWidth = 0.02f;
    float outlineWidth = 0.05f;
    //float outlineWidth = 0.005f;

	// Extrude along normal
	//vec4 pos = vec4(inPos.xyz + inNormal * ubo.outlineWidth, inPos.w);
	//vec4 pos = vec4(inPos.xyz + inNormal * outlineWidth, inPos.w);
	vec3 pos = vec3(inPos.xyz + inNormal * outlineWidth); //sally1130
    //vec3 pos = vec3(inPos.x + gl_InstanceIndex * 0.5 + inNormal.x * outlineWidth, inPos.yz + inNormal.y * outlineWidth); //sally1128
    
	//gl_Position = ubo.proj * ubo.model * pos;
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(pos, 1.0); //sally1128
}
