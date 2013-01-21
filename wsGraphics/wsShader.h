//  wsShader.h
//  D. Scott Nettleton
/*
 *  This file implements the class wsShader.
 *  The wsShader class loads, stores, and operates OpenGL
 *  shaders using the GLSL framework.
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

#ifndef WS_SHADER_H_
#define WS_SHADER_H_

#include "../wsUtils.h"
#include "GL/glew.h"

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
    wsShader(const char* vertexShaderPath, const char* fragmentShaderPath);
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
    void setUniformMat4(const char* varName, const mat4& values);
    void setVertexAttribute(const char* varName, const u32 attributeIndex);
    /// Operational Member Functions
    bool addVertexShader(const char* shaderFilePath);
    bool addFragmentShader(const char* shaderFilePath);
    bool install();
    void use(); //  For using pairs (vertex and fragment shaders) given the same name
    void end();
};

#endif // WS_SHADER_H_
