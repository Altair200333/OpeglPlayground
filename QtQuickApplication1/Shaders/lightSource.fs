#version 330

out vec4 fragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 cameraPos;

uniform vec4 color;

const float PI = 3.14159265359;

void main() 
{
   fragColor = vec4(color.rgb, 1.0f);
}
