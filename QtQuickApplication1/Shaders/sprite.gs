#version 330
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

out vec2 texcoord;
uniform int w;
uniform int h;

uniform int viewportW;
uniform int viewportH;

uniform int x;
uniform int y;

uniform bool useRel;
uniform float relX;
uniform float relY;

void main() 
{
    float width = float(w)/viewportW;
    float height = float(h)/viewportH;
    float _x, _y;
    if(!useRel)
    {
        _x = (float(x)/viewportW-0.5f)*2;
        _y = (float(y)/viewportH-0.5f)*2;
    }
    else
    {
        _x = relX;
        _y = relY;
    }

    gl_Position = vec4(_x+ width/2, _y+height/2, 0.9999, 1.0 );
    texcoord = vec2( 1.0, 1.0 );
    EmitVertex();

    gl_Position = vec4(_x-width/2, _y+height/2, 0.9999, 1.0 );
    texcoord = vec2( 0.0, 1.0 ); 
    EmitVertex();

    gl_Position = vec4( _x+width/2,_y-height/2, 0.9999, 1.0 );
    texcoord = vec2( 1.0, 0.0 ); 
    EmitVertex();

    gl_Position = vec4(_x-width/2,_y-height/2, 0.9999, 1.0 );
    texcoord = vec2( 0.0, 0.0 ); 
    EmitVertex();

    EndPrimitive(); 
}