attribute vec3 position;

varying vec2 tex_coord;

void main(void)
{

   // Clean up inaccuracies
   vec2 Pos = sign(position.xy);

   gl_Position = vec4(Pos.xy, 0.0, 1.0);

   tex_coord.x = Pos.x * 0.5 + 0.5;
   tex_coord.y = Pos.y * 0.5 + 0.5;
}