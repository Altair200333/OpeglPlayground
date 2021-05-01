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

const float EARTH_RADIUS = 6300e3;
const float CLOUD_START = 800.0;
const float CLOUD_HEIGHT = 600.0;
const vec3 SUN_POWER = vec3(1.0,0.9,0.6) * 750.;
const vec3 LOW_SCATTER = vec3(1.0, 0.7, 0.5);

float intersectSphere(vec3 origin, vec3 dir, vec3 spherePos, float sphereRad)
{
	vec3 oc = origin - spherePos;
	float b = 2.0 * dot(dir, oc);
	float c = dot(oc, oc) - sphereRad*sphereRad;
	float disc = b * b - 4.0 * c;
	if (disc < 0.0)
		return -1.0;    
    float q = (-b + ((b < 0.0) ? -sqrt(disc) : sqrt(disc))) / 2.0;
	float t0 = q;
	float t1 = c / q;
	if (t0 > t1) {
		float temp = t0;
		t0 = t1;
		t1 = temp;
	}
	if (t1 < 0.0)
		return -1.0;
    
    return (t0 < 0.0) ? t1 : t0;
}
// From https://www.shadertoy.com/view/4sjBDG
float numericalMieFit(float costh)
{
    // This function was optimized to minimize (delta*delta)/reference in order to capture
    // the low intensity behavior.
    float bestParams[10];
    bestParams[0]=9.805233e-06;
    bestParams[1]=-6.500000e+01;
    bestParams[2]=-5.500000e+01;
    bestParams[3]=8.194068e-01;
    bestParams[4]=1.388198e-01;
    bestParams[5]=-8.370334e+01;
    bestParams[6]=7.810083e+00;
    bestParams[7]=2.054747e-03;
    bestParams[8]=2.600563e-02;
    bestParams[9]=-4.552125e-12;
    
    float p1 = costh + bestParams[3];
    vec4 expValues = exp(vec4(bestParams[1] *costh+bestParams[2], bestParams[5] *p1*p1, bestParams[6] *costh, bestParams[9] *costh));
    vec4 expValWeight= vec4(bestParams[0], bestParams[4], bestParams[7], bestParams[8]);
    return dot(expValues, expValWeight);
}
float hash( float n )
{
    return fract(sin(n)*43758.5453);
}

float hash( vec2 p ) {
    return fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453123);
}
float iTime = 100;
float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
#ifdef VOLUME_TEXTURES    
    return textureLod(iChannel2, (p+f+0.5)/32.0, 0.0).x;
#else
	vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;
	vec2 rg = textureLod(iChannel1, (uv+0.5)/256.0, 0.0).yx;
	return mix( rg.x, rg.y, f.z );
#endif
}

