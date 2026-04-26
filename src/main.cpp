#include <exception>
#include <vector>
#include "raii.hpp"

void compileShaderSource(GLuint shader, const GLchar *const *source) {
    glShaderSource(shader, 1, source, nullptr);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<GLchar> infoLog = std::vector<GLchar>(length);
        glGetShaderInfoLog(shader, length, nullptr, infoLog.data());
        throw std::runtime_error(std::string("Failed to compile shader: ") + infoLog.data());
    }
}
void linkProgram(GLuint program) {
    glLinkProgram(program);
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::vector<GLchar> infoLog = std::vector<GLchar>(length);
        glGetProgramInfoLog(program, length, nullptr, infoLog.data());
        throw std::runtime_error(std::string("Failed to link shader program: ") + infoLog.data());
    }
}

void initViewport(SDL_Window *window) {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
}

void entry() {
    SDL sdl = SDL(SDL_INIT_VIDEO);
    SDL_WindowPtr window = SDL_WindowPtr(SDL_CreateWindow("Turing Village 2", 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY));
    if (window.get() == nullptr) {
        throw std::runtime_error("Failed to create SDL window");
    }
    SDL_GLContextPtr glContext = SDL_GLContextPtr(SDL_GL_CreateContext(window.get()));
    if (glContext.get() == nullptr) {
        throw std::runtime_error("Failed to create OpenGL context");
    }
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
    initViewport(window.get());

    GL_ShaderProgram shaderProgram = GL_ShaderProgram(glCreateProgram());
    GL_Shader vertexShader = GL_Shader(glCreateShader(GL_VERTEX_SHADER));
    const char *vertexShaderSource = R"(
        #version 410
        layout(location = 0) in vec2 a_Position;
        layout(location = 0) out vec3 v_Color;

        void main() {
            gl_Position = vec4(a_Position, 0.0, 1.0);
            v_Color = vec3(a_Position + 0.5, 0.2);
        }
    )";
    compileShaderSource(vertexShader.get(), &vertexShaderSource);

    GL_Shader fragmentShader = GL_Shader(glCreateShader(GL_FRAGMENT_SHADER));
    const char *fragmentShaderSource = R"(
        #version 410

        layout(location = 0) in vec3 v_Color;
        layout(location = 0) out vec4 f_Color;

        void main() {
            f_Color = vec4(v_Color, 1.0);
        }
    )";
    compileShaderSource(fragmentShader.get(), &fragmentShaderSource);
    
    glAttachShader(shaderProgram.get(), vertexShader.get());
    glAttachShader(shaderProgram.get(), fragmentShader.get());
    linkProgram(shaderProgram.get());

    GL_VertexArrayObject vao = GL_CreateVertexArrayObject();
    glBindVertexArray(vao.get());

    GL_BufferObject vbo = GL_CreateBufferObject();
    glBindBuffer(GL_ARRAY_BUFFER, vbo.get());
    constexpr float vertices[] = {
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
                glViewport(0, 0, event.window.data1, event.window.data2);
            }
        }

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram.get());
        glBindVertexArray(vao.get());
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window.get());
    }
}

int main() {
    try {
        entry();
    } catch (const std::exception &exception) {
        SDL_Log("Exception: %s", exception.what());
        return 1;
    }
    return 0;
}