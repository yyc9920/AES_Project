attribute vec4 my_Vertex;
attribute vec4 my_texUV;

uniform   mat4 my_TransformMatrix;

varying vec4 texUV;

void main()
{
	texUV = my_texUV;
	gl_Position = my_TransformMatrix * my_Vertex;
}
