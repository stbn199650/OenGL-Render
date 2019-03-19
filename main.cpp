/*
 CG HW1
 Please fill the functions whose parameters are replaced by ... in the following function
 static int add_obj(unsigned int program, const char *filename)(line 140)
 static void render() (line 202)
 
 For example : line 156 //glGenVertexArrays(...);
 
 */

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "tiny_obj_loader.h"
#include <glm/ext.hpp>
#define GLM_FORCE_RADIANS

float aspect;
GLint modelLoc;
GLint projLoc;

struct object_struct{
     unsigned int program;
     unsigned int vao;
     unsigned int vbo[3];
     unsigned int texture;
     glm::mat4 model;
} ;

std::vector<object_struct> objects;//vertex array object,vertex buffer object and texture(color) for objs
unsigned int program;
std::vector<int> indicesCount;//Number of indice of objs    //size of this vector means how many objects we want to draw

static void error_callback(int error, const char* description)
{
     fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
     if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
          glfwSetWindowShouldClose(window, GL_TRUE);
}

static unsigned int setup_shader(const char *vertex_shader, const char *fragment_shader)
{
     GLuint vs=glCreateShader(GL_VERTEX_SHADER);
     glShaderSource(vs, 1, (const GLchar**)&vertex_shader, nullptr);
     glCompileShader(vs);     //compile vertex shader
     
     int status, maxLength;
     char *infoLog=nullptr;
     glGetShaderiv(vs, GL_COMPILE_STATUS, &status);          //get compile status
     if(status==GL_FALSE)                                        //if compile error
     {
          printf("Compile shader Error\n");
          glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);     //get error message length
          
          /* The maxLength includes the NULL character */
          infoLog = new char[maxLength];
          
          glGetShaderInfoLog(vs, maxLength, &maxLength, infoLog);          //get error message
          
          fprintf(stderr, "Vertex Shader Error: %s\n", infoLog);
          
          /* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
          /* In this simple program, we'll just leave */
          delete [] infoLog;
          return 0;
     }
     
     //     for fragment shader --> same as vertex shader
     GLuint fs=glCreateShader(GL_FRAGMENT_SHADER);
     glShaderSource(fs, 1, (const GLchar**)&fragment_shader, nullptr);
     glCompileShader(fs);
     
     glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
     if(status==GL_FALSE)
     {
          glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &maxLength);
          
          /* The maxLength includes the NULL character */
          infoLog = new char[maxLength];
          
          glGetShaderInfoLog(fs, maxLength, &maxLength, infoLog);
          
          fprintf(stderr, "Fragment Shader Error: %s\n", infoLog);
          
          /* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
          /* In this simple program, we'll just leave */
          delete [] infoLog;
          return 0;
     }
     
     // vertex shader & fragment shader must be put together to a unit before it is possible to link.
     unsigned int program=glCreateProgram();
     // Attach our shaders to our program
     glAttachShader(program, vs);
     glAttachShader(program, fs);
     
     glLinkProgram(program);
     
     glGetProgramiv(program, GL_LINK_STATUS, &status);
     
     if(status==GL_FALSE)
     {
          glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
          
          /* The maxLength includes the NULL character */
          infoLog = new char[maxLength];
          glGetProgramInfoLog(program, maxLength, NULL, infoLog);
          
          glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);
          
          fprintf(stderr, "Link Error: %s\n", infoLog);
          
          /* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
          /* In this simple program, we'll just leave */
          delete [] infoLog;
          return 0;
     }
     
     return program;
}

static std::string readfile(const char *filename)
{
     std::ifstream ifs(filename);
     if(!ifs){
          printf("Readfile Error\n");
          exit(EXIT_FAILURE);
     }
     printf("Readfile Success\n");
     return std::string( std::istreambuf_iterator<char>(ifs),
                        std::istreambuf_iterator<char>());
}

static int add_obj(unsigned int program, const char *filename)
{
     object_struct new_node;
     
     std::vector<tinyobj::shape_t> shapes;
     std::vector<tinyobj::material_t> materials;
     
     std::string err = tinyobj::LoadObj(shapes, materials, filename);
     
     if (!err.empty()||shapes.size()==0)
     {
          std::cerr<<err<<std::endl;
          exit(1);
     }
     
     //Generate memory for buffers.
     glGenVertexArrays(1, &new_node.vao);
     glGenBuffers(3, new_node.vbo);
     glGenTextures(1, &new_node.texture);
     //glGenVertexArrays(...);
     //glGenBuffers(...);
     //glGenTextures(...);
     
     //Tell the program which VAO you are going to modify
     glBindVertexArray(new_node.vao);
     //glBindVertexArray(...);
     
     // Upload postion array
     glBindBuffer(GL_ARRAY_BUFFER,  new_node.vbo[0]);
     glBufferData(GL_ARRAY_BUFFER,  shapes[0].mesh.positions.size()*sizeof(float),  shapes[0].mesh.positions.data() ,  GL_STATIC_DRAW);
     glEnableVertexAttribArray(0);
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//     std::cout <<"\nmesh.positions"<<std::endl;
//     for(int i=0;i<shapes[0].mesh.positions.size();i++)
//          std::cout << ' ' << shapes[0].mesh.positions[i];

     //glBindBuffer(...);
     //glBufferData(...);
     //glEnableVertexAttribArray(...);
     //glVertexAttribPointer(...);
     
     if(shapes[0].mesh.normals.size()>0)
     {
          // Upload normal array
          glBindBuffer(GL_ARRAY_BUFFER, new_node.vbo[1]);
          glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.normals.size()*sizeof(float), shapes[0].mesh.normals.data(), GL_STATIC_DRAW);
          glEnableVertexAttribArray(1);
          glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//          std::cout <<"\nmesh.normals"<<std::endl;
//          for(int i=0;i<shapes[0].mesh.normals.size();i++)
//               std::cout << ' ' << shapes[0].mesh.normals[i];

          //glBindBuffer(...);
          //glBufferData(...);
          //glEnableVertexAttribArray(...);
          //glVertexAttribPointer(...);
     }
     
     // Setup index buffer for glDrawElements
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_node.vbo[2]);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size()*sizeof(GLint), shapes[0].mesh.indices.data() , GL_STATIC_DRAW);
     indicesCount.push_back(shapes[0].mesh.indices.size());    //indicesCount = number of objects
     std::cout<<shapes[0].mesh.indices.size()<<std::endl;
     //glBindBuffer(...);
     //glBufferData(...);
     //indicesCount.push_back(...);
     
     
