#include "buffer.hpp"

static GLuint boundVertexArrayObject = 0;

void GL_Deleter_VertexArrayObject(GLuint vao) {
    glDeleteVertexArrays(1, &vao);
}
GL_VertexArrayObject GL_CreateVertexArrayObject() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    return GL_VertexArrayObject(vao);
}
void GL_Deleter_BufferObject(GLuint vbo) {
    glDeleteBuffers(1, &vbo);
}
GL_BufferObject GL_CreateBufferObject() {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    return GL_BufferObject(vbo);
}
void GL_BindVertexArray(GLuint vao) {
    if (boundVertexArrayObject != vao) {
        glBindVertexArray(vao);
        boundVertexArrayObject = vao;
    }
}

QuadMesh::QuadMesh() : vao(GL_CreateVertexArrayObject()), vbo(GL_CreateBufferObject()) {
    constexpr float vertices[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
    GL_BindVertexArray(this->vao.get());
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo.get());
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
}
QuadMesh::~QuadMesh() = default;

void QuadMesh::draw() const {
    GL_BindVertexArray(this->vao.get());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}