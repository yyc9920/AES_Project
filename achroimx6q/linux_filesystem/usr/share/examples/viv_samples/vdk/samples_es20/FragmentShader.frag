precision mediump float;
varying vec3 vBinormal;

uniform float gamma;

vec3 map( in vec3 s );

void main(void)
{
   vec3 color = map( normalize( vBinormal ) );
   color = pow ( color, vec3( 1.0/gamma ) );
   gl_FragColor = vec4( color, 1.0 );

}

vec3 map( in vec3 s )
{
   vec3 result = s * 0.5 + 0.5;
   return result;
}