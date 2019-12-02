#include "ShaderProgram.h"

namespace Render
{
  static std::string getFullFile(char* source)
  {
    std::ifstream file(source);
    std::string str;
    std::string file_contents;

    while (std::getline(file, str))
    {
      file_contents += str;
      file_contents.push_back('\n');
    }

    //Close the file and return the source.
    file.close();
    return file_contents;
  }

  ShaderProgram::ShaderProgram(char* vertexSource, char* fragSource)
  {
    //Load files
    std::string sourceCodeVert = getFullFile(vertexSource);
    std::string sourceCodeFrag = getFullFile(fragSource);

    //Create shaders
    this->vertShader = glCreateShader(GL_VERTEX_SHADER);
    this->fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    //Compile the shaders.
    if(!compileShader(&this->vertShader, (const GLchar *) sourceCodeVert.c_str()))
    {
      std::cout << "Failed on vertex shader compilation." << '\n';
    }

    if(!compileShader(&this->fragShader, (const GLchar *) sourceCodeFrag.c_str()))
    {
      std::cout << "Failed on fragment shader compilation." << '\n';
    }

    //Link the program.
    if(!compileProgram())
    {
      std::cout << "Failed to link the program." << '\n';
    }

  }

  bool ShaderProgram::compileShader(GLuint *shader, const GLchar* source)
  {
    glShaderSource(*shader, 1, &source, 0);
    glCompileShader(*shader);

    GLint isCompiled = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &isCompiled);

    if(isCompiled == GL_FALSE)
    {
      GLint maxLength = 0;
      glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &maxLength);

      // The maxLength includes the NULL character
      std::vector<GLchar> infoLog(maxLength);
      glGetShaderInfoLog(*shader, maxLength, &maxLength, &infoLog[0]);

      // We don't need the shader anymore.
      glDeleteShader(*shader);

      //Print the infolog.
      std::cout << &infoLog[0] << '\n';
      return false;
    }

    return true;
  }

  bool ShaderProgram::compileProgram()
  {
    this->program = glCreateProgram();
    glAttachShader(this->program, this->vertShader);
    glAttachShader(this->program, this->fragShader);

    glLinkProgram(this->program);

    GLint isLinked = 0;
    glGetProgramiv(this->program, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
      GLint maxLength = 0;
      glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &maxLength);

      // The maxLength includes the NULL character
      std::vector<GLchar> infoLog(maxLength);
      glGetProgramInfoLog(this->program, maxLength, &maxLength, &infoLog[0]);

      // We don't need the program anymore.
      glDeleteProgram(this->program);

      // Don't leak shaders either.
      glDeleteShader(this->vertShader);
      glDeleteShader(this->fragShader);

      //Print the infolog.
      std::cout << &infoLog[0] << '\n';

      // In this simple program, we'll just leave
      std::cout << "Failed to link program!" << '\n';
      return false;
    }

    //Detach the shader after a succesful linking.
    glDetachShader(this->program, this->vertShader);
    glDetachShader(this->program, this->fragShader);

    return true;
  }

  void ShaderProgram::bindProgram()
  {
    glUseProgram(this->program);
  }

  void ShaderProgram::unbindProgram()
  {
    glUseProgram(0);
  }

  GLint ShaderProgram::getAttribLocation(char* attribName)
  {
    std::cout << "Attrib location is: " << glGetAttribLocation(this->program, attribName) << '\n';
    return glGetAttribLocation(this->program, attribName);
  }

  GLint ShaderProgram::getUniformLocation(char *uniformName)
  {
    std::cout << "Uniform location is: " << glGetUniformLocation(this->program, uniformName) << '\n';
    return glGetUniformLocation(this->program, uniformName);
  }
}
