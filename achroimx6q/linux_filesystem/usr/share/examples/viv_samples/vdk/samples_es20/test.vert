attribute vec3 position;

uniform mat4 mvp;

varying vec4 b;

void main()
{
	gl_Position = mvp * vec4(position.xy, 0.0, 1.0);
}