#version 330

out vec4 fragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

uniform vec3 cameraPos;

uniform vec4 color;
uniform vec4 ambient;

uniform float roughness;

uniform sampler2D texture_background;
uniform bool useBackground;

uniform int albedoCount;
uniform sampler2D texture_diffuse;

uniform int specularCount;
uniform sampler2D texture_specular;

uniform int normalCount;
uniform sampler2D texture_normal;

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
   if(albedoCount != 0)
     return texture(texture_diffuse, TexCoords).xyz;
   return color.xyz;
}

vec3 getNormal()
{
    if(normalCount == 0)
        return normalize(Normal);

    vec3 normal_rgb = texture(texture_normal, TexCoords).rgb; 
    normal_rgb = normalize(normal_rgb * 2.0 - 1.0); 
    normal_rgb = normalize(TBN * normal_rgb); 
    return normal_rgb; 
}
float getSpecular()
{
   if(specularCount == 0)
      return 1.0f;
   return texture(texture_specular, TexCoords).r;
}
float getRoughness()
{
   if(specularCount == 0)
      return roughness;
   return texture(texture_specular, TexCoords).r/2;
}
float attenuation(float dist)
{
   return 1.0f / (1.0f + 0.01f * dist + 0.01f * dist*dist);
}
vec3 getLighting()
{
   vec3 dirToFrag = normalize(FragPos - cameraPos);
   vec3 norm = getNormal();
   
   vec3 result = vec3(0);
   vec3 baseColor = getDiffuseColor();
   vec3 envColor = texture(texture_background, SampleSphericalMap(reflect(dirToFrag, norm))).xyz;
   
   for(int i=0; i < lightsCount; ++i)
   {
      vec3 dirToLight = normalize(lights[i].position - FragPos);

      vec3 diffuse = baseColor * vec3(lights[i].color);
      diffuse = diffuse * max(dot(dirToLight, norm), 0.0f);

      vec3 reflectDir = reflect(-dirToLight, norm);
      float spec = pow(max(dot(-dirToFrag, reflectDir), 0.0), 8.0);
      vec3 lightSpecular = lights[i].color.rgb * spec*getSpecular(); 
      result +=  ((diffuse + lightSpecular) * attenuation(length(lights[i].position - FragPos)))*lights[i].intensity;
   }

   for(int i=0; i < spotLightsCount; ++i)
   {
      vec3 dirToLight = normalize(spotLights[i].position - FragPos);
      vec3 diffuse = baseColor * spotLights[i].color.rgb;
      diffuse = diffuse * max(dot(dirToLight, norm), 0.0f);
         
      vec3 reflectDir = reflect(-dirToLight, norm);  
      float spec = pow(max(dot(-dirToFrag, reflectDir), 0.0), 32.0f);
      vec3 lightSpecular = spotLights[i].color.rgb * spec*getSpecular();

      float theta = dot(dirToLight, normalize(-spotLights[i].direction)); 
      float epsilon = (spotLights[i].cutOff - spotLights[i].outerCutOff);
      float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);
      diffuse  *= intensity;
      lightSpecular *= intensity;

      result += (diffuse + lightSpecular) * attenuation(length(spotLights[i].position - FragPos))*spotLights[i].intensity;
   }
   result = (1-getRoughness())*(result + ambient.rgb*0.089f) + getRoughness()*envColor;
   return result;
}
void main() 
{
   if(wireframe)
   {
      fragColor = vec4(1, 0.682, 0, 1.0f);
   }
   else
   {
      fragColor = vec4(getLighting(), 1.0f);
   }
}