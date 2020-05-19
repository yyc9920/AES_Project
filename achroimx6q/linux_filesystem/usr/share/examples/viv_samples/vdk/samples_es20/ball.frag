/*
varying vec3 v_normal;

void main(void)
{
	vec3 n = normalize(v_normal);

	gl_FragColor = vec4(v_normal, 1.0);
}
*/

uniform vec4 ball_color;
uniform vec4 light_dir;

varying vec3 v_normal;
varying vec3 v_toeye;

float saturate( float value)
{
    return clamp( value, 0.0, 1.0);
}

void main(void)
{
   vec3 n  = normalize(v_normal);
   vec3 lv = -light_dir.xyz;

   // Soft diffuse
   float diffuse = 0.5 + 0.5 * dot(lv, n);

   // Standard specular
   vec3 reflectionVector = reflect(-normalize(v_toeye), n);
   float specular = pow(saturate(dot(reflect(-normalize(v_toeye), n), lv)), 32.0);

   gl_FragColor = diffuse * ball_color + specular;
}
