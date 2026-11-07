#include <glad/glad.h>
#include <iostream>

#include "shader.h"

// fonction qui compile un shader
unsigned int compilerShader(unsigned int type, const char *source) {
  unsigned int shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  int succes;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &succes);

  if (!succes) {
    char message[512];
    glGetShaderInfoLog(shader, 512, nullptr, message);
    std::cout << "erreur shader : " << message << std::endl;
  }

  return shader;
}
