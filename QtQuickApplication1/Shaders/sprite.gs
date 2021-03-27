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

void main() 
{
    float width = float(w)/viewportW;
    float height = float(h)/viewportH;

    gl_Position = vec4( width/2, height/2, 0.9999, 1.0 );
    texcoord = vec2( 1.0, 1.0 );
    EmitVertex();

    gl_Position = vec4(-width/2, height/2, 0.9999, 1.0 );
    texcoord = vec2( 0.0, 1.0 ); 
    EmitVertex();

    gl_Position = vec4( width/2,-height/2, 0.9999, 1.0 );
    texcoord = vec2( 1.0, 0.0 ); 
    EmitVertex();

    gl_Position = vec4(-width/2,-height/2, 0.9999, 1.0 );
    texcoord = vec2( 0.0, 0.0 ); 
    EmitVertex();

    EndPrimitive(); 
}