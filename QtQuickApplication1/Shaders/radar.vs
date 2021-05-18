#version 400

in vec3 posAttr;
in vec3 colAttr;
in vec3 normalAttr;
in vec2 aTexCoords;
in vec3 aTangent;
in vec3 aBitangent;

out vec3 col;
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main() 
{
    col = colAttr;

    FragPos = vec3(model * vec4(posAttr, 1.0));

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * normalAttr;  
    
    TexCoords = aTexCoords;

    
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(normalMatrix * aBitangent);
    vec3 N = normalize(normalMatrix * normalAttr);
    
    TBN = (mat3(T, B, N));
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position =  projection * view * model * vec4(posAttr, 1.0f);
}