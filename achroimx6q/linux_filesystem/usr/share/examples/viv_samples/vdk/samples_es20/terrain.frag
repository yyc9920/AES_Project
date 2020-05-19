uniform sampler2D base;

varying vec2 v_texcoord0;

void main()
{
	vec4 base_color = texture2D( base, v_texcoord0);

	gl_FragColor = base_color;
}