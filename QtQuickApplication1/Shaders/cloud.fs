#version 330

out vec4 fragColor;
in vec3 col;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 cameraPos;
uniform vec3 position;

uniform vec4 color;
uniform int mode;

struct LightSource
{
   vec3 position;
   vec4 color;
};

uniform LightSource lights[10];
uniform int lightsCount;
uniform bool isLightSource;

uniform int albedoCount;
uniform sampler2D texture;

const float PI = 3.14159265359;

//	<https://www.shadertoy.com/view/4dS3Wd>
//	By Morgan McGuire @morgan3d, http://graphicscodex.com
//
float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise(float x) {
	float i = floor(x);
	float f = fract(x);
	float u = f * f * (3.0 - 2.0 * f);
	return mix(hash(i), hash(i + 1.0), u);
}

float noise(vec2 x) {
	vec2 i = floor(x);
	vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));

	// Simple 2D lerp using smoothstep envelope between the values.
	// return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
	//			mix(c, d, smoothstep(0.0, 1.0, f.x)),
	//			smoothstep(0.0, 1.0, f.y)));

	// Same code, with the clamps in smoothstep and common subexpressions
	// optimized away.
	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// This one has non-ideal tiling properties that I'm still tuning
float noise(vec3 x) {
	const vec3 step = vec3(110, 241, 171);

	vec3 i = floor(x);
	vec3 f = fract(x);
 
	// For performance, compute the base input to a 1D hash from the integer part of the argument and the 
	// incremental change to the 1D based on the 3D -> 1D wrapping
    float n = dot(i, step);

	vec3 u = f * f * (3.0 - 2.0 * f);
	return mix(mix(mix( hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
               mix(mix( hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}
float hit_sphere(vec3 center, float radius, vec3 dir, vec3 origin)
{
    vec3 oc = origin - center;
    float a = dot(dir, dir);
    float b = 2.0 * dot(oc, dir);
    float c = dot(oc,oc) - radius*radius;
    float discriminant = b*b - 4*a*c;
    if(discriminant < 0)
	{
        return -1.0;
    }
    else
	{
		float numerator = -b - sqrt(discriminant);
		if (numerator > 0.0)
		{
			return numerator / (2.0 * a);
		}

		numerator = -b + sqrt(discriminant);
		if( numerator > 0.0 )
		{
			return numerator / (2.0 * a);
		}
		else 
		{
			return -1.0f;
		}

	}
}
void main() 
{
	vec3 viewDir = normalize(FragPos - cameraPos);

	float radius = 4.1;
	float planetRadius = 3;

	float t = hit_sphere(position, radius, viewDir, FragPos);
	if(t > 0)
	{
		vec3 atmoEntry = FragPos+viewDir * t;
		//float alpha = noise(contact.xyz);

		float t2 = hit_sphere(position, radius, viewDir, atmoEntry + viewDir*0.001f);	
		vec3 atmoExit = atmoEntry + viewDir*t2;
		
		float t3 = hit_sphere(position, planetRadius, viewDir, FragPos);
		vec3 planetEntry = FragPos+viewDir * t3;

		vec3 closest = atmoExit;
		if(t3>0 && length(FragPos - planetEntry) < length(FragPos - atmoExit))
		{
			closest = planetEntry;
		}
		
		float ratio = length(atmoEntry-closest)/(2*radius);

		fragColor = vec4(0.5,0.5,1, ratio*ratio*ratio);	
	}
	else
	{
		fragColor = vec4(1,1,1, 0);
	}
}