#version 420
out vec4 fragColor;

in vec2 texcoord;

const float PI = 3.14159265359;

uniform vec3 origin;
uniform vec3 front;
uniform vec3 right;
uniform vec3 up;
uniform float fov;
uniform float aspectRatio;
uniform int width;
uniform int height;

uniform vec3 sunDir;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

vec3 getDirection()
{
   float h = 2.0f * tan(fov * 0.5f * PI / 180.0f);
   float x = (texcoord.x-0.5f)*aspectRatio*h;
   float y = (texcoord.y-0.5f)*h;
   return front+right*x+up*y;
}
float atan2(vec2 dir)
{
    float angle = asin(dir.x) > 0 ? acos(dir.y) : -acos(dir.y);
    return angle;
}

vec2 toUV(vec3 n)
{
    vec2 uv;

    uv.x = atan(-n.x, n.z);
    uv.x = (uv.x + PI / 2.0) / (PI * 2.0) + PI * (28.670 / 360.0);

    uv.y = acos(n.y) / PI;
    uv.y = 1 - uv.y;
    return uv;
}
vec3 getSky(vec2 uv, vec2 sunUV)
{
    float atmosphere = sqrt(1.0-uv.y);
    vec3 skyColor = vec3(0.2,0.4,0.8);
    sunUV.y = max(sunUV.y-0.5, 0)*2;
    float scatter = pow(sunUV.y, 1.0 / 15.0);
    scatter = 1.0 - clamp(scatter,0.8,1.0);
    
    vec3 scatterColor = mix(vec3(1.0),vec3(1.0,0.3,0.0) * 1.5,scatter);
    return mix(skyColor,vec3(scatterColor),atmosphere / 1.3);
    
}
vec3 getSun(vec2 uv, vec3 dir, vec3 sunDir)
{
	float sun = pow(max(dot(dir, -sunDir), 0), pow(2, 4));
    //sun = 0;
    sun = clamp(sun,0.0,1.0);
    
    float glow = sun;
    glow = clamp(glow,0.0,1.0);
    
    sun = pow(sun,100.0);
    sun *= 100.0;
    sun = clamp(sun,0.0,1.0);
    
    uv.y = max(uv.y-0.5, 0)*2;

    glow = pow(glow,6.0) * 1.0;
    glow = pow(glow,(uv.y));
    glow = clamp(glow,0.0,1.0);
    
    sun *= pow(dot(uv.y, uv.y), 1.0 / 1.65);
    
    glow *= pow(dot(uv.y, uv.y), 1.0 / 2.0);
    
    sun += glow;
    
    vec3 sunColor = vec3(1.0,0.6,0.05) * sun;
    
    return vec3(sunColor);
}
//----

void main() 
{
    vec3 direction = normalize(getDirection());
    vec2 uv = toUV(direction);// SampleSphericalMap(direction);
    vec2 sunUV = toUV(-sunDir);

    vec3 color = getSky(uv, sunUV);
    
    //fragColor = vec4(getSky(uv, sunUV)+getSun(uv, direction, sunDir), 1.0f);
    fragColor = vec4(color+getSun(uv, direction, sunDir), 1.0f);
}