#version 120
varying  vec3 v_color;
varying  vec4 v_normal;
varying  vec2 v_uv;



uniform sampler2D u_sampler;
uniform vec3 u_lightDirection;
uniform float u_time;


void main(void){

    vec3 lightDirection = vec3(3,1,2);
    //vec3 lightDirection = u_lightDirection;
    float lightning = dot(lightDirection,vec3(v_normal));

    vec4 color = texture2D(u_sampler,v_uv);
    lightning = clamp(lightning,0.0,1.0);
    color = (lightning + .6) * color;


    gl_FragColor =  color;
}