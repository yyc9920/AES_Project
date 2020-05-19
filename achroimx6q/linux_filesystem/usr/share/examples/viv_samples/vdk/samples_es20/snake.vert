
uniform float time;

uniform mat4 mv;
uniform mat4 mvp;

attribute vec3 position;

varying vec2   v_texcoord;
varying float  v_color;

void main(void)
{
   vec3 pos;

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.

   pos = 15.0 * position.z * ( position.x * vec3(mv[0][0], mv[1][0], mv[2][0] ) +
                                position.y * vec3(mv[0][1], mv[1][1], mv[2][1]));

   // Move the quads around along some odd path

   float t = time + 4.0 * position.z;
   pos.x += 50.0 * cos(1.24 * t) * sin(3.12 * t);
   pos.y += 50.0 * sin(2.97 * t) * cos(0.81 * t);
   pos.z += 50.0 * cos(t) * sin(t * 1.231);

   gl_Position = mvp * vec4(pos, 1.0);

   v_texcoord   = position.xy;
   v_color      = position.z;

}