#version 120
varying  vec3 v_color;
varying  vec4 v_normal;
varying  vec2 v_uv;


uniform sampler2D u_texcoord0;
uniform sampler2D u_texcoord1;


uniform vec3 u_lightDirection;
uniform float u_time;
uniform vec4 u_localRotation_quat;
uniform vec4 u_localRotation_conj;


void main(void){

    vec3 lightDirection =u_lightDirection;

    float lightning = dot(lightDirection.xyz,vec3(v_normal));

    vec4 color = texture2D(u_texcoord0,v_uv);

    lightning = clamp(lightning,0.0,1.0) ;
    //vec4 finalColor = mix(color2,color,lightning);

    color = mix(color-0.5,color + 0.3,lightning);

    //color = (lightning - .6 ) * color;

    gl_FragColor = vec4(color.xyz,1.0);
}