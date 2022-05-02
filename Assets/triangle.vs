#version 120

attribute vec3 a_position;
attribute vec3 a_color;
attribute vec3 a_normal;
attribute vec2 a_uv;


varying  vec3 v_color;
varying  vec4 v_normal;
varying  vec2 v_uv;


uniform float u_time;
uniform vec3 u_lightDirection;

uniform mat4 u_rotationMatrix;
uniform mat4 u_rotationMatrixZ;
uniform mat4 u_projectionMatrix;



void main(void){

    vec2 offset = vec2(sin(u_time),0);

    gl_Position = u_projectionMatrix *
                    u_rotationMatrix  *
                    vec4(a_position,1.0);
    v_color = a_color;



    v_normal = (u_rotationMatrix * vec4(a_normal,0.0)); ;
    v_uv = vec2(a_uv.x,1-a_uv.y);
}