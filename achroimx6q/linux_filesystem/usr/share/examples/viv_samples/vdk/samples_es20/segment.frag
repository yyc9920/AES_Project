uniform sampler2D image;
uniform float time;
uniform float length_in_xy;
uniform float offset_in_texcoord;

varying float v_texcoord;

void main()
{
	float texcoord = v_texcoord;

	texcoord -= offset_in_texcoord;

	if(texcoord < 0.0 || texcoord > 1.0)
	{
		gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
	else
	{
		gl_FragColor = + texture2D(image, vec2(texcoord, 0.0));
	}
}