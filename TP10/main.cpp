#include <cfloat>
#include <iostream>
#include <vector>
#include "glimac/FreeflyCamera.hpp"
#include "glimac/TrackballCamera.hpp"
#include "glimac/common.hpp"
#include "glimac/sphere_vertices.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "img/src/Image.h"
#include "p6/p6.h"

int const window_width  = 1920;
int const window_height = 1080;
int const shadow_size   = 4096;

// Simple point light and dir light
class ShadowMapFBO {
public:
    ShadowMapFBO();

    ~ShadowMapFBO();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading(GLenum TextureUnit);

private:
    unsigned int m_width     = 0;
    unsigned int m_height    = 0;
    GLuint       m_fbo       = 0;
    GLuint       m_shadowMap = 0;
};

ShadowMapFBO::ShadowMapFBO() = default;

ShadowMapFBO::~ShadowMapFBO()
{
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteTextures(1, &m_shadowMap);
}

bool ShadowMapFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    m_width  = WindowWidth;
    m_height = WindowHeight;

    // Create the FBO
    glGenFramebuffers(1, &m_fbo);

    // create the depth buffer
    glGenTextures(1, &m_shadowMap);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap, 0);

    // Disable writes to the color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "FB error status : " << Status << std::endl;
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void ShadowMapFBO::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glpp::viewport(0, 0, m_width, m_height); // Width/height of shadow map
}

void ShadowMapFBO::BindForReading(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
}

// STRUCT FOR CUBEMAP
struct CameraDirection {
    GLenum CubemapFace;
    float  theta;
    float  phi;
};

std::vector<CameraDirection> CameraDirections = {
    {GL_TEXTURE_CUBE_MAP_POSITIVE_X, 180.f, -90.f},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 180.f, 90.f},
    {GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 90.f, 180.f},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, -90.f, 0.f},
    {GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 180.f, 180.f},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 180.f, 0.f}};

// cube point light
class ShadowCubeMapFBO {
public:
    ShadowCubeMapFBO();
    ~ShadowCubeMapFBO();
    bool Init(unsigned int size);
    void BindForWriting(GLenum CubeFace);
    void BindForReading(GLenum TextureUnit);

private:
    unsigned int m_size          = 0;
    GLuint       m_fbo           = 0;
    GLuint       m_shadowCubeMap = 0;
    // GLuint       m_depth         = 0;
};

ShadowCubeMapFBO::ShadowCubeMapFBO() = default;

ShadowCubeMapFBO::~ShadowCubeMapFBO()
{
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteTextures(1, &m_shadowCubeMap);
    // glDeleteTextures(1, &m_depth);
}

bool ShadowCubeMapFBO::Init(unsigned int size)
{
    m_size = size;

    // // create the depth buffer
    // glGenTextures(1, &m_depth);
    // glBindTexture(GL_TEXTURE_2D, m_depth);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_size, m_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // create the cube map
    glGenTextures(1, &m_shadowCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowCubeMap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (size_t i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, m_size, m_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    // Create the FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowCubeMap, 0);

    // Disable writes to the color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "FB error status : " << Status << std::endl;
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return true;
}

void ShadowCubeMapFBO::BindForWriting(GLenum CubeFace)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_size, m_size);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, CubeFace, m_shadowCubeMap, 0);
    // glDrawBuffer(GL_DEPTH_ATTACHMENT);
}

void ShadowCubeMapFBO::BindForReading(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowCubeMap);
}

// MY STRUCTS

struct ShadowProgram {
    p6::Shader m_Program;
    GLint      uMVPLight;
    GLint      uMMatrix;
    GLint      uLightPos;
    GLint      ufar_plane;

    ShadowProgram()
        : m_Program(p6::load_shader("shaders/shadow.vs.glsl", "shaders/shadow.fs.glsl"))
    {
        uMVPLight  = glGetUniformLocation(m_Program.id(), "uMVPLight");
        uMMatrix   = glGetUniformLocation(m_Program.id(), "uMMatrix");
        uLightPos  = glGetUniformLocation(m_Program.id(), "uLightPos");
        ufar_plane = glGetUniformLocation(m_Program.id(), "ufar_plane");
    }
};

struct EarthProgram {
    p6::Shader m_Program;

    std::vector<glimac::ShapeVertex> m_sphere;

    GLint uMMatrix;
    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;

