varying vec2 tex_coord;

uniform sampler2D image;

void main()
{
	gl_FragColor = texture2D(image, tex_coord);
}