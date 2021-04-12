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

vec2 SampleSphericalMap(vec3 direction)
{
   float theta = atan(direction.z, direction.x) * 180 / PI + 180;
   float alpha = atan(direction.y, sqrt(direction.x * direction.x + direction.z * direction.z)) * 180 / PI + 90;  
   float x = theta / 360;
   float y = alpha / 180;

   return vec2(x, 1-y);
}

vec3 getDiffuseColor()
{
   if(albedoCount == 0)
      return color.xyz;
   vec3 result = texture(texture_diffuse[0], TexCoords).xyz;
   for(int i=1;i<albedoCount;++i)
   {
      result = texture(texture_diffuse[i], TexCoords).xyz*result;
   }
   return result;
   
}

vec3 getNormal()
{
    vec3 normal_rgb = texture(texture_normal, TexCoords).rgb; 
    normal_rgb = normalize(normal_rgb * 2.0 - 1.0); 
    normal_rgb = normalize(TBN * normal_rgb); 
    return normalize(Normal)*(1-normalCount) + normal_rgb*normalCount; 
}
float getSpecular()
{
   return texture(texture_specular, TexCoords).r*specularCount + (1-specularCount);
}
float getRoughness()
{
   return texture(texture_specular, TexCoords).r/2 * specularCount + roughness*(1-specularCount);
}
float attenuation(float dist)
{
   return 1.0f / (1.0f + 0.01f * dist + 0.01f * dist*dist);
}

vec3 getLighting()
{
   vec3 dirToFrag = normalize(FragPos - cameraPos);
   vec3 norm = getNormal();
   
   vec3 result = getDiffuseColor();
   vec3 envColor = texture(texture_background, SampleSphericalMap(reflect(dirToFrag, norm))).xyz;
   
   result = (1-getRoughness())*(result + ambient.rgb*0.089f) + getRoughness()*envColor;
   return result;
}
void main() 
{
   if(wireframe)
   {
      fragColor = vec4(1, 0.682, 0, alpha);
   }
   else
   {
      fragColor = vec4(getLighting(), alpha);
   }
}