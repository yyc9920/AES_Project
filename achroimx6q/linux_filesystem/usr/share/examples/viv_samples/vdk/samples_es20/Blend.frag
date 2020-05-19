varying vec2 tex_coord;

///
//  Samplers
//
uniform sampler2D back_back;
uniform sampler2D back_front;
uniform sampler2D background;
uniform sampler2D front_back;
uniform sampler2D front_front;

vec4 lerp(vec4 a, vec4 b, float s)
{
  return (a + s * (b - a));
}

void main(void)
{

   // Sample all textures
   vec4 bg = texture2D(background, tex_coord);
   vec4 bb = texture2D(back_back,   tex_coord);
   vec4 bf = texture2D(back_front,  tex_coord);
   vec4 fb = texture2D(front_back,  tex_coord);
   vec4 ff = texture2D(front_front, tex_coord);

   // Do transparency lerp for each layer
   vec4 col = lerp(bg, bb, bb.a);
   col = lerp(col, bf, bf.a);
   col = lerp(col, fb, fb.a);
   col = lerp(col, ff, ff.a);

	//vec4 col = lerp(bg, ff, ff.a);
   gl_FragColor = col;

   //gl_FragColor = fb;
}