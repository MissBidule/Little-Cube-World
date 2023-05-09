#include <cstdarg>
#include <glimac/SimpleObjectManager.hpp>
#include <glimac/SkinnedObjectManager.hpp>
#include <glimac/TrackballCamera.hpp>
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
    // HERE WE CREATE THE TYPE OF EACH OBJECT - SIMPLE / TEXTURE / FBX AND WE ADD THEM TO THE GLOBAL LIST//

    // sun
    m_sun = new SimpleObjectManager("shaders/3D.vs.glsl", "shaders/dirPoslight.fs.glsl");
    m_ObjList.emplace_back(m_sun);

    // moon
    m_moon = new SimpleObjectManager("shaders/3D.vs.glsl", "shaders/dirPoslight.fs.glsl");
    m_ObjList.emplace_back(m_moon);

    // ground
    m_ground = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_ground);

    // river
    m_river = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_river);

    // boids
    for (int i = 0; i < BIRDS_NB; i++)
    {
        auto* bird = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
        m_birds.emplace_back(bird);
    }

    for (int i = 0; i < FISHES_NB; i++)
    {
        auto* fish = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
        m_fishes.emplace_back(fish);
    }


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

    // clovers
    for (int i = 0; i < CLOVER_NB; i++)
    {
        auto* clover = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
        m_clovers.emplace_back(clover);
        m_ObjList.emplace_back(clover);
    }

    // flowers
    for (int i = 0; i < FLOWER_NB; i++)
    {
        auto* flower = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
        m_flowers.emplace_back(flower);
        m_ObjList.emplace_back(flower);
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

    // bridge
    m_bridge = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_bridge);

    // arch
    m_arch = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_arch);

    // vegetables
    m_veggies = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_veggies);

    // wheelbarrow
    m_wheelb = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_wheelb);

    // bench
    m_bench = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_bench);

    // fountain stone
    m_fountainStone = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_fountainStone);

    // house
    m_house = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_house);

    // fountain
    m_fountain = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_fountain);

    // cat
    m_cat = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_cat);

    // cat head
    m_catHead = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_catHead);

    // fire
    m_fire = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_fire);

    // fire wood
    m_firewood = new SkinnedObjectManager("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", m_ctx);
    m_ObjList.emplace_back(m_firewood);

    // Limit
    m_limit = new TexObjectManager("shaders/3D.vs.glsl", "shaders/limit.fs.glsl");
    m_ObjList.emplace_back(m_limit);

    // Main character mesh
    m_Character.createCharacter("assets/models/character.fbx", m_ctx);

    // THEN WE ADD THE MAIN CONFIGURATION (COLOR/TEXTURE/MODEL) AND MOVEMENT IF THE OBJECT IS STATIC//

    //--------------------------------SUN---------------------

    glimac::Color sunColor{
        glm::vec3(1, 1, 0.35),
        glm::vec3(1, 1, 0.35),
        glm::vec3(1, 1, 0.35),
        1,
        1};

    m_sun->addManualMesh(glimac::quad_vertices(1.f), sunColor);
    m_sun->ignoreShadowRender = true;

    //--------------------------------MOON---------------------

    glimac::Color moonColor{
        glm::vec3(1, 1, 0.55),
        glm::vec3(1, 1, 0.55),
        glm::vec3(1, 1, 0.55),
        1,
        1};

    m_moon->addManualMesh(glimac::quad_vertices(1.f), moonColor);
    m_moon->ignoreShadowRender = true;

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
    glm::mat4 river_MMatrix = glm::translate(glm::mat4(1), glm::vec3(12.5f, 0, -12.5f));
    // idk what happened with the model ??
    river_MMatrix = glm::scale(river_MMatrix, glm::vec3(0.01, 0.01, 0.01));
    //
    m_river->m_MMatrix = river_MMatrix;

    //-------------------------BOIDS---------------------

    
    for (int i = 0; i < BIRDS_NB; i++)
    {
        m_birds[i]->addSkinnedMesh("assets/models/lowpoly_bird.fbx");
        m_birds[i]->addSkinnedMesh("assets/models/lowpoly_bird.fbx");

        glm::mat4 birdMMatrix = glm::translate(glm::mat4(1), birdFlock.myBoids[i].getPos());
        m_birds[i]->m_MMatrix = birdMMatrix;
    }

    for (int i = 0; i < FISHES_NB; i++)
    {
        m_fishes[i]->addSkinnedMesh("assets/models/lowpoly_fish.fbx");
        m_fishes[i]->addSkinnedMesh("assets/models/lowpoly_fish.fbx");

        glm::mat4 fishMMatrix = glm::translate(glm::mat4(1), fishFlock.myBoids[i].getPos());
        m_fishes[i]->m_MMatrix = fishMMatrix;
    }

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

    //-------------------------CLOVERS---------------------

    std::vector<glm::vec3> cloverT = {
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

    for (int i = 0; i < CLOVER_NB; i++)
    {
        m_clovers[i]->addSkinnedMesh("assets/models/clover.fbx");
        m_clovers[i]->addSkinnedMesh("assets/models/clover2.fbx");

        glm::mat4 cloverMMatrix = glm::translate(glm::mat4(1), cloverT[i]);
        cloverMMatrix           = glm::rotate(cloverMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        m_clovers[i]->m_MMatrix = cloverMMatrix;
    }

    //-------------------------FLOWERS---------------------

    std::vector<glm::vec3> flowerT = {
        glm::vec3(2, 0, -21),
        glm::vec3(3, 0, -19),
        glm::vec3(4, 0, -22),
        glm::vec3(5, 0, -20),
        glm::vec3(7, 0, -19),
        glm::vec3(9, 0, -21),
        glm::vec3(10, 0, -23),
        glm::vec3(11, 0, -19),
        glm::vec3(13, 0, -23),
        glm::vec3(15, 0, -19),
        glm::vec3(17, 0, -22),
        glm::vec3(18, 0, -20)};

    for (int i = 0; i < FLOWER_NB; i++)
    {
        m_flowers[i]->addSkinnedMesh("assets/models/flower.fbx");
        m_flowers[i]->addSkinnedMesh("assets/models/flower2.fbx");

        glm::mat4 flowerMMatrix = glm::translate(glm::mat4(1), flowerT[i]);
        flowerMMatrix           = glm::rotate(flowerMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        m_flowers[i]->m_MMatrix = flowerMMatrix;
    }

    //-------------------------LONG TREES---------------------

    std::vector<glm::vec3> longtreeT = {
        glm::vec3(1, 0, -19),
        glm::vec3(1, 0, -23),
        glm::vec3(3, 0, 0),
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

    //--------------------------------BRIDGE---------------------

    m_bridge->addSkinnedMesh("assets/models/bridge.fbx");

    glm::mat4 bridgeMMatrix = glm::translate(glm::mat4(1), glm::vec3(10, 0, -8));
    bridgeMMatrix           = glm::rotate(bridgeMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    m_bridge->m_MMatrix     = bridgeMMatrix;

    //--------------------------------ARCH---------------------

    m_arch->addSkinnedMesh("assets/models/arch.fbx");

    glm::mat4 archMMatrix = glm::translate(glm::mat4(1), glm::vec3(12, 0, -18));
    archMMatrix           = glm::rotate(archMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    m_arch->m_MMatrix     = archMMatrix;

    //--------------------------------VEGETABLES---------------------

    m_veggies->addSkinnedMesh("assets/models/vegetables.fbx");
    m_veggies->addSkinnedMesh("assets/models/vegetables2.fbx");

    glm::mat4 veggieMMatrix = glm::translate(glm::mat4(1), glm::vec3(12, 0, -20));
    veggieMMatrix           = glm::rotate(veggieMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    m_veggies->m_MMatrix    = veggieMMatrix;

    //--------------------------------WHEELBARROW---------------------

    m_wheelb->addSkinnedMesh("assets/models/wheelb.fbx");

    glm::mat4 wheelbMMatrix = glm::translate(glm::mat4(1), glm::vec3(6, 0, -15));
    wheelbMMatrix           = glm::rotate(wheelbMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    m_wheelb->m_MMatrix     = wheelbMMatrix;

    //--------------------------------BENCH---------------------

    m_bench->addSkinnedMesh("assets/models/bench.fbx");

    glm::mat4 benchMMatrix = glm::translate(glm::mat4(1), glm::vec3(2, 0, -20.375));
    benchMMatrix           = glm::rotate(benchMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    benchMMatrix           = glm::rotate(benchMMatrix, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    benchMMatrix           = glm::scale(benchMMatrix, glm::vec3(0.75f, 0.75f, 0.75f));
    m_bench->m_MMatrix     = benchMMatrix;

    //--------------------------------FOUNTAIN---------------------

    m_fountain->addSkinnedMesh("assets/models/fountain.fbx");
    m_fountain->addSkinnedMesh("assets/models/fountain2.fbx");

    // idk what happened with the model ??
    glm::mat4 fountainMMatrix = glm::translate(glm::mat4(1), glm::vec3(7.5f, 0.25f, -21.5f));
    fountainMMatrix           = glm::scale(fountainMMatrix, glm::vec3(0.01, 0.01, 0.01));
    m_fountain->m_MMatrix     = fountainMMatrix;

    //--------------------------------FOUNTAIN STONE---------------------

    m_fountainStone->addSkinnedMesh("assets/models/fountainStone.fbx");
    m_fountainStone->addSkinnedMesh("assets/models/fountainStone2.fbx");

    glm::mat4 fountainStoneMMatrix = glm::translate(glm::mat4(1), glm::vec3(7.5f, 0.25f, -21.5f));
    fountainStoneMMatrix           = glm::rotate(fountainStoneMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    m_fountainStone->m_MMatrix     = fountainStoneMMatrix;

    //--------------------------------HOUSE---------------------

    m_house->addSkinnedMesh("assets/models/house.fbx");

    glm::mat4 houseMMatrix = glm::translate(glm::mat4(1), glm::vec3(18.f, 0, -18.f));
    houseMMatrix           = glm::rotate(houseMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    m_house->m_MMatrix     = houseMMatrix;

    //--------------------------------CAT---------------------

    m_cat->addSkinnedMesh("assets/models/cat.fbx");

    // idk what happened with the model ??
    glm::mat4 catMMatrix = glm::translate(glm::mat4(1), glm::vec3(20.8f, 1.5f, -14.f));
    catMMatrix           = glm::scale(catMMatrix, glm::vec3(0.01, 0.01, 0.01));
    m_cat->m_MMatrix     = catMMatrix;

    //--------------------------------CAT HEAD---------------------

    m_catHead->addSkinnedMesh("assets/models/catHead.fbx");

    glm::mat4 catHeadMMatrix = glm::translate(glm::mat4(1), glm::vec3(20.8f, 1.55f, -14.f));
    catHeadMMatrix           = glm::rotate(catHeadMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    m_catHead->m_MMatrix     = catHeadMMatrix;

    //--------------------------------FIRE---------------------

    m_fire->addSkinnedMesh("assets/models/fire.fbx");

    glm::mat4 fireMMatrix      = glm::translate(glm::mat4(1), glm::vec3(13.f, 0.f, -1.f));
    fireMMatrix                = glm::rotate(fireMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    m_fire->m_MMatrix          = fireMMatrix;
    m_fire->ignoreShadowRender = true;

    //--------------------------------FIRE WOOD---------------------

    m_firewood->addSkinnedMesh("assets/models/firewood.fbx");

    glm::mat4 firewoodMMatrix = glm::translate(glm::mat4(1), glm::vec3(13.f, 0.f, -1.f));
    firewoodMMatrix           = glm::rotate(firewoodMMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    m_firewood->m_MMatrix     = firewoodMMatrix;

    //--------------------------------LIMIT---------------------

    img::Image limitImg = p6::load_image_buffer("assets/textures/limit.png");

    glimac::Texture limitTex{
        glimac::textureToUVtex(limitImg),
        glimac::textureToUVtex(limitImg),
        glimac::textureToUVtex(limitImg),
        0.6,
        1};

    m_limit->addManualTexMesh(glimac::quad_vertices(1.f), limitTex);

    glm::mat4 limitMMatrix = glm::translate(glm::mat4(1), glm::vec3(12.5f, 4.f, -12.5f));
    limitMMatrix           = glm::scale(limitMMatrix, glm::vec3(31.f, 25.f, 31.f));
    m_limit->m_MMatrix     = limitMMatrix;
    // We ignore the shadow because we navigate IN the limits
    m_limit->ignoreShadowRender = true;

    // WE INIT ALL VAO AND VBO - NOTHING TO ADD HERE//

    m_Character.initVaoVbo();

    // Do not forget particles//
    for (auto& i : m_FireParticles.m_meshes)
    {
        i->initVaoVbo();
    }
    for (auto& i : m_ChimneyParticles.m_meshes)
    {
        i->initVaoVbo();
    }

    for (auto& i : m_ObjList)
    {
        i->initVaoVbo();
    }
    //BOIDS

    for (auto& i : m_birds)
    {
        i->initVaoVbo();
    }
    for (auto& i : m_fishes)
    {
        i->initVaoVbo();
    }

    //--------------------------------LIGHT---------------------

    // CREATE LIGHTS up to 7
    m_LightList.emplace_back(glimac::LightType::Directional);
    m_LightList[0].setPosition(glm::vec3(0, 50, 0));
    m_LightList[0].m_color = glm::vec3(.8f, .8f, .6f);
    m_LightList.emplace_back(glimac::LightType::Point);
    m_LightList[1].setPosition(glm::vec3(14.f, 0.625f, -2.f));
    m_LightList[1].m_color = glm::vec3(.5f, .3f, .1f);
    m_LightList.emplace_back(glimac::LightType::Point);
    //m_LightList[2].m_color = glm::vec3(.3f, .3f, .1f);

    m_Character.setLight(m_LightList[2]);

    m_LightList[0].rotateLeft(90);
}

void App::initViewProjection()
{
    // THE CHARACTER AND CAMERA
    m_Character.setPosition(glm::vec3(13, 1.5, -5));
    m_Character.zoom(-2.f);

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
