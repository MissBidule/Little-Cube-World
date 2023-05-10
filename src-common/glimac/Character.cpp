#include "Character.hpp"
#include <glimac/LightManager.hpp>
#include <glimac/SkinnedObjectManager.hpp>
#include <glimac/TrackballCamera.hpp>
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

Character::Character(glm::vec3 position)
{
    m_POV = TrackballCamera(position);
}

glm::mat4 Character::getViewMatrix()
{
    return m_POV.getViewMatrix();
}

void Character::setLight(LightManager& Light)
{
    m_light      = &Light;
    m_lightColor = Light.m_color;
}

void Character::setCharacter(unsigned int chosenMesh)
{
    m_chosenMesh = (chosenMesh >= m_characterMesh.size() ? m_characterMesh.size() - 1 : chosenMesh);
}

void Character::setPosition(glm::vec3 position)
{
    m_POV.setPos(position);
}

void Character::zoom(float delta)
{
    m_POV.zoom(delta);
    m_zoom = delta;
}

void Character::moveFront(float delta)
{
    m_POV.moveFront(delta);
    if (!checkPosition())
    {
        m_POV.moveFront(-delta);
        return;
    }
    if (delta > 0)
    {
        m_angleMovement = m_POV.getAngleY();
    }
    else
    {
        m_angleMovement = m_POV.getAngleY() + 180;
    }
}

void Character::moveLeft(float delta)
{
    m_POV.moveLeft(delta);
    if (!checkPosition())
    {
        m_POV.moveLeft(-delta);
    }
}

void Character::rotateUp(float degrees)
{
    m_POV.rotateUp(degrees);

    int angle = static_cast<int>(m_POV.getAngleX());
    if (angle < 0)
    {
        angle = 360 - (-angle) % 360;
    }
    else
    {
        angle = angle % 360;
    }

    if (angle > 270 && degrees > 0)
    {
        m_POV.zoom(degrees * -0.01);
    }
    else if (angle > 270 && degrees < 0)
    {
        m_POV.zoom(degrees * -0.01);
    }

    if (angle > 180 && angle < 270 && degrees > 0)
    {
        m_POV.zoom(degrees * -0.01);
    }
    else if (angle > 180 && angle < 270 && degrees < 0)
    {
        m_POV.zoom(degrees * -0.01);
    }
}

void Character::rotateLeft(float degrees)
{
    m_POV.rotateLeft(degrees);
}

void Character::createCharacter(const std::string& objectPath, p6::Context& ctx)
{
    m_characterMesh.emplace_back(std::make_unique<SkinnedObjectManager>("shaders/3Dbones.vs.glsl", "shaders/dirPoslight.fs.glsl", ctx));
    m_characterMesh.back()->addSkinnedMesh(objectPath);
}

void Character::initVaoVbo()
{
    for (auto& mesh : m_characterMesh)
    {
        mesh->initVaoVbo();
    }
}

void Character::updatePosition()
{ /*
     glm::mat4 LightMMatrix = glm::translate(glm::mat4(1), getPosition());
     LightMMatrix           = glm::rotate(LightMMatrix, glm::radians(-m_angleMovement), glm::vec3(0, 1, 0));
     LightMMatrix           = glm::translate(LightMMatrix, glm::vec3(-0.45f, -0.75f, -0.5f));
     LightMMatrix           = glm::scale(LightMMatrix, glm::vec3(0.01, 0.01, 0.01));
     m_light->setPosition(glm::vec3(LightMMatrix * glm::vec4(0, 0, 0, 1)));

     m_light->m_color = glm::vec3(m_lightColor.x * m_switch, m_lightColor.y * m_switch, m_lightColor.z * m_switch);*/

    glm::mat4 CharMMatrix = glm::translate(glm::mat4(1), getPosition());
    CharMMatrix           = glm::translate(CharMMatrix, glm::vec3(0, -1.5, 0));
    CharMMatrix           = glm::rotate(CharMMatrix, glm::radians(180.f - m_angleMovement), glm::vec3(0, 1, 0));
    CharMMatrix           = glm::scale(CharMMatrix, glm::vec3(0.01, 0.01, 0.01));

    m_characterMesh[m_chosenMesh]->m_MMatrix = CharMMatrix;
}

void Character::uniformRender(const std::vector<LightManager>& AllLights, glm::mat4& ProjMatrix)
{
    m_characterMesh[m_chosenMesh]->autoplay = m_isMoving;
    m_characterMesh[m_chosenMesh]->uniformRender(AllLights, 0, getViewMatrix(), ProjMatrix);
}

void Character::render(const std::vector<LightManager>& AllLights)
{
    m_characterMesh[m_chosenMesh]->render(AllLights, 0);
}

void Character::use()
{
    m_characterMesh[m_chosenMesh]->m_Program.use();
}

void Character::shadowRender()
{
    m_characterMesh[m_chosenMesh]->autoplay = m_isMoving;
    m_characterMesh[m_chosenMesh]->shadowRender(0);
}

glm::vec3 Character::getPosition()
{
    return m_POV.getPosition();
}

glm::mat4 Character::getMMatrix()
{
    return m_characterMesh[m_chosenMesh]->m_MMatrix;
}

std::vector<glm::mat4> Character::getBoneTransforms()
{
    return m_characterMesh[m_chosenMesh]->getBoneTransforms(0);
}

bool Character::checkPosition()
{
    glm::vec3 positionToCheck = getPosition();

    if (positionToCheck.x < 0 || positionToCheck.x >= m_width)
        return false;
    if (-positionToCheck.z < 0 || -positionToCheck.z >= m_height)
        return false;
    if (!m_allowedPosition[-static_cast<int>(positionToCheck.z) * 25 + static_cast<int>(positionToCheck.x)])
        return false;

    return true;
}