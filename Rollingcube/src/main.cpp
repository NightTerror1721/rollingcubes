#include <iostream>
#include <vector>

#include "core/window.h"

#include "math/color.h"
#include "math/bases.h"

#include "engine/objmodel.h"
#include "engine/shader.h"
#include "engine/camera.h"
#include "engine/texture.h"
#include "engine/sampler.h"
#include "engine/entities.h"
#include "engine/text.h"

#include "utils/logger.h"
#include "utils/bmp_decoder.h"
#include "utils/image.h"
#include "utils/shader_constants.h"

#include "game/cube_model.h"

#include <JPEG/jpeglib.h>

#include <ft2build.h>

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

    CoordSystem cs0 = CoordSystem::fromYAxis(0, 1, 0);
    CoordSystem cs1 = CoordSystem::fromYAxis(1, 0, 0);
    CoordSystem cs2 = cs0.rotate(45, 45, 0);

    /*static constexpr GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f
    };*/


    //std::shared_ptr<Texture> tex = std::make_shared<Texture>();
    //if (!tex->loadFromImage("test/tile1.jpg"))
        //return;

    ShaderProgramManager::instance().loadInternalShaders();
    ShaderProgram::Ref lightningShader = ShaderProgramManager::instance().getLightningShaderProgram();

    Texture::Ref tex = TextureManager::root().loadFromImage("tuto01", "test/tile1.jpg");
    if (!tex)
        return;

    //Sampler sampler;
    //sampler.create();
    //sampler.setFilter(Sampler::MagnificationFilter::Bilinear);
    //sampler.setFilter(Sampler::MinificationFilter::BilinearMipmap);

    tex->bind();
    tex->setRepeat(true);
    tex->setFilter(Texture::MagnificationFilter::Bilinear);
    tex->setFilter(Texture::MinificationFilter::BilinearMipmap);
    tex->unbind();

    
    Font font;
    font.load("default.ttf", 256);

    Camera ortoCam;
    ortoCam.setToOrthographic(0, window::default_width, 0, window::default_height, -1.f, 1.f);

    



    //Shader shader;
    //shader.load("data/shaders/default.vert", "data/shaders/default.frag");
    //std::shared_ptr<Shader> shader = Shader::getDefault();


    //const ObjModel& objmodel = *cubes::model::getModel().get();
    //std::shared_ptr<ObjModel> objmodel = std::make_shared<ObjModel>();
    //objmodel->load("test/suzanne.obj");
    std::shared_ptr<ObjModel> objmodel = cubes::model::getModel();

    ModeledEntity entityCube1, entityCube2;
    entityCube1.setObjectModel(objmodel);
    entityCube2.setObjectModel(objmodel);

    entityCube2.move(1, 0, 0);

    Material material;
    material.setDiffuseColor({ 1, 1, 1 });
    material.setAmbientColor({ 0.1, 0.1, 0.1 });
    material.setSpecularColor({ 0.3, 0.3, 0.3 });
    material.setShininess(10);
    material.setDiffuseTexture(tex);

    entityCube1.setMaterial(material);
    entityCube2.setMaterial(material);

    TutoMove mover;

    //glm::mat4 model = glm::mat4(1.f);
    //model = glm::scale(model, { 3, 3, 3 });

    DirectionalLight dirLight;
    dirLight.setColor(glm::vec3(1, 1, 1));
    dirLight.setDirection(glm::vec3(-0.2f, -1.0f, -0.3f));
    dirLight.setIntensity(0.5);

    std::shared_ptr<StaticLightManager> lightManager = std::make_shared<StaticLightManager>();
    entityCube1.linkStaticLightManager(lightManager);
    entityCube2.linkStaticLightManager(lightManager);

    //auto slights = lightManager->createShaderLights();

    Light light;
    //light.setColor(glm::vec3(1, 1, 1));
    light.setAmbientColor({ 0, 0, 0 });
    light.setDiffuseColor({ 0., 0., 0. });
    light.setSpecularColor({ 1, 1, 1 });
    light.setIntensity(50.f);
    light.setPosition({ 5, 2, 0 });
    //light.setQuadraticAttenuation(0.25);
    auto lightId = lightManager->createNewLight(light);

    Light light2;
    light2.setColor({ 1, 0, 0 });
    light2.setIntensity(30.f);
    light2.setPosition({ -5, 2, 0 });
    auto lightId2 = lightManager->createNewLight(light2);

    Camera cam;
    cam.setToPerspective(glm::radians(45.f), float(window::default_width) / float(window::default_height), 0.1f, 100.f);
    cam.lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));


    double lastTime = 0;

    resetMousePosition();

    Time timeAccum;
    Time marksAccum;
    Time maxTime;
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

        cam.rotate(5.0f * float(deltaTime) * mpos.x, { 0, 1, 0 }, true);
        cam.rotate(5.0f * float(deltaTime) * mpos.y, { 1, 0, 0 }, true);


        if (glfwGetKey(window::getMainWindow(), GLFW_KEY_Z) == GLFW_PRESS)
            cam.rotate(60.0f * float(deltaTime), { 0, 0, 1 }, true);
        else if (glfwGetKey(window::getMainWindow(), GLFW_KEY_C) == GLFW_PRESS)
            cam.rotate(-60.0f * float(deltaTime), { 0, 0, 1 }, true);

        if (glfwGetKey(window::getMainWindow(), GLFW_KEY_R) == GLFW_PRESS)
            cam.setOrientation({0, 0, 0});

        



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


        if (glfwGetKey(window::getMainWindow(), GLFW_KEY_G) == GLFW_PRESS)
            cam.lookAt(cam.getEye(), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));


        light.setPosition(cam.getEye() + glm::vec3(0, 0, 0));
        lightManager->updateLight(lightId, light);
        //slights->build();

        dirLight.setDirection(glm::normalize(cam.getFront()));

        entityCube1.update(elapsedTime);
        entityCube2.update(elapsedTime);

        //cam.bindToDefaultShader();
        //shader->bind();
        //material.bindTextures();
        //shader->unbind();

        //entity.render();

        //tex->bind();
        //glActiveTexture(GL_TEXTURE0);
        //glUniform1i(textureLoc, 0);

        //sampler.activate(0);
        //sampler.activate(1);

        cam.bindToShader(lightningShader);

        lightningShader->use();
        lightningShader->setUniformDirectionalLight(dirLight);

        //entity.getMaterial().bindTextures();

        //Shader::getDefault()->setUniformMatrix("model", entity.getModelMatrix());
        //Shader::getDefault()->setUniformMatrix("viewProjection", cam.getViewprojectionMatrix());
        //Shader::getDefault()->setUniformMatrix("modelNormal", glm::utils::normalMatrix(entity.getModelMatrix()));
        //Shader::getDefault()->setUniform("viewPos", cam.getPosition());
        //Shader::getDefault()->setUniformStaticLights(entity.getStaticLightContainer());
        //Shader::getDefault()->setUniformDirectionalLight(dirLight);
        //Shader::getDefault()->setUniformMaterial(material);

        entityCube1.render();
        entityCube2.render();

        const auto& pos = cam.getPosition();
        const auto& rot = cam.getEulerAngles();
        const auto& camFront = cam.getFront();
        const auto& camUp = cam.getUp();
        font.setColor({ 1, 1, 1 });
        font.print(ortoCam, 5, 5, 16, "Cam pos: (x:{:.2f}, y:{:.2f}, z:{:.2f})", pos.x, pos.y, pos.z);
        font.print(ortoCam, 5, 5 + 16 + 5, 16, "Cam rot: (pitch:{:.2f}, yaw:{:.2f}, roll:{:.2f})", rot.x, rot.y, rot.z);
        font.print(ortoCam, 5, 5 + 32 + 10, 16, "Cam FRONT: (x:{:.2f}, y:{:.2f}, z:{:.2f})", camFront.x, camFront.y, camFront.z);
        font.print(ortoCam, 5, 5 + 48 + 15, 16, "Cam UP: (x:{:.2f}, y:{:.2f}, z:{:.2f})", camUp.x, camUp.y, camUp.z);

        //font.setColor({ 1, 0, 0 });
        //font.print(ortoCam, 5, window::default_height - 80, 64, "Test");

        //entity.getObjectModel()->render();

        //objmodel->render();

        //material.unbindTextures();
        //shader->unbind();

        Time t = clock.restart();

        timeAccum += t;
        marksAccum += t;
        maxTime = Time::max(maxTime, t);
        marksCount++;
        if (timeAccum >= Time::seconds(1))
        {
            Time t_avg = marksAccum / marksCount;
            std::cout << maxTime.toMilliseconds() << "ms poor performance; ";
            std::cout << t_avg.toMilliseconds() << "ms avg performance." << std::endl;
            timeAccum = Time::zero();
            marksAccum = Time::zero();
            maxTime = Time::zero();
            marksCount = 0;
        }
    }, [&](const TimeController& tc) {
        font.setColor({ 0, 1, 0 });
        font.print(ortoCam, 5, window::default_height - 16, 16, "{} fps", tc.getFPS());
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
