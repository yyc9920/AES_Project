
attribute vec2 position;
attribute float texcoord;

uniform mat4 mvp;

varying float v_texcoord;

void main()
{
	//float x = position;
	//float y = x * x;

	gl_Position = mvp * vec4(position, 0.0, 1.0);

	//gl_Position = mvp * vec4(texcoord, 0.0, 0.0, 1.0);

	v_texcoord = texcoord;

	gl_PointSize = 2.0;
}