#version 330

out vec4 fragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

uniform vec4 color;

void main() 
{
   fragColor = vec4(color.rgb, 1.0f);
}