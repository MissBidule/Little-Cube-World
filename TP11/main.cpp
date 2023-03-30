#include <iostream>
#include <vector>
#include "glimac/FreeflyCamera.hpp"
#include "glimac/SkinnedMesh.hpp"
#include "glimac/TrackballCamera.hpp"
#include "glimac/common.hpp"
#include "glimac/sphere_vertices.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "p6/p6.h"

int const window_width  = 1920;
int const window_height = 1080;
int const shadow_width  = 4096;
int const shadow_height = 4096;

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.f, 1.f, 1.f, 1.f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

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

struct ShadowProgram {
    p6::Shader m_Program;
    GLint      uMVPLight;

    ShadowProgram()
        : m_Program(p6::load_shader("shaders/shadow.vs.glsl", "shaders/shadow.fs.glsl"))
    {
        uMVPLight = glGetUniformLocation(m_Program.id(), "uMVPLight");
    }
};

struct EarthProgram {
    p6::Shader m_Program;

    std::vector<glimac::ShapeVertex> m_sphere;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;

    GLint uKa;
    GLint uKd;
    GLint uKs;
    GLint uShininess;

    GLint uLightDir_vs;
    GLint uLightPos_vs;
    GLint uLightIntensity;
    GLint uMVPLight;
    GLint uTexture;

    EarthProgram()
        : m_Program(p6::load_shader("shaders/3D.vs.glsl", "shaders/dirPoslight.fs.glsl"))
    {
        uMVPLight       = glGetUniformLocation(m_Program.id(), "uMVPLight");
        uMVPMatrix      = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix       = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix   = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uKa             = glGetUniformLocation(m_Program.id(), "uKa");
        uKd             = glGetUniformLocation(m_Program.id(), "uKd");
        uKs             = glGetUniformLocation(m_Program.id(), "uKs");
        uShininess      = glGetUniformLocation(m_Program.id(), "uShininess");
        uLightDir_vs    = glGetUniformLocation(m_Program.id(), "uLightDir_vs");
        uLightPos_vs    = glGetUniformLocation(m_Program.id(), "uLightPos_vs");
        uLightIntensity = glGetUniformLocation(m_Program.id(), "uLightIntensity");
        uTexture        = glGetUniformLocation(m_Program.id(), "uShadowMap");
    }

    void uniformSender(glm::mat4 camera, glm::mat4 ProjMatrix, glm::vec3 LightDir, glm::vec3 LightPos, float Rotation)
    {
        // Terre
        glm::mat4 MVMatrix     = camera;
        MVMatrix               = glm::rotate(MVMatrix, Rotation, glm::vec3(0, 1, 0));
        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        glUniform3fv(uKa, 1, glm::value_ptr(glm::vec3(0.0215, 0.1745, 0.0215)));
        glUniform3fv(uKd, 1, glm::value_ptr(glm::vec3(0.07568, 0.61424, 0.07568)));
        glUniform3fv(uKs, 1, glm::value_ptr(glm::vec3(0.633, 0.727811, 0.633)));
        glUniform1f(uShininess, 0.6);

        glUniform3fv(uLightDir_vs, 1, glm::value_ptr(LightDir));
        glUniform3fv(uLightPos_vs, 1, glm::value_ptr(LightPos));
        glUniform3fv(uLightIntensity, 1, glm::value_ptr(glm::vec3(.8f, .8f, .8f)));

        glUniform1i(uTexture, 0);
    }

    void render()
    {
        glDrawArrays(GL_TRIANGLES, 0, m_sphere.size());
    }
};

struct FloorProgram {
    p6::Shader m_Program;

    std::vector<glimac::ShapeVertex> m_plane;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;

    GLint uKa;
    GLint uKd;
    GLint uKs;
    GLint uShininess;

    GLint uLightDir_vs;
    GLint uLightPos_vs;
    GLint uLightIntensity;
    GLint uMVPLight;
    GLint uTexture;
    GLint uTexture2;

    FloorProgram()
        : m_Program(p6::load_shader("shaders/3D.vs.glsl", "shaders/dirPoslightTex.fs.glsl"))
    {
        uMVPLight       = glGetUniformLocation(m_Program.id(), "uMVPLight");
        uMVPMatrix      = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix       = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix   = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uKa             = glGetUniformLocation(m_Program.id(), "uKa");
        uKd             = glGetUniformLocation(m_Program.id(), "uKd");
        uKs             = glGetUniformLocation(m_Program.id(), "uKs");
        uShininess      = glGetUniformLocation(m_Program.id(), "uShininess");
        uLightDir_vs    = glGetUniformLocation(m_Program.id(), "uLightDir_vs");
        uLightPos_vs    = glGetUniformLocation(m_Program.id(), "uLightPos_vs");
        uLightIntensity = glGetUniformLocation(m_Program.id(), "uLightIntensity");
        uTexture        = glGetUniformLocation(m_Program.id(), "uShadowMap");
        uTexture2       = glGetUniformLocation(m_Program.id(), "uTexture2");
    }

