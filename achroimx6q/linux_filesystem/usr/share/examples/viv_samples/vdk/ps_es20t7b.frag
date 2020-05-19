precision highp float;
varying vec4 texUV;
uniform sampler2D my_Texture;

void main (void)
{
	gl_FragColor = texture2D(my_Texture, texUV.xy);
}
