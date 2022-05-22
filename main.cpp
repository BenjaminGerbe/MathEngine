//#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

#include "../common/GLShader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "tiny_obj_loader.cc"
#include "MathLib.cpp"

GLShader g_triangleShader;
unsigned int texId;

std::string inputfile = "Assets/coffee_cup_obj.obj";
tinyobj::attrib_t attrib;

std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;

std::string warn;
std::string err;

float lightDirection[3] {
        1.0f,1.0f,1.0f
};

int step = 0;

bool ret = tinyobj::LoadObj(&attrib,&shapes,&materials,&err,inputfile.c_str());

struct vertex{
    float positions[3];
    float normal[3];
    float uv[2];
};

std::vector<vertex> vert;


void Initialize(){
    g_triangleShader.LoadVertexShader("Assets/triangle.vs");
    g_triangleShader.LoadFragmentShader("Assets/triangle.fs");
    g_triangleShader.Create();

    int t[2*2] =
    {
    10,5,
    9,4
    };
    int t2[2*2] =
    {
    6,5,
    3,1
    };

    Matrix<2,2> m1(t);
    Matrix<2,2> m2(t2);

    multMatrice(m1,m2).display();


    Complex comp(1,1);
    Complex comp2(9,2);

    glGenTextures(1,&texId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int w,h;
    unsigned char *data = stbi_load("Assets/Cup_Texture.png" , &w,&h,nullptr,STBI_rgb_alpha);
    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0,GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

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

                vert.push_back({

                    {float(vx),float(vy),float(vz)},
                    {nx,ny,nz},
                    {tx,ty}});

            }

            indexoffset+=fv;
        }

    }

}

void Terminate(){
    g_triangleShader.Destroy();

    glDeleteTextures(1,&texId);

}


void Render(GLFWwindow* window)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);
    glClearColor(0.0f, 1.0f, 1.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    float time = glfwGetTime();

    auto triangleProgram = g_triangleShader.GetProgram();
    glUseProgram(triangleProgram);


    float temp[3] {0,1,0};

    Quaternion quaternion(temp,time * 30);

   // std::cout  << time << std::endl;

  // std::cout <<  quaternion.getRotationMatrix() << std::endl;



    const float stride = sizeof(vertex);
    int loc_position = glGetAttribLocation(triangleProgram,"a_position");
    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE,
                          stride,&vert[0].positions);

    glEnableVertexAttribArray(loc_position);

    int loc_normal = glGetAttribLocation(triangleProgram,"a_normal");
    glEnableVertexAttribArray(loc_normal);
    glVertexAttribPointer(loc_normal, 3, GL_FLOAT, GL_FALSE,
                          stride,&vert[0].normal);

    glEnableVertexAttribArray(loc_normal);

    int loc_uv = glGetAttribLocation(triangleProgram,"a_uv");
    glEnableVertexAttribArray(loc_uv);
    glVertexAttribPointer(loc_uv, 2, GL_FLOAT, GL_FALSE,
                          stride,&vert[0].uv);

    glEnableVertexAttribArray(loc_uv);


    auto loc_lightDirection = glGetAttribLocation(triangleProgram,"u_lightDirection");
    glUniform3fv(loc_lightDirection,1,lightDirection);


    auto loc_tex = glGetUniformLocation(triangleProgram,"u_sampler");
    glUniform1f(loc_tex,0);


    float positionMatrix[16] = {
            1,0.0f,0,0.0f,
            0.0,1.0f,0.0f,0.0f,
            0,0.0f,1,0.0f,
            0.0,-.5f,-2.f,1.0f
    };


    float RotationCamera[16] = {
            cosf(time),0,-sinf(time),0.0f,
            0,1.0f,0,0.0f,
            sinf(time),0,cosf(time),0.0f,
            0.0,0.0,0,1.0f
    };


    float TranslationMatrix[16] = {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0.0,0.0,-10.0,1.0f
    };



    const auto loc_rotz_mat = glGetUniformLocation(
            triangleProgram,"u_positionMatrix"
            );
  //  std::cout << quaternion.getRotationMatrix() << std::endl;
    glUniformMatrix4fv(loc_rotz_mat,1,GL_FALSE,quaternion.getRotationMatrix().getTab());



    const auto loc_rot_cam = glGetUniformLocation(
            triangleProgram,"u_rotCamera"
    );

    glUniformMatrix4fv(loc_rot_cam,1,GL_FALSE,RotationCamera);


    const auto translationModele = glGetUniformLocation(
            triangleProgram,"u_translation_mat"
    );

    glUniformMatrix4fv(translationModele,1,GL_FALSE,TranslationMatrix);


    // Projetion matrice

    const float aspectRatio = float(width)/float(height);
    const float zNear = 0.1f,zFar = 100.0f;
    const float fovY = (45.f * M_PI)/180.0f;
    const float cot = 1.f /tanf(fovY/2.f);

    float projectionMatrix[16] = {
            cot/aspectRatio, 0.f,0.f,0.f,
            0.f,cot,0.f,0.f,
            0.f,0.f,-zFar/(zFar-zNear),-1,
            0.f,0.f,-zFar*zNear/(zFar-zNear),0.f,
    };




    const auto loc_proj_mat = glGetUniformLocation(
            triangleProgram,"u_projectionMatrix"
    );


    glUniformMatrix4fv(loc_proj_mat,1,GL_FALSE,projectionMatrix);



    // Passage du temps
    const auto loc_time = glGetUniformLocation(triangleProgram,"u_time");
    glUniform1f(loc_time,time);


    glDrawArrays(GL_TRIANGLES,0,vert.size());

}

int main(void)
{
    GLFWwindow* window;



    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "MathEngine", NULL, NULL);
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



    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
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