uniform float particle_exp;
uniform sampler2D palette;

varying vec2 v_texcoord;
varying float v_color;

void main(void)
{
   gl_FragColor = vec4( v_texcoord, 0.9, 1.0 );
   gl_FragColor = (1.0 - pow(dot(v_texcoord, v_texcoord), particle_exp)) * texture2D(palette, vec2(v_color,0.0));
}