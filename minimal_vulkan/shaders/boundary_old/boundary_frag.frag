// Fragment shader
#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragNormal;

//layout(set = 0, binding = 0) uniform sampler2D textureSampler;
//layout(binding = 1) uniform sampler2D textureSampler;
layout(binding = 1) uniform sampler2D inputTexture;


const float normalThreshold = 0.1;
const float edgeThreshold = 0.1; 
const float gradientThreshold = 0.02;
const float dilationRadius = 1.0f;

void main() {
    // Get the texture coordinates
    vec2 texCoord = gl_FragCoord.xy / textureSize(inputTexture, 0);
    
    // Sample the texture
    vec4 center = texture(inputTexture, texCoord);
    
    // Apply the Sobel operator to detect edges
    float edgeDetection = (
        -1.0 * texture(inputTexture, texCoord + vec2(-1, -1) / textureSize(inputTexture, 0)).r +
        -2.0 * texture(inputTexture, texCoord + vec2(0, -1) / textureSize(inputTexture, 0)).r +
        -1.0 * texture(inputTexture, texCoord + vec2(1, -1) / textureSize(inputTexture, 0)).r +
        -2.0 * texture(inputTexture, texCoord + vec2(-1, 0) / textureSize(inputTexture, 0)).r +
         12.0 * center.r +
        -2.0 * texture(inputTexture, texCoord + vec2(1, 0) / textureSize(inputTexture, 0)).r +
        -1.0 * texture(inputTexture, texCoord + vec2(-1, 1) / textureSize(inputTexture, 0)).r +
        -2.0 * texture(inputTexture, texCoord + vec2(0, 1) / textureSize(inputTexture, 0)).r +
        -1.0 * texture(inputTexture, texCoord + vec2(1, 1) / textureSize(inputTexture, 0)).r
    );

    // Apply the effect to the output color
    if (edgeDetection > 0.1) {
        outColor = vec4(1.0, 0.0, 0.0, 1.0); // Highlight edges with red color
    } else {
        //outColor = center; // Keep the original color
        outColor = texture(inputTexture, fragTexCoord);
    }
}

/*
RED FOR EDGE, REST IS NORMAL
void main() {
    // Calculate the gradient of the depth values along X and Y using Sobel filter
    float depthX = (
        -texture(textureSampler, fragTexCoord + vec2(1.0, -1.0) / textureSize(textureSampler, 0)).r +
         texture(textureSampler, fragTexCoord + vec2(1.0,  0.0) / textureSize(textureSampler, 0)).r +
        -texture(textureSampler, fragTexCoord + vec2(1.0,  1.0) / textureSize(textureSampler, 0)).r +
        -texture(textureSampler, fragTexCoord + vec2(-1.0, -1.0) / textureSize(textureSampler, 0)).r +
         texture(textureSampler, fragTexCoord + vec2(-1.0,  0.0) / textureSize(textureSampler, 0)).r +
        -texture(textureSampler, fragTexCoord + vec2(-1.0,  1.0) / textureSize(textureSampler, 0)).r
    ) / 8.0;

    float depthY = (
        -texture(textureSampler, fragTexCoord + vec2(-1.0, 1.0) / textureSize(textureSampler, 0)).r +
         texture(textureSampler, fragTexCoord + vec2( 0.0, 1.0) / textureSize(textureSampler, 0)).r +
        -texture(textureSampler, fragTexCoord + vec2( 1.0, 1.0) / textureSize(textureSampler, 0)).r +
        -texture(textureSampler, fragTexCoord + vec2(-1.0, -1.0) / textureSize(textureSampler, 0)).r +
         texture(textureSampler, fragTexCoord + vec2( 0.0, -1.0) / textureSize(textureSampler, 0)).r +
        -texture(textureSampler, fragTexCoord + vec2( 1.0, -1.0) / textureSize(textureSampler, 0)).r
    ) / 8.0;

    // Calculate the depth gradient magnitude
    float depthGradient = length(vec2(depthX, depthY));

    // Calculate the gradient magnitude of the normal
    float normalGradientMagnitude = length(fragNormal);

    // Determine if the fragment is an edge based on depth gradient and normal gradient
    bool isEdgeFragment = depthGradient > edgeThreshold && normalGradientMagnitude < edgeThreshold;

    // Highlight boundaries in red, leave the rest as is
    
    vec3 edgeColor = vec3(1.0, 0.0, 0.0);
    //vec3 finalColor = isEdgeFragment ? edgeColor : fragColor;

    if (isEdgeFragment) {
        outColor = vec4(edgeColor, 1.0);
    }
    else {
        outColor = texture(textureSampler, fragTexCoord);
    }    
}
*/

