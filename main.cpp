//#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <string>

#include "../common/GLShader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "tiny_obj_loader.cc"
#include "MathLib/MathLib.cpp"
#include "skyboxvertices.h"
#include "camera.cpp"

GLShader g_triangleShader;
GLShader g_skyBox;
GLShader g_earth;
GLShader g_basicPlanet;
GLShader g_Sun;
unsigned int texId;
float distancelune = -35.0;
float distanceTerre = -10;

tinyobj::attrib_t attrib;

std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;

std::string warn;
std::string err;

Camera cameraCT();

float lightDirection[3] {
        1.0f,0.0f,1.0f
};

int step = 0;



struct vertex{
    float positions[3];
    float normal[3];
    float uv[2];
};



unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);



    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


std::vector<std::string> faces
{
"Assets/1.png", // LE 1
"Assets/3.png", // Le 3
"Assets/5.png",
"Assets/6.png",
"Assets/2.png", // LE 2
"Assets/4.png"
};
unsigned int cubemapTexture;

std::vector<vertex> vert;
std::vector<vertex> vertSphere;

std::vector<vertex> loadObj(std::string Path){
    std::string inputfile = Path;
    bool ret = tinyobj::LoadObj(&attrib,&shapes,&materials,&err,inputfile.c_str());

    std::vector<vertex> _v;
    for (size_t s = 0; s < shapes.size(); s++) {
        size_t indexoffset = 0;

        for(size_t f = 0;f < shapes[s].mesh.num_face_vertices.size();f++){
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            for(size_t v = 0;v < fv;v++){
                tinyobj::index_t idx = shapes[s].mesh.indices[indexoffset + v];

                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

                tinyobj::real_t nx = 0;
                tinyobj::real_t ny = 0;
                tinyobj::real_t nz = 0;

                tinyobj::real_t tx;
                tinyobj::real_t ty;

                if (idx.texcoord_index >= 0) {
                    tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                }

                if (idx.normal_index >= 0) {
                    nx = attrib.normals[3*size_t(idx.normal_index)+0];
                    ny = attrib.normals[3*size_t(idx.normal_index)+1];
                    nz = attrib.normals[3*size_t(idx.normal_index)+2];
                }

                _v.push_back({

                                       {float(vx),float(vy),float(vz)},
                                       {nx,ny,nz},
                                       {tx,ty}});

            }

            indexoffset+=fv;
        }

    }
    return _v;
}


void loadImage(int f, char* path){
    glGenTextures(1,&texId);

    glActiveTexture(GL_TEXTURE0 + f);
    glBindTexture(GL_TEXTURE_2D, texId);



    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int w,h;
    unsigned char *data = stbi_load(path , &w,&h,nullptr,STBI_rgb_alpha);
    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0,GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
}

void Initialize(){


    g_skyBox.LoadVertexShader("Assets/Shaders/skyBoxV.shader");
    g_skyBox.LoadFragmentShader("Assets/Shaders/skyBoxF.shader");
    g_skyBox.Create();

    g_triangleShader.LoadVertexShader("Assets/Shaders/triangleV.shader");
    g_triangleShader.LoadFragmentShader("Assets/Shaders/triangleF.shader");
    g_triangleShader.Create();

    g_basicPlanet.LoadVertexShader("Assets/Shaders/basicPlanetV.shader");
    g_basicPlanet.LoadFragmentShader("Assets/Shaders/basicPlanetF.shader");
    g_basicPlanet.Create();

    g_earth.LoadVertexShader("Assets/Shaders/earthV.shader");
    g_earth.LoadFragmentShader("Assets/Shaders/earthF.shader");
    g_earth.Create();

    g_Sun.LoadVertexShader("Assets/Shaders/sunV.shader");
    g_Sun.LoadFragmentShader("Assets/Shaders/sunF.shader");
    g_Sun.Create();




    loadImage(0,"Assets/2k_earth_daymap.jpg");
    loadImage(1,"Assets/2k_earth_nightmap.jpg");
    loadImage(2,"Assets/2k_moon.jpg");
    loadImage(3,"Assets/8k_sun.jpg");


    loadCubemap(faces);

    vert = loadObj("Assets/coffee_cup_obj.obj");
    vertSphere = loadObj("Assets/Sphere.obj");

}

