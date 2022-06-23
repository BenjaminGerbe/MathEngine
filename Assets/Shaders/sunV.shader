#version 120

attribute vec3 a_position;
attribute vec3 a_color;
attribute vec3 a_normal;
attribute vec2 a_uv;

uniform float u_time;
uniform vec3 u_lightDirection;

uniform vec4 u_localRotation_quat;
uniform vec4 u_localRotation_conj;
uniform vec4 u_globalRotation_quat;
uniform vec4 u_globalRotation_conj;


varying vec4 v_globalRotation_quat;
varying vec4 v_globalRotation_conj;

uniform mat4 u_rotationCamera;

uniform mat4 u_projectionMatrix;
uniform mat4 u_localTranslation_mat;
uniform mat4 u_cameraTranslation_mat;
uniform mat4 u_scale_mat;

varying  vec3 v_color;
varying  vec4 v_normal;
varying  vec2 v_uv;


vec4 multQuat(vec4 a,vec4 b){

    vec4 qtr;
    qtr.w = (a.w*b.w) - (a.x*b.x) - (a.y*b.y) - (a.z * b.z);
    qtr.x = (a.w * b.x) + (a.x*b.w) + (a.y * b.z) - (a.z * b.y);
    qtr.y = (a.w*b.y) - (a.x * b.z) + (a.y*b.w) + (a.z * b.x);
    qtr.z = (a.w*b.z) + (a.x * b.y) - (a.y*b.x) + (a.z * b.w);

    return qtr;
}



vec4 rotateVertex(vec3 VertexPosition, vec4 quat , vec4 conj){

    vec4 pQtr = vec4(VertexPosition.x,VertexPosition.y,VertexPosition.z,0.0);

    vec4 r = multQuat(quat,pQtr);
    r = multQuat(r,conj);

    return vec4(r.x,r.y,r.z,1);
}



mat4 matN= mat4(1,0,0,0,
0,1,0,0,
0,0,1,0,
-3.0,.5f,-3.0f,1.0f);


mat4 matr12= mat4(1,0,0,0,
0,1,0,0,
0,0,1,0,
-3,0.0,-10.0f,1.0f);

mat4 matScale= mat4(
0.5,0,0,0,
0,0.5,0,0,
0,0,0.5,0,
0.0,0.0,0.0,1.0f
);

void main(void){



    vec4 position =   u_localTranslation_mat* rotateVertex(a_position.xyz,u_localRotation_quat,u_localRotation_conj) ;
    vec4 rotation =   rotateVertex(vec3(position.xyz),u_globalRotation_quat,u_globalRotation_conj);

    vec4 positionNormal =   rotateVertex(a_normal.xyz,u_localRotation_quat,u_localRotation_conj) ;
    vec4 rotationNormal =   rotateVertex(vec3(positionNormal.xyz),u_globalRotation_quat,u_globalRotation_conj);


    v_normal =  vec4(rotationNormal.xyz,1.0);
    v_color = a_color;
    v_uv = vec2(a_uv.x,1-a_uv.y);

    mat4 cam =  u_cameraTranslation_mat * u_rotationCamera * u_scale_mat ;

    gl_Position = (u_projectionMatrix *cam *rotation  );

}