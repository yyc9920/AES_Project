///
//  Uniforms
//
uniform float transparency_bias;
uniform float transparency_scale;

varying vec2 tex_coord;

///
//  Samplers
//
uniform sampler2D base;

void main(void)
{
   vec4 color = texture2D(base, tex_coord);

   color.a *= transparency_scale;
   color.a += transparency_bias;

   gl_FragColor = color;
}