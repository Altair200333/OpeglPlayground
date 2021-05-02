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
#define AA 1   // make this 2 or even 3 if you have a really powerful GPU

#define USE_SMOOTH_NOISE 1   // enable to prevent discontinuities

#define SC (250.0)

// value noise, and its analytical derivatives
vec3 noised( in vec2 x )
{
    vec2 f = fract(x);
    #if USE_SMOOTH_NOISE==0
    vec2 u = f*f*(3.0-2.0*f);
    vec2 du = 6.0*f*(1.0-f);
    #else
    vec2 u = f*f*f*(f*(f*6.0-15.0)+10.0);
    vec2 du = 30.0*f*f*(f*(f-2.0)+1.0);
    #endif

#if 1
    // texel fetch version
    ivec2 p = ivec2(floor(x));
    float a = texelFetch( iChannel0, (p+ivec2(0,0))&255, 0 ).x;
	float b = texelFetch( iChannel0, (p+ivec2(1,0))&255, 0 ).x;
	float c = texelFetch( iChannel0, (p+ivec2(0,1))&255, 0 ).x;
	float d = texelFetch( iChannel0, (p+ivec2(1,1))&255, 0 ).x;
#else    
    // texture version    
    vec2 p = floor(x);
	float a = textureLod( iChannel0, (p+vec2(0.5,0.5))/256.0, 0.0 ).x;
	float b = textureLod( iChannel0, (p+vec2(1.5,0.5))/256.0, 0.0 ).x;
	float c = textureLod( iChannel0, (p+vec2(0.5,1.5))/256.0, 0.0 ).x;
	float d = textureLod( iChannel0, (p+vec2(1.5,1.5))/256.0, 0.0 ).x;
#endif
    
	return vec3(a+(b-a)*u.x+(c-a)*u.y+(a-b-c+d)*u.x*u.y,
				du*(vec2(b-a,c-a)+(a-b-c+d)*u.yx));
}
const mat2 m2 = mat2(0.8,-0.6,0.6,0.8);

float fbm( vec2 p )
{
    float f = 0.0;
    f += 0.5000*texture( iChannel0, p/256.0 ).x; p = m2*p*2.02;
    f += 0.2500*texture( iChannel0, p/256.0 ).x; p = m2*p*2.03;
    f += 0.1250*texture( iChannel0, p/256.0 ).x; p = m2*p*2.01;
    f += 0.0625*texture( iChannel0, p/256.0 ).x;
    return f/0.9375;
}
void main() 
{
    vec3 rd = normalize(getDirection());
    vec2 uv = toUV(rd);// SampleSphericalMap(direction);
    vec2 sunUV = toUV(-sunDir);

    float sundot = clamp(dot(rd,-sunDir),0.0,1.0);
    // sky		
    vec3 col = vec3(0.3,0.5,0.85) - rd.y*rd.y*0.5;
    col = mix( col, 0.85*vec3(0.7,0.75,0.85), pow( 1.0-max(rd.y,0.0), 4.0 ) );
     // sun
	//col += 0.25*vec3(1.0,0.7,0.4)*pow( sundot,5.0 );
	//col += 0.25*vec3(1.0,0.8,0.6)*pow( sundot,64.0 );
	//col += 0.2*vec3(1.0,0.8,0.6)*pow( sundot,512.0 );
     // clouds
	vec2 sc = origin.xz + rd.xz*(SC*1000.0-origin.y)/rd.y;
	col = mix( col, vec3(1.0,0.95,1.0), 0.5*smoothstep(0.5,0.8,fbm(0.0005*sc/SC)) );
     // horizon
    col = mix( col, 0.68*vec3(0.4,0.65,1.0), pow( 1.0-max(rd.y,0.0), 16.0 ) );
     // sun scatter
    col += 0.3*vec3(1.0,0.7,0.3)*pow( sundot, 8.0 );

    // gamma
	col = sqrt(col);
    fragColor = vec4(col+getSun(uv, rd, sunDir), 1.0);
}