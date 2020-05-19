attribute vec3 position;

uniform mat4 mvp;

varying vec2 tex_coord;

void main()
{
	gl_Position = mvp * vec4(position.xy, 0.0, 1.0);

	tex_coord = gl_Position.xy;

	tex_coord += 1.0;

	tex_coord /= 2.0;
}