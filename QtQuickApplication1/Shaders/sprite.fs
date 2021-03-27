#version 330
out vec4 fragColor;

in vec2 texcoord;
uniform sampler2D sprite;

const float PI = 3.14159265359;

void main() 
{
   fragColor = vec4(1,1,1, 1.0f);
}