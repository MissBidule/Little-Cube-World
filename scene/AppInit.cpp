#include <cstdarg>
#include <glimac/SimpleObjectManager.hpp>
#include <glimac/SkinnedObjectManager.hpp>
#include <vector>
#include "App.hpp"
#include "glm/ext/matrix_transform.hpp"

App::App(int window_width, int window_height)
    : m_ctx(p6::Context{{window_width, window_height, "OpenGL little scene"}})
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

    // rocks
    for (int i = 0; i < ROCK_NB; i++)
    {
        auto* rock = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
        m_rocks.emplace_back(rock);
        m_ObjList.emplace_back(rock);
    }

    // little rocks
    for (int i = 0; i < LILROCK_NB; i++)
    {
        auto* lilrock = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
        m_lilrocks.emplace_back(lilrock);
        m_ObjList.emplace_back(lilrock);
    }

    // long trees
    for (int i = 0; i < LONGTREE_NB; i++)
    {
        auto* longtree = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
        m_longtrees.emplace_back(longtree);
        m_ObjList.emplace_back(longtree);
    }

    // big trees
    for (int i = 0; i < BIGTREE_NB + (TREESIDE / 3) * 4; i++)
    {
        auto* bigtree = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
        m_bigtrees.emplace_back(bigtree);
        m_ObjList.emplace_back(bigtree);
    }

    // bushes
    for (int i = 0; i < BUSH_NB; i++)
    {
        auto* bush = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
        m_bushes.emplace_back(bush);
        m_ObjList.emplace_back(bush);
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

    std::vector<glm::vec3> wlilyT = {
        glm::vec3(1, 0, -13),
        glm::vec3(6, 0, -12),
        glm::vec3(16, 0, -10),
        glm::vec3(20, 0, -10),
        glm::vec3(24, 0, -8)};

    std::vector<float> wlilyR = {
        0.f,
        180.f,
        0.f,
        180.f,
        0.f};

    for (int i = 0; i < WLILY_NB; i++)
    {
        m_wlily[i]->addSkinnedMesh("assets/models/waterlily.fbx");
        m_wlily[i]->addSkinnedMesh("assets/models/waterlily2.fbx");

        glm::mat4 wlilyMMatrix = glm::translate(glm::mat4(1), wlilyT[i]);
        wlilyMMatrix           = glm::rotate(wlilyMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        wlilyMMatrix           = glm::rotate(wlilyMMatrix, glm::radians(wlilyR[i]), glm::vec3(0, 0, 1));
        m_wlily[i]->m_MMatrix  = wlilyMMatrix;
    }

    //--------------------------------ROCKS---------------------

    std::vector<glm::vec3> rockT = {
        glm::vec3(19, 0, 0),
        glm::vec3(21, 0, 0),
        glm::vec3(18, 0, -1),
        glm::vec3(19, 0, -1),
        glm::vec3(22, 0, -4),
        glm::vec3(23, 0, -4)};

    std::vector<glm::vec3> rockS = {
        glm::vec3(1, 1, 1),
        glm::vec3(4, 4, 4),
        glm::vec3(1, 1, 1),
        glm::vec3(2, 2, 2),
        glm::vec3(1, 1, 1),
        glm::vec3(2, 2, 2)};

    for (int i = 0; i < ROCK_NB; i++)
    {
        m_rocks[i]->addSkinnedMesh("assets/models/rock.fbx");
        m_rocks[i]->addSkinnedMesh("assets/models/rock2.fbx");

        glm::mat4 rockMMatrix = glm::translate(glm::mat4(1), rockT[i]);
        rockMMatrix           = glm::rotate(rockMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        rockMMatrix           = glm::scale(rockMMatrix, rockS[i]);
        m_rocks[i]->m_MMatrix = rockMMatrix;
    }

    //-------------------------LITTLE ROCKS---------------------

    std::vector<glm::vec3> lilrockT = {
        glm::vec3(2, 0, -10),
        glm::vec3(2, 0, -17),
        glm::vec3(8, 0, -7),
        glm::vec3(9, 0, -14),
        glm::vec3(10, 0, -4),
        glm::vec3(11, 0, -1),
        glm::vec3(13, 0, -5),
        glm::vec3(13, 0, -16),
        glm::vec3(15, 0, -3),
        glm::vec3(16, 0, 0),
        glm::vec3(17, 0, -13),
        glm::vec3(18, 0, -7),
        glm::vec3(21, 0, -5),
        glm::vec3(21, 0, -16),
        glm::vec3(24, 0, -11)};

    for (int i = 0; i < LILROCK_NB; i++)
    {
        m_lilrocks[i]->addSkinnedMesh("assets/models/littlerock.fbx");

        glm::mat4 lilrockMMatrix = glm::translate(glm::mat4(1), lilrockT[i]);
        lilrockMMatrix           = glm::rotate(lilrockMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        m_lilrocks[i]->m_MMatrix = lilrockMMatrix;
    }

    //-------------------------LONG TREES---------------------

    std::vector<glm::vec3> longtreeT = {
        glm::vec3(1, 0, -19),
        glm::vec3(1, 0, -23),
        glm::vec3(3, 0, 0),
        glm::vec3(3, 0, -7),
        glm::vec3(7, 0, -3)};

    for (int i = 0; i < LONGTREE_NB; i++)
    {
        m_longtrees[i]->addSkinnedMesh("assets/models/longtree.fbx");
        m_longtrees[i]->addSkinnedMesh("assets/models/longtree2.fbx");

        glm::mat4 longtreeMMatrix = glm::translate(glm::mat4(1), longtreeT[i]);
        longtreeMMatrix           = glm::rotate(longtreeMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        m_longtrees[i]->m_MMatrix = longtreeMMatrix;
    }

    //-------------------------BIG TREES---------------------

    // in scene
    std::vector<glm::vec3> bigtreeT = {
        glm::vec3(1, 0, -1),
        glm::vec3(1, 0, -7),
        glm::vec3(2, 0, -4),
        glm::vec3(5, 0, -5),
        glm::vec3(5, 0, -2),
        glm::vec3(8, 0, -1)};

    // around the scene

    const int TREEWIDTH = 3;

    for (int i = 0; i <= (TREESIDE); i += TREEWIDTH)
    {
        for (int j = TREEWIDTH; j <= (TREESIDE - TREEWIDTH) && (i == 0 || i == TREESIDE); j += TREEWIDTH)
        {
            bigtreeT.emplace_back(TREEBEGIN + i, 0, -(TREEBEGIN + j));
        }
        bigtreeT.emplace_back(TREEBEGIN + i, 0, -TREEBEGIN);
        bigtreeT.emplace_back(TREEBEGIN + i, 0, -(TREEBEGIN + TREESIDE));
    }

    for (int i = 0; i < BIGTREE_NB + (TREESIDE / 3) * 4; i++)
    {
        m_bigtrees[i]->addSkinnedMesh("assets/models/tree.fbx");

        glm::mat4 bigtreeMMatrix = glm::translate(glm::mat4(1), bigtreeT[i]);
        bigtreeMMatrix           = glm::rotate(bigtreeMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        m_bigtrees[i]->m_MMatrix = bigtreeMMatrix;
    }

    //-------------------------BUSHES---------------------

    std::vector<glm::vec3> bushT = {
        glm::vec3(-1, 0, 1),
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, -25),
        glm::vec3(25, 0, 1),
        glm::vec3(0, 0, -18),
        glm::vec3(1, 0, -24),
        glm::vec3(1, 0, -18),
        glm::vec3(24, 0, -18),
        glm::vec3(17, 0, -18)};

    std::vector<glm::vec3> bushS = {
        glm::vec3(1, 27, 1),
        glm::vec3(25, 1, 1),
        glm::vec3(25, 1, 1),
        glm::vec3(1, 27, 1),
        glm::vec3(1, 7, 1),
        glm::vec3(23, 1, 1),
        glm::vec3(12, 1, 1),
        glm::vec3(1, 7, 1),
        glm::vec3(7, 1, 1)};

    for (int i = 0; i < BUSH_NB; i++)
    {
        m_bushes[i]->addSkinnedMesh("assets/models/bush.fbx");

        glm::mat4 bushMMatrix  = glm::translate(glm::mat4(1), bushT[i]);
        bushMMatrix            = glm::rotate(bushMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        bushMMatrix            = glm::scale(bushMMatrix, bushS[i]);
        m_bushes[i]->m_MMatrix = bushMMatrix;
    }

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
    m_LightList[0].m_color = glm::vec3(1.f, 1.f, .8f);
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
