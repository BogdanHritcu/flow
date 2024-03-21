#version 460 core

in vec4 vColor;
in vec2 vTexCoords;
in float vTexLayer;

out vec4 outColor;

uniform sampler2DArray textureArray;

void main()
{
    outColor = texture(textureArray, vec3(vTexCoords, vTexLayer)) * vColor;
}