    GLint uKa;
    GLint uKd;
    GLint uKs;
    GLint uShininess;

    GLint uLightDir_vs;
    GLint uLightPos_vs;
    GLint uLightPos;
    GLint uLightIntensity;
    GLint uTexture;
    GLint ufar_plane;

    EarthProgram()
        : m_Program(p6::load_shader("shaders/3D.vs.glsl", "shaders/dirPoslight.fs.glsl"))
    {
        uMMatrix        = glGetUniformLocation(m_Program.id(), "uMMatrix");
        uMVPMatrix      = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix       = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix   = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uKa             = glGetUniformLocation(m_Program.id(), "uKa");
        uKd             = glGetUniformLocation(m_Program.id(), "uKd");
        uKs             = glGetUniformLocation(m_Program.id(), "uKs");
        uShininess      = glGetUniformLocation(m_Program.id(), "uShininess");
        uLightDir_vs    = glGetUniformLocation(m_Program.id(), "uLightDir_vs");
        uLightPos_vs    = glGetUniformLocation(m_Program.id(), "uLightPos_vs");
        uLightPos       = glGetUniformLocation(m_Program.id(), "uLightPos");
        uLightIntensity = glGetUniformLocation(m_Program.id(), "uLightIntensity");
        uTexture        = glGetUniformLocation(m_Program.id(), "uTexture");
        ufar_plane      = glGetUniformLocation(m_Program.id(), "ufar_plane");
    }

    void uniformSender(glm::mat4 camera, glm::mat4 ProjMatrix, glm::vec3 LightDir, glm::vec3 LightPos, float Rotation)
    {
        // Terre
        glm::mat4 MMatrix      = glm::rotate(glm::mat4(1.f), Rotation, glm::vec3(0, 1, 0));
        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(camera * MMatrix));

        glUniformMatrix4fv(uMMatrix, 1, GL_FALSE, glm::value_ptr(MMatrix));
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * camera * MMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(camera * MMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        glUniform3fv(uKa, 1, glm::value_ptr(glm::vec3(0.0215, 0.1745, 0.0215)));
        glUniform3fv(uKd, 1, glm::value_ptr(glm::vec3(0.07568, 0.61424, 0.07568)));
        glUniform3fv(uKs, 1, glm::value_ptr(glm::vec3(0.633, 0.727811, 0.633)));
        glUniform1f(uShininess, 0.6);

        glUniform3fv(uLightDir_vs, 1, glm::value_ptr(LightDir));
        glUniform3fv(uLightPos_vs, 1, glm::value_ptr(LightPos));
        glUniform3fv(uLightIntensity, 1, glm::value_ptr(glm::vec3(0.8, .8, .8)));

        glUniform1i(uTexture, 1);
    }

    void render()
    {
        glDrawArrays(GL_TRIANGLES, 0, m_sphere.size());
    }
};

struct FloorProgram {
    p6::Shader m_Program;

    std::vector<glimac::ShapeVertex> m_plane;

    GLint uMMatrix;
    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;

    GLint uKa;
    GLint uKd;
    GLint uKs;
    GLint uShininess;

    GLint uLightDir_vs;
    GLint uLightPos_vs;
    GLint uLightPos;
    GLint uLightIntensity;
    GLint uTexture;
    GLint ufar_plane;

    FloorProgram()
        : m_Program(p6::load_shader("shaders/3D.vs.glsl", "shaders/dirPoslight.fs.glsl"))
    {
        uMMatrix        = glGetUniformLocation(m_Program.id(), "uMMatrix");
        uMVPMatrix      = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix       = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix   = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uKa             = glGetUniformLocation(m_Program.id(), "uKa");
        uKd             = glGetUniformLocation(m_Program.id(), "uKd");
        uKs             = glGetUniformLocation(m_Program.id(), "uKs");
        uShininess      = glGetUniformLocation(m_Program.id(), "uShininess");
        uLightDir_vs    = glGetUniformLocation(m_Program.id(), "uLightDir_vs");
        uLightPos_vs    = glGetUniformLocation(m_Program.id(), "uLightPos_vs");
        uLightPos       = glGetUniformLocation(m_Program.id(), "uLightPos");
        uLightIntensity = glGetUniformLocation(m_Program.id(), "uLightIntensity");
        uTexture        = glGetUniformLocation(m_Program.id(), "uTexture");
        ufar_plane      = glGetUniformLocation(m_Program.id(), "ufar_plane");
    }

