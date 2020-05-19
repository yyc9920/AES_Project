
attribute vec3 position;
attribute vec2 texcoord;

uniform mat4 mvp;

varying vec2 v_texcoord;
varying float v_depth;

void main()
{
	vec4 pos = mvp * vec4(position, 1.0);
	gl_Position = pos;

	//gl_Position = vec4(position / 640.0, 1.0);

	v_texcoord = texcoord;

	v_depth = position.y;

	gl_PointSize = 2.0;
}