attribute vec3 position;

uniform mat4	mvp;

//uniform vec3	view_positon;
//uniform float	internal_object_radius;

varying vec3 normal;

void main()
{
	gl_Position = mvp * vec4(position.xyz, 1.0);//mvp * vec4( position + vec3(internal_object_radius) + view_positon, 1.0);

	normal = position;
}