    void uniformSender(glm::mat4 camera, glm::mat4 ProjMatrix, glm::vec3 LightDir, glm::vec3 LightPos)
    {
        // Floor
        glm::mat4 MVMatrix     = camera;
        MVMatrix               = glm::translate(MVMatrix, glm::vec3(0, -3.f, 0));
        MVMatrix               = glm::rotate(MVMatrix, glm::radians(90.f), glm::vec3(1, 0, 0));
        MVMatrix               = glm::scale(MVMatrix, glm::vec3(8.f, 8.f, 8.f));
        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        glUniform3fv(uKa, 1, glm::value_ptr(glm::vec3(0.0215, 0.1745, 0.0215)));
        glUniform3fv(uKd, 1, glm::value_ptr(glm::vec3(0.07568, 0.61424, 0.07568)));
        glUniform3fv(uKs, 1, glm::value_ptr(glm::vec3(0.633, 0.727811, 0.633)));
        glUniform1f(uShininess, 0.6);

        glUniform3fv(uLightDir_vs, 1, glm::value_ptr(LightDir));
        glUniform3fv(uLightPos_vs, 1, glm::value_ptr(LightPos));
        glUniform3fv(uLightIntensity, 1, glm::value_ptr(glm::vec3(.8f, .8f, .8f)));

        glUniform1i(uTexture, 0);
        glUniform1i(uTexture2, 1);
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

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;

    GLint uKa;
    GLint uKd;
    GLint uKs;
    GLint uShininess;

    GLint uLightDir_vs;
    GLint uLightPos_vs;
    GLint uLightIntensity;
    GLint uMVPLight;
    GLint uTexture;

    std::vector<glm::vec3> RotAxes;
    std::vector<glm::vec3> RotDir;
    std::vector<glm::vec3> Ka;
    std::vector<glm::vec3> Kd;
    std::vector<glm::vec3> Ks;
    std::vector<float>     Shininess;

    MoonProgram()
        : m_Program(p6::load_shader("shaders/3D.vs.glsl", "shaders/dirPoslight.fs.glsl"))
    {
        uMVPLight       = glGetUniformLocation(m_Program.id(), "uMVPLight");
        uMVPMatrix      = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix       = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix   = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uKa             = glGetUniformLocation(m_Program.id(), "uKa");
        uKd             = glGetUniformLocation(m_Program.id(), "uKd");
        uKs             = glGetUniformLocation(m_Program.id(), "uKs");
        uShininess      = glGetUniformLocation(m_Program.id(), "uShininess");
        uLightDir_vs    = glGetUniformLocation(m_Program.id(), "uLightDir_vs");
        uLightPos_vs    = glGetUniformLocation(m_Program.id(), "uLightPos_vs");
        uLightIntensity = glGetUniformLocation(m_Program.id(), "uLightIntensity");
        uTexture        = glGetUniformLocation(m_Program.id(), "uShadowMap");

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
        glm::mat4 MVMatrix_moon     = camera;
        MVMatrix_moon               = glm::rotate(MVMatrix_moon, Rotation, glm::vec3(RotDir[i][0], RotDir[i][1], RotDir[i][2]));
        MVMatrix_moon               = glm::translate(MVMatrix_moon, RotAxes[i]);
        MVMatrix_moon               = glm::scale(MVMatrix_moon, glm::vec3(0.2, 0.2, 0.2));
        glm::mat4 NormalMatrix_moon = glm::transpose(glm::inverse(MVMatrix_moon));

        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix_moon));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix_moon));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix_moon));

        glUniform3fv(uKa, 1, glm::value_ptr(Ka[i]));
        glUniform3fv(uKd, 1, glm::value_ptr(Kd[i]));
        glUniform3fv(uKs, 1, glm::value_ptr(Ks[i]));
        glUniform1f(uShininess, Shininess[i]);

        glUniform3fv(uLightDir_vs, 1, glm::value_ptr(LightDir));
        glUniform3fv(uLightPos_vs, 1, glm::value_ptr(LightPos));
        glUniform3fv(uLightIntensity, 1, glm::value_ptr(glm::vec3(.8f, .8f, .8f)));

        glUniform1i(uTexture, 0);
    }

    void render()
    {
        glDrawArrays(GL_TRIANGLES, 0, m_sphere.size());
    }
};