float noise( in vec2 p ) {
    vec2 i = floor( p );
    vec2 f = fract( p );	
	f = f*f*(3.0-2.0*f);
#ifdef NOISE_TEXTURES
    return textureLod(iChannel3, (i+f+vec2(0.5))/64.0, 0.0).x*2.0 -1.0;
#else    
    return -1.0+2.0*mix( mix( hash( i + vec2(0.0,0.0) ), 
                     hash( i + vec2(1.0,0.0) ), f.x),
                mix( hash( i + vec2(0.0,1.0) ), 
                     hash( i + vec2(1.0,1.0) ), f.x), f.y);
#endif
}
float fbm( vec3 p )
{
    mat3 m = mat3( 0.00,  0.80,  0.60,
              -0.80,  0.36, -0.48,
              -0.60, -0.48,  0.64 );    
    float f;
    f  = 0.5000*noise( p ); p = m*p*2.02;
    f += 0.2500*noise( p ); p = m*p*2.03;
    f += 0.1250*noise( p );
    return f;
}
float clouds(vec3 p, out float cloudHeight, bool fast)
{
#if 0
    float atmoHeight = length(p - vec3(0.0, -EARTH_RADIUS, 0.0)) - EARTH_RADIUS;
    cloudHeight = clamp((atmoHeight-CLOUD_START)/(CLOUD_HEIGHT), 0.0, 1.0);
    p.z += iTime*10.3;
    float largeWeather = clamp((textureLod(iChannel0, -0.00005*p.zx, 0.0).x-0.18)*5.0, 0.0, 2.0);
    p.x += iTime*8.3;
    float weather = largeWeather*max(0.0, textureLod(iChannel0, 0.0002*p.zx, 0.0).x-0.28)/0.72;
    weather *= smoothstep(0.0, 0.5, cloudHeight) * smoothstep(1.0, 0.5, cloudHeight);
    float cloudShape = pow(weather, 0.3+1.5*smoothstep(0.2, 0.5, cloudHeight));
    if(cloudShape <= 0.0)
        return 0.0;    
    p.x += iTime*12.3;
	float den= max(0.0, cloudShape-0.7*fbm(p*.01));
    if(den <= 0.0)
        return 0.0;
    
    if(fast)
    	return largeWeather*0.2*min(1.0, 5.0*den);

    p.y += iTime*15.2;
    den= max(0.0, den-0.2*fbm(p*0.05));
    return largeWeather*0.2*min(1.0, 5.0*den);
#else
    return 0.0;
#endif
}
float lightRay(vec3 p, float phaseFunction, float dC, float mu, vec3 sun_direction, float cloudHeight, bool fast)
{
    int nbSampleLight = fast ? 7 : 20;
	float zMaxl         = 600.;
    float stepL         = zMaxl/float(nbSampleLight);
    
    float lighRayDen = 0.0;    
    p += sun_direction*stepL*hash(dot(p, vec3(12.256, 2.646, 6.356)) + iTime);
    for(int j=0; j<nbSampleLight; j++)
    {
        float cloudHeight;
        lighRayDen += clouds( p + sun_direction*float(j)*stepL, cloudHeight, fast);
    }    
    if(fast)
    {
        return (0.5*exp(-0.4*stepL*lighRayDen) + max(0.0, -mu*0.6+0.3)*exp(-0.02*stepL*lighRayDen))*phaseFunction;
    }
    float scatterAmount = mix(0.008, 1.0, smoothstep(0.96, 0.0, mu));
    float beersLaw = exp(-stepL*lighRayDen)+0.5*scatterAmount*exp(-0.1*stepL*lighRayDen)+scatterAmount*0.4*exp(-0.02*stepL*lighRayDen);
    return beersLaw * phaseFunction * mix(0.05 + 1.5*pow(min(1.0, dC*8.5), 0.3+5.5*cloudHeight), 1.0, clamp(lighRayDen*0.4, 0.0, 1.0));
}
vec3 skyRay(vec3 org, vec3 dir, vec3 sun_direction, bool fast)
{
    const float ATM_START = EARTH_RADIUS+CLOUD_START;
	const float ATM_END = ATM_START+CLOUD_HEIGHT;
    
    int nbSample = fast ? 13 : 35;   
    vec3 color = vec3(0.0);
	float distToAtmStart = intersectSphere(org, dir, vec3(0.0, -EARTH_RADIUS, 0.0), ATM_START);
    float distToAtmEnd = intersectSphere(org, dir, vec3(0.0, -EARTH_RADIUS, 0.0), ATM_END);
    vec3 p = org + distToAtmStart * dir;    
    float stepS = (distToAtmEnd-distToAtmStart) / float(nbSample);    
    float T = 1.;    
    float mu = dot(sun_direction, dir);
    return vec3(mu);
    float phaseFunction = numericalMieFit(mu);
    p += dir*stepS*hash(dot(dir, vec3(12.256, 2.646, 6.356)) + iTime);
    if(dir.y > 0.015)
	for(int i=0; i<nbSample; i++)
	{        
        float cloudHeight;
		float density = clouds(p, cloudHeight, fast);
		if(density>0.0f)
		{
			float intensity = lightRay(p, phaseFunction, density, mu, sun_direction, cloudHeight, fast);        
            vec3 ambient = (0.5 + 0.6*cloudHeight)*vec3(0.2, 0.5, 1.0)*6.5 + vec3(0.8) * max(0.0, 1.0-2.0*cloudHeight);
            vec3 radiance = ambient + SUN_POWER*intensity;
            radiance*=density;			
            color += T*(radiance - radiance * exp(-density * stepS)) / density;   // By Seb Hillaire                  
            T *= exp(-density*stepS);            
			if( T <= 0.05)
				break;
        }
		p += dir*stepS;
	}
        
    if(!fast)
    {
        vec3 pC = org + intersectSphere(org, dir, vec3(0.0, -EARTH_RADIUS, 0.0), ATM_END+1000.0)*dir;
    	color += T*vec3(3.0)*max(0.0, fbm(vec3(1.0, 1.0, 1.8)*pC*0.002)-0.4);
    }
	vec3 background = 6.0*mix(vec3(0.2, 0.52, 1.0), vec3(0.8, 0.95, 1.0), pow(0.5+0.5*mu, 15.0))+mix(vec3(3.5), vec3(0.0), min(1.0, 2.3*dir.y));
    if(!fast) 	background += T*vec3(1e4*smoothstep(0.9998, 1.0, mu));
    color += background * T;
    
    return color;
}
float HenyeyGreenstein(float mu, float inG)
{
	return (1.-inG * inG)/(pow(1.+inG*inG - 2.0 * inG*mu, 1.5)*4.0* PI);
}
void main() 
{
   vec3 direction = normalize(getDirection());
   vec2 uv = toUV(direction);// SampleSphericalMap(direction);
   vec2 sunUV = toUV(-sunDir);

   vec3 color = skyRay(origin, direction, sunDir, false);
   float fogDistance = intersectSphere(origin, direction, vec3(0.0, -EARTH_RADIUS, 0.0), EARTH_RADIUS+160.0);
   float mu = dot(sunDir, direction);

   float fogPhase = 0.5*HenyeyGreenstein(mu, 0.7)+0.5*HenyeyGreenstein(mu, -0.6);    

   vec3 res = mix(fogPhase*0.1*LOW_SCATTER*SUN_POWER+10.0*vec3(0.55, 0.8, 1.0), color, exp(-0.0003*fogDistance));
   //fragColor = vec4(getSky(uv, sunUV)+getSun(uv, direction, sunDir), 1.0f);
   fragColor = vec4(res+getSun(uv, direction, sunDir), 1.0f);
}