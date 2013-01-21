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
#include "wsRenderSystem.h"
//*
// #include <fstream>
// #include <iostream>
// #include <sstream>
// #include <stdlib.h>
#if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
  wsShader::wsShader() {
    shaderProgram = glCreateProgram();
    vertShader = 0;
    fragShader = 0;
  }

  wsShader::wsShader(const char* vertexShaderPath, const char* fragmentShaderPath) {
    shaderProgram = glCreateProgram();
    wsAssert(addVertexShader(vertexShaderPath), "Problem adding vertex shader.");
    wsAssert(addFragmentShader(fragmentShaderPath), "Problem adding fragment shader.");
    wsAssert(install(), "Problem installing shader program.");
  }

  wsShader::~wsShader() {
    if (vertShader) { glDeleteShader(vertShader); }
    if (fragShader) { glDeleteShader(fragShader); }
    glDeleteProgram(shaderProgram);
  }

  bool wsShader::addVertexShader(const char* filePath) {
    /// Open File
    FILE* pFile;
    wsAssert( (pFile = fopen(filePath, "r")), "Error Loading file." );
    fseek(pFile, 0, SEEK_END);
    u32 fileLength = ftell(pFile);
    rewind(pFile);
    const char* fileContents = wsNewArrayTmp(char, fileLength);
    u32 resultingLength = fread((void*)fileContents, 1, fileLength, pFile);
    wsAssert(resultingLength == fileLength, "Problem reading shader file.");

    i32 compiled;
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &fileContents, NULL);
    glCompileShader(vertShader);
    glGetObjectParameterivARB(vertShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
      wsLog(WS_LOG_SHADER, "Vertex Shader did not compile\n");
      i32 logSize, charsWritten;
      glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logSize);
      char* shaderLog = wsNewArrayTmp(char, logSize);
      glGetShaderInfoLog(vertShader, logSize, &charsWritten, shaderLog);
      wsLog(WS_LOG_SHADER, "%s\n", shaderLog);
      vertShader = 0;
      return false;
    }
    glAttachShader(shaderProgram, vertShader);

    fclose(pFile);
    return true;
  }

  bool wsShader::addFragmentShader(const char* filePath) {
    /// Open File
    FILE* pFile;
    wsAssert( (pFile = fopen(filePath, "r")), "Error Loading file." );
    fseek(pFile, 0, SEEK_END);
    u32 fileLength = ftell(pFile);
    rewind(pFile);
    const char* fileContents = wsNewArrayTmp(char, fileLength);
    u32 resultingLength = fread((void*)fileContents, 1, fileLength, pFile);
    wsAssert(resultingLength == fileLength, "Problem reading shader file.");

    i32 compiled;
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fileContents, NULL);
    glCompileShader(fragShader);
    glGetObjectParameterivARB(fragShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
      wsLog(WS_LOG_SHADER, "Fragment Shader did not compile\n");
      i32 logSize, charsWritten;
      glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logSize);
      char* shaderLog = wsNewArrayTmp(char, logSize);
      glGetShaderInfoLog(fragShader, logSize, &charsWritten, shaderLog);
      wsLog(WS_LOG_SHADER, "%s\n", shaderLog);
      fragShader = 0;
      return false;
    }
    glAttachShader(shaderProgram, fragShader);

    fclose(pFile);
    return true;
  }

  bool wsShader::install() {
    wsLog(WS_LOG_SHADER, "Installing Shader Program.\n");
    glLinkProgram(shaderProgram);
    wsLog(WS_LOG_SHADER, "  Shader linked\n");
    i32 linked;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
    wsLog(WS_LOG_SHADER, "  Link status retrieved...\n");
    if (!linked) {
      wsLog(WS_LOG_SHADER, "  Shader failed to link.\n");
      i32 logSize, charsWritten;
      glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logSize);
      char* shaderLog = wsNewArrayTmp(char, logSize);
      glGetProgramInfoLog(shaderProgram, logSize, &charsWritten, shaderLog);
      wsLog(WS_LOG_SHADER, "  %s\n", shaderLog);
      return false;
    }
    wsLog(WS_LOG_SHADER, "Shader installed.\n");
    return true;
  }

  void wsShader::setTextureSampler2D(const char* varName, const u32 textureIndex) {
    i32 currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != shaderProgram) {
      glUseProgram(shaderProgram);
    }
    glUniform1i(glGetUniformLocation(shaderProgram, varName), textureIndex);
    if (currentProgram != shaderProgram) {
      glUseProgram(currentProgram);
    }
  }

  void wsShader::setUniform(const char* varName, const f32 value) {
    i32 currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != shaderProgram) {
      glUseProgram(shaderProgram);
    }
    glUniform1f(glGetUniformLocation(shaderProgram, varName), value);
    if (currentProgram != shaderProgram) {
      glUseProgram(currentProgram);
    }
  }

  void wsShader::setUniformArray(const char* varName, const f32* array, const u32 numItems) {
    i32 currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != shaderProgram) {
      glUseProgram(shaderProgram);
    }
    glUniform1fv(glGetUniformLocation(shaderProgram, varName), numItems, array);
    if (currentProgram != shaderProgram) {
      glUseProgram(currentProgram);
    }
  }

  void wsShader::setUniformInt(const char* varName, const i32 value) {
    i32 currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != shaderProgram) {
      glUseProgram(shaderProgram);
    }
    glUniform1i(glGetUniformLocation(shaderProgram, varName), value);
    if (currentProgram != shaderProgram) {
      glUseProgram(currentProgram);
    }
  }

  void wsShader::setUniformVec2(const char* varName, const f32 valueX, const f32 valueY) {
    i32 currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != shaderProgram) {
      glUseProgram(shaderProgram);
    }
    glUniform2f(glGetUniformLocation(shaderProgram, varName), valueX, valueY);
    if (currentProgram != shaderProgram) {
      glUseProgram(currentProgram);
    }
  }

  void wsShader::setUniformVec2(const char* varName, const vec4& values) {
    i32 currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != shaderProgram) {
      glUseProgram(shaderProgram);
    }
    glUniform2f(glGetUniformLocation(shaderProgram, varName), values.x, values.y);
    if (currentProgram != shaderProgram) {
      glUseProgram(currentProgram);
    }
  }

  void wsShader::setUniformVec3(const char* varName, const f32 valueX, const f32 valueY, const f32 valueZ) {
    i32 currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != shaderProgram) {
      glUseProgram(shaderProgram);
    }
    glUniform3f(glGetUniformLocation(shaderProgram, varName), valueX, valueY, valueZ);
    if (currentProgram != shaderProgram) {
      glUseProgram(currentProgram);
    }
  }

  void wsShader::setUniformVec3(const char* varName, const vec4& values) {
    i32 currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != shaderProgram) {
      glUseProgram(shaderProgram);
    }
    glUniform3f(glGetUniformLocation(shaderProgram, varName), values.x, values.y, values.z);
    if (currentProgram != shaderProgram) {
      glUseProgram(currentProgram);
    }
  }

  void wsShader::setUniformVec4(const char* varName, const f32 valueX, const f32 valueY, const f32 valueZ, const f32 valueW) {
    i32 currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != shaderProgram) {
      glUseProgram(shaderProgram);
    }
    glUniform4f(glGetUniformLocation(shaderProgram, varName), valueX, valueY, valueZ, valueW);
    if (currentProgram != shaderProgram) {
      glUseProgram(currentProgram);
    }
  }

  void wsShader::setUniformVec4(const char* varName, const vec4& values) {
    i32 currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != shaderProgram) {
      glUseProgram(shaderProgram);
    }
    glUniform4f(glGetUniformLocation(shaderProgram, varName), values.x, values.y, values.z, values.w);
    if (currentProgram != shaderProgram) {
      glUseProgram(currentProgram);
    }
  }

  void wsShader::setUniformMat4(const char* varName, const mat4& values) {
    i32 currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (currentProgram != shaderProgram) {
      glUseProgram(shaderProgram);
    }
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, varName), 16, false, values.data);
    if (currentProgram != shaderProgram) {
      glUseProgram(currentProgram);
    }
  }

  void wsShader::setVertexAttribute(const char* varName, const u32 attributeIndex) {
    glBindAttribLocation(shaderProgram, attributeIndex, varName);
  }

  void wsShader::use() {
    glUseProgram(shaderProgram);
  }

  void wsShader::end() {
    glUseProgram(0);
  }

#endif
//*/
