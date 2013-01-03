//  wsShader.cpp
//  D. Scott Nettleton
/*
    This file declares the class wsShader.
    The wsShader class loads, stores, and operates OpenGL
    shaders using the GLSL framework.
 *
 *  Copyright D. Scott Nettleton, 2013
 *  This software is released under the terms of the
 *  Lesser GNU Public License (LGPL).
 *  This file is part of the Whipstitch Game Engine.
 *  The Whipstitch Game Engine is free software: you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  The Whipstitch Game Engine is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *  You should have received a copy of the GNU Lesser Public License
 *  along with The Whipstitch Game Engine.
 *  If not, see <http://www.gnu.org/licenses/>.
*/

#include "wsShader.h"
/*
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>

wsShader::wsShader() {
  shaderProgram = glCreateProgram();
}

wsShader::~wsShader() {
  while (!vertShaders.empty()) {
    glDeleteShader(vertShaders.back());
    vertShaders.pop_back();
  }
  while (!fragShaders.empty()) {
    glDeleteShader(fragShaders.back());
    fragShaders.pop_back();
  }
  glDeleteProgram(shaderProgram);
}

bool wsShader::addVertexShader(std::string shaderName, std::string shaderFilePath) {
  /// Open File
  std::ifstream file;
  file.open(shaderFilePath.c_str());
  if (!file) {
    printf("ERROR: file %s does not exist\n", shaderFilePath.c_str());
    return false;
  }
  std::stringstream stream;
  stream << file.rdbuf();
  file.close();
  std::string fileContents;
  const char* convertedString;
  fileContents = stream.str() + "\0";
  convertedString = fileContents.c_str();

  GLuint myShader;
  GLint compiled;
  myShader = glCreateShader(GL_VERTEX_SHADER);
  vertShaders.push_back(myShader);
  glShaderSource(myShader, 1, &convertedString, NULL);
  glCompileShader(myShader);
  glGetObjectParameterivARB(myShader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    printf("Vertex Shader did not compile\n");
    GLint logSize, charsWritten;
    glGetShaderiv(myShader, GL_INFO_LOG_LENGTH, &logSize);
    GLchar* shaderLog;
    shaderLog = (GLchar*)malloc(logSize);
    glGetShaderInfoLog(myShader, logSize, &charsWritten, shaderLog);
    printf("%s\n", shaderLog);
    vertShaders.pop_back();
    delete[] shaderLog;
    return false;
  }
  glAttachShader(shaderProgram, vertShaders.back());
  return true;
}

bool wsShader::addFragmentShader(std::string shaderName, std::string shaderFilePath) {
  /// Open File
  std::ifstream file;
  file.open(shaderFilePath.c_str());
  if (!file) {
    printf("ERROR: file %s does not exist\n", shaderFilePath.c_str());
    return false;
  }
  std::stringstream stream;
  stream << file.rdbuf();
  file.close();
  std::string fileContents;
  const char* convertedString;
  fileContents = stream.str() + "\0";
  convertedString = fileContents.c_str();

  GLuint myShader;
  GLint compiled;
  myShader = glCreateShader(GL_FRAGMENT_SHADER);
  fragShaders.push_back(myShader);
  glShaderSource(fragShaders.back(), 1, &convertedString, NULL);
  glCompileShader(fragShaders.back());
  glGetShaderiv(fragShaders.back(), GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    printf("Fragment Shader did not compile\n");
    GLint logSize, charsWritten;
    glGetShaderiv(fragShaders.back(), GL_INFO_LOG_LENGTH, &logSize);
    GLchar* shaderLog;
    shaderLog = (GLchar*)malloc(logSize);
    glGetShaderInfoLog(fragShaders.back(), logSize, &charsWritten, shaderLog);
    printf("%s\n", shaderLog);
    fragShaders.pop_back();
    delete[] shaderLog;
    return false;
  }
  glAttachShader(shaderProgram, fragShaders.back());
  return true;
}

bool wsShader::install() {
  printf("Installing Shader Program.\n");
  glLinkProgram(shaderProgram);
  printf("  Shader linked\n");
  GLint linked;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
  printf("  Link status retrieved...\n");
  if (!linked) {
    printf("  Shader failed to link.\n");
    GLint logSize, charsWritten;
    glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logSize);
    GLchar* shaderLog;
    shaderLog = (GLchar*)malloc(logSize);
    glGetProgramInfoLog(shaderProgram, logSize, &charsWritten, shaderLog);
    printf("  %s\n", shaderLog);
    delete[] shaderLog;
    //glGetProgramInfoLog();
    return false;
  }
  printf("Shader installed.\n");
  return true;
}

void wsShader::setTextureSampler2D(std::string varName, GLuint textureIndex) {
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  glUseProgram(shaderProgram);
  glUniform1i(glGetUniformLocation(shaderProgram, varName.c_str()), textureIndex);
  glUseProgram(currentProgram);
}

void wsShader::setUniform(std::string varName, GLfloat value) {
  //  Suspend Current Shader (in case it's different from this one)
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  glUseProgram(shaderProgram);
  glUniform1f(glGetUniformLocation(shaderProgram, varName.c_str()), value);
  glUseProgram(currentProgram);
}

void wsShader::setUniformArray(std::string varName, GLint numItems, GLfloat* array) {
  //  Suspend Current Shader (in case it's different from this one)
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  glUseProgram(shaderProgram);
  glUniform1fv(glGetUniformLocation(shaderProgram, varName.c_str()), numItems, array);
  glUseProgram(currentProgram);
}

void wsShader::setUniformInt(std::string varName, GLint value) {
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  glUseProgram(shaderProgram);
  glUniform1i(glGetUniformLocation(shaderProgram, varName.c_str()), value);
  glUseProgram(currentProgram);
}

void wsShader::setUniformVec2(std::string varName, GLfloat valueX, GLfloat valueY) {
  //  Suspend Current Shader (in case it's different from this one)
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  glUseProgram(shaderProgram);
  glUniform2f(glGetUniformLocation(shaderProgram, varName.c_str()), valueX, valueY);
  glUseProgram(currentProgram);
}

void wsShader::setUniformVec3(std::string varName, GLfloat valueX, GLfloat valueY, GLfloat valueZ) {
  //  Suspend Current Shader (in case it's different from this one)
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  glUseProgram(shaderProgram);
  glUniform3f(glGetUniformLocation(shaderProgram, varName.c_str()), valueX, valueY, valueZ);
  glUseProgram(currentProgram);
}

void wsShader::setUniformVec3(std::string varName, cogVector values) {
  //  Suspend Current Shader (in case it's different from this one)
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  glUseProgram(shaderProgram);
  glUniform3f(glGetUniformLocation(shaderProgram, varName.c_str()), values.x, values.y, values.z);
  glUseProgram(currentProgram);
}

void wsShader::setUniformVec3(std::string varName, Color values) {
  //  Suspend Current Shader (in case it's different from this one)
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  glUseProgram(shaderProgram);
  glUniform3f(glGetUniformLocation(shaderProgram, varName.c_str()), values.getRf(), values.getGf(), values.getBf());
  glUseProgram(currentProgram);
}

void wsShader::setUniformVec4(std::string varName, GLfloat valueX, GLfloat valueY, GLfloat valueZ, GLfloat valueW) {
  //  Suspend Current Shader (in case it's different from this one)
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  glUseProgram(shaderProgram);
  glUniform4f(glGetUniformLocation(shaderProgram, varName.c_str()), valueX, valueY, valueZ, valueW);
  glUseProgram(currentProgram);
}

void wsShader::setUniformVec4(std::string varName, cogVector values, GLfloat valueW) {
  //  Suspend Current Shader (in case it's different from this one)
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  glUseProgram(shaderProgram);
  glUniform4f(glGetUniformLocation(shaderProgram, varName.c_str()), values.x, values.y, values.z, valueW);
  glUseProgram(currentProgram);
}

void wsShader::setUniformVec4(std::string varName, Color values) {
  //  Suspend Current Shader (in case it's different from this one)
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  glUseProgram(shaderProgram);
  glUniform4f(glGetUniformLocation(shaderProgram, varName.c_str()), values.getRf(), values.getGf(), values.getBf(), values.getAf());
  glUseProgram(currentProgram);
}

void wsShader::setVertexAttribute(std::string varName, GLuint attributeIndex) {
  glBindAttribLocation(shaderProgram, attributeIndex, varName.c_str());
}

void wsShader::use() {
  glUseProgram(shaderProgram);
}

void wsShader::end() {
  glUseProgram(0);
}
//*/