    void uniformSender(glm::mat4 camera, glm::mat4 ProjMatrix, glm::vec3 LightDir, glm::vec3 LightPos)
    {
        // Floor
        glm::mat4 MMatrix      = glm::translate(glm::mat4(1.f), glm::vec3(0, -3.f, 0));
        MMatrix                = glm::rotate(MMatrix, glm::radians(90.f), glm::vec3(1, 0, 0));
        MMatrix                = glm::scale(MMatrix, glm::vec3(50.f, 50.f, 50.f));
        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(camera * MMatrix));

        glUniformMatrix4fv(uMMatrix, 1, GL_FALSE, glm::value_ptr(MMatrix));
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * camera * MMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(camera * MMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        glUniform3fv(uKa, 1, glm::value_ptr(glm::vec3(0.0215, 0.1745, 0.0215)));
        glUniform3fv(uKd, 1, glm::value_ptr(glm::vec3(0.07568, 0.61424, 0.07568)));
        glUniform3fv(uKs, 1, glm::value_ptr(glm::vec3(0.633, 0.727811, 0.633)));
        glUniform1f(uShininess, 0.6);

        glUniform3fv(uLightDir_vs, 1, glm::value_ptr(LightDir));
        glUniform3fv(uLightPos_vs, 1, glm::value_ptr(LightPos));
        glUniform3fv(uLightIntensity, 1, glm::value_ptr(glm::vec3(0.8, .8, .8)));

        glUniform1i(uTexture, 1);
    }

    void render()
    {
        glDrawArrays(GL_TRIANGLES, 0, m_plane.size());
    }
};

struct MoonProgram {
    p6::Shader                       m_Program;
    std::vector<glimac::ShapeVertex> m_sphere;

    const int MOON_NB = 32;

    GLint uMMatrix;
    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;

    GLint uKa;
    GLint uKd;
    GLint uKs;
    GLint uShininess;

    GLint uLightDir_vs;
    GLint uLightPos_vs;
    GLint uLightPos;
    GLint uLightIntensity;
    GLint uTexture;
    GLint ufar_plane;

    std::vector<glm::vec3> RotAxes;
    std::vector<glm::vec3> RotDir;
    std::vector<glm::vec3> Ka;
    std::vector<glm::vec3> Kd;
    std::vector<glm::vec3> Ks;
    std::vector<float>     Shininess;

    MoonProgram()
        : m_Program(p6::load_shader("shaders/3D.vs.glsl", "shaders/dirPoslight.fs.glsl"))
    {
        uMMatrix        = glGetUniformLocation(m_Program.id(), "uMMatrix");
        uMVPMatrix      = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix       = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix   = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uKa             = glGetUniformLocation(m_Program.id(), "uKa");
        uKd             = glGetUniformLocation(m_Program.id(), "uKd");
        uKs             = glGetUniformLocation(m_Program.id(), "uKs");
        uShininess      = glGetUniformLocation(m_Program.id(), "uShininess");
        uLightDir_vs    = glGetUniformLocation(m_Program.id(), "uLightDir_vs");
        uLightPos_vs    = glGetUniformLocation(m_Program.id(), "uLightPos_vs");
        uLightPos       = glGetUniformLocation(m_Program.id(), "uLightPos");
        uLightIntensity = glGetUniformLocation(m_Program.id(), "uLightIntensity");
        uTexture        = glGetUniformLocation(m_Program.id(), "uTexture");
        ufar_plane      = glGetUniformLocation(m_Program.id(), "ufar_plane");

        for (int i = 0; i < MOON_NB; i++)
        {
            RotAxes.push_back(glm::ballRand(2.f));
            RotDir.emplace_back(glm::linearRand(0, 1), glm::linearRand(0, 1), glm::linearRand(0, 1));
            Ka.emplace_back(glm::linearRand(0.f, 0.05f), glm::linearRand(0.f, 0.05f), glm::linearRand(0.f, 0.05f));
            Kd.emplace_back(glm::linearRand(0.f, 0.5f), glm::linearRand(0.f, 0.5f), glm::linearRand(0.f, 0.5f));
            Ks.emplace_back(glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f));
            Shininess.emplace_back(glm::linearRand(0.f, 1.f));
        }
    }

    void uniformSender(int i, glm::mat4 camera, glm::mat4 ProjMatrix, glm::vec3 LightDir, glm::vec3 LightPos, float Rotation)
    {
        glm::mat4 MMatrix_moon      = glm::rotate(glm::mat4(1.f), Rotation, glm::vec3(RotDir[i][0], RotDir[i][1], RotDir[i][2]));
        MMatrix_moon                = glm::translate(MMatrix_moon, RotAxes[i]);
        MMatrix_moon                = glm::scale(MMatrix_moon, glm::vec3(0.2, 0.2, 0.2));
        glm::mat4 NormalMatrix_moon = glm::transpose(glm::inverse(camera * MMatrix_moon));

        glUniformMatrix4fv(uMMatrix, 1, GL_FALSE, glm::value_ptr(MMatrix_moon));
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * camera * MMatrix_moon));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(camera * MMatrix_moon));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix_moon));

        glUniform3fv(uKa, 1, glm::value_ptr(Ka[i]));
        glUniform3fv(uKd, 1, glm::value_ptr(Kd[i]));
        glUniform3fv(uKs, 1, glm::value_ptr(Ks[i]));
        glUniform1f(uShininess, Shininess[i]);

        glUniform3fv(uLightDir_vs, 1, glm::value_ptr(LightDir));
        glUniform3fv(uLightPos_vs, 1, glm::value_ptr(LightPos));
        glUniform3fv(uLightIntensity, 1, glm::value_ptr(glm::vec3(0.8, .8, .8)));

        glUniform1i(uTexture, 1);
    }

    void render()
    {
        glDrawArrays(GL_TRIANGLES, 0, m_sphere.size());
    }
};

