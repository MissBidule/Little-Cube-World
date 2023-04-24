#include <glimac/ShadowCubeMap.hpp>
#include <glimac/common.hpp>
#include <iostream>
#include <vector>
#include "glimac/FreeflyCamera.hpp"
#include "glimac/Light.hpp"
#include "glimac/ObjProgram.hpp"
#include "glimac/ShadowMapFBO.hpp"
#include "glimac/ShadowProgram.hpp"
#include "glimac/SimpleObjProgram.hpp"
#include "glimac/SkinnedMesh.hpp"
#include "glimac/SkinnedObjProgram.hpp"
#include "glimac/TexObjProgram.hpp"
#include "glimac/TrackballCamera.hpp"
#include "glimac/sphere_vertices.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "p6/p6.h"

int main()
{
    int const window_width  = 1920;
    int const window_height = 1080;
    int const shadow_size   = 4096;

    auto ctx = p6::Context{{window_width, window_height, "TP11"}};
    ctx.maximize_window();

    // BEGINNING OF MY INIT CODE//

    std::vector<ObjProgram*> ObjList;

    // HERE WE CREATE THE TYPE OF EACH OBJECT - SIMPLE / TEXTURE / ANIMATED AND WE ADD THEM TO THE GLOBAL LIST//

    // Thingy
    SkinnedObjProgram Thingy("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", ctx);
    ObjList.emplace_back(&Thingy);

    // Earth
    SimpleObjProgram earth("shaders/3D.vs.glsl", "shaders/dirPoslight.fs.glsl");
    ObjList.emplace_back(&earth);

    // Moon
    const int                      MOON_NB = 32;
    std::vector<SimpleObjProgram*> moons;
    std::vector<glm::vec3>         RotDir;
    std::vector<glm::vec3>         RotAxes;

    for (size_t i = 0; i < MOON_NB; i++)
    {
        SimpleObjProgram* moon = new SimpleObjProgram("shaders/3D.vs.glsl", "shaders/dirPoslight.fs.glsl");
        moons.emplace_back(moon);
        ObjList.emplace_back(moon);
    }

    // Floor
    TexObjProgram floor("shaders/3D.vs.glsl", "shaders/limit.fs.glsl");
    ObjList.emplace_back(&floor);

    // THEN WE ADD THE MAIN CONFIGURATION (COLOR/TEXTURE/MODEL) AND MOVEMENT IF THE OBJECT IS STATIC//

    //--------------------------------THINGY---------------------
    Thingy.addSkinnedMesh("assets/models/test.fbx");

    //--------------------------------EARTH---------------------

    glimac::Color earthColor{
        glm::vec3(0.0215, 0.1745, 0.0215),
        glm::vec3(0.07568, 0.61424, 0.07568),
        glm::vec3(0.633, 0.727811, 0.633),
        0.6,
        1};

    earth.addManualMesh(glimac::sphere_vertices(1.f, 32, 16), earthColor);

    //--------------------------------MOON---------------------

    for (size_t i = 0; i < MOON_NB; i++)
    {
        glimac::Color moonColor{
            glm::vec3(glm::linearRand(0.f, 0.05f), glm::linearRand(0.f, 0.05f), glm::linearRand(0.f, 0.05f)),
            glm::vec3(glm::linearRand(0.f, 0.5f), glm::linearRand(0.f, 0.5f), glm::linearRand(0.f, 0.5f)),
            glm::vec3(glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f)),
            glm::linearRand(0.f, 1.f),
            1};

        moons[i]->addManualMesh(glimac::sphere_vertices(1.f, 32, 16), moonColor);

        RotAxes.push_back(glm::ballRand(2.f));
        RotDir.emplace_back(glm::linearRand(0, 1), glm::linearRand(0, 1), glm::linearRand(0, 1));
    }

    //--------------------------------FLOOR---------------------

    img::Image floorImg = p6::load_image_buffer("assets/textures/test.png");

    glimac::Texture floorTex{
        glimac::textureToUVtex(floorImg),
        glimac::textureToUVtex(floorImg),
        glimac::textureToUVtex(floorImg),
        0.6,
        1};

    // to be changed with the quad afterwards
    floor.addManualTexMesh(glimac::sphere_vertices(1.f, 32, 16), floorTex);

    glm::mat4 floorMMatrix = glm::translate(glm::mat4(1), glm::vec3(0, -15.f, 0));
    floorMMatrix           = glm::rotate(floorMMatrix, glm::radians(90.f), glm::vec3(1, 0, 0));
    floorMMatrix           = glm::scale(floorMMatrix, glm::vec3(8.f, 8.f, 8.f));
    floor.m_MMatrix        = floorMMatrix;

    // WE INIT ALL VAO AND VBO - NOTHING TO ADD HERE//

    for (auto& i : ObjList)
    {
        i->initVaoVbo();
    }

    //--------------------------------LIGHT---------------------
    std::vector<Light> LightList;

    // CREATE LIGHTS
    LightList.emplace_back(glimac::LightType::Directional);
    LightList[0].setPosition(glm::vec3(-1.5, -1.5, -1.5));
    LightList[0].m_color = glm::vec3(.8f, .8f, .8f);

    //--------------------------------MVP---------------------

    // MAIN CAMERA
    FreeflyCamera ViewMatrixCamera = FreeflyCamera();
    ViewMatrixCamera.moveFront(-5);

    // PROJECTION
    glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.f), window_width / static_cast<float>(window_height), 0.1f, 100.f);

    // ADDING THE PROJ TO ALL OBJECT - NOTHING TO BE DONE HERE//
    for (auto& i : ObjList)
    {
        i->m_ProjMatrix = ProjMatrix;
    }

    bool Z = false;
    bool Q = false;
    bool S = false;
    bool D = false;

    //--------------------------------SHADOW---------------------

    // FOR EACH LIGHT WE HAVE A SHADOW MAP - NOTHING TO BE DONE HERE//
    std::vector<ShadowProgram> shadowProgList;
    for (auto& i : LightList)
    {
        shadowProgList.emplace_back().SetLight(i);
        i.initShadowMap(shadow_size);
    }

    glEnable(GL_POLYGON_OFFSET_FILL);
    float factor = 1.f;
    float units  = 1.f;
    glPolygonOffset(factor, units);

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << "\n";

    // END OF MY INIT CODE//

    //--------------------------------LOOP---------------------

    /* Loop until the user closes the window */
    ctx.update = [&]() {
        // CAMERA CONTROL
        if (Z)
        {
            ViewMatrixCamera.moveFront(0.1);
        }
        if (Q)
        {
            ViewMatrixCamera.moveLeft(0.1);
        }
        if (S)
        {
            ViewMatrixCamera.moveFront(-0.1);
        }
        if (D)
        {
            ViewMatrixCamera.moveLeft(-0.1);
        }

        for (auto& i : ObjList)
        {
            i->m_ViewMatrix = ViewMatrixCamera.getViewMatrix();
        }

        // POSITION OF LIGHT IF IT IS UPDATED//
        LightList[0].rotateLeft(glm::degrees(ctx.delta_time()));

        // UPDATES OF ALL MMATRIX IF IT IS UPDATED//
        //  MM OF EARTH
        earth.m_MMatrix = glm::rotate(glm::mat4(1), -ctx.time(), glm::vec3(0, 1, 0));

        // MM OF MOONS
        for (size_t i = 0; i < MOON_NB; i++)
        {
            glm::mat4 MMatrix_moon = glm::rotate(glm::mat4(1), ctx.time(), glm::vec3(RotDir[i][0], RotDir[i][1], RotDir[i][2]));
            MMatrix_moon           = glm::translate(MMatrix_moon, RotAxes[i]);
            MMatrix_moon           = glm::scale(MMatrix_moon, glm::vec3(0.2, 0.2, 0.2));

            moons[i]->m_MMatrix = MMatrix_moon;
        }

        // MM OF THINGY
        glm::mat4 thingy_MMatrix = glm::translate(glm::mat4(1), glm::vec3(3.f, -3.f, 0.f));
        // BCS IT'S AN FBX OBJECT
        thingy_MMatrix = glm::rotate(thingy_MMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        //
        Thingy.m_MMatrix = thingy_MMatrix;

        // BEGIN OF MY DRAW CODE//

        //--------------------------------SHADOW MAP PASS---------------------

        // AUTOMATIC//
        for (size_t i = 0; i < shadowProgList.size(); i++)
        {
            LightList[i].refreshPosition();
            size_t loop = 1;
            if (LightList[i].getType() != (glimac::LightType::Point))
            {
                LightList[i].bindWShadowMap();
            }
            else
                loop = 6;

            for (size_t j = 0; j < loop; j++)
            {
                if (LightList[i].getType() == (glimac::LightType::Point))
                {
                    LightList[i].bindWShadowMap(j);
                }

                glClear(GL_DEPTH_BUFFER_BIT);

                shadowProgList[i].m_Program.use();

                for (auto& obj : ObjList)
                {
                    shadowProgList[i].SendOBJtransform(obj->m_MMatrix);
                    obj->shadowRender(0);
                }

                glBindVertexArray(0);
            }
        }

        //--------------------------------LIGHTING PASS---------------------

        ctx.render_to_main_canvas();

        glViewport(0, 0, ctx.current_canvas_width(), ctx.current_canvas_height());

        glClearColor(0.2f, 0.2f, 0.2f, 1.f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // AUTOMATIC//
        for (auto& obj : ObjList)
        {
            obj->m_Program.use();

            obj->uniformRender(LightList, 0);

            obj->render(LightList, 0);
        }

        // END OF MY DRAW CODE//
    };

    //--------------------------------INPUTS---------------------

    ctx.key_pressed = [&Z, &Q, &S, &D](const p6::Key& key) {
        if (key.physical == GLFW_KEY_W)
        {
            Z = true;
        }
        if (key.physical == GLFW_KEY_A)
        {
            Q = true;
        }
        if (key.physical == GLFW_KEY_S)
        {
            S = true;
        }
        if (key.physical == GLFW_KEY_D)
        {
            D = true;
        }
    };

    ctx.key_released = [&Z, &Q, &S, &D](const p6::Key& key) {
        if (key.physical == GLFW_KEY_W)
        {
            Z = false;
        }
        if (key.physical == GLFW_KEY_A)
        {
            Q = false;
        }
        if (key.physical == GLFW_KEY_S)
        {
            S = false;
        }
        if (key.physical == GLFW_KEY_D)
        {
            D = false;
        }
    };

    ctx.mouse_dragged = [&ViewMatrixCamera](const p6::MouseDrag& button) {
        ViewMatrixCamera.rotateLeft(button.delta.x * 50);
        ViewMatrixCamera.rotateUp(-button.delta.y * 50);
    };

    ctx.start();

    //--------------------------------CLEANUP---------------------

    for (auto& i : ObjList)
    {
        i->clear();
    }

    return 0;
}