int main()
{
    auto ctx = p6::Context{{window_width, window_height, "TP11"}};
    ctx.maximize_window();

    // BEGINNING OF MY INIT CODE//

    img::Image floorTex = p6::load_image_buffer("assets/textures/test.jpg");

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

    // init ONE texture
    GLuint uvtex = 0;
    glGenTextures(1, &uvtex);
    // texture_2D is for simple 2D
    glBindTexture(GL_TEXTURE_2D, uvtex);

    // specify the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, floorTex.width(), floorTex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, floorTex.data());
    // needed for pixel overlapping (I guess)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // debind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    //--------------------------------MVP---------------------

    // MVP
    glm::vec3       Light(0, 2, 2);
    FreeflyCamera   ViewMatrixCamera = FreeflyCamera();
    TrackballCamera ViewMatrixLight  = TrackballCamera();
    ViewMatrixCamera.moveFront(-5);
    // Only for point light
    // ViewMatrixLight.moveFront(-glm::distance(Light, glm::vec3(0.f)));
    ViewMatrixLight.rotateUp(glm::degrees(glm::orientedAngle(glm::normalize(Light), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0))));
    ViewMatrixLight.rotateLeft(-45);
    glm::mat4 ProjMatrix         = glm::perspective(glm::radians(70.f), window_width / static_cast<float>(window_height), 0.1f, 100.f);
    glm::mat4 ProjMatrixLight    = glm::perspective(glm::radians(70.f), shadow_width / static_cast<float>(shadow_height), 0.1f, 25.f);
    float     valueOrtho         = 35.f;
    glm::mat4 shadowOrthoProjMat = glm::ortho(-valueOrtho, valueOrtho, -valueOrtho, valueOrtho, -valueOrtho, valueOrtho);
    glm::mat4 cameraOrthoProjMat = glm::ortho(-window_width / 250.f, window_width / 250.f, -window_height / 250.f, window_height / 250.f, 0.1f, 100.f);

    bool Z = false;
    bool Q = false;
    bool S = false;
    bool D = false;

    //--------------------------------SHADOW---------------------

    ShadowMapFBO  shadowMap;
    ShadowProgram shadowProg;
    shadowMap.Init(shadow_width, shadow_height);

    glEnable(GL_POLYGON_OFFSET_FILL);
    float factor = 1.f;
    float units  = 1.f;
    glPolygonOffset(factor, units);

    // glEnable(GL_CULL_FACE);

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;

    //--------------------------------Trying to load a mesh---------------------

    SkinnedMesh Thingy;
    Thingy.LoadMesh("assets/models/test.fbx");
    int DisplayBoneIndex = 0;

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

        // POSITION OF LIGHT
        glm::vec3 uLightDir = glm::vec3(glm::rotate(ViewMatrixCamera.getViewMatrix(), ctx.time(), glm::vec3(0, 1, 0)) * glm::vec4(Light, 0));
        glm::vec3 uLightPos = glm::vec3(glm::rotate(ViewMatrixCamera.getViewMatrix(), ctx.time(), glm::vec3(0, 1, 0)) * glm::vec4(Light, 1));

        ViewMatrixLight.rotateLeft(-glm::degrees(ctx.delta_time()));

        // BEGIN OF MY DRAW CODE//

        //--------------------------------SHADOW MAP PASS---------------------

        // glCullFace(GL_FRONT);

        shadowMap.BindForWriting();

        glClear(GL_DEPTH_BUFFER_BIT);

        shadowProg.m_Program.use();

        // send vao to the objects
        glBindVertexArray(vao);

        {
            glm::mat4 MVMatrix = ViewMatrixLight.getViewMatrix();
            MVMatrix           = glm::rotate(MVMatrix, -ctx.time(), glm::vec3(0, 1, 0));
            glUniformMatrix4fv(shadowProg.uMVPLight, 1, GL_FALSE, glm::value_ptr(shadowOrthoProjMat * MVMatrix));
        }

        earth.render();

        for (int i = 0; i < moon.MOON_NB; i++)
        {
            glm::mat4 MVMatrix_moon = ViewMatrixLight.getViewMatrix();
            MVMatrix_moon           = glm::rotate(MVMatrix_moon, ctx.time(), glm::vec3(moon.RotDir[i][0], moon.RotDir[i][1], moon.RotDir[i][2]));
            MVMatrix_moon           = glm::translate(MVMatrix_moon, moon.RotAxes[i]);
            MVMatrix_moon           = glm::scale(MVMatrix_moon, glm::vec3(0.2, 0.2, 0.2));
            glUniformMatrix4fv(shadowProg.uMVPLight, 1, GL_FALSE, glm::value_ptr(shadowOrthoProjMat * MVMatrix_moon));

            moon.render();
        }

        glBindVertexArray(0);

        {
            glm::mat4 MVMatrix = ViewMatrixLight.getViewMatrix();
            MVMatrix           = glm::translate(MVMatrix, glm::vec3(3.f, -3.f, 0.f));
            // BCS IT'S AN FBX OBJECT
            MVMatrix = glm::rotate(MVMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
            glUniformMatrix4fv(shadowProg.uMVPLight, 1, GL_FALSE, glm::value_ptr(shadowOrthoProjMat * MVMatrix));
        }

        Thingy.Render();

        glBindVertexArray(vaoF);

        {
            glm::mat4 MVMatrix = ViewMatrixLight.getViewMatrix();
            MVMatrix           = glm::translate(MVMatrix, glm::vec3(0, -3.f, 0));
            MVMatrix           = glm::rotate(MVMatrix, glm::radians(90.f), glm::vec3(1, 0, 0));
            MVMatrix           = glm::scale(MVMatrix, glm::vec3(8.f, 8.f, 8.f));

            glUniformMatrix4fv(shadowProg.uMVPLight, 1, GL_FALSE, glm::value_ptr(shadowOrthoProjMat * MVMatrix));
        }

        floor.render();

        glBindVertexArray(0);

        //--------------------------------LIGHTING PASS---------------------

        ctx.render_to_main_canvas();

        // glCullFace(GL_BACK);

        glViewport(0, 0, ctx.current_canvas_width(), ctx.current_canvas_height());

        glClearColor(0.2f, 0.2f, 0.2f, 1.f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ViewMatrixLight.moveFront(glm::distance(Light, glm::vec3(0.f)));

        // send vao to the objects
        glBindVertexArray(vao);

        earth.m_Program.use();

        shadowMap.BindForReading(GL_TEXTURE0);

        {
            glm::mat4 MVMatrix = ViewMatrixLight.getViewMatrix();
            MVMatrix           = glm::rotate(MVMatrix, -ctx.time(), glm::vec3(0, 1, 0));
            glUniformMatrix4fv(earth.uMVPLight, 1, GL_FALSE, glm::value_ptr(shadowOrthoProjMat * MVMatrix));
        }

        earth.uniformSender(ViewMatrixCamera.getViewMatrix(), ProjMatrix, uLightDir, uLightPos, -ctx.time());
        earth.render();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        moon.m_Program.use();

        {
            for (int i = 0; i < moon.MOON_NB; i++)
            {
                shadowMap.BindForReading(GL_TEXTURE0);

                glm::mat4 MVMatrix_moon = ViewMatrixLight.getViewMatrix();
                MVMatrix_moon           = glm::rotate(MVMatrix_moon, ctx.time(), glm::vec3(moon.RotDir[i][0], moon.RotDir[i][1], moon.RotDir[i][2]));
                MVMatrix_moon           = glm::translate(MVMatrix_moon, moon.RotAxes[i]);
                MVMatrix_moon           = glm::scale(MVMatrix_moon, glm::vec3(0.2, 0.2, 0.2));
                glUniformMatrix4fv(earth.uMVPLight, 1, GL_FALSE, glm::value_ptr(shadowOrthoProjMat * MVMatrix_moon));

                moon.uniformSender(i, ViewMatrixCamera.getViewMatrix(), ProjMatrix, uLightDir, uLightPos, ctx.time());
                moon.render();

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }

        glBindVertexArray(0);

        Thingy.MeshProgramUse();

        shadowMap.BindForReading(GL_TEXTURE0);

        Thingy.UpdateDebug(DisplayBoneIndex);

        Thingy.Render(ViewMatrixCamera.getViewMatrix(), ProjMatrix, uLightDir, uLightPos, ViewMatrixLight.getViewMatrix(), shadowOrthoProjMat, ctx.time());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(vaoF);

        floor.m_Program.use();

        shadowMap.BindForReading(GL_TEXTURE0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, uvtex);

        {
            glm::mat4 MVMatrix = ViewMatrixLight.getViewMatrix();
            MVMatrix           = glm::translate(MVMatrix, glm::vec3(0, -3.f, 0));
            MVMatrix           = glm::rotate(MVMatrix, glm::radians(90.f), glm::vec3(1, 0, 0));
            MVMatrix           = glm::scale(MVMatrix, glm::vec3(8.f, 8.f, 8.f));

            glUniformMatrix4fv(floor.uMVPLight, 1, GL_FALSE, glm::value_ptr(shadowOrthoProjMat * MVMatrix));
        }

        floor.uniformSender(ViewMatrixCamera.getViewMatrix(), ProjMatrix, uLightDir, uLightPos);
        floor.render();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(0);

        // END OF MY DRAW CODE//
    };

    //--------------------------------INPUTS---------------------

    ctx.key_pressed = [&Z, &Q, &S, &D, &DisplayBoneIndex, &Thingy](const p6::Key& key) {
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

        // FOR SKINNING DEBUG
        if (key.physical == GLFW_KEY_SPACE)
        {
            DisplayBoneIndex = ++DisplayBoneIndex % Thingy.NumBones();
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
    glDeleteVertexArrays(1, &vaoF);

    return 0;
}
