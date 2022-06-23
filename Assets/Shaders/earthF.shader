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

void main(void){

    vec3 lightDirection =u_lightDirection;

    float lightning = dot(lightDirection.xyz,vec3(v_normal));

    vec4 color = texture2D(u_texcoord0,v_uv);
    vec4 color2 = texture2D(u_texcoord1,v_uv);

    lightning = clamp(lightning,0.0,1.0) * 1.5;
    vec4 finalColor = mix(color2,color,lightning);


    color = (lightning + .6) * color;

    gl_FragColor = vec4(finalColor.xyz,0);
}