void Terminate(){
    g_triangleShader.Destroy();

    glDeleteTextures(1,&texId);

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        //getting cursor position
        glfwGetCursorPos(window, &xpos, &ypos);

    }
}

double xpos,ypos;
double Oldxpos,Oldypos;
float angle = 0;
float angleb = 0;

void InitializeObject(unsigned int shader, Quaternion& localRot, Quaternion& globalRot,Quaternion& camQuat, float* localTrans, float* originRot,
                      float* projectionMatrix, float* scale, float* cameraTransation,float time, std::vector<vertex>& _vert, int* textureid ){


    glUseProgram(shader);

    // POSITIONS

    const float stride = sizeof(vertex);
    int loc_position = glGetAttribLocation(shader,"a_position");
    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE,
                          stride,&_vert[0].positions);

    glEnableVertexAttribArray(loc_position);

    // NORMAL

    int loc_normal = glGetAttribLocation(shader,"a_normal");
    glEnableVertexAttribArray(loc_normal);
    glVertexAttribPointer(loc_normal, 3, GL_FLOAT, GL_FALSE,
                          stride,&_vert[0].normal);

    glEnableVertexAttribArray(loc_normal);


    // UV
    int loc_uv = glGetAttribLocation(shader,"a_uv");
    glEnableVertexAttribArray(loc_uv);
    glVertexAttribPointer(loc_uv, 2, GL_FLOAT, GL_FALSE,
                          stride,&_vert[0].uv);

    glEnableVertexAttribArray(loc_uv);


    // LIGHT
    auto loc_lightDirection = glGetUniformLocation(shader,"u_lightDirection");
    glUniform3fv(loc_lightDirection,1,lightDirection);



    //  PROJECTION MATRIX
    const auto loc_proj_mat = glGetUniformLocation(
            shader,"u_projectionMatrix"
    );
    glUniformMatrix4fv(loc_proj_mat,1,GL_FALSE,projectionMatrix);



    // PASSAGE DU TEMPS
    const auto loc_time = glGetUniformLocation(shader,"u_time");
    glUniform1f(loc_time,time);



    // LOCAL ROTATION ---------------------
    const auto localRotLoc = glGetUniformLocation(shader,"u_localRotation_quat");
    glUniform4fv(localRotLoc,1,&localRot.getQutrnormalized()[0]);

    const auto localRotConjLoc = glGetUniformLocation(shader,"u_localRotation_conj");
    glUniform4fv(localRotConjLoc,1,&localRot.conj()[0]);


    const auto centerRotation = glGetUniformLocation(shader,"u_origin_rotation");
    glUniformMatrix4fv(centerRotation,1,GL_FALSE,originRot);


    // GLOBAL ROTATION ----------------
    const auto globalRotLoc = glGetUniformLocation(shader,"u_globalRotation_quat");
    glUniform4fv(globalRotLoc,1,&globalRot.getQutrnormalized()[0]);

    const auto globalRotConjLoc = glGetUniformLocation(shader,"u_globalRotation_conj");
    glUniform4fv(globalRotConjLoc,1,&globalRot.conj()[0]);



    // LOCAL TRANSLATION ---------------
    const auto localTranslationLoc = glGetUniformLocation(
            shader,"u_localTranslation_mat"
    );
    glUniformMatrix4fv(localTranslationLoc,1,GL_FALSE,localTrans);


    // Camera TRANSLATION ---------------
    const auto cameraTranslation = glGetUniformLocation(
            shader,"u_cameraTranslation_mat"
    );
    glUniformMatrix4fv(cameraTranslation,1,GL_FALSE,cameraTransation);

    const auto cameraGlobalRotation = glGetUniformLocation(
            shader,"u_CenterCamera_mat"
    );


    // CAMERA
    const auto localCameraRot = glGetUniformLocation(
            shader,"u_rotationCamera"
    );

    glUniformMatrix4fv(localCameraRot,1,GL_FALSE,camQuat.getRotationMatrix().getTab());

    // SCALE
    const auto matScale = glGetUniformLocation(
            shader,"u_scale_mat"
    );
    glUniformMatrix4fv(matScale,1,GL_FALSE,scale);

    // Texture
    for (int i = 0; i < (int)sizeof(textureid)/sizeof(int); ++i) {
        const char* coord = ("u_texcoord" + std::to_string( i)).c_str();
        auto text = glGetUniformLocation(shader,coord );
        glUniform1i(text,  textureid[i]);
    }




    glDrawArrays(GL_TRIANGLES,0,_vert.size());

}

