#include "ObjProgram.hpp"
#include <glimac/Light.hpp>
#include <glimac/SkinnedMesh.hpp>
#include <vector>
#include "glm/gtc/type_ptr.hpp"

ObjProgram::ObjProgram(const std::string& vsPath, const std::string& fsPath)
    : m_Program(p6::load_shader(vsPath, fsPath))
{
    m_uMVPMatrix    = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
    m_uMVMatrix     = glGetUniformLocation(m_Program.id(), "uMVMatrix");
    m_uMMatrix      = glGetUniformLocation(m_Program.id(), "uMMatrix");
    m_uNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
    m_uNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
    m_uLightNB      = glGetUniformLocation(m_Program.id(), "uLightNB");

    std::string appendVarName;
    for (unsigned int i = 0; i < MAXTAB; i++)
    {
        appendVarName = "uMVPLight[" + std::to_string(i) + "]";
        m_uMVPLight.emplace_back(glGetUniformLocation(m_Program.id(), appendVarName.c_str()));
        appendVarName = "uLight[" + std::to_string(i) + "].type";
        m_uLightType.emplace_back(glGetUniformLocation(m_Program.id(), appendVarName.c_str()));
        appendVarName = "uLight[" + std::to_string(i) + "].position";
        m_uLightPos.emplace_back(glGetUniformLocation(m_Program.id(), appendVarName.c_str()));
        appendVarName = "uLight[" + std::to_string(i) + "].position_vs";
        m_uLightPos_vs.emplace_back(glGetUniformLocation(m_Program.id(), appendVarName.c_str()));
        appendVarName = "uLight[" + std::to_string(i) + "].color";
        m_uLightIntensity.emplace_back(glGetUniformLocation(m_Program.id(), appendVarName.c_str()));
        appendVarName = "uShadowMap_" + std::to_string(i);
        m_uShadowMap.emplace_back(glGetUniformLocation(m_Program.id(), appendVarName.c_str()));
        appendVarName = "uShadowCubeMap_" + std::to_string(i);
        m_uShadowMapCube.emplace_back(glGetUniformLocation(m_Program.id(), appendVarName.c_str()));
        appendVarName = "uLight[" + std::to_string(i) + "].ufar_plane";
        m_ufar_plane.emplace_back(glGetUniformLocation(m_Program.id(), appendVarName.c_str()));
    }
}

void ObjProgram::uniformRender(const std::vector<Light>& AllLights, [[maybe_unused]] int LOD)
{
    glm::mat4 MVMatrix     = m_ViewMatrix * m_MMatrix;
    glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

    glUniformMatrix4fv(m_uMVPMatrix, 1, GL_FALSE, glm::value_ptr(m_ProjMatrix * MVMatrix));
    glUniformMatrix4fv(m_uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
    glUniformMatrix4fv(m_uMMatrix, 1, GL_FALSE, glm::value_ptr(m_MMatrix));
    glUniformMatrix4fv(m_uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

    glUniform1i(m_uLightNB, AllLights.size());

    for (size_t i = 0; i < AllLights.size() && i < MAXTAB; i++)
    {
        glm::mat4 MVPLight = AllLights[i].getProjMatrix() * AllLights[i].getVMatrix() * m_MMatrix;
        glUniformMatrix4fv(m_uMVPLight[i], 1, GL_FALSE, glm::value_ptr(MVPLight));
        glUniform1i(m_uLightType[i], static_cast<int>(AllLights[i].getType()));

        // calcul of lightPos depending of its movement
        glm::vec4 lightInitPosition = glm::vec4(AllLights[i].getPosition(), (static_cast<int>(AllLights[i].getType()) > static_cast<int>(glimac::LightType::Directional)));

        glUniform3fv(m_uLightPos[i], 1, glm::value_ptr(glm::vec3((AllLights[i].getMMatrix()) * lightInitPosition)));
        glUniform3fv(m_uLightPos_vs[i], 1, glm::value_ptr(glm::vec3((m_ViewMatrix * AllLights[i].getMMatrix()) * lightInitPosition)));
        glUniform3fv(m_uLightIntensity[i], 1, glm::value_ptr(AllLights[i].m_color));

        glUniform1f(m_ufar_plane[i], AllLights[i].getFarPlane());
    }
}

void ObjProgram::clear()
{
    for (unsigned int& i : m_VAO)
    {
        glDeleteBuffers(1, &i);
    }
    for (unsigned int& i : m_VBO)
    {
        glDeleteBuffers(1, &i);
    }
}

void ObjProgram::initVaoVbo()
{
    for (unsigned int i = 0; i < m_LODsNB; i++)
    {
        // init ONE vbo with coord data
        GLuint vbo = 0;
        glGenBuffers(1, &vbo);
        // array_buffer is for vbo
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // sphere
        const std::vector<glimac::ShapeVertex> currentShape = m_shapes[i];

        // fill those coords in the vbo / Static is for constant variables
        glBufferData(GL_ARRAY_BUFFER, currentShape.size() * sizeof(glimac::ShapeVertex), currentShape.data(), GL_STATIC_DRAW);

        // Depth option
        glEnable(GL_DEPTH_TEST);
        // a little transparency...
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // init ONE vao with info data
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // enable the INDEX attribut we want / POSITION is index 0 by default
        const GLuint VERTEX_ATTR_POSITION = 0;
        const GLuint VERTEX_ATTR_NORM     = 1;
        const GLuint VERTEX_ATTR_UV       = 2;
        glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
        glEnableVertexAttribArray(VERTEX_ATTR_NORM);
        glEnableVertexAttribArray(VERTEX_ATTR_UV);

        // rebind the vbo to specify vao attribute
        glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, position)));
        glVertexAttribPointer(VERTEX_ATTR_NORM, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, normal)));
        glVertexAttribPointer(VERTEX_ATTR_UV, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, texCoords)));

        // debind the vbo
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // debind the vao
        glBindVertexArray(0);

        m_VBO.emplace_back(vbo);
        m_VAO.emplace_back(vao);
    }
}

void ObjProgram::prerender(const std::vector<Light>& AllLights) const
{
    for (size_t i = 0; i < MAXTAB; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i * 2);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUniform1i(m_uShadowMap[i], static_cast<int>(i * 2));
        glActiveTexture(GL_TEXTURE0 + i * 2 + 1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glUniform1i(m_uShadowMapCube[i], static_cast<int>(i * 2 + 1));
    }

    for (size_t i = 0; i < AllLights.size() && i < MAXTAB; i++)
    {
        if (AllLights[i].getType() == glimac::LightType::Point)
        {
            AllLights[i].bindRShadowMap(GL_TEXTURE0 + i * 2 + 1);
        }
        else
        {
            AllLights[i].bindRShadowMap(GL_TEXTURE0 + i * 2);
        }
    }
}

void ObjProgram::postrender() const
{
    for (size_t i = 0; i < MAXTAB; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i * 2);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0 + i * 2 + 1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
}