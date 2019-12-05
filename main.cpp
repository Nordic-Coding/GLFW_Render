#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <iostream>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Shaders/ShaderProgram.h"
#include <cmath>
#include <vector>

#define xWidth 640
#define yWidth 640
#define nPoints 30000

static int oldX = 0,oldY = 0;
static bool resetCursorPos,mouseDown,rightMouseDown;
static float rotate_x = 0.0f, rotate_y = 0.0f, dist_z = -30.0f;

static void callBackMouseClick(GLFWwindow* window, int button, int action, int mods)
{
  if(button == GLFW_MOUSE_BUTTON_LEFT)
  {
    if(action == GLFW_PRESS)
    {
      mouseDown = true;
      resetCursorPos = true;
    }
    else
    {
      mouseDown = false;
    }
  }
  else if(button == GLFW_MOUSE_BUTTON_RIGHT)
  {
    if(action == GLFW_PRESS)
    {
      rightMouseDown = true;
      resetCursorPos = true;
    }
    else
    {
      rightMouseDown = false;
    }
  }
}


static void callBackMouseMotion(GLFWwindow* window, double xpos, double ypos)
{
  if((mouseDown || rightMouseDown) && resetCursorPos)
  {
    oldX = xpos;
    oldY = ypos;
    resetCursorPos = false;
  }
  else if(mouseDown)
  {
    //std::cout << "Moving with mouse down!" << '\n';
    rotate_x += 0.002f*(xpos-oldX);
    rotate_y += 0.002f*(ypos-oldY);
    std::cout << "Updated rotate is now: " << rotate_x << " " << rotate_y << '\n';
    oldX = xpos;
    oldY = ypos;
  }
  else if(rightMouseDown)
  {
    float dist = (ypos-oldY);
    dist_z += dist*0.001f*dist_z;
    oldX = xpos;
    oldY = ypos;
  }
}


/*
  This function should render some triangular thing.
 */
int main(int argc, char** args)
{
  //Create the window.
  if(!glfwInit())
  {
    std::cout << "Could not initialize GLFW3!" << '\n';
  }

  //Null for windowed mode, resources will not be shared with another
  //window and therefore the share is also Null. Also set this as the current OpenGL context.
  GLFWwindow* window = glfwCreateWindow(xWidth, yWidth, "someTest", NULL, NULL);
  glfwMakeContextCurrent(window);

  if(glewInit() != GLEW_OK)
  {
    std::cout << "Failed to Initialize GLEW! exiting" << '\n';
    return -1;
  }

  glfwSetCursorPosCallback(window, callBackMouseMotion);
  glfwSetMouseButtonCallback(window, callBackMouseClick);

  //Also remove the 60 FPS cap.
  glfwSwapInterval(1);


  //All the GL initializations here.
  glClearColor(0.0,0.0,0.0,1.0);
  //glEnable(GL_DEPTH_TEST); //Make stuff at front appear at front.
  //glEnable(GL_ALPHA_TEST);
  glEnable (GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Make alpha blending a thing
  glBlendFunc(GL_ONE, GL_ONE);
  glBlendEquation(GL_MAX);
  glPointSize(3.0f); //Make the points a bit larger to avoid the crystal effect.
  glViewport(0, 0, xWidth, yWidth);

  //Set up the matrices, the view matrix is recalculated every time.
  glm::mat4 Projection = glm::perspective<float>(glm::radians(45.0f), 4.0 / 4.0f, 0.1f, 10000.f);
  glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
  glm::mat4 View;
  glm::quat rot;

  //Create and compile the shaders, the ShaderProgram functions fixes almost everything.
  Render::ShaderProgram* program = new Render::ShaderProgram("Shaders/shader.vert", "Shaders/shader.frag");


  //Get the attributes from the program.
  GLint posAttrib = program->getAttribLocation("pos");
  GLint colAttrib = program->getAttribLocation("col");
  GLint mvpAttrib = program->getUniformLocation("MVP");

  //Set up the vao.
  GLuint vao,vbo;
  glGenVertexArrays(1, &vao);

  //Set up the vbo and vao.
  float* geomVals = new float[nPoints*6];
  GLuint vboGeom,vboCol;

  for(int i = 0; i < nPoints; i++)
  {
    geomVals[i*3] = ((float)(rand()%257)-128.0)/(256.0f);
    geomVals[i*3+1] = ((float)(rand()%257)-128.0)/(256.0f);
    geomVals[i*3+2] = ((float)(rand()%257)-128.0)/(256.0f);
    geomVals[i*3+nPoints*3] = 0.001f*((float)(rand()%257)-128.0)/(256.0f);
    geomVals[i*3+nPoints*3+1] = 0.001f*((float)(rand()%257)-128.0)/(256.0f);
    geomVals[i*3+nPoints*3+2] = 0.001f*((float)(rand()%257)-128.0)/(256.0f);
  }

  //Generate and upload buffers.
  glGenBuffers(1,&vboGeom);
  glBindBuffer(GL_ARRAY_BUFFER, vboGeom);
  glBufferData(GL_ARRAY_BUFFER, nPoints*3*sizeof(float), geomVals, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1,&vboCol);
  glBindBuffer(GL_ARRAY_BUFFER, vboCol);
  glBufferData(GL_ARRAY_BUFFER, nPoints*3*sizeof(float), geomVals+nPoints*3, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  //Free the local memory.
  //delete[] geomVals;

  //Define the vao, bind the vao and carry out all parts necessary for the
  //program to save it.
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vboGeom);
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib,3,GL_FLOAT,GL_FALSE,0,0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(posAttrib);
  glBindBuffer(GL_ARRAY_BUFFER, vboCol);
  glEnableVertexAttribArray(colAttrib);
  glVertexAttribPointer(colAttrib,3,GL_FLOAT,GL_FALSE,0,0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(colAttrib);
  glBindVertexArray(0);

  float r_x = 0.0f;
  float r_y = 0.0f;

  glPointSize(1.0f);
  
  while (!glfwWindowShouldClose(window))
  {
    float ratio;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    //Update the points?
    for(int i = 0; i < nPoints; i++)
    {
      geomVals[i*3+nPoints*3] += 0.01f*((float)(rand()%257)-128.0)/(256.0f);
      geomVals[i*3+nPoints*3+1] += 0.01f*((float)(rand()%257)-128.0)/(256.0f);
      geomVals[i*3+nPoints*3+2] += 0.01f*((float)(rand()%257)-128.0)/(256.0f);

      geomVals[i*3] += 0.1f*(geomVals[i*3+nPoints*3]);
      geomVals[i*3+1] += 0.1f*(geomVals[i*3+nPoints*3+1]);
      geomVals[i*3+2] += 0.1f*(geomVals[i*3+nPoints*3+2]);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vboGeom);
    glBufferData(GL_ARRAY_BUFFER, nPoints*3*sizeof(float), geomVals, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vboCol);
    glBufferData(GL_ARRAY_BUFFER, nPoints*3*sizeof(float), geomVals+nPoints*3, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Set the matrices.

    //Choose the program.
    program->bindProgram();

    r_x += 0.0f*(((float)(rand()%256))/(1000.0f)-255.0/(2.0f*1000.0f));
    r_y += 0.0f*(((float)(rand()%256))/(1000.0f)-255.0/(2.0f*1000.0f));

    //Upload the matrix.
    View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,dist_z));
    //View = glm::rotate(View, rotate_x, glm::vec3(sin(6.28f*rotate_x/(360.0f)),cos(6.28f*rotate_y/(360.0f)),0.0f));
    //View = glm::rotate(View, rotate_y, glm::vec3(cos(6.28f*rotate_x/(360.0f)),sin(6.28f*rotate_y/(360.0f)),0.0f));
    View = glm::rotate(View, rotate_x, glm::vec3(sin(6.28f*rotate_x/(360.0f)),cos(6.28f*rotate_y/(360.0f)),sin(6.28f*rotate_x/(360.0f))*cos(6.28f*rotate_y/(360.0f))));
    View = glm::rotate(View, rotate_y, glm::vec3(cos(6.28f*rotate_y/(360.0f)),sin(6.28f*rotate_x/(360.0f)),sin(6.28f*rotate_y/(360.0f))*cos(6.28f*rotate_x/(360.0f))));
    Projection = glm::perspective<float>(glm::radians(90.0f), ((float)width)/((float)height), 0.01f, 1000.f);
    glm::mat4 MVP = Projection * View * Model;
    glUniformMatrix4fv(mvpAttrib, 1, GL_FALSE, &MVP[0][0]);

    //Enable the attributes and vao.
    glBindVertexArray(vao);
    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(colAttrib);
    //Draw it as points for now.
    glDrawArrays(GL_POINTS, 0, nPoints);

    //Disable the attributes.
    glDisableVertexAttribArray(posAttrib);
    glDisableVertexAttribArray(colAttrib);
    glBindVertexArray(0);

    //unbind the program again
    program->unbindProgram();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

}