float* getMidPoint(float* pos1, float* pos2){

    float norme =  sqrtf( powf( (pos1[0] - pos2[0] ),2.0) + powf( (pos1[1] - pos2[1] ),2.0) + powf((pos1[2] - pos2[2] ),2.0) );

    float dir[3] = {  (pos1[0] - pos2[0])/norme, (pos1[1] - pos2[1])/norme, (pos1[2] - pos2[2])/norme};
    std::cout << dir[0] << " " << dir[1] << " " << dir[2] << std::endl;
    float* pos= new float[3];
    pos[0] = pos2[0] + dir[0] * norme/2;
    pos[1] = pos2[1] + dir[1] * norme/2;
    pos[2] = pos2[2] + dir[2] * norme/2;

    return pos;
}




Camera* cmd = new Camera();
void Render(GLFWwindow* window)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glfwGetCursorPos(window, &xpos, &ypos);

    float time = glfwGetTime();

    // Projetion matrice

    const float aspectRatio = float(width)/float(height);
    const float zNear = 0.1f,zFar = 10000000.0f;
    const float fovY = (45.f * M_PI)/180.0f;
    const float cot = 1.f /tanf(fovY/2.f);

    float projectionMatrix[16] = {
            cot/aspectRatio, 0.f,0.f,0.f,
            0.f,cot,0.f,0.f,
            0.f,0.f,-zFar/(zFar-zNear),-1,
            0.f,0.f,-zFar*zNear/(zFar-zNear),0.f,
    };



    float a = 0;

    if(Oldxpos - xpos > 0.5){
        angle--;


    }
    else if(Oldxpos - xpos < -0.5){
        angle++;
    }


    float b = 0;

    if(Oldypos - ypos > 0.5 && a==0){
        angleb--;


    }
    else if(Oldypos - ypos < -0.5 && a==0){
        angleb++;
    }



    Oldxpos = xpos;
    Oldypos = ypos;

    glViewport(0, 0, width, height);
    glClearColor(0.0f, 1.0f, 1.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    float TranslationMatrixSky[16] = {
            1,0,0,0.0f,
            0,1.0f,0,0.0f,
            0,0,1,0.0f,
            0.0,0,0,1.0f
    };

    auto skybox = g_skyBox.GetProgram();
    auto planet = g_basicPlanet.GetProgram();
    glUseProgram(skybox);

    float temp[3] {1,1,0};
    float cam[3] { 0 , 1 , 0 };
    float cat[3] {1,0,0};

    Quaternion quatCamera(cam,(angle*10.0f));



    quatCamera= quatCamera   ;

    glDepthMask(GL_FALSE);





    const float strideskyBox = sizeof(float)*3;
    int sky_position = glGetAttribLocation(skybox,"aPos");
    glEnableVertexAttribArray(sky_position);
    glVertexAttribPointer(sky_position, 3, GL_FLOAT, GL_FALSE,
                          strideskyBox,&skyboxVertices[0]);

    const auto loc_proj_mat_sky = glGetUniformLocation(
            skybox,"u_projectionMatrix"
    );
    glUniformMatrix4fv(loc_proj_mat_sky,1,GL_FALSE,projectionMatrix);

    const auto skyTrans = glGetUniformLocation(
            skybox,"u_translation_mat"
    );
    glUniformMatrix4fv(skyTrans,1,GL_FALSE,TranslationMatrixSky);


    const auto cameraSky = glGetUniformLocation(
            skybox,"u_quaternionRotation"
    );
    glUniform4fv(cameraSky,1,quatCamera.getTab());

    const auto cameraSkyCongj = glGetUniformLocation(
            skybox,"u_conjQuaternionRotation"
    );
    glUniform4fv(cameraSkyCongj,1,quatCamera.conj());

    const auto translationMatSky = glGetUniformLocation(
            skybox,"u_translation"
    );
    glUniformMatrix4fv(translationMatSky,1,GL_FALSE,quatCamera.conj());


    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);


    auto triangleProgram = g_triangleShader.GetProgram();
    auto earth = g_earth.GetProgram();
    auto sun = g_Sun.GetProgram();



    float TranslationMatrix[16] = {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0.0,-.5f,-2.0f,1.0f
    };

    float positionEarth[3] = {0.0,-.5f,distanceTerre};
    float EarthPosition[16] = {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0.0,-.5f,0,1.0f
    };

    float SunPosition[16] = {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            150.0f,-.0f,150.0f,1.0f
    };

    float positionMoon[3] = {3.0,-.5f,3.0f};
    float TranslationMatrix2[16] = {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            3.0,-.5f,3.0f,1.0f
    };


   // float* mid  = getMidPoint(positionMoon,positionEarth);

/*

    float TargetRotation[16] = {
            1.0f,0.0,0.0,0.0,
            0.0,1.0f,0.0,0.0,
            0.0,0.0,1.0f,0.0,
            mid[0],mid[1],mid[2],1.0
    };
*/

    float TranslationMatrixMoon[16] = {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0.0,-.5f,distancelune,1.0f
    };



    float CameraTranslation[16] = {
            1.0,0.0,0.0,0.0,
                    0.0,1.0,0.0,0.0,
                    0.0,0.0,1.0,0.0,
                    0.0,0.0,-10.0,1.0
    };

    float ScaleMoon[16] = {
            .27f,0.0,0.0,0.0,
            0.0,.27f,0.0,0.0,
            0.0,0.0,.27f,0.0,
            0.0,0.0,0.0,1.0
    };

    float ScaleSun[16] = {
            109.0,0.0,0.0,0.0,
            0.0,109.0,0.0,0.0,
            0.0,0.0,109.0,0.0,
            0.0,0.0,0.0,1.0
    };

    float Identity[16] = {
            1.0f,0.0,0.0,0.0,
            0.0,1.0f,0.0,0.0,
            0.0,0.0,1.0f,0.0,
            0.0,0.0,0.0,1.0
    };



    distancelune +=  0.002;



    float temp2[3] {0,1,0};
    float temp1[3] {1,0,1};
    float temp3[3] {0,0,1};

    Quaternion localRotation(temp2,time * 10.0f);
    Quaternion localRotationMoon(temp2,time * 10.0f);
    Quaternion localRotationEarth2(temp3, 23.5f);
    Quaternion globalRotation(temp2,time*13.0f);
    Quaternion globalRotationEarth(temp2,time*13.0f + 360);

    Quaternion globalRotationSun(temp2,10.0f);

    localRotation = localRotationEarth2 * localRotation;

    int t[3] = {0,1,2};
    int t2[3] = {2,1,2};
    int t3[1] = {3};


     InitializeObject(planet,localRotationMoon,
                      localRotationMoon,quatCamera,TranslationMatrixMoon,Identity, projectionMatrix,ScaleMoon,CameraTranslation,time,vertSphere,t2);


     InitializeObject(earth,localRotation,
                      globalRotationSun,quatCamera,EarthPosition,Identity, projectionMatrix,Identity,CameraTranslation,time,vertSphere,t);

    InitializeObject(sun,localRotation,
                     globalRotationSun,quatCamera,SunPosition,Identity, projectionMatrix,ScaleSun,CameraTranslation,time,vertSphere,t3);

}


int main(void)
{
    GLFWwindow* window;


    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(720, 405, "MathEngine", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }


    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum error = glewInit();

    if(error != GL_NO_ERROR){
        std::cout << "Erreur d'initalisation de GLEW" << std::endl;
    }

    Initialize();

    //std::cout << (distancelune <= -3 )<< std::endl;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window) && (distancelune <= -2.5)  )
    {
        /* Render here */
        Render(window);




        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    Terminate();

    glfwTerminate();
    return 0;
}