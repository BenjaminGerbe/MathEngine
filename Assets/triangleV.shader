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
uniform vec4 u_quaternionRotation;
uniform vec4 u_conjQuaternionRotation;


uniform mat4 u_positionMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_rotCamera;
uniform mat4 u_translation_mat;


vec4 multQuat(vec4 a,vec4 b){

    vec4 qtr;
    qtr.w = (a.w*b.w) - (a.x*b.x) - (a.y*b.y) - (a.z * b.z);
    qtr.x = (a.w * b.x) + (a.x*b.w) + (a.y * b.z) - (a.z * b.y);
    qtr.y = (a.w*b.y) - (a.x * b.z) + (a.y*b.w) + (a.z * b.x);
    qtr.z = (a.w*b.z) + (a.x * b.y) - (a.y*b.x) + (a.z * b.w);

    return qtr;
}



vec4 rotateVertex(vec3 VertexPosition){

    vec4 pQtr = vec4(VertexPosition.x,VertexPosition.y,VertexPosition.z,0.0);

    vec4 r = multQuat(u_quaternionRotation,pQtr);
    r = multQuat(r,u_conjQuaternionRotation);

    return vec4(r.x,r.y,r.z,1);
}


void main(void){


    vec4 rotation =u_rotCamera * rotateVertex( a_position.xyz );
    vec4 rotationNormal = rotateVertex( a_normal.xyz );


    mat4 t= u_projectionMatrix;

    gl_Position = u_projectionMatrix * rotation ;

    v_normal =  vec4(rotationNormal.xyz,0.0);


    /*
    mat4 t= u_projectionMatrix  *u_rotCamera *u_positionMatrix;

    gl_Position = t *
        vec4( a_position,1.0);

    v_normal =  u_positionMatrix* vec4(a_normal,0.0);

    */

    v_color = a_color;


    v_uv = vec2(a_uv.x,1-a_uv.y);
}