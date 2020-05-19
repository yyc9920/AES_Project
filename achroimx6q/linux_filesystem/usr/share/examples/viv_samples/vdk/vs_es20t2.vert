attribute vec4 my_Vertex;
uniform   mat4 my_TransformMatrix;

void main()
{
	gl_Position = my_TransformMatrix * my_Vertex;
}
