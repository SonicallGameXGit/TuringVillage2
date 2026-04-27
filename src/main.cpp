// TODO: #define SDL_MAIN_USE_CALLBACKS 1 - for easier future integration with other platforms
#include <stdexcept>
#include <exception>

#include "raii.hpp"
#include "camera.hpp"
#include "villager.hpp"
#include "maths.hpp"
#include "window.hpp"
#include "cursor.hpp"
#include "gl/shader/quad.hpp"
#include "gl/shader/connection_line.hpp"
#include "gl/buffer.hpp"
#include "gl/texture.hpp"

void initViewport() {
    int width, height;
    SDL_GetWindowSizeInPixels(SDL_GL_GetCurrentWindow(), &width, &height);
    glViewport(0, 0, width, height);
}
// void buildEmptyTexture(GLuint texture) {
//     uint32_t emptyData = 0xffffffff;
//     glBindTexture(GL_TEXTURE_2D, texture);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &emptyData);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// }

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

    // GL_Texture emptyTexture = GL_CreateTexture();
    // buildEmptyTexture(emptyTexture.get());

    QuadShader quadShader = QuadShader();
    ConnectionLineShader connectionLineShader = ConnectionLineShader();

    Texture pidorasTexture = Texture::loadFromFile("./assets/pidoras.png", GL_NEAREST, GL_CLAMP_TO_EDGE);
    Texture connectionLineTexture = Texture::loadFromFile("./assets/connection_line.png", GL_NEAREST, GL_REPEAT);

    QuadMesh quadMesh = QuadMesh();

    Camera camera = { .zoom = 0.01f };
    Cursor cursor = Cursor();

    std::vector<Villager> villagers = std::vector<Villager>();
    for (int i = 0; i < 15; i++) {
        Villager villager;
        villager.position.x = static_cast<float>(rand() % 2000 - 1000) / 10.0f; // -10.0f to 10.0f
        villager.position.y = static_cast<float>(rand() % 2000 - 1000) / 10.0f; // -10.0f to 10.0f
        villagers.push_back(villager);
    }
    for (size_t i = 0; i < villagers.size(); i++) {
        Villager &villager = villagers[i];
        if (rand() % 100 < 30) {
            size_t coworkerId = rand() % villagers.size();
            while (coworkerId == i) {
                coworkerId = rand() % villagers.size();
            }
            villager.coworkersIds.push_back(coworkerId);
        }
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

        cursor.update(Vec2 { .x = static_cast<float>(windowWidth), .y = static_cast<float>(windowHeight) }, camera);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

        connectionLineShader.bind();
        connectionLineShader.setAspectRatio(aspectRatio);
        connectionLineShader.setCamera(camera);
        connectionLineShader.setWidth(3.0f);
        connectionLineShader.setTime(SDL_GetTicks() / 1000.0f);

        glBindTexture(GL_TEXTURE_2D, connectionLineTexture.glTexture.get());
        for (const Villager &villager : villagers) {
            for (uint32_t coworkerId : villager.coworkersIds) {
                const Villager &coworker = villagers[coworkerId]; // FIXME: It often can cause undefined behavior because of villagers dying, spawning, removing, etc. It should be handled somehow (e.g. Restrict access to all fields in the structure and give hepler functions that'd help to handle everything correctly, + create some villager manager class)
                connectionLineShader.setPoints(
                    Vec2 { .x = villager.position.x + pidorasTexture.width / 2.0f, .y = villager.position.y + pidorasTexture.height / 2.0f },
                    Vec2 { .x = coworker.position.x + pidorasTexture.width / 2.0f, .y = coworker.position.y + pidorasTexture.height / 2.0f }
                );
                quadMesh.draw();
            }
        }

        quadShader.bind();
        quadShader.setAspectRatio(aspectRatio);
        quadShader.setCamera(camera);

        for (size_t i = 0; i < villagers.size(); i++) {
            const Villager &villager = villagers[i];

            glBindTexture(GL_TEXTURE_2D, pidorasTexture.glTexture.get());
            quadShader.setTransform(villager.position, Vec2 { .x = static_cast<float>(pidorasTexture.width), .y = static_cast<float>(pidorasTexture.height) });
            quadMesh.draw();

            if (cursor.getX() >= villager.position.x && cursor.getX() <= villager.position.x + pidorasTexture.width && cursor.getY() >= villager.position.y && cursor.getY() <= villager.position.y + pidorasTexture.height) {
                printf("Mouse is hovering over villager (%zu) at coordinates: %f, %f\n", i, villager.position.x, villager.position.y);
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