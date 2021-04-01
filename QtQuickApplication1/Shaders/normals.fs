#version 330

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
uniform sampler2D texture_diffuse;

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
   if(albedoCount != 0)
     return texture(texture_diffuse, TexCoords).xyz;
   return color.xyz;
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
vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);
// Returns a random number based on a vec3 and an int.
float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}
float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = -dirLights[0].direction;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    for(int x = 0; x <= 8; ++x)
    {
      int index = int(16.0*random(floor(FragPos.xyz*1000.0), x))%16;
      float pcfDepth = texture(shadowMap, projCoords.xy + poissonDisk[index]*texelSize).r; 
      shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
          
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
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
   float shadow = ShadowCalculation(FragPosLightSpace); 
   for(int i=0; i < dirLightsCount; ++i)
   {
      vec3 dirToLight = normalize(-dirLights[i].direction);

      vec3 diffuse = baseColor * vec3(dirLights[i].color);

      diffuse = diffuse * max(dot(dirToLight, norm), 0.0f);
      
      vec3 reflectDir = reflect(-dirToLight, norm);
      float spec = pow(max(dot(-dirToFrag, reflectDir), 0.0), 8.0);
      vec3 lightSpecular = dirLights[i].color.rgb * spec*getSpecular(); 

      result +=  ((diffuse + lightSpecular))*dirLights[i].intensity*(1.0 - shadow);
   }
   
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