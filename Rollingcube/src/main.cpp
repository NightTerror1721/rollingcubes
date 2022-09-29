#include <iostream>
#include <vector>

#include "core/window.h"

#include "math/color.h"

#include "engine/objmodel.h"
#include "engine/shader.h"
#include "engine/camera.h"
#include "engine/texture.h"

#include "utils/logger.h"
#include "utils/bmp_decoder.h"
#include "utils/image.h"

#include <JPEG/jpeglib.h>

int test_window();
void tutos();

ObjModel createCubeModel();

int main(int argc, char** argv)
{
    tutos();


    //test_window();
    
	return 0;
}


struct TutoMove
{
    glm::vec3 position{ 0, 0, 5 };
    float horizontalAngle = glm::pi<float>();
    float verticalAngle = 0;
    float initialFoV = 45.f;
    float speed = 3.f;
    float mouseSpeed = .025f;


    void updateOrientation(double delta, const glm::vec2& mousePos)
    {
        horizontalAngle += float(mouseSpeed * delta * ((window::default_width / 2) - mousePos.x));
        verticalAngle += float(mouseSpeed * delta * ((window::default_height / 2) - mousePos.y));
    }

    inline glm::vec3 direction() const
    {
        return {
            glm::cos(verticalAngle) * glm::sin(horizontalAngle),
            glm::sin(verticalAngle),
            glm::cos(verticalAngle) * glm::cos(horizontalAngle)
        };
    }

    inline glm::vec3 right() const
    {
        return {
            glm::sin(horizontalAngle) - glm::pi<float>() / 2.f,
            0,
            glm::cos(horizontalAngle) - glm::pi<float>() / 2.f
        };
    }

    inline glm::vec3 up() const { return glm::cross(right(), direction()); }
};


glm::vec2 getMousePosition()
{
    double x, y;
    glfwGetCursorPos(window::getMainWindow(), &x, &y);
    return { float(x), float(y) };
}

void setMousePosition(const glm::vec2& pos)
{
    glfwSetCursorPos(window::getMainWindow(), double(pos.x), double(pos.y));
}

void resetMousePosition()
{
    setMousePosition({ window::default_width / 2, window::default_height / 2 });
}


ObjModel createSimpleModel(const std::vector<glm::vec3>& vertices)
{
    ObjModel model;
    auto& mesh = model.createMesh("default").get();
    mesh.setVertices(vertices);

    return model;
}

ObjModel createTexturedModel(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs)
{
    ObjModel model;
    auto& mesh = model.createMesh("default").get();
    mesh.setVertices(vertices);
    mesh.setUVs(uvs);

    return model;
}

void tutos()
{
    window::createMainWindow();

    /*static constexpr GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f
    };*/


    Texture tex;
    if (!tex.load("test/uvmap.jpg"))
        return;

    tex.bind();
    tex.setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT, GL_TEXTURE_2D);
    tex.setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT, GL_TEXTURE_2D);
    tex.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR, GL_TEXTURE_2D);
    tex.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR, GL_TEXTURE_2D);
    tex.unbind();




    Shader shader;
    shader.load("test/SimpleVertexShader.vs", "test/SimpleFragmentShader.fs");

    ObjModel objmodel;
    objmodel.load("test/suzanne.obj");

    Material material;
    material.setDiffuseColor({ 1, 1, 1 });
    material.setAmbientColor({ 0.1, 0.1, 0.1 });
    material.setSpecularColor({ 0.3, 0.3, 0.3 });
    material.setShininess(10);

    TutoMove mover;

    glm::mat4 model = glm::mat4(1.f);

    Camera cam;
    cam.setToPerspective(glm::radians(45.f), float(window::default_width) / float(window::default_height), 0.1f, 100.f);
    cam.lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    auto mvp = cam.mvp(model);


    GLuint textureLoc = glGetUniformLocation(shader.programId(), "mainTexture");


    glm::vec3 lightPos = glm::vec3(4, 4, 4);


    double lastTime = 0;

    resetMousePosition();

    Time timeAccum;
    Time marksAccum;
    unsigned int marksCount = 0;
    Clock clock;

    window::simpleLoop(false, [&](Time elapsedTime) {


        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;


        auto mpos = getMousePosition();
        resetMousePosition();

        mpos.x = (window::default_width / 2) - mpos.x;
        mpos.y = (window::default_height / 2) - mpos.y;

        cam.rotateLocal(0.1f * deltaTime * mpos.x, { 0, 1, 0 });
        cam.rotateLocal(0.1f * deltaTime * mpos.y, { 1, 0, 0 });


        glm::vec3 translation{};
        if (glfwGetKey(window::getMainWindow(), GLFW_KEY_W) == GLFW_PRESS)
            translation.z += float(deltaTime) * 3.f;
        else if (glfwGetKey(window::getMainWindow(), GLFW_KEY_S) == GLFW_PRESS)
            translation.z -= float(deltaTime) * 3.f;

        if (glfwGetKey(window::getMainWindow(), GLFW_KEY_A) == GLFW_PRESS)
            translation.x += float(deltaTime) * 3.f;
        else if (glfwGetKey(window::getMainWindow(), GLFW_KEY_D) == GLFW_PRESS)
            translation.x -= float(deltaTime) * 3.f;

        if (glfwGetKey(window::getMainWindow(), GLFW_KEY_E) == GLFW_PRESS)
            translation.y += float(deltaTime) * 3.f;
        else if (glfwGetKey(window::getMainWindow(), GLFW_KEY_Q) == GLFW_PRESS)
            translation.y -= float(deltaTime) * 3.f;

        cam.move(translation);

        shader.bind();

        tex.bind();
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(textureLoc, 0);

        shader.setUniformMatrix("model", model);
        shader.setUniformMatrix("view", cam.getViewMatrix());
        shader.setUniformMatrix("viewProjection", cam.getViewprojectionMatrix());
        shader.setUniform("lightPos", lightPos);
        shader.setUniformMaterial(material);

        objmodel.render();

        tex.unbind();

        shader.unbind();

        Time t = clock.restart();

        timeAccum += t;
        marksAccum += t;
        marksCount++;
        if (timeAccum >= Time::seconds(1))
        {
            Time t_avg = marksAccum / marksCount;
            std::cout << t_avg.toMilliseconds() << " performance." << std::endl;
            timeAccum = Time::zero();
            marksAccum = Time::zero();
            marksCount = 0;
        }
    });

    gl::terminate();
}


ObjModel createCubeModel()
{
    return createTexturedModel({
        { -1.0f,-1.0f,-1.0f }, // triangle 1 : begin
        { -1.0f,-1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f }, // triangle 1 : end
        { 1.0f, 1.0f,-1.0f }, // triangle 2 : begin
        { -1.0f,-1.0f,-1.0f },
        { -1.0f, 1.0f,-1.0f }, // triangle 2 : end
        { 1.0f,-1.0f, 1.0f },
        { -1.0f,-1.0f,-1.0f },
        { 1.0f,-1.0f,-1.0f },
        { 1.0f, 1.0f,-1.0f },
        { 1.0f,-1.0f,-1.0f },
        { -1.0f,-1.0f,-1.0f },
        { -1.0f,-1.0f,-1.0f },
        { -1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f,-1.0f },
        { 1.0f,-1.0f, 1.0f },
        { -1.0f,-1.0f, 1.0f },
        { -1.0f,-1.0f,-1.0f },
        { -1.0f, 1.0f, 1.0f },
        { -1.0f,-1.0f, 1.0f },
        { 1.0f,-1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f,-1.0f,-1.0f },
        { 1.0f, 1.0f,-1.0f },
        { 1.0f,-1.0f,-1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f,-1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f,-1.0f },
        { -1.0f, 1.0f,-1.0f },
        { 1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f,-1.0f },
        { -1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f },
        { 1.0f,-1.0f, 1.0f }
        }, {
            { 0.000059f, 1.0f - 0.000004f },
            { 0.000103f, 1.0f - 0.336048f },
            { 0.335973f, 1.0f - 0.335903f },
            { 1.000023f, 1.0f - 0.000013f },
            { 0.667979f, 1.0f - 0.335851f },
            { 0.999958f, 1.0f - 0.336064f },
            { 0.667979f, 1.0f - 0.335851f },
            { 0.336024f, 1.0f - 0.671877f },
            { 0.667969f, 1.0f - 0.671889f },
            { 1.000023f, 1.0f - 0.000013f },
            { 0.668104f, 1.0f - 0.000013f },
            { 0.667979f, 1.0f - 0.335851f },
            { 0.000059f, 1.0f - 0.000004f },
            { 0.335973f, 1.0f - 0.335903f },
            { 0.336098f, 1.0f - 0.000071f },
            { 0.667979f, 1.0f - 0.335851f },
            { 0.335973f, 1.0f - 0.335903f },
            { 0.336024f, 1.0f - 0.671877f },
            { 1.000004f, 1.0f - 0.671847f },
            { 0.999958f, 1.0f - 0.336064f },
            { 0.667979f, 1.0f - 0.335851f },
            { 0.668104f, 1.0f - 0.000013f },
            { 0.335973f, 1.0f - 0.335903f },
            { 0.667979f, 1.0f - 0.335851f },
            { 0.335973f, 1.0f - 0.335903f },
            { 0.668104f, 1.0f - 0.000013f },
            { 0.336098f, 1.0f - 0.000071f },
            { 0.000103f, 1.0f - 0.336048f },
            { 0.000004f, 1.0f - 0.671870f },
            { 0.336024f, 1.0f - 0.671877f },
            { 0.000103f, 1.0f - 0.336048f },
            { 0.336024f, 1.0f - 0.671877f },
            { 0.335973f, 1.0f - 0.335903f },
            { 0.667969f, 1.0f - 0.671889f },
            { 1.000004f, 1.0f - 0.671847f },
            { 0.667979f, 1.0f - 0.335851f }
    });
}


int test_window()
{
    /*SDL_Window* window = nullptr;

    SDL_Surface* screenSurface = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        std::cerr << "SDL INITI ERROR: " << SDL_GetError() << std::endl;
    else
    {
        constexpr std::size_t screen_width = 640;
        constexpr std::size_t screen_height = 480;

        window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN);
        if (window == nullptr)
            std::cerr << "SDL Window error: " << SDL_GetError() << std::endl;
        else
        {
            //Get window surface
            screenSurface = SDL_GetWindowSurface(window);

            //Fill the surface white
            SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0xff, 0xff, 0xff));

            //Update the surface
            SDL_UpdateWindowSurface(window);

            //Hack to get window to stay up
            SDL_Event e;
            bool quit = false;
            while (!quit)
            {
                while (SDL_PollEvent(&e))
                {
                    if (e.type == SDL_QUIT)
                        quit = true;
                }
            }

            SDL_DestroyWindow(window);
            SDL_Quit();
        }
    }*/


    /*GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();*/
    return 0;
}
