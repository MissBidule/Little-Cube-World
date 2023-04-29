#include <cstdarg>
#include <glimac/SimpleObjectManager.hpp>
#include <glimac/SkinnedObjectManager.hpp>
#include "App.hpp"
#include "glm/ext/matrix_transform.hpp"

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

    // sun
    m_sun = new SimpleObjectManager("shaders/3D.vs.glsl", "shaders/dirPoslight.fs.glsl");
    m_ObjList.emplace_back(m_sun);

    // ground
    m_ground = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_ground);

    // river
    m_river = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_river);

    // waterlily
    for (int i = 0; i < WLILY_NB; i++)
    {
        auto* wlily = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
        m_wlily.emplace_back(wlily);
        m_ObjList.emplace_back(wlily);
    }

    // Floor HELPER
    m_floor = new TexObjectManager("shaders/3D.vs.glsl", "shaders/dirPoslightTex.fs.glsl");
    m_ObjList.emplace_back(m_floor);

    // Limit
    m_limit = new TexObjectManager("shaders/3D.vs.glsl", "shaders/limit.fs.glsl");
    m_ObjList.emplace_back(m_limit);

    // THEN WE ADD THE MAIN CONFIGURATION (COLOR/TEXTURE/MODEL) AND MOVEMENT IF THE OBJECT IS STATIC//

    //--------------------------------SUN---------------------

    glimac::Color sunColor{
        glm::vec3(1, 1, 0.45),
        glm::vec3(1, 1, 0.45),
        glm::vec3(1, 1, 0.45),
        1,
        1};

    m_sun->addManualMesh(glimac::quad_vertices(1.f), sunColor);
    m_sun->ignoreShadowRender = true;

    //--------------------------------GROUND---------------------

    m_ground->addSkinnedMesh("assets/models/floor.fbx");

    // MM OF GROUND
    glm::mat4 ground_MMatrix = glm::mat4(1);
    // BCS IT'S AN FBX OBJECT
    ground_MMatrix = glm::rotate(ground_MMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    //
    m_ground->m_MMatrix = ground_MMatrix;

    //--------------------------------RIVER---------------------

    m_river->addSkinnedMesh("assets/models/river.fbx");

    // MM OF RIVER
    glm::mat4 river_MMatrix = glm::mat4(1);
    // BCS IT'S AN FBX OBJECT
    river_MMatrix = glm::rotate(river_MMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    //
    m_river->m_MMatrix = river_MMatrix;

    //--------------------------------WATERLILY---------------------

    for (int i = 0; i < WLILY_NB; i++)
    {
        m_wlily[i]->addSkinnedMesh("assets/models/waterlily.fbx");
        m_wlily[i]->addSkinnedMesh("assets/models/waterlily2.fbx");
    }

    glm::mat4 wlilyMMatrix = glm::translate(glm::mat4(1), glm::vec3(1, 0, -13));
    wlilyMMatrix           = glm::rotate(wlilyMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    wlilyMMatrix           = glm::rotate(wlilyMMatrix, glm::radians(0.f), glm::vec3(0, 0, 1));
    m_wlily[0]->m_MMatrix  = wlilyMMatrix;

    wlilyMMatrix          = glm::translate(glm::mat4(1), glm::vec3(6, 0, -12));
    wlilyMMatrix          = glm::rotate(wlilyMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    wlilyMMatrix          = glm::rotate(wlilyMMatrix, glm::radians(180.f), glm::vec3(0, 0, 1));
    m_wlily[1]->m_MMatrix = wlilyMMatrix;

    wlilyMMatrix          = glm::translate(glm::mat4(1), glm::vec3(16, 0, -10));
    wlilyMMatrix          = glm::rotate(wlilyMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    wlilyMMatrix          = glm::rotate(wlilyMMatrix, glm::radians(0.f), glm::vec3(0, 0, 1));
    m_wlily[2]->m_MMatrix = wlilyMMatrix;

    wlilyMMatrix          = glm::translate(glm::mat4(1), glm::vec3(20, 0, -10));
    wlilyMMatrix          = glm::rotate(wlilyMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    wlilyMMatrix          = glm::rotate(wlilyMMatrix, glm::radians(180.f), glm::vec3(0, 0, 1));
    m_wlily[3]->m_MMatrix = wlilyMMatrix;

    wlilyMMatrix          = glm::translate(glm::mat4(1), glm::vec3(24, 0, -8));
    wlilyMMatrix          = glm::rotate(wlilyMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    wlilyMMatrix          = glm::rotate(wlilyMMatrix, glm::radians(0.f), glm::vec3(0, 0, 1));
    m_wlily[4]->m_MMatrix = wlilyMMatrix;

    //--------------------------------FLOOR---------------------

    img::Image floorImg = p6::load_image_buffer("assets/textures/test.jpg");

    glimac::Texture floorTex{
        glimac::textureToUVtex(floorImg),
        glimac::textureToUVtex(floorImg),
        glimac::textureToUVtex(floorImg),
        0.6,
        0.1};

    m_floor->addManualTexMesh(glimac::quad_vertices(1.f), floorTex);

    glm::mat4 floorMMatrix      = glm::translate(glm::mat4(1), glm::vec3(12.5f, -0.4f, -12.5f));
    floorMMatrix                = glm::scale(floorMMatrix, glm::vec3(25.f, 1.f, 25.f));
    m_floor->m_MMatrix          = floorMMatrix;
    m_floor->ignoreShadowRender = true;

    //--------------------------------LIMIT---------------------

    img::Image limitImg = p6::load_image_buffer("assets/textures/test.png");

    glimac::Texture limitTex{
        glimac::textureToUVtex(limitImg),
        glimac::textureToUVtex(limitImg),
        glimac::textureToUVtex(limitImg),
        0.6,
        1};

    m_limit->addManualTexMesh(glimac::quad_vertices(1.f), limitTex);

    glm::mat4 limitMMatrix = glm::translate(glm::mat4(1), glm::vec3(12.5f, 10.f, -12.5f));
    limitMMatrix           = glm::scale(limitMMatrix, glm::vec3(31.f, 29.f, 31.f));
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
    m_LightList[0].setPosition(glm::vec3(8, 50, 0));
    m_LightList[0].m_color = glm::vec3(1.f, 1.f, .45f);
}

void App::initViewProjection()
{
    // MAIN CAMERA
    m_ViewMatrixCamera = FreeflyCamera();
    m_ViewMatrixCamera.setPos(glm::vec3(13, 2, -5));

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
