#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D deferredTexture;
uniform sampler2D forwardTexture;

void main()
{
    vec4 deferredColor = texture(deferredTexture, TexCoords);
    vec4 forwardColor = texture(forwardTexture, TexCoords);
    FragColor = deferredColor + forwardColor;
}




