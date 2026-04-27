#pragma once
#include <glad/glad.h>
#include "../raii.hpp"

// GL_VertexArrayObject
void GL_Deleter_VertexArrayObject(GLuint vao);
using GL_VertexArrayObject = Resource<GLuint, GL_Deleter_VertexArrayObject>;
GL_VertexArrayObject GL_CreateVertexArrayObject();

void GL_BindVertexArray(GLuint vao);

// GL_BufferObject
void GL_Deleter_BufferObject(GLuint vbo);
using GL_BufferObject = Resource<GLuint, GL_Deleter_BufferObject>;
GL_BufferObject GL_CreateBufferObject();

class QuadMesh {
private:
    GL_VertexArrayObject vao;
    GL_BufferObject vbo;
public:
    QuadMesh();
    ~QuadMesh();

    void draw() const;
};