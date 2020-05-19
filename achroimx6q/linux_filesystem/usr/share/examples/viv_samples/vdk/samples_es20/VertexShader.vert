precision mediump float;

uniform mat4 matViewProjection;

attribute vec4 rm_Vertex;
attribute vec3 rm_Binormal;

varying vec3 vBinormal;

void main(void)
{
   gl_Position = matViewProjection * rm_Vertex;
   vBinormal = rm_Binormal;
}