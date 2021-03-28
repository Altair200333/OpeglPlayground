#version 330

in vec3 posAttr;
in vec3 colAttr;
in vec3 normalAttr;
in vec2 aTexCoords;
in vec3 aTangent;
in vec3 aBitangent;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() 
{
    gl_Position = lightSpaceMatrix * model * vec4(posAttr, 1.0);
}