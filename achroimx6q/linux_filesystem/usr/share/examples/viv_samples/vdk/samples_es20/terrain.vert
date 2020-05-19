uniform mat4 mvp;

attribute vec3 position;
attribute vec2 texcoord0;

varying vec2 v_texcoord0;

void main()
{
	gl_Position = mvp * vec4(position, 1.0);

	v_texcoord0 = texcoord0;
}