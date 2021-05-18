#version 400

out vec4 fragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;
in vec4 FragPosLightSpace;

uniform vec3 cameraPos;

uniform vec4 color;
uniform vec4 ambient;
uniform float alpha;

uniform float roughness;

uniform sampler2D texture_background;
uniform bool useBackground;

uniform int albedoCount;
uniform sampler2D texture_diffuse[4];

uniform int specularCount;
uniform sampler2D texture_specular;

uniform int normalCount;
uniform sampler2D texture_normal;

uniform sampler2D shadowMap;

const float PI = 3.14159265359;

struct LightSource
{
   vec3 position;
   vec4 color;
   float intensity;
};

uniform LightSource lights[10];
uniform int lightsCount;


struct SpotLightSource
{
   vec3 position;
   vec3 direction;
   vec4 color;
   float cutOff;
   float outerCutOff;
   float intensity;
};

uniform SpotLightSource spotLights[10];
uniform int spotLightsCount;

struct DirectionalLight
{
   vec3 direction;
   vec4 color;
   float intensity;
};

uniform DirectionalLight dirLights[4];
uniform int dirLightsCount;

uniform bool wireframe;
uniform vec3 planePos;
float span = 5000;

vec3 drawCircle(vec2 pos, float rad)
{
   return vec3(1,0,0) * int(length(pos - TexCoords)<rad);
}
vec3 drawBlueCircle(vec2 pos, float rad)
{
   return vec3(0.02,0,0.8) * int(length(pos - TexCoords)<rad);
}

void main() 
{
   vec3 pos = -planePos*(1.0f/span);
   vec2 horizPos = vec2(pos.x, pos.z);
   horizPos.x = (horizPos.x+1.0f)*0.5f;
   horizPos.y = (horizPos.y+1.0f)*0.5f;

   vec3 result = vec3(0);
   
   result+= drawCircle(horizPos, 0.04);

   result+= drawBlueCircle(vec2(0.5,0.5), 0.04);

   fragColor = vec4(result + vec3(0.1), 1);
}