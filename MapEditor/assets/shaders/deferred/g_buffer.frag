#version 460 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D diffuseTexture;
uniform sampler2D texture_specular1;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main()
{    
    // Store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    
    // Store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    
    // Store the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(diffuseTexture, TexCoords).rgb;
    
    // Store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
}
