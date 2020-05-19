attribute vec3 position;

uniform mat4 mvp;

varying vec2 tex_coord;

void main()
{
	gl_Position = mvp * vec4( position.xyz, 1.0);

	tex_coord = sign(position.xy);

	tex_coord += vec2(1.0, 1.0);

	tex_coord /= 2.0;
}