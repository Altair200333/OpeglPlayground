#version 330

out vec4 fragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

uniform int id;

void main() 
{
   fragColor = vec4(id, 0.682, 0, 1.0f);
}