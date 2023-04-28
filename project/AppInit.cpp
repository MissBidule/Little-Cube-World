#include <cstdarg>
#include <glimac/SkinnedObjectManager.hpp>
#include "App.hpp"

App::App(int window_width, int window_height)
    : m_ctx(p6::Context{{window_width, window_height, "TP11"}})
{
    m_ctx.maximize_window();
}

void App::init()
{
    initAllObject();
    initViewProjection();
    initShadow();
}

void App::initAllObject()
{
    // HERE WE CREATE THE TYPE OF EACH OBJECT - SIMPLE / TEXTURE / ANIMATED AND WE ADD THEM TO THE GLOBAL LIST//

    // Thingy
    m_Thingy = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_Thingy);

    // Earth
    m_earth = new SimpleObjectManager("shaders/3D.vs.glsl", "shaders/dirPoslight.fs.glsl");
    m_ObjList.emplace_back(m_earth);

    // Moon
    for (int i = 0; i < MOON_NB; i++)
    {
        auto* moon = new SimpleObjectManager("shaders/3D.vs.glsl", "shaders/dirPoslight.fs.glsl");
        m_moons.emplace_back(moon);
        m_ObjList.emplace_back(moon);
    }

    // Floor
    m_floor = new TexObjectManager("shaders/3D.vs.glsl", "shaders/dirPoslightTex.fs.glsl");
    m_ObjList.emplace_back(m_floor);

    // Limit
    m_limit = new TexObjectManager("shaders/3D.vs.glsl", "shaders/limit.fs.glsl");
    m_ObjList.emplace_back(m_limit);

    // THEN WE ADD THE MAIN CONFIGURATION (COLOR/TEXTURE/MODEL) AND MOVEMENT IF THE OBJECT IS STATIC//

    //--------------------------------THINGY---------------------
    m_Thingy->addSkinnedMesh("assets/models/test.fbx");

    //--------------------------------EARTH---------------------

    glimac::Color earthColor{
        glm::vec3(0.0215, 0.1745, 0.0215),
        glm::vec3(0.07568, 0.61424, 0.07568),
        glm::vec3(0.633, 0.727811, 0.633),
        0.6,
        1};

    m_earth->addManualMesh(glimac::sphere_vertices(1.f, 32, 16), earthColor);

    //--------------------------------MOON---------------------

    for (int i = 0; i < MOON_NB; i++)
    {
        glimac::Color moonColor{
            glm::vec3(glm::linearRand(0.f, 0.05f), glm::linearRand(0.f, 0.05f), glm::linearRand(0.f, 0.05f)),
            glm::vec3(glm::linearRand(0.f, 0.5f), glm::linearRand(0.f, 0.5f), glm::linearRand(0.f, 0.5f)),
            glm::vec3(glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f)),
            glm::linearRand(0.f, 1.f),
            1};

        m_moons[i]->addManualMesh(glimac::sphere_vertices(1.f, 32, 16), moonColor);

        m_MoonRotAxes.emplace_back(glm::ballRand(2.f));

        auto rotation                   = glm::vec3(0);
        rotation[glm::linearRand(0, 2)] = 1;
        rotation[glm::linearRand(0, 2)] = 1;
        rotation[glm::linearRand(0, 2)] = 1;
        m_MoonRotDir.emplace_back(rotation);
    }

    //--------------------------------FLOOR---------------------

    img::Image floorImg = p6::load_image_buffer("assets/textures/test.png");

    glimac::Texture floorTex{
        glimac::textureToUVtex(floorImg),
        glimac::textureToUVtex(floorImg),
        0.6,
        1};

    // to be changed with the quad afterwards
    m_floor->addManualTexMesh(glimac::sphere_vertices(1.f, 32, 16), floorTex);

    glm::mat4 floorMMatrix = glm::translate(glm::mat4(1), glm::vec3(0, -15.f, 0));
    floorMMatrix           = glm::scale(floorMMatrix, glm::vec3(16.f, 8.f, 16.f));
    m_floor->m_MMatrix     = floorMMatrix;

    //--------------------------------LIMIT---------------------

    // to be changed with the quad afterwards
    m_limit->addManualTexMesh(glimac::sphere_vertices(1.f, 32, 16), floorTex);

    glm::mat4 limitMMatrix = glm::scale(glm::mat4(1), glm::vec3(15.f, 15.f, 15.f));
    m_limit->m_MMatrix     = limitMMatrix;
    // We ignore the shadow because we navigate IN the limits
    m_limit->ignoreShadowRender = true;

    // WE INIT ALL VAO AND VBO - NOTHING TO ADD HERE//

    // Do not forget particles//
    for (auto& i : m_Particles.m_meshes)
    {
        i->initVaoVbo();
    }

    for (auto& i : m_ObjList)
    {
        i->initVaoVbo();
    }

    //--------------------------------LIGHT---------------------

    // CREATE LIGHTS up to 7
    m_LightList.emplace_back(glimac::LightType::Directional);
    m_LightList[0].setPosition(glm::vec3(2, 2, 0));
    m_LightList[0].m_color = glm::vec3(.8f, .8f, .8f);
    m_LightList.emplace_back(glimac::LightType::Point);
    m_LightList[1].setPosition(glm::vec3(1.5f, -1.5f, 0));
    m_LightList[1].m_color = glm::vec3(.8f, .8f, .8f);
}

void App::initViewProjection()
{
    // MAIN CAMERA
    m_ViewMatrixCamera = FreeflyCamera();
    m_ViewMatrixCamera.moveFront(-5);

    // PROJECTION
    m_ProjMatrix = glm::perspective(glm::radians(70.f), m_ctx.aspect_ratio(), 0.1f, 100.f);
}

void App::initShadow()
{
    // FOR EACH LIGHT WE HAVE A SHADOW MAP - NOTHING TO BE DONE HERE//
    for (auto& i : m_LightList)
    {
        m_ShadowProgList.emplace_back().SetLight(i);
        i.initShadowMap(shadow_size);
    }

    glEnable(GL_POLYGON_OFFSET_FILL);
    float factor = 1.f;
    float units  = 1.f;
    glPolygonOffset(factor, units);
}