/*
TEST DILATION
void main() {
    // Apply dilation to make the boundary thicker
    bool isBoundaryFragment = false;

    for (float dx = -dilationRadius; dx <= dilationRadius; dx += 1.0) {
        for (float dy = -dilationRadius; dy <= dilationRadius; dy += 1.0) {
            vec2 offset = vec2(dx, dy);
            float dilationSample = texture(textureSampler, fragTexCoord + offset / textureSize(textureSampler, 0)).r;
            isBoundaryFragment = isBoundaryFragment || dilationSample > 0.0;
        }
    }

    // Mix the fragment color with red based on the boundary condition
    vec3 outlineColor = vec3(1.0, 0.0, 0.0);
    vec3 finalColor = mix(fragColor, outlineColor, float(isBoundaryFragment));

    outColor = vec4(finalColor, 1.0);
}
*/

/*
DILATION + EDGE HIGHLIGHT (RED)
void main() {
    // Calculate the gradient of the depth values along X and Y using Sobel filter
    float depthX = (
        -texture(textureSampler, fragPosition.xy + vec2(1.0, -1.0)).r +
         texture(textureSampler, fragPosition.xy + vec2(1.0,  0.0)).r +
        -texture(textureSampler, fragPosition.xy + vec2(1.0,  1.0)).r +
        -texture(textureSampler, fragPosition.xy + vec2(-1.0, -1.0)).r +
         texture(textureSampler, fragPosition.xy + vec2(-1.0,  0.0)).r +
        -texture(textureSampler, fragPosition.xy + vec2(-1.0,  1.0)).r
    ) / 8.0;

    float depthY = (
        -texture(textureSampler, fragPosition.xy + vec2(-1.0, 1.0)).r +
         texture(textureSampler, fragPosition.xy + vec2( 0.0, 1.0)).r +
        -texture(textureSampler, fragPosition.xy + vec2( 1.0, 1.0)).r +
        -texture(textureSampler, fragPosition.xy + vec2(-1.0, -1.0)).r +
         texture(textureSampler, fragPosition.xy + vec2( 0.0, -1.0)).r +
        -texture(textureSampler, fragPosition.xy + vec2( 1.0, -1.0)).r
    ) / 8.0;

    // Calculate the length of the gradient vector
    float gradientMagnitude = length(vec2(depthX, depthY));

    // Calculate the normal gradient magnitude
    float normalGradientMagnitude = length(fragNormal);

    // Determine if the fragment is on a boundary based on depth gradient and normal gradient
    bool isBoundaryFragment = gradientMagnitude > gradientThreshold && normalGradientMagnitude < normalThreshold;

    // Apply dilation to make the boundary thicker
    for (float dx = -dilationRadius; dx <= dilationRadius; dx += 1.0) {
        for (float dy = -dilationRadius; dy <= dilationRadius; dy += 1.0) {
            vec2 offset = vec2(dx, dy);
            float dilationSample = texture(textureSampler, fragPosition.xy + offset).r;
            isBoundaryFragment = isBoundaryFragment || dilationSample > 0.0;
        }
    }

    vec4 fragColorNew = vec4(fragColor.x, fragColor.y, fragColor.z, 1.0);

    // Mix the fragment color with red based on the boundary condition
    //vec4 boundaryColor = mix(fragColor, vec4(1.0, 0.0, 0.0, fragColor.a), float(isBoundaryFragment));
    vec4 boundaryColor = mix(fragColorNew, vec4(1.0, 0.0, 0.0, fragColorNew.a), float(isBoundaryFragment));

    outColor = boundaryColor;
}
*/
/*
HIGHLIGHT EDGE AS RED
#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragNormal;

//layout(location = 0) in vec3 fragPosition;
//layout(location = 1) in vec3 fragNormal; // Receive the vertex normal as an input

layout(set = 0, binding = 1) uniform sampler2D textureSampler;
layout(set = 0, binding = 2) buffer BoundaryVertices {
    vec3 boundaryVertices[];
};

mat3 sx = mat3( 
    1.0, 2.0, 1.0, 
    0.0, 0.0, 0.0, 
   -1.0, -2.0, -1.0 
);
mat3 sy = mat3( 
    1.0, 0.0, -1.0, 
    2.0, 0.0, -2.0, 
    1.0, 0.0, -1.0 
);

void main() {
    vec3 normal = normalize(fragNormal);

    // Calculate the gradient of the depth values along X and Y using Sobel filter
    vec3 gradientX = vec3(
        texture(textureSampler, fragPosition.xy + vec2(-1.0, -1.0)).r,
        texture(textureSampler, fragPosition.xy + vec2(-1.0,  0.0)).r,
        texture(textureSampler, fragPosition.xy + vec2(-1.0,  1.0)).r
    ) -
    vec3(
        texture(textureSampler, fragPosition.xy + vec2( 1.0, -1.0)).r,
        texture(textureSampler, fragPosition.xy + vec2( 1.0,  0.0)).r,
        texture(textureSampler, fragPosition.xy + vec2( 1.0,  1.0)).r
    );

    vec3 gradientY = vec3(
        texture(textureSampler, fragPosition.xy + vec2(-1.0, -1.0)).r,
        texture(textureSampler, fragPosition.xy + vec2( 0.0, -1.0)).r,
        texture(textureSampler, fragPosition.xy + vec2( 1.0, -1.0)).r
    ) -
    vec3(
        texture(textureSampler, fragPosition.xy + vec2(-1.0,  1.0)).r,
        texture(textureSampler, fragPosition.xy + vec2( 0.0,  1.0)).r,
        texture(textureSampler, fragPosition.xy + vec2( 1.0,  1.0)).r
    );

    float depthCenter = fragPosition.z;
    float depthX = dot(texture(textureSampler, fragPosition.xy + gradientX.xy).rgb, vec3(0.2989, 0.5870, 0.1140));
    float depthY = dot(texture(textureSampler, fragPosition.xy + gradientY.xy).rgb, vec3(0.2989, 0.5870, 0.1140));

    vec3 gradient = vec3(depthX, depthY, 1.0);
    float boundaryIntensity = length(gradient);

    // Debugging: Encode depth gradient as color
    vec3 depthGradientColor = vec3(depthX, depthY, 1.0);

    // Determine if the vertex is on the boundary
    int vertexIndex = int(gl_FragCoord.x) + int(gl_FragCoord.y) * int(textureSize(textureSampler, 0).x);
    bool isBoundaryVertex = boundaryVertices[vertexIndex] == fragPosition;

    // Debugging: Encode boundary condition as color
    vec3 boundaryColor = isBoundaryVertex ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 1.0, 0.0);

    // Combine debugging colors and output
    vec3 finalColor = depthGradientColor + boundaryColor;
    outColor = vec4(finalColor, 1.0);

    // Highlight the boundary with a red color
    //vec3 finalColor = isBoundaryVertex ? vec3(1.0, 0.0, 0.0) : vec3(1.0);
    //outColor = vec4(finalColor, 1.0);
}
*/
