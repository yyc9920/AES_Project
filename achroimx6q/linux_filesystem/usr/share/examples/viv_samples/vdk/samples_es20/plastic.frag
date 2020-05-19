uniform vec4 color;

varying vec3 v_normal;
varying vec3 v_pos_to_eye;

void main(void)
{
   float v = 0.5 * (1.0 + dot(normalize(v_pos_to_eye), v_normal));
   gl_FragColor = v * color;
}