#include <iostream>
#include <vector>
#include "glm/ext/scalar_constants.hpp"
#include "p6/p6.h"

int const window_width  = 1920;
int const window_height = 1080;

struct Vertex2DColor {
    glm::vec2 m_position;
    glm::vec3 m_color;

    Vertex2DColor()
        : m_position(glm::vec2()), m_color(glm::vec3()) {}
    Vertex2DColor(glm::vec2 position, glm::vec3 color)
        : m_position(position), m_color(color) {}
};

float Deg2Rad(float deg)
{
    return deg * (glm::pi<float>() / 180);
}

// DISC WITH IBO
int main()
{
    auto ctx = p6::Context{{window_width, window_height, "TP1"}};
    ctx.maximize_window();

    // BEGINNING OF MY INIT CODE//

    p6::Shader triangleShader = p6::load_shader("shaders/triangle.vs.glsl", "shaders/triangle.fs.glsl");

    // init ONE vao with info data
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // init ONE vbo with coord data
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    // array_buffer is for vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const int   DIVISION = 200;
    const float RAYON    = 0.5;

    // triangle coords
    std::vector<Vertex2DColor> vertices;
    vertices.emplace_back(glm::vec2(0, 0), glm::vec3(0, 0, 0));

    for (int i = 0; i < DIVISION; i++)
    {
        vertices.emplace_back(glm::vec2(RAYON * glm::cos(Deg2Rad((360 / static_cast<float>(DIVISION)) * (i))), RAYON * glm::sin(Deg2Rad((360 / static_cast<float>(DIVISION)) * (i)))), glm::vec3(1, 0, 0));
    }

    // fill those coords in the vbo / Static is for constant variables
    glBufferData(GL_ARRAY_BUFFER, (DIVISION + 1) * sizeof(Vertex2DColor), vertices.data(), GL_STATIC_DRAW);

    // init ONE vbo with coord data
    GLuint ibo = 0;
    glGenBuffers(1, &ibo);
    // array_buffer is for vbo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // triangle coords
    std::vector<uint32_t> indices;

    for (uint32_t i = 1; i < DIVISION; i++)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    indices.push_back(0);
    indices.push_back(static_cast<uint32_t>(DIVISION));
    indices.push_back(1);

    // fill those coords in the vbo / Static is for constant variables
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (static_cast<unsigned>(3 * DIVISION)) * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    // enable the INDEX attribut we want / POSITION is index 0 by default
    const GLuint VERTEX_ATTR_POSITION = 3;
    const GLuint VERTEX_ATTR_COLOR    = 8;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_COLOR);

    // rebind the vbo to specify vao attribute
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)(offsetof(Vertex2DColor, m_position)));
    glVertexAttribPointer(VERTEX_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)(offsetof(Vertex2DColor, m_color)));
    // debind the vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // debind the vao
    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // END OF MY INIT CODE//

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;

    /* Loop until the user closes the window */
    ctx.update = [&]() {
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // BEGIN OF MY DRAW CODE//

        triangleShader.use();

        glBindVertexArray(vao);

        glDrawElements(GL_TRIANGLES, 3 * DIVISION, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        // END OF MY DRAW CODE//
    };

    ctx.start();

    // Clear vbo & vao (&ibo)
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return 0;
}
