#include <iostream>
#include <vector>
#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "img/src/Image.h"
#include "p6/p6.h"

int const window_width  = 1920;
int const window_height = 1080;

struct Vertex2DUV {
    glm::vec2 m_position;
    glm::vec2 m_UV;

    Vertex2DUV()
        : m_position(glm::vec2()), m_UV(glm::vec2()) {}
    Vertex2DUV(glm::vec2 position, glm::vec2 UV)
        : m_position(position), m_UV(UV) {}
};

struct TriforceTransform {
    float m_tx;
    float m_ty;
    float m_scale;
    float m_angleFactor;

    TriforceTransform()
        : m_tx(0), m_ty(0), m_scale(1), m_angleFactor(0) {}
    TriforceTransform(float tx, float ty, float scale, float angleFactor)
        : m_tx(tx), m_ty(ty), m_scale(scale), m_angleFactor(angleFactor) {}
};

glm::mat3 translate(float tx, float ty)
{
    glm::mat3 translation;

    translation[0] = glm::vec3(1., 0., 0.);
    translation[1] = glm::vec3(0., 1., 0.);
    translation[2] = glm::vec3(tx, ty, 1.);

    return translation;
}

glm::mat3 scale(float sx, float sy)
{
    glm::mat3 scale;

    scale[0] = glm::vec3(sx, 0., 0.);
    scale[1] = glm::vec3(0., sy, 0.);
    scale[2] = glm::vec3(0., 0., 1.);

    return scale;
}

glm::mat3 rotate(float a)
{
    glm::mat3 rotation;
    float     angle = glm::radians(a);

    rotation[0] = glm::vec3(cos(angle), sin(angle), 0.);
    rotation[1] = glm::vec3(-sin(angle), cos(angle), 0.);
    rotation[2] = glm::vec3(0., 0., 1.);

    return rotation;
}

int main()
{
    auto ctx = p6::Context{{window_width, window_height, "TP4"}};
    ctx.maximize_window();

    // BEGINNING OF MY INIT CODE//

    p6::Shader Shader   = p6::load_shader("shaders/tex2D.vs.glsl", "shaders/tex2D.fs.glsl");
    img::Image triforce = p6::load_image_buffer("assets/textures/triforce.png");

    GLint                          uModelMatrix = glGetUniformLocation(Shader.id(), "uModelMatrix");
    float                          angle        = 0;
    std::vector<TriforceTransform> ModelMatrix;

    GLint uTexture = glGetUniformLocation(Shader.id(), "uTexture");

    ModelMatrix.emplace_back(0.5, 0.5, 0.15, 2);
    ModelMatrix.emplace_back(0.5, -0.5, 0.15, -2);
    ModelMatrix.emplace_back(-0.5, -0.5, 0.15, 2);
    ModelMatrix.emplace_back(-0.5, 0.5, 0.15, -2);

    // init ONE vbo with coord data
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    // array_buffer is for vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // triangle coords
    std::vector<Vertex2DUV> vertices = {
        Vertex2DUV(glm::vec2(-1, -1), glm::vec2(0, 0)),  // 0
        Vertex2DUV(glm::vec2(1, -1), glm::vec2(1, 0)),   // 1
        Vertex2DUV(glm::vec2(0., 1), glm::vec2(0.5, 1)), // 2
    };

    // fill those coords in the vbo / Static is for constant variables
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex2DUV), vertices.data(), GL_STATIC_DRAW);

    // init ONE texture
    GLuint uvtex = 0;
    glGenTextures(1, &uvtex);
    // texture_2D is for simple 2D
    glBindTexture(GL_TEXTURE_2D, uvtex);

    // a little transparency...
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // specify the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, triforce.width(), triforce.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, triforce.data());
    // needed for pixel overlapping (I guess)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // debind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // init ONE vao with info data
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // enable the INDEX attribut we want / POSITION is index 0 by default
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_UV       = 1;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_UV);

    // rebind the vbo to specify vao attribute
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), (const GLvoid*)(offsetof(Vertex2DUV, m_position)));
    glVertexAttribPointer(VERTEX_ATTR_UV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), (const GLvoid*)(offsetof(Vertex2DUV, m_UV)));

    // debind the vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // debind the vao
    glBindVertexArray(0);

    // END OF MY INIT CODE//

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;

    /* Loop until the user closes the window */
    ctx.update = [&]() {
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // BEGIN OF MY DRAW CODE//

        Shader.use();

        glBindTexture(GL_TEXTURE_2D, uvtex);
        glUniform1i(uTexture, 0);
        glBindVertexArray(vao);

        for (int i = 0; i < 4; i++)
        {
            glUniformMatrix3fv(uModelMatrix, 1, GL_FALSE, glm::value_ptr(rotate(angle) * translate(ModelMatrix[i].m_tx, ModelMatrix[i].m_ty) * scale(ModelMatrix[i].m_scale, ModelMatrix[i].m_scale) * rotate(ModelMatrix[i].m_angleFactor * angle)));
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        angle++;

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // END OF MY DRAW CODE//
    };

    ctx.start();

    // Clear vbo & vao & texture
    glDeleteTextures(1, &uvtex);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return 0;
}
