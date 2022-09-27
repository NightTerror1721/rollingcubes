#include <iostream>
#include <vector>

#include "core/window.h"

#include "math/Vector2.h"
#include "math/Vector3.h"

#include "math/Matrix44.h"

#include "math/color.h"

#include "engine/objmodel.h"
#include "engine/shader.h"
#include "engine/camera.h"

#include "utils/logger.h"

int test_window();
void tutos();

ObjModel createCubeModel();

int main(int argc, char** argv)
{
    vector2f p = { 15, 3.14 };
    vector2i p2 = vector2i(p);

    auto p3 = vector2f(10, 4);
    auto p4 = p + p3;
    p4 *= 2;

    operator+(p, p3);

    vector3f vec = { 2, 5, 3 };
    vec = vec + vector3f(p);

    std::cout << *(&p4) << std::endl;


    std::vector<float> fvec = { 1, 2, 3, 45 };
    matrix44f mat = fvec;

    Color c = Color::white();
    vector4f cvec = static_cast<vector4f>(c);


    tutos();


    //test_window();
    
	return 0;
}


ObjModel createSimpleModel(const std::vector<glm::vec3>& vertices)
{
    ObjModel model;
    auto& mesh = model.createMesh("default").get();
    mesh.setVertices(vertices);

    return model;
}

ObjModel createColoredModel(const std::vector<glm::vec3>& vertices, const std::vector<Color>& colors)
{
    ObjModel model;
    auto& mesh = model.createMesh("default").get();
    mesh.setVertices(vertices);
    mesh.setColors(colors);

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



    Shader shader;
    shader.load("test/SimpleVertexShader.vs", "test/SimpleFragmentShader.fs");

    shader.activate();

    auto objmodel = createCubeModel();

    /*GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);*/

    glm::mat4 model = glm::mat4(1.f);

    Camera cam;
    cam.setToPerspective(glm::radians(45.f), float(window::default_width) / float(window::default_height), 0.1f, 100.f);
    cam.lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    auto mvp = cam.mvp(model);


    GLuint mvpId = glGetUniformLocation(shader.programId(), "mvp");
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);


    window::simpleLoop(false, [&objmodel, &shader]() {
        objmodel.render();
        //glBindVertexArray(VertexArrayID);
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        //glDisableVertexAttribArray(0);
    });
    shader.deactivate();

    //glDeleteBuffers(1, &vertexbuffer);
    //glDeleteVertexArrays(1, &VertexArrayID);

    gl::terminate();
}


ObjModel createCubeModel()
{
    return createColoredModel({
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
            { 0.583f,  0.771f,  0.014f },
            { 0.609f,  0.115f,  0.436f },
            { 0.327f,  0.483f,  0.844f },
            { 0.822f,  0.569f,  0.201f },
            { 0.435f,  0.602f,  0.223f },
            { 0.310f,  0.747f,  0.185f },
            { 0.597f,  0.770f,  0.761f },
            { 0.559f,  0.436f,  0.730f },
            { 0.359f,  0.583f,  0.152f },
            { 0.483f,  0.596f,  0.789f },
            { 0.559f,  0.861f,  0.639f },
            { 0.195f,  0.548f,  0.859f },
            { 0.014f,  0.184f,  0.576f },
            { 0.771f,  0.328f,  0.970f },
            { 0.406f,  0.615f,  0.116f },
            { 0.676f,  0.977f,  0.133f },
            { 0.971f,  0.572f,  0.833f },
            { 0.140f,  0.616f,  0.489f },
            { 0.997f,  0.513f,  0.064f },
            { 0.945f,  0.719f,  0.592f },
            { 0.543f,  0.021f,  0.978f },
            { 0.279f,  0.317f,  0.505f },
            { 0.167f,  0.620f,  0.077f },
            { 0.347f,  0.857f,  0.137f },
            { 0.055f,  0.953f,  0.042f },
            { 0.714f,  0.505f,  0.345f },
            { 0.783f,  0.290f,  0.734f },
            { 0.722f,  0.645f,  0.174f },
            { 0.302f,  0.455f,  0.848f },
            { 0.225f,  0.587f,  0.040f },
            { 0.517f,  0.713f,  0.338f },
            { 0.053f,  0.959f,  0.120f },
            { 0.393f,  0.621f,  0.362f },
            { 0.673f,  0.211f,  0.457f },
            { 0.820f,  0.883f,  0.371f },
            { 0.982f,  0.099f,  0.879f }
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
