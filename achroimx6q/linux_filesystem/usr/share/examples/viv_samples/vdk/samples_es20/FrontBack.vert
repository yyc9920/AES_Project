uniform mat4 mvp;

attribute vec3 position;
attribute vec2 tex_coord0;

varying vec2 tex_coord;

void main(void)
{
	gl_Position = mvp * vec4(position, 1.0);

	tex_coord = tex_coord0;
}