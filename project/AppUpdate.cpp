#include <glimac/SimpleObjectManager.hpp>
#include "App.hpp"

void App::update()
{
    /* Loop until the user closes the window */
    m_ctx.update = [&]() {
        keyEvent();
        loop();

        shadowPass();
        lightPass();
    };

    actionEvent();

    m_ctx.start();
}

void App::keyEvent()
{
    // CAMERA CONTROL
    if (Z)
    {
        m_ViewMatrixCamera.moveFront(0.1);
    }
    if (Q)
    {
        m_ViewMatrixCamera.moveLeft(0.1);
    }
    if (S)
    {
        m_ViewMatrixCamera.moveFront(-0.1);
    }
    if (D)
    {
        m_ViewMatrixCamera.moveLeft(-0.1);
    }
}

void App::actionEvent()
{
    m_ctx.key_pressed = [&](const p6::Key& key) {
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

    m_ctx.key_released = [&](const p6::Key& key) {
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

    m_ctx.mouse_dragged = [&](const p6::MouseDrag& button) {
        m_ViewMatrixCamera.rotateLeft(button.delta.x * 50);
        m_ViewMatrixCamera.rotateUp(-button.delta.y * 50);
    };
}

void App::loop()
{
    // PROJ UPDATE
    m_ProjMatrix = glm::perspective(glm::radians(70.f), m_ctx.aspect_ratio(), 0.1f, 100.f);

    // ALL OBJECTS UPDATES//

    // LA. Création et actualisation des particules de particule
    m_Particles.refreshParticles(glm::vec3(3.f, -1.5f, 0.f), m_ViewMatrixCamera.getPosition(), m_ctx.delta_time());

    // POSITION OF LIGHT IF IT IS UPDATED//
    m_LightList[0].rotateLeft(glm::degrees(m_ctx.delta_time()));

    // UPDATES OF ALL OBJECT MMATRIX IF IT IS UPDATED//
    //  MM OF EARTH
    m_earth->m_MMatrix = glm::mat4(1);

    // MM OF MOONS
    for (int i = 0; i < MOON_NB; i++)
    {
        glm::mat4 MMatrix_moon = glm::rotate(glm::mat4(1), m_ctx.time(), glm::vec3(m_MoonRotDir[i][0], m_MoonRotDir[i][1], m_MoonRotDir[i][2]));
        MMatrix_moon           = glm::translate(MMatrix_moon, m_MoonRotAxes[i]);
        MMatrix_moon           = glm::scale(MMatrix_moon, glm::vec3(0.2, 0.2, 0.2));

        m_moons[i]->m_MMatrix = MMatrix_moon;
    }

    // MM OF THINGY
    glm::mat4 thingy_MMatrix = glm::translate(glm::mat4(1), glm::vec3(3.f, -3.f, 0.f));
    // BCS IT'S AN FBX OBJECT
    thingy_MMatrix = glm::rotate(thingy_MMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    //
    m_Thingy->m_MMatrix = thingy_MMatrix;
}

void App::shadowPass()
{
    // AUTOMATIC//
    for (size_t i = 0; i < m_ShadowProgList.size(); i++)
    {
        m_LightList[i].refreshPosition();
        size_t loop = 1;
        if (m_LightList[i].getType() != (glimac::LightType::Point))
        {
            m_LightList[i].bindWShadowMap();
        }
        else
            loop = 6;

        for (size_t j = 0; j < loop; j++)
        {
            if (m_LightList[i].getType() == (glimac::LightType::Point))
            {
                m_LightList[i].bindWShadowMap(j);
            }

            glClear(GL_DEPTH_BUFFER_BIT);

            m_ShadowProgList[i].use();

            for (auto& obj : m_ObjList)
            {
                if (obj->ignoreShadowRender)
                    continue;

                int LOD = LODtoShow(obj);

                m_ShadowProgList[i].SendOBJtransform(obj->m_MMatrix, obj->getBoneTransforms(LOD));
                obj->shadowRender(LOD);
            }

            glBindVertexArray(0);
        }
    }
}

void App::lightPass()
{
    m_ctx.render_to_main_canvas();

    glViewport(0, 0, m_ctx.current_canvas_width(), m_ctx.current_canvas_height());

    glClearColor(0.2f, 0.2f, 0.2f, 1.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // AUTOMATIC//
    for (auto& obj : m_ObjList)
    {
        int LOD = LODtoShow(obj);

        obj->m_Program.use();

        obj->uniformRender(m_LightList, LOD, m_ViewMatrixCamera.getViewMatrix(), m_ProjMatrix);

        obj->render(m_LightList, LOD);
    }

    // PARTICLES //
    for (auto& particle : m_Particles.m_meshes)
    {
        particle->m_Program.use();

        particle->uniformRender(std::vector<LightManager>(), 0, m_ViewMatrixCamera.getViewMatrix(), m_ProjMatrix);

        particle->render({}, 0);
    }
}

int App::LODtoShow(const ObjectManager* obj)
{
    glm::vec3 position = obj->getPosition(m_ViewMatrixCamera.getViewMatrix(), m_ProjMatrix);
    double    distance = glm::distance2(position, glm::vec3(0));

    // when distance is > LODdistance use 2nd LOD
    if (distance > LODdistance && obj->getLODmax() > 1)
    {
        return 1;
    }

    return 0;
}