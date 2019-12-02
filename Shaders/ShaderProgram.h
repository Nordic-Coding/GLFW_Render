#include <GL/glew.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

namespace Render
{
#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

  class ShaderProgram
  {
  public:
    ShaderProgram(char* vertexSource, char* fragSource);
    ~ShaderProgram();

    void bindProgram();
    void unbindProgram();

    GLint getAttribLocation(char* attribName);
    GLint getUniformLocation(char* uniformName);

  private:
    GLuint program, vertShader, fragShader;
    bool compileShader(GLuint *shader, const GLchar* source);
    bool compileProgram();
  };

#endif
}
