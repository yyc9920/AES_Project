uniform mat4 mvp;
uniform vec3 eye_position;

attribute vec3 position;
attribute vec3 normal;

varying vec3 v_normal;
varying vec3 v_pos_to_eye;

void main(void)
{
   gl_Position = mvp * vec4(position, 1.0);

   // World-space lighting
   v_normal = normal;
   v_pos_to_eye = eye_position - position;
}