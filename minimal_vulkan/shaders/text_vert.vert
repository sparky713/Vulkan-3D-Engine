#version 450

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Final Version
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
vec2 uvs[4] = vec2[](
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0)
);

vec2 texture_uvs[4] = vec2[](
    vec2(0.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0)
);

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inTexCoord;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec4 fragPosition;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragColor;

void main() {
    //------------------------------------------------------------------------------
    // Position
    //------------------------------------------------------------------------------
    gl_Position = vec4(inPosition.x + (inPosition.z * uvs[gl_VertexIndex].x), inPosition.y - (inPosition.w * uvs[gl_VertexIndex].y), 0, 1); // input + uv

    //------------------------------------------------------------------------------
    // Color
    //------------------------------------------------------------------------------
    fragColor = inColor;                // input

    //------------------------------------------------------------------------------
    // Texture Coordinate
    //------------------------------------------------------------------------------
    fragTexCoord = vec2(inTexCoord.x + (inTexCoord.z * texture_uvs[gl_VertexIndex].x), inTexCoord.y + (inTexCoord.w * texture_uvs[gl_VertexIndex].y)); // input
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Original Ver.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/*
layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inTexCoord;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec4 fragPosition;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragColor;

void main() {
    // vert id 0 = 0000, uv = (0, 0)
	// vert id 1 = 0001, uv = (1, 0)
	// vert id 2 = 0010, uv = (0, 1)
	// vert id 3 = 0011, uv = (1, 1)
    vec2 uv = vec2(gl_InstanceIndex & 1, (gl_InstanceIndex >> 1) & 1);

	// set the position for the vertex based on which vertex it is (uv)
	//output.pos = float(input.pos.x + (input.pos.z * uv.x), input.pos.y - (input.pos.w * uv.y), 0, 1);
    fragPosition = vec4(inPosition.x + (inPosition.z * uv.x), inPosition.y - (inPosition.w * uv.y), 0, 1);
    gl_Position = vec4(inPosition.x + (inPosition.z * uv.x), inPosition.y - (inPosition.w * uv.y), 0, 1);
    //output.color = input.color;
    fragColor = inColor;

	// set the texture coordinate based on which vertex it is (uv)
	//output.texCoord = short(input.texCoord.x + (input.texCoord.z * uv.x), input.texCoord.y + (input.texCoord.w * uv.y));
    fragTexCoord = vec2(inTexCoord.x + (inTexCoord.z * uv.x), inTexCoord.y + (inTexCoord.w * uv.y));
}*/
