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

uniform mat4 u_positionMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_rotCamera;
uniform mat4 u_translation_mat;

vec4 createQuaternion(vec3 axis,float angle){

    float t = (angle/2) * 3.14159 / 180.0;
    vec4 quaternion;
    quaternion.x = axis.x * sin(angle);
    quaternion.y = axis.y * sin(angle);
    quaternion.z = axis.z * sin(angle);
    quaternion.w = cos(angle);

    return quaternion;
}

/* (aa0 − bb0 − cc0 − dd0
    , ab0 + ba0 + cd0 − dc0
   , ac0 − bd0 + ca0 + db0
   , ad0 + bc0 − cb0 + da0
   ) */

vec4 multQuat(vec4 a,vec4 b){

    vec4 qtr;
    qtr.w = (a.w*b.w) - (a.x*b.x) - (a.y*b.y) - (a.z * b.z);
    qtr.x = (a.w * b.x) + (a.x*b.w) + (a.y * b.z) - (a.z * b.y);
    qtr.y = (a.w*b.y) - (a.x * b.z) + (a.y*b.w) + (a.z * b.x);
    qtr.z = (a.w*b.z) + (a.x * b.y) - (a.y*b.x) + (a.z * b.w);

    return qtr;
}


vec4 getConj(vec4 quaternion){
    return vec4(-quaternion.x,-quaternion.y,-quaternion.z,quaternion.w);
}

vec3 rotateVertex(vec3 VertexPosition,vec3 axis,float angle){

    vec4 qtr = createQuaternion(axis,angle);
    vec4 conj = getConj(qtr);
    vec4 pQtr = vec4(VertexPosition.x,VertexPosition.y,VertexPosition.z,0.0);

    vec4 r = multQuat(qtr,pQtr);
    r = multQuat(r,conj);

    return vec3(r.x,r.y,r.z);
}


void main(void){


    /*
        vec3 rotation = rotateVertex( a_position.xyz,vec3(0,1,0),mod(u_time,360.0) );
        vec3 rotationNormal = rotateVertex( a_normal.xyz,vec3(0,1,0),mod(u_time,360.0) );

        rotation += vec3(0.0,-0.5,-2.f);
        gl_Position = u_projectionMatrix *
                        vec4(rotation,1.0);

         v_normal =  vec4(rotationNormal,0.0);
    */

    mat4 t= u_projectionMatrix *u_translation_mat *u_rotCamera;

    gl_Position = t *
        u_positionMatrix *
        vec4( a_position,1.0);

    v_normal =  u_positionMatrix* vec4(a_normal,0.0);


    v_color = a_color;



    v_uv = vec2(a_uv.x,1-a_uv.y);
}