//  wsShader.h
//  D. Scott Nettleton
/*
 *  This file implements the class wsShader.
 *  The wsShader class loads, stores, and operates OpenGL
 *  shaders using the GLSL framework.
 *
 *  This software is provided under the terms of the MIT license
 *  Copyright (c) D. Scott Nettleton, 2013
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy,
 *  modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef WS_SHADER_H_
#define WS_SHADER_H_

#include "../wsUtils.h"

class wsShader {
  private:
    //  Unsigned Integer storing a GL reference to the shader program
    u32 shaderProgram;
    //  Unsigned integers storing GL references to compiled shaders
    u32 vertShader;
    u32 fragShader;
  public:
    /// Constructor
    wsShader();
    wsShader(const char* vertexShaderPath, const char* fragmentShaderPath, bool delayLinking = false);
    ~wsShader();
    /// Getters
    u32 getProgram() { return shaderProgram; }
    void setTextureSampler2D(const char* varName, const u32 textureIndex);
    void setUniform(const char* varName, const f32 value);
    void setUniformArray(const char* varName, const f32* array, const u32 numItems);
    void setUniformInt(const char* varName, const i32 value);
    void setUniformVec2(const char* varName, const f32 valueX, const f32 valueY);
    void setUniformVec2(const char* varName, const vec4& values);
    void setUniformVec3(const char* varName, const f32 valueX, const f32 valueY, const f32 valueZ);
    void setUniformVec3(const char* varName, const vec4& values);
    void setUniformVec4(const char* varName, const f32 valueX, const f32 valueY, const f32 valueZ, const f32 valueW);
    void setUniformVec4(const char* varName, const vec4& values);
    void setUniformVec4Array(const char* varName, const vec4* array, const u32 numItems);
    void setUniformMat4(const char* varName, const mat4& values);
    void setVertexAttribute(const char* varName, const u32 attributeIndex);
    /// Operational Member Functions
    bool addVertexShader(const char* shaderFilePath);
    bool addFragmentShader(const char* shaderFilePath);
    bool install();
    void use(); //  For using pairs (vertex and fragment shaders) given the same name
    static void end();
};

#endif // WS_SHADER_H_
