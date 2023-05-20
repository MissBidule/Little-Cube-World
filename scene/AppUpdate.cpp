#include <glimac/SimpleObjectManager.hpp>
#include "App.hpp"
#include "glm/ext/matrix_transform.hpp"

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
        m_Character.moveFront(0.15);
    }
    if (Q)
    {
        m_Character.moveLeft(0.15);
    }
    if (S)
    {
        m_Character.moveFront(-0.15);
    }
    if (D)
    {
        m_Character.moveLeft(-0.15);
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
        m_Character.rotateLeft(button.delta.x * 50);
        m_Character.rotateUp(-button.delta.y * 50);
    };
}

void App::loop()
{
    // PROJ UPDATE
    m_ProjMatrix = glm::perspective(glm::radians(70.f), m_ctx.aspect_ratio(), 0.1f, 100.f);

    // ALL OBJECTS UPDATES//

    // character update
    m_Character.m_isMoving = Z || Q || S || D;
    m_Character.updatePosition();

    // Boids
    fishFlock.simulate();
    fishFlock.displayParam();

    birdFlock.simulate();
    birdFlock.displayParam();

    for (int i = 0; i < BIRDS_NB; i++)
    {
        glm::mat4 movMatrix = glm::translate(glm::mat4(1.0f), birdFlock.myBoids[i].getPos());
        movMatrix *= birdFlock.myBoids[i].getRotationMatrix();

        m_birds[i]->m_MMatrix = movMatrix;
    }

    for (int i = 0; i < FISHES_NB; i++)
    {

        glm::mat4 movMatrix = glm::translate(glm::mat4(1.0f), fishFlock.myBoids[i].getPos());
        movMatrix *= fishFlock.myBoids[i].getRotationMatrix();

        m_fishes[i]->m_MMatrix = movMatrix;
        movMatrix           = glm::scale(movMatrix, glm::vec3(0.2, 0.2, 0.2));

        m_fishes[i]->m_MMatrix = movMatrix;
    }

    if (!timeIsPaused)
    {
        // LA. Création et actualisation des particules de particule
        m_FireParticles.refreshParticles(glm::vec3(14.f, 0.625f, -2.f), m_Character.getPosition(), m_ctx.delta_time());

        m_ChimneyParticles.refreshParticles(glm::vec3(21.f, 6.5f, -22.f), m_Character.getPosition(), m_ctx.delta_time());

        // POSITION OF LIGHT IF IT IS UPDATED//
        m_LightList[0].rotateUp(m_ctx.delta_time() * 0.05f * 180.f * timeCoefficient);

        // UPDATES OF ALL OBJECT MMATRIX IF IT IS UPDATED//
        glm::vec3 lightPosition = glm::vec3(m_LightList[0].getMMatrix() * glm::vec4(m_LightList[0].getPosition(), 1));
        glm::mat4 sun_MMatrix   = glm::translate(glm::mat4(1), lightPosition);
        sun_MMatrix             = glm::translate(sun_MMatrix, glm::vec3(12.5f, 0, -20.f));
        sun_MMatrix             = glm::scale(sun_MMatrix, glm::vec3(10, 10, 10));

        m_sun->m_MMatrix = sun_MMatrix;

        glm::mat4 moon_MMatrix = glm::translate(glm::mat4(1), glm::vec3(-lightPosition.x, -lightPosition.y, lightPosition.z));
        moon_MMatrix           = glm::translate(moon_MMatrix, glm::vec3(12.5f, 0, -20.f));
        moon_MMatrix           = glm::scale(moon_MMatrix, glm::vec3(5, 5, 5));

        m_moon->m_MMatrix = moon_MMatrix;
    }

    for (auto& obj : m_ObjList)
    {
        obj->autoplay = !timeIsPaused;
    }
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

            // BOIDS

            for (auto& currentBoid : m_birds)
            {
                if (currentBoid->ignoreShadowRender)
                    continue;

                int LOD = LODtoShow(currentBoid);

                m_ShadowProgList[i].SendOBJtransform(currentBoid->m_MMatrix, currentBoid->getBoneTransforms(LOD));
                currentBoid->shadowRender(LOD);
            }

            for (auto& currentBoid : m_fishes)
            {
                if (currentBoid->ignoreShadowRender)
                    continue;

                int LOD = LODtoShow(currentBoid);

                m_ShadowProgList[i].SendOBJtransform(currentBoid->m_MMatrix, currentBoid->getBoneTransforms(LOD));
                currentBoid->shadowRender(LOD);
            }

            for (auto& obj : m_ObjList)
            {
                if (obj->ignoreShadowRender)
                    continue;

                int LOD = LODtoShow(obj);

                m_ShadowProgList[i].SendOBJtransform(obj->m_MMatrix, obj->getBoneTransforms(LOD));
                obj->shadowRender(LOD);
            }

            m_ShadowProgList[i].SendOBJtransform(m_Character.getMMatrix(), m_Character.getBoneTransforms());
            m_Character.shadowRender();

            glBindVertexArray(0);
        }
    }
}

void App::lightPass()
{
    m_ctx.render_to_main_canvas();

    glViewport(0, 0, m_ctx.current_canvas_width(), m_ctx.current_canvas_height());

    ImGui::Begin("Scene settings");
    ImGui::Checkbox("Pause Time", &timeIsPaused);
    ImGui::SliderFloat("Time coefficient", &timeCoefficient, .01f, 2.f);
    ImGui::SliderInt("Chose character", &m_characterChosen, 0, m_Character.getMaxCharacter());
    ImGui::Checkbox("Auto LOD", &m_autoLOD);
    ImGui::End();

    if (!timeIsPaused)
        m_skyTime += (m_night ? -m_ctx.delta_time() * 0.05 : m_ctx.delta_time() * 0.05) * timeCoefficient;

    if (m_skyTime > 1)
    {
        m_night   = true;
        m_skyTime = 1;
    }
    if (m_skyTime < 0)
    {
        m_night   = false;
        m_skyTime = 0;
    }

    glClearColor(m_skyTime * 0.4f, m_skyTime * 0.7f, 0.2f + m_skyTime * 0.8f, 1.f);

    // turns the light on during the night
    m_Character.m_switch = m_skyTime <= 0.5;
    m_Character.setCharacter(m_characterChosen);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& currentBoid : m_birds)
    {
        int LOD = LODtoShow(currentBoid);

        currentBoid->m_Program.use();

        currentBoid->uniformRender(m_LightList, LOD, m_Character.getViewMatrix(), m_ProjMatrix);

        currentBoid->render(m_LightList, LOD);
    }

    for (auto& currentBoid : m_fishes)
    {
        int LOD = LODtoShow(currentBoid);

        currentBoid->m_Program.use();

        currentBoid->uniformRender(m_LightList, LOD, m_Character.getViewMatrix(), m_ProjMatrix);

        currentBoid->render(m_LightList, LOD);
    }

    // AUTOMATIC//
    for (auto& obj : m_ObjList)
    {
        int LOD = LODtoShow(obj);

        obj->m_Program.use();

        obj->uniformRender(m_LightList, LOD, m_Character.getViewMatrix(), m_ProjMatrix);

        obj->render(m_LightList, LOD);
    }

    m_Character.use();

    m_Character.uniformRender(m_LightList, m_ProjMatrix);

    m_Character.render(m_LightList);

    // PARTICLES //
    for (auto& particle : m_FireParticles.m_meshes)
    {
        particle->m_Program.use();

        particle->uniformRender(std::vector<LightManager>(), 0, m_Character.getViewMatrix(), m_ProjMatrix);

        particle->render({}, 0);
    }

    for (auto& particle : m_ChimneyParticles.m_meshes)
    {
        particle->m_Program.use();

        particle->uniformRender(std::vector<LightManager>(), 0, m_Character.getViewMatrix(), m_ProjMatrix);

        particle->render({}, 0);
    }
}

int App::LODtoShow(const ObjectManager* obj)
{
    glm::vec3 position = obj->getPosition(m_Character.getViewMatrix(), m_ProjMatrix);
    double    distance = glm::distance2(position, glm::vec3(0));

    // when distance is > LODdistance use 2nd LOD
    if ((distance > LODdistance || !m_autoLOD) && obj->getLODmax() > 1)
    {
        return 1;
    }

    return 0;
}