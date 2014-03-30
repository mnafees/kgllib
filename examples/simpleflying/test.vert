varying vec3 n;
varying vec3 l;
varying vec3 pos;

uniform vec3 lightDir;

void main()
{
    n = gl_Normal * gl_NormalMatrix;
    l = lightDir * gl_NormalMatrix;
    pos = gl_Vertex.xyz;
    gl_Position = ftransform();
}
