uniform mat4 mv;
uniform mat4 mvp;
uniform mat4 mv_inverse_transpose;

uniform float ball_size;
uniform float time;
uniform float bounce_speed;
uniform float bounce_height;

attribute vec3 position;
attribute vec3 normal;

varying vec3 v_normal;
varying vec3 v_toeye;

void main(void)
{
   vec3 pos = ball_size * normalize(vec3(position));

   float t = fract( time * bounce_speed )  ;

   t *= 4.0 * (1.0 - t) ; /* Top Point : (1/2, 1), Pass Point : (0, 0), (1, 0) */

   pos.y += bounce_height * t;

   gl_Position = mvp * vec4(pos,1.0);

   v_normal = (mv_inverse_transpose * vec4(normal, 1.0)).xyz;

   v_toeye   = vec3(mv * vec4(pos.xyz, 1.0));
   v_toeye.z = -v_toeye.z;
}



































