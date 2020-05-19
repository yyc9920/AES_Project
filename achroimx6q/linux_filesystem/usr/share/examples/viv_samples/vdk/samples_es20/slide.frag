uniform sampler2D image;

uniform float length_in_xy;
uniform float length_in_z;

uniform float time;

uniform float scale;

varying vec2 v_texcoord;
varying float v_depth;


void main()
{
	vec2 texcoord = v_texcoord;

	texcoord.x /= length_in_xy;
	texcoord.y /= length_in_z;

	texcoord *= scale;

	texcoord.y -= (scale * 1.0 / 2.0 - 1.0 / 2.0);

	texcoord.x -= scale * fract(time);


	if(texcoord.x < 0.0 || texcoord.x > 1.0 || texcoord.y > 1.0 || texcoord.y < 0.0)
	{
		//discard;
		gl_FragColor = vec4(0.0);

		float dep = v_depth + 500.0;


		gl_FragColor.b = dep / 1000.0;

	}
	else
	{
	vec4 color = texture2D(image, texcoord);
	gl_FragColor = color;
	}





	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) + color;



	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}