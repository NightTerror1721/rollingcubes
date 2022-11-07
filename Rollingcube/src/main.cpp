#include <iostream>
#include <vector>

#include "core/window.h"

#include "math/color.h"
#include "math/bases.h"

#include "engine/model.h"
#include "engine/shader.h"
#include "engine/camera.h"
#include "engine/texture.h"
#include "engine/sampler.h"
#include "engine/entities.h"
#include "engine/text.h"
#include "engine/skybox.h"

#include "engine/lua/lua.h"

#include "utils/logger.h"
#include "utils/bmp_decoder.h"
#include "utils/image.h"
#include "utils/shader_constants.h"
#include "utils/unicode.h"

#include "game/cube_model.h"
#include "game/block.h"
#include "game/theme.h"


int test_window();
void tutos();
void lua_test();

Model createCubeModel();

int main(int argc, char** argv)
{
    lua::initCustomLibs();

    //lua_test();
    tutos();


    //test_window();
    
	return 0;
}


void lua_test()
{
    LuaScript script = LuaScriptManager::instance().getScript("test/test_lua.lua");
    script();
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


Model createSimpleModel(const std::vector<glm::vec3>& vertices)
{
    Model model;
    auto& mesh = model.createMesh("default").get();
    mesh.setVertices(vertices);

    return model;
}

Model createTexturedModel(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs)
{
    Model model;
    auto& mesh = model.createMesh("default").get();
    mesh.setVertices(vertices);
    mesh.setUVs(uvs);

    return model;
}



static void renderIfNotTransparency(EntityCameraDistanceCollection<ModelEntity>& transps, std::shared_ptr<ModelEntity>& entity, const Camera& cam)
{
    if (entity->hasTransparency())
        entity->render(cam);
    else
        transps.addEntity(entity, cam);
}

void tutos()
{
    window::createMainWindow();

    Theme::changeCurrentTheme("test_theme");

    auto blockModel = Theme::getCurrentTheme().getBlockModel("test");
    auto block1 = Block();
    block1.setBoundingType(BoundingVolumeType::AABB);
    block1.setBlockModel(blockModel);
    block1.init();

    block1.setPosition(0, 1, -3);

    UnicodeString ustr = "ola k ase tú!";

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

    tex->bind();
    tex->setRepeat(true);
    tex->setFilter(Texture::MagnificationFilter::Bilinear);
    tex->setFilter(Texture::MinificationFilter::BilinearMipmap);
    tex->unbind();

    Texture::Ref tex_sm = TextureManager::root().loadFromImage("tuto01_sm", "test/tile1_sm.jpg");

    tex_sm->bind();
    tex_sm->setRepeat(true);
    tex_sm->setFilter(Texture::MagnificationFilter::Bilinear);
    tex_sm->setFilter(Texture::MinificationFilter::BilinearMipmap);
    tex_sm->unbind();

    Texture::Ref tex_nm = TextureManager::root().loadFromImage("tuto01_nm", "test/tile1_nm.jpg");

    tex_sm->bind();
    tex_sm->setRepeat(true);
    tex_sm->setFilter(Texture::MagnificationFilter::Bilinear);
    tex_sm->setFilter(Texture::MinificationFilter::BilinearMipmap);
    tex_sm->unbind();


    CubeMapTexture::Ref cmTex = CubeMapTextureManager::root().loadFromImage("sky01", {
        "test/skybox/front.jpg",
        "test/skybox/back.jpg",

        "test/skybox/left.jpg",
        "test/skybox/right.jpg",

        "test/skybox/up.jpg",
        "test/skybox/down.jpg"
    });

    Skybox skybox;
    skybox.setTexture(cmTex);

    skybox.setPosition({ 5, 0, 0 });


    
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
    Model::Ref objmodel = cubes::model::getModel();

    std::shared_ptr<ModelEntity> entityCube1 = std::make_shared<ModelEntity>(),
        entityCube2 = std::make_shared<ModelEntity>(),
        entityCube3 = std::make_shared<ModelEntity>();
    entityCube1->setModel(objmodel);
    entityCube2->setModel(objmodel);
    entityCube3->setModel(objmodel);

    entityCube1->setBoundingType(BoundingVolumeType::AABB);
    entityCube2->setBoundingType(BoundingVolumeType::AABB);
    entityCube3->setBoundingType(BoundingVolumeType::AABB);

    entityCube2->move(1, 0, 0);
    entityCube3->move(-1, -2, 0);

    const glm::vec3 cube3InitialPos = entityCube3->getPosition();

    Material material;
    material.setDiffuseColor({ 1, 1, 1 });
    material.setAmbientColor({ 0.1, 0.1, 0.1 });
    material.setSpecularColor({ 0.3, 0.3, 0.3 });
    material.setShininess(50);
    material.setDiffuseTexture(tex);
    material.setSpecularTexture(tex_sm);
    material.setNormalsTexture(tex_nm);
    material.setOpacity(0.5);

    entityCube1->setMaterial(material);

    material.setOpacity(0.75);
    entityCube2->setMaterial(material);

    material.setOpacity(1);
    entityCube3->setMaterial(material);


    TutoMove mover;

    //glm::mat4 model = glm::mat4(1.f);
    //model = glm::scale(model, { 3, 3, 3 });

    DirectionalLight dirLight;
    dirLight.setColor(glm::vec3(1, 1, 1));
    dirLight.setDirection(glm::vec3(-0.2f, -1.0f, -0.3f));
    dirLight.setIntensity(0.5);

    std::shared_ptr<StaticLightManager> lightManager = std::make_shared<StaticLightManager>();
    entityCube1->linkStaticLightManager(lightManager);
    entityCube2->linkStaticLightManager(lightManager);
    entityCube3->linkStaticLightManager(lightManager);

    //auto slights = lightManager->createShaderLights();

    Light mainLight;
    //light.setColor(glm::vec3(1, 1, 1));
    mainLight.setAmbientColor({ 0, 0, 0 });
    mainLight.setDiffuseColor({ 0., 0., 0. });
    mainLight.setSpecularColor({ 1, 1, 1 });
    mainLight.setIntensity(50.f);
    mainLight.setPosition({ 5, 2, 0 });
    //light.setQuadraticAttenuation(0.25);
    //auto lightId = lightManager->createNewLight(light);

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

    EntityCameraDistanceCollection<ModelEntity> transparentEntities;
    transparentEntities.setRenderBoundings(true);

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


        mainLight.setPosition(cam.getEye() + glm::vec3(0, 0, 0));
        //lightManager->updateLight(lightId, light);
        //slights->build();

        dirLight.setDirection(glm::normalize(cam.getFront()));

        static float sangle = 0;
        sangle += float(deltaTime) * 90;
        float fsin = glm::sin(glm::radians(sangle));

        entityCube3->rotate({ 0, float(deltaTime) * 20, 0});

        entityCube3->setPosition({ cube3InitialPos.x, cube3InitialPos.y + (0.5 * fsin), cube3InitialPos.z});

        entityCube1->update(elapsedTime);
        entityCube2->update(elapsedTime);
        entityCube3->update(elapsedTime);
        block1.update(elapsedTime);

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

        //cam.bindToShader(lightningShader);

        lightningShader->use();
        lightningShader->setUniformDirectionalLight(dirLight);
        lightningShader->setUniformMainStaticLight(mainLight);

        //entity.getMaterial().bindTextures();

        //Shader::getDefault()->setUniformMatrix("model", entity.getModelMatrix());
        //Shader::getDefault()->setUniformMatrix("viewProjection", cam.getViewprojectionMatrix());
        //Shader::getDefault()->setUniformMatrix("modelNormal", glm::utils::normalMatrix(entity.getModelMatrix()));
        //Shader::getDefault()->setUniform("viewPos", cam.getPosition());
        //Shader::getDefault()->setUniformStaticLights(entity.getStaticLightContainer());
        //Shader::getDefault()->setUniformDirectionalLight(dirLight);
        //Shader::getDefault()->setUniformMaterial(material);

        
        transparentEntities.clear();
        //entityCube1->render(cam);
        //entityCube2->render(cam);

        using WP = EntityCameraDistanceWrapper<ModelEntity>;

        WP wCube1;
        wCube1.setEntity(entityCube1);
        wCube1.setDistance(-cam.getDistanceTo(entityCube1->getPosition()));
        transparentEntities.addWrappedEntity(wCube1);

        WP wCube2;
        wCube2.setEntity(entityCube2);
        wCube2.setDistance(-cam.getDistanceTo(entityCube2->getPosition()));
        transparentEntities.addWrappedEntity(wCube2);

        WP wCube3;
        wCube3.setEntity(entityCube3);
        wCube3.setDistance(-cam.getDistanceTo(entityCube3->getPosition()));
        transparentEntities.addWrappedEntity(wCube3);

        block1.render(cam);

        skybox.render(cam);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        transparentEntities.render(cam);

        glDisable(GL_BLEND);

        const auto& pos = cam.getPosition();
        const auto& rot = cam.getEulerAngles();
        const auto& camFront = cam.getFront();
        const auto& camUp = cam.getUp();
        font.setColor({ 1, 1, 1 });
        font.print(ortoCam, 5, 5, 16, "Cam pos: (x:{:.2f}, y:{:.2f}, z:{:.2f})", pos.x, pos.y, pos.z);
        font.print(ortoCam, 5, 5 + 16 + 5, 16, "Cam rot: (pitch:{:.2f}, yaw:{:.2f}, roll:{:.2f})", rot.x, rot.y, rot.z);
        font.print(ortoCam, 5, 5 + 32 + 10, 16, "Cam FRONT: (x:{:.2f}, y:{:.2f}, z:{:.2f})", camFront.x, camFront.y, camFront.z);
        font.print(ortoCam, 5, 5 + 48 + 15, 16, "Cam UP: (x:{:.2f}, y:{:.2f}, z:{:.2f})", camUp.x, camUp.y, camUp.z);

        font.print(ortoCam, 5, 5 + 64 + 20, 16, "Cube3 Is Visible: {}", entityCube3->isVisibleInCamera(cam));
        font.print(ortoCam, 5, 5 + 80 + 25, 16, "Cube2 Is Visible: {}", entityCube2->isVisibleInCamera(cam));
        font.print(ortoCam, 5, 5 + 96 + 30, 16, "Cube1 Is Visible: {}", entityCube1->isVisibleInCamera(cam));



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


Model createCubeModel()
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