//     for(int i=0;i<shapes[0].mesh.indices.size();i++){
//          std::cout << ' ' << shapes[0].mesh.indices[i];
//     }
     //std::cout<<indicesCount[0]<<std::endl;
     glBindVertexArray(0);
     //glBindVertexArray(...);
     
     new_node.program = program;
     
     objects.push_back(new_node);
     return objects.size()-1;
}

static void releaseObjects()
{
     for(int i=0;i<objects.size();i++){
          glDeleteVertexArrays(1, &objects[i].vao);
          glDeleteTextures(1, &objects[i].texture);
          glDeleteBuffers(4, objects[i].vbo);
     }
     glDeleteProgram(program);
}

static void render()
{
     float scale = 20.0;
     float time = glfwGetTime() / 100.0f;
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     
     glUseProgram(program);
     glm::mat4 proj_matrix, model_matrix, view_matrix, eye(1.0f);
     
     //set position of glm::lookAt
     glm::vec3 Cam(20.0f, 20.0f, 20.0f); // Camera is at (4,3,3), in World Space
     glm::vec3 Tar(0, 0, 0); // Camera looks at the origin
     glm::vec3 Up(0, -1, 0);  // Head is up

     //set camera matrix
     proj_matrix = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
                    /*glm::frustum(...) or glm::perspective(...)*/
     view_matrix = glm::lookAt(Cam, Tar, Up) * glm::mat4(1.0f);
                    /*glm::lookAt(...) * glm::mat4(1.0f)*/
     
     //Draw array
     glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj_matrix*view_matrix));
     
     for(int i=0;i<objects.size();i++){
          
          //Bind VAO
          glBindVertexArray(objects[i].vao);
          //glBindVertexArray(...);
          
          //If you don't want to rotate or move your object, you can comment the functions below.
          model_matrix = glm::translate(eye, glm::vec3(0.0f))
          * glm::rotate(eye, 98.70f * time, glm::vec3(0.0f, 0.0f, 1.0f))
          * glm::rotate(eye, 123.40f * time, glm::vec3(1.0f, 0.0f, 0.0f));
          
          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
          
          //Draw object
          glDrawElements( GL_TRIANGLES, indicesCount[1], GL_FLOAT, 0);
          //glDrawElements(...);
     }
     
     //Unbind VAO
     glBindVertexArray(0);
     //glBindVertexArray(...);
}

static void reshape(GLFWwindow* window, int width, int height)
{
     aspect = (float) width / height;
     glViewport(0, 0, width, height);
}

void init_shader()
{
     modelLoc = glGetUniformLocation(program, "model");
     projLoc      = glGetUniformLocation(program, "proj");
}

int main(int argc, char *argv[])
{
     GLFWwindow* window;
     glfwSetErrorCallback(error_callback);
     if (!glfwInit()){
          printf("GLFW Init Error\n");
          exit(EXIT_FAILURE);
     }
     // OpenGL 3.3, Mac OS X is reported to have some problem. However I don't have Mac to test
     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);          //set hint to glfwCreateWindow, (target, hintValue)
     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
     //hint--> window not resizable,  explicit use core-profile,  opengl version 3.3
     // For Mac OS X
     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
     window = glfwCreateWindow(800, 600, "Simple Example", NULL, NULL);
     if (!window)
     {
          printf("Open Window Error\n");
          glfwTerminate();
          return EXIT_FAILURE;
     }
     glfwSetFramebufferSizeCallback(window, reshape);
     reshape(window, 800, 600);
     glfwMakeContextCurrent(window);     //set current window as main window to focus
     
     // This line MUST put below glfwMakeContextCurrent
     glewExperimental = GL_TRUE;          //tell glew to use more modern technique for managing OpenGL functionality
     glewInit();
     
     // Enable vsync
     glfwSwapInterval(1);
     
     // Setup input callback
     glfwSetKeyCallback(window, key_callback);     //set key event handler
     
     // load shader program
     program = setup_shader(readfile("light.vert").c_str(), readfile("light.frag").c_str());
     //program = setup_shader(readfile("vs.txt").c_str(), readfile("fs.txt").c_str());
     init_shader();
     
     if (argc == 1)
          add_obj(program, "torus.obj");
     else
          add_obj(program, argv[1]);

     glEnable(GL_DEPTH_TEST);
     // prevent faces rendering to the front while they're behind other faces.
     glCullFace(GL_BACK);
     glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
     
     while (!glfwWindowShouldClose(window))
     {//program will keep draw here until you close the window
          render();
          
          glfwSwapBuffers(window);     //swap the color buffer and show it as output to the screen.
          glfwPollEvents();               //check if there is any event being triggered
     }
     
     releaseObjects();
     glfwDestroyWindow(window);
     glfwTerminate();
     return EXIT_SUCCESS;
}
