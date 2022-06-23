#version 330 core


attribute vec3 aPos;

varying vec3 TexCoords;

uniform mat4 u_projectionMatrix;
uniform mat4 view;
uniform vec4 u_quaternionRotation;
uniform vec4 u_conjQuaternionRotation;
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


void main()
{
    TexCoords = aPos;
    gl_Position = u_projectionMatrix  * u_translation_mat* rotateVertex(aPos.xyz);
}