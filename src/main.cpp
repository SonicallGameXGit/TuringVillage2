// TODO: #define SDL_MAIN_USE_CALLBACKS 1 - for easier future integration with other platforms
#include <stdexcept>
#include <exception>
#include <vector>

#include <SDL3_image/SDL_image.h>
#include "raii.hpp"
#include "camera.hpp"
#include "villager.hpp"

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
struct Texture {
    GL_Texture glTexture;
    uint32_t width, height;

    Texture(const char *filename, GLenum filter) : glTexture(GL_CreateTexture()) {
        SDL_Surface *surface = IMG_Load(filename);
        if (surface == nullptr) {
            throw std::runtime_error(std::string("Failed to load image: ") + SDL_GetError());
        }
        this->width = static_cast<uint32_t>(surface->w);
        this->height = static_cast<uint32_t>(surface->h);

        // Let's instead just make proper assets, instead of doubling loading time by converting them for no reason every time?
        // SDL_Surface *rgbaSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
        // SDL_DestroySurface(surface);
        // if (rgbaSurface == nullptr) {
        //     throw std::runtime_error(std::string("Failed to convert image to RGBA32: ") + SDL_GetError());
        // }

        glBindTexture(GL_TEXTURE_2D, this->glTexture.get());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
        SDL_DestroySurface(surface);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    }
};

void initViewport() {
    int width, height;
    SDL_GetWindowSizeInPixels(SDL_GL_GetCurrentWindow(), &width, &height);
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

    int windowWidth, windowHeight;
    SDL_GetWindowSize(window.get(), &windowWidth, &windowHeight);

    initViewport();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GL_ShaderProgram shaderProgram = GL_ShaderProgram(glCreateProgram());
    // We no longer need shaders after linking, so let RAII auto-delete them sooner :3
    {
        GL_Shader vertexShader = GL_Shader(glCreateShader(GL_VERTEX_SHADER));
        const char *vertexShaderSource = R"(
            #version 410
            layout(location = 0) in vec2 a_Position;
            layout(location = 0) out vec2 v_TexCoord;

            uniform vec4 u_Transform;
            uniform vec3 u_CameraTransform;
            uniform float u_AspectRatio;

            void main() {
                gl_Position = vec4(a_Position, 0.0, 1.0);
                gl_Position.xy = gl_Position.xy * u_Transform.zw + u_Transform.xy;
                gl_Position.xy = (gl_Position.xy - u_CameraTransform.xy) * u_CameraTransform.z;
                gl_Position.x /= u_AspectRatio;
                v_TexCoord = vec2(a_Position.x, 1.0 - a_Position.y);
            }
        )";
        compileShaderSource(vertexShader.get(), &vertexShaderSource);

        GL_Shader fragmentShader = GL_Shader(glCreateShader(GL_FRAGMENT_SHADER));
        const char *fragmentShaderSource = R"(
            #version 410
            layout(location = 0) in vec2 v_TexCoord;
            layout(location = 0) out vec4 f_Color;

            uniform sampler2D u_Texture;

            void main() {
                f_Color = texture(u_Texture, v_TexCoord);
            }
        )";
        compileShaderSource(fragmentShader.get(), &fragmentShaderSource);
        
        glAttachShader(shaderProgram.get(), vertexShader.get());
        glAttachShader(shaderProgram.get(), fragmentShader.get());
        linkProgram(shaderProgram.get());
    }

    GL_VertexArrayObject vao = GL_CreateVertexArrayObject();
    glBindVertexArray(vao.get());

    GL_BufferObject vbo = GL_CreateBufferObject();
    glBindBuffer(GL_ARRAY_BUFFER, vbo.get());
    constexpr float vertices[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    Texture texture = Texture("./assets/pidoras.png", GL_NEAREST);

    Camera camera = { .zoom = 0.01f };

    std::vector<Villager> villagers = std::vector<Villager>();
    for (int i = 0; i < 10; i++) {
        Villager villager;
        villager.x = static_cast<float>(rand() % 2000 - 1000) / 10.0f; // -10.0f to 10.0f
        villager.y = static_cast<float>(rand() % 2000 - 1000) / 10.0f; // -10.0f to 10.0f
        villagers.push_back(villager);
    }

    bool running = true;
    while (running) {
        // DO NOT EVER DO ANYTHING BEFORE EVENT HANDLING! IT CAN CAUSE WEIRD BUGS BECAUSE OF IMMEDIATE LOOP EXITING!
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                break; // ps. Made to ensure window will be closed immediately, without a chance to get frozen, which is awful.
            }
            if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
                glViewport(0, 0, event.window.data1, event.window.data2);
            }
            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                windowWidth = event.window.data1;
                windowHeight = event.window.data2;
            }
        }

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

        glUseProgram(shaderProgram.get());
        glUniform1f(glGetUniformLocation(shaderProgram.get(), "u_AspectRatio"), aspectRatio);
        glUniform3f(glGetUniformLocation(shaderProgram.get(), "u_CameraTransform"), camera.x, camera.y, camera.zoom); // X, Y, Zoom

        glBindVertexArray(vao.get());
        
        float worldMouseX, worldMouseY;
        SDL_GetGlobalMouseState(&worldMouseX, &worldMouseY);
        worldMouseX /= static_cast<float>(windowWidth);
        worldMouseY /= static_cast<float>(windowHeight);
        worldMouseY = 1.0f - worldMouseY;
        worldMouseX *= 2.0f;
        worldMouseY *= 2.0f;
        worldMouseX -= 1.0f;
        worldMouseY -= 1.0f;
        worldMouseX *= aspectRatio;
        worldMouseX /= camera.zoom;
        worldMouseY /= camera.zoom;
        worldMouseX += camera.x;
        worldMouseY += camera.y;

        for (size_t i = 0; i < villagers.size(); i++) {
            const Villager &villager = villagers[i];
            
            glBindTexture(GL_TEXTURE_2D, texture.glTexture.get());
            glUniform4f(glGetUniformLocation(shaderProgram.get(), "u_Transform"), villager.x, villager.y, static_cast<float>(texture.width), static_cast<float>(texture.height)); // X, Y, Width, Height
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            if (worldMouseX >= villager.x && worldMouseX <= villager.x + texture.width && worldMouseY >= villager.y && worldMouseY <= villager.y + texture.height) {
                printf("Mouse is hovering over villager (%zu) at coordinates: %f, %f\n", i, villager.x, villager.y);
            }
        }

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