int main()
{
    auto ctx = p6::Context{{window_width, window_height, "TP10"}};
    ctx.maximize_window();

    // BEGINNING OF MY INIT CODE//

    // create the programs
    EarthProgram earth;
    MoonProgram  moon;
    FloorProgram floor;

    //--------------------------------SPHERE---------------------

    // init ONE vbo with coord data
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    // array_buffer is for vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // sphere
    const std::vector<glimac::ShapeVertex> sphere = glimac::sphere_vertices(1.f, 32, 16);

    // fill those coords in the vbo / Static is for constant variables
    glBufferData(GL_ARRAY_BUFFER, sphere.size() * sizeof(glimac::ShapeVertex), sphere.data(), GL_STATIC_DRAW);
    earth.m_sphere = sphere;
    moon.m_sphere  = sphere;

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

    //--------------------------------FLOOR---------------------

    // init ONE vbo with coord data
    GLuint vboF = 0;
    glGenBuffers(1, &vboF);
    // array_buffer is for vbo
    glBindBuffer(GL_ARRAY_BUFFER, vboF);

    // plane
    std::vector<glimac::ShapeVertex> plane;
    plane.reserve(6);
    for (int i = 0; i < 6; i++)
    {
        plane.emplace_back();
    }
    plane[0].position  = glm::vec3(-0.5, 0.5, 0);
    plane[0].normal    = glm::vec3(0, 0, -1);
    plane[0].texCoords = glm::vec2(0, 1);
    plane[1].position  = glm::vec3(-0.5, -0.5, 0);
    plane[1].normal    = glm::vec3(0, 0, -1);
    plane[1].texCoords = glm::vec2(0, 0);
    plane[2].position  = glm::vec3(0.5, -0.5, 0);
    plane[2].normal    = glm::vec3(0, 0, -1);
    plane[2].texCoords = glm::vec2(1, 0);
    plane[3].position  = glm::vec3(0.5, -0.5, 0);
    plane[3].normal    = glm::vec3(0, 0, -1);
    plane[3].texCoords = glm::vec2(1, 0);
    plane[4].position  = glm::vec3(0.5, 0.5, 0);
    plane[4].normal    = glm::vec3(0, 0, -1);
    plane[4].texCoords = glm::vec2(1, 1);
    plane[5].position  = glm::vec3(-0.5, 0.5, 0);
    plane[5].normal    = glm::vec3(0, 0, -1);
    plane[5].texCoords = glm::vec2(0, 1);

    // fill those coords in the vbo / Static is for constant variables
    glBufferData(GL_ARRAY_BUFFER, plane.size() * sizeof(glimac::ShapeVertex), plane.data(), GL_STATIC_DRAW);
    floor.m_plane = plane;

    // Depth option
    glEnable(GL_DEPTH_TEST);
    // a little transparency...
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // init ONE vao with info data
    GLuint vaoF = 0;
    glGenVertexArrays(1, &vaoF);
    glBindVertexArray(vaoF);

    // enable the INDEX attribut we want / POSITION is index 0 by default
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

    //--------------------------------MVP---------------------

    // MVP
    glm::vec3     Light(0, -2.f, 2.f);
    FreeflyCamera ViewMatrixCamera = FreeflyCamera();
    FreeflyCamera ViewMatrixLight  = FreeflyCamera();
    ViewMatrixCamera.moveFront(-5);
    ViewMatrixLight.setPos(Light);
    glm::mat4 ProjMatrix         = glm::perspective(glm::radians(70.f), window_width / static_cast<float>(window_height), 0.1f, 100.f);
    float     far_plane          = 20.f;
    glm::mat4 ProjMatrixLight    = glm::perspective(glm::radians(90.f), shadow_size / static_cast<float>(shadow_size), 0.1f, far_plane);
    float     valueOrtho         = 25.f;
    glm::mat4 shadowOrthoProjMat = glm::ortho(-valueOrtho, valueOrtho, -valueOrtho, valueOrtho, -valueOrtho, valueOrtho);
    glm::mat4 cameraOrthoProjMat = glm::ortho(-window_width / 250.f, window_width / 250.f, -window_height / 250.f, window_height / 250.f, 0.1f, 100.f);

    bool Z = false;
    bool Q = false;
    bool S = false;
    bool D = false;

    //--------------------------------SHADOW---------------------

    ShadowCubeMapFBO shadowCubeMap;
    ShadowProgram    shadowProg;
    shadowCubeMap.Init(shadow_size);

    glEnable(GL_POLYGON_OFFSET_FILL);
    float factor = 1.f;
    float units  = 1.f;
    glPolygonOffset(factor, units);

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;

    // END OF MY INIT CODE//

    //--------------------------------LOOP---------------------

    /* Loop until the user closes the window */
    ctx.update = [&]() {
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

        // BEGIN OF MY DRAW CODE//

        //--------------------------------SHADOW MAP PASS---------------------

        shadowProg.m_Program.use();

        // POSITION OF LIGHT
        glm::vec3 uLightDir = glm::vec3(glm::rotate(ViewMatrixCamera.getViewMatrix(), ctx.time(), glm::vec3(0, 1, 0)) * glm::vec4(Light, 0));

        glm::vec3 uLightPos = glm::vec3(glm::rotate(glm::mat4(1.f), ctx.time(), glm::vec3(0, 1, 0)) * glm::vec4(Light, 1));

        glm::vec3 uMVLightPos = glm::vec3(ViewMatrixCamera.getViewMatrix() * glm::vec4(uLightPos, 1));

        std::vector<glm::mat4> shadowTransforms;

        // for (size_t i = 0; i < 6; i++)
        // {
        //     ViewMatrixLight.setPos(uLightPos);
        //     ViewMatrixLight.setTheta(CameraDirections[i].theta);
        //     ViewMatrixLight.setPhi(CameraDirections[i].phi);
        //     shadowTransforms.push_back(ProjMatrixLight * ViewMatrixLight.getViewMatrix());
        // }

        // loop on the cube faces (logically 6)
        for (size_t i = 0; i < 6; i++)
        {
            shadowCubeMap.BindForWriting(CameraDirections[i].CubemapFace);

            glClear(GL_DEPTH_BUFFER_BIT);

            ViewMatrixLight.setPos(uLightPos);
            ViewMatrixLight.setTheta(CameraDirections[i].theta);
            ViewMatrixLight.setPhi(CameraDirections[i].phi);

            // send vao to the objects
            glBindVertexArray(vao);

            {
                glm::mat4 MMatrix = glm::rotate(glm::mat4(1.f), -ctx.time(), glm::vec3(0, 1, 0));

                glUniform1f(shadowProg.ufar_plane, far_plane);
                glUniform3fv(shadowProg.uLightPos, 1, glm::value_ptr(uLightPos));
                glUniformMatrix4fv(shadowProg.uMMatrix, 1, GL_FALSE, glm::value_ptr(MMatrix));
                glUniformMatrix4fv(shadowProg.uMVPLight, 1, GL_FALSE, glm::value_ptr(ProjMatrixLight * ViewMatrixLight.getViewMatrix() * MMatrix));
            }

            earth.render();

            for (int i = 0; i < moon.MOON_NB; i++)
            {
                glm::mat4 MMatrix_moon = glm::rotate(glm::mat4(1.f), ctx.time(), glm::vec3(moon.RotDir[i][0], moon.RotDir[i][1], moon.RotDir[i][2]));
                MMatrix_moon           = glm::translate(MMatrix_moon, moon.RotAxes[i]);
                MMatrix_moon           = glm::scale(MMatrix_moon, glm::vec3(0.2, 0.2, 0.2));

                glUniform1f(shadowProg.ufar_plane, far_plane);
                glUniform3fv(shadowProg.uLightPos, 1, glm::value_ptr(uLightPos));
                glUniformMatrix4fv(shadowProg.uMMatrix, 1, GL_FALSE, glm::value_ptr(MMatrix_moon));
                glUniformMatrix4fv(shadowProg.uMVPLight, 1, GL_FALSE, glm::value_ptr(ProjMatrixLight * ViewMatrixLight.getViewMatrix() * MMatrix_moon));

                moon.render();
            }

            glBindVertexArray(0);

            glBindVertexArray(vaoF);

            {
                glm::mat4 MMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0, -3.f, 0));
                MMatrix           = glm::rotate(MMatrix, glm::radians(90.f), glm::vec3(1, 0, 0));
                MMatrix           = glm::scale(MMatrix, glm::vec3(50.f, 50.f, 50.f));

                glUniform1f(shadowProg.ufar_plane, far_plane);
                glUniform3fv(shadowProg.uLightPos, 1, glm::value_ptr(uLightPos));
                glUniformMatrix4fv(shadowProg.uMMatrix, 1, GL_FALSE, glm::value_ptr(MMatrix));
                glUniformMatrix4fv(shadowProg.uMVPLight, 1, GL_FALSE, glm::value_ptr(ProjMatrixLight * ViewMatrixLight.getViewMatrix() * MMatrix));
            }

            floor.render();

            glBindVertexArray(0);
        }

        ctx.render_to_main_canvas();

        //--------------------------------LIGHTING PASS---------------------

        // TEMP
        ViewMatrixLight.setTheta(CameraDirections[(int)ctx.time() % 6].theta);
        ViewMatrixLight.setPhi(CameraDirections[(int)ctx.time() % 6].phi);
        // ViewMatrixLight.setTheta(CameraDirections[0].theta);
        // ViewMatrixLight.setPhi(CameraDirections[0].phi);
        // TEMP

        glViewport(0, 0, ctx.current_canvas_width(), ctx.current_canvas_height());

        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ViewMatrixLight.moveFront(glm::distance(Light, glm::vec3(0.f)));

        // send vao to the objects
        glBindVertexArray(vao);

        earth.m_Program.use();

        shadowCubeMap.BindForReading(GL_TEXTURE1);

        earth.uniformSender(ViewMatrixCamera.getViewMatrix(), ProjMatrix, uLightDir, uMVLightPos, -ctx.time());
        glUniform1f(earth.ufar_plane, far_plane);
        glUniform3fv(earth.uLightPos, 1, glm::value_ptr(uLightPos));

        earth.render();

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        moon.m_Program.use();

        {
            for (int i = 0; i < moon.MOON_NB; i++)
            {
                shadowCubeMap.BindForReading(GL_TEXTURE1);

                moon.uniformSender(i, ViewMatrixCamera.getViewMatrix(), ProjMatrix, uLightDir, uMVLightPos, ctx.time());
                glUniform1f(moon.ufar_plane, far_plane);
                glUniform3fv(moon.uLightPos, 1, glm::value_ptr(uLightPos));

                moon.render();

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            }
        }

        glBindVertexArray(0);

        glBindVertexArray(vaoF);

        floor.m_Program.use();

        shadowCubeMap.BindForReading(GL_TEXTURE1);

        floor.uniformSender(ViewMatrixCamera.getViewMatrix(), ProjMatrix, uLightDir, uMVLightPos);
        glUniform1f(floor.ufar_plane, far_plane);
        glUniform3fv(floor.uLightPos, 1, glm::value_ptr(uLightPos));

        floor.render();

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        glBindVertexArray(0);

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

    // Clear vbo & vao & texture
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return 0;
}
