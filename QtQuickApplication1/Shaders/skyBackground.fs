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

vec3 traceWater(vec3 dir);

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

    vec3 waterCol = traceWater(rd);
    fragColor = vec4((col+getSun(uv, rd, sunDir))*int(uv.y>=0.49)+waterCol*int(uv.y<0.49), 1.0);
}

///---------------------------------------------
///---------------------------------------------
///---------------------------------------------
///---------------------------------------------

const int NUM_STEPS = 1;
const float EPSILON	= 1e-3;
#define EPSILON_NRM (0.1 / 100.0f)
//#define AA

// sea
const int ITER_GEOMETRY = 2;
const int ITER_FRAGMENT = 2;
const float SEA_HEIGHT = 0.3;
const float SEA_CHOPPY = 4.1;
const float SEA_SPEED = 0.8;
const float SEA_FREQ = 0.16;
const vec3 SEA_BASE = vec3(0.0,0.09,0.18);
const vec3 SEA_WATER_COLOR = vec3(0.8,0.9,0.6)*0.6;
#define SEA_TIME (1.0 + sin(0.0001*PI*(int(iTime)%10000)) * SEA_SPEED)
const mat2 octave_m = mat2(1.6,1.2,-1.2,1.6);

// math
mat3 fromEuler(vec3 ang) {
	vec2 a1 = vec2(sin(ang.x),cos(ang.x));
    vec2 a2 = vec2(sin(ang.y),cos(ang.y));
    vec2 a3 = vec2(sin(ang.z),cos(ang.z));
    mat3 m;
    m[0] = vec3(a1.y*a3.y+a1.x*a2.x*a3.x,a1.y*a2.x*a3.x+a3.y*a1.x,-a2.y*a3.x);
	m[1] = vec3(-a2.y*a1.x,a1.y*a2.y,a2.x);
	m[2] = vec3(a3.y*a1.x*a2.x+a1.y*a3.x,a1.x*a3.x-a1.y*a3.y*a2.x,a2.y*a3.y);
	return m;
}
float hash( vec2 p ) {
	float h = dot(p,vec2(127.1,311.7));	
    return fract(sin(h)*43758.5453123);
}
float noise( in vec2 p )
 {
    vec2 i = floor( p );
    vec2 f = fract( p );	
	vec2 u = f*f*(3.0-2.0*f);
    return -1.0+2.0*mix( mix( hash( i + vec2(0.0,0.0) ), 
                     hash( i + vec2(1.0,0.0) ), u.x),
                mix( hash( i + vec2(0.0,1.0) ), 
                     hash( i + vec2(1.0,1.0) ), u.x), u.y);
}
float iTime = 0;
// lighting
float diffuse(vec3 n,vec3 l,float p) {
    return pow(dot(n,l) * 0.4 + 0.6,p);
}
float specular(vec3 n,vec3 l,vec3 e,float s) {    
    float nrm = (s + 8.0) / (PI * 8.0);
    return pow(max(dot(reflect(e,n),l),0.0),s) * nrm;
}

// sky
vec3 getSkyColor(vec3 e) {
    e.y = (max(e.y,0.0)*0.8+0.2)*0.8;
    return vec3(pow(1.0-e.y,2.0), 1.0-e.y, 0.6+(1.0-e.y)*0.4) * 1.1;
}

// sea
float sea_octave(vec2 uv, float choppy) {
    uv += noise(uv);        
    vec2 wv = 1.0-abs(sin(uv));
    vec2 swv = abs(cos(uv));    
    wv = mix(wv,swv,wv);
    return pow(1.0-pow(wv.x * wv.y,0.65),choppy);
}

float map(vec3 p) {
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    vec2 uv = p.xz; uv.x *= 0.75;
    
    float d, h = 0.0;    
    for(int i = 0; i < ITER_GEOMETRY; i++) {        
    	d = sea_octave((uv+SEA_TIME)*freq,choppy);
    	d += sea_octave((uv-SEA_TIME)*freq,choppy);
        h += d * amp;        
    	uv *= octave_m; freq *= 1.9; amp *= 0.22;
        choppy = mix(choppy,1.0,0.2);
    }
    return p.y - h;
}

float map_detailed(vec3 p) {
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    vec2 uv = p.xz; uv.x *= 0.75;
    
    float d, h = 0.0;    
    for(int i = 0; i < ITER_FRAGMENT; i++) {        
    	d = sea_octave((uv+SEA_TIME)*freq,choppy);
    	d += sea_octave((uv-SEA_TIME)*freq,choppy);
        h += d * amp;        
    	uv *= octave_m; freq *= 1.9; amp *= 0.22;
        choppy = mix(choppy,1.0,0.2);
    }
    return p.y - h;
}

vec3 getSeaColor(vec3 p, vec3 n, vec3 l, vec3 eye, vec3 dist) {  
    float fresnel = clamp(1.0 - dot(n,-eye), 0.0, 1.0);
    fresnel = pow(fresnel,3.0) * 0.5;
        
    vec3 reflected = getSkyColor(reflect(eye,n));    
    vec3 refracted = SEA_BASE + diffuse(n,l,80.0) * SEA_WATER_COLOR * 0.12; 
    
    vec3 color = mix(refracted,reflected,fresnel);
    
    float atten = max(1.0 - dot(dist,dist) * 0.001, 0.0);
    color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;
    
    color += vec3(specular(n,l,eye,60.0));
    
    return color;
}

// tracing
vec3 getNormal(vec3 p, float eps) {
    vec3 n;
    n.y = map_detailed(p);    
    n.x = map_detailed(vec3(p.x+eps,p.y,p.z)) - n.y;
    n.z = map_detailed(vec3(p.x,p.y,p.z+eps)) - n.y;
    n.y = eps;
    return normalize(n);
}

float heightMapTracing(vec3 ori, vec3 dir, out vec3 p) {  
    float tm = 0.0;
    float tx = 1000.0;    
    float hx = map(ori + dir * tx);
    if(hx > 0.0) return tx;   
    float hm = map(ori + dir * tm);    
    float tmid = 0.0;
    for(int i = 0; i < NUM_STEPS; i++) {
        tmid = mix(tm,tx, hm/(hm-hx));                   
        p = ori + dir * tmid;                   
    	float hmid = map(p);
		if(hmid < 0.0) {
        	tx = tmid;
            hx = hmid;
        } else {
            tm = tmid;
            hm = hmid;
        }
    }
    return tmid;
}
vec3 traceWater(vec3 dir)
{
   dir = normalize(dir);
   vec3 p;
   heightMapTracing(origin, dir, p);
   vec3 dist = p - origin;
   vec3 n = getNormal(p, dot(dist,dist) * 0.0003);
   //fragColor = vec4(n, 1.0);
   //return;
   vec3 light = normalize(vec3(0.0,1.0,0.8)); 
             
    // color
   vec3 res = mix(
        getSkyColor(dir),
        getSeaColor(p,n,light,dir,dist),
    	pow(smoothstep(0.0,-0.02,dir.y),0.2));

   return pow(res,vec3(0.65));
}