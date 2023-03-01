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

int main()
{
    auto ctx = p6::Context{{window_width, window_height, "TP2"}};
    ctx.maximize_window();

    // BEGINNING OF MY INIT CODE//

    p6::Shader Shader = p6::load_shader("shaders/halo2D.vs.glsl", "shaders/textProc2D.fs.glsl");

    // init ONE vao with info data
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // init ONE vbo with coord data
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    // array_buffer is for vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // triangle coords
    std::vector<Vertex2DColor> vertices = {
        Vertex2DColor(glm::vec2(-0.5, -0.5), glm::vec3(1, 0.75, 0)),
        Vertex2DColor(glm::vec2(0.5, -0.5), glm::vec3(0.75, 0, 0.5)),
        Vertex2DColor(glm::vec2(0, 0.5), glm::vec3(0, 0, 0.75))};

    // fill those coords in the vbo / Static is for constant variables
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex2DColor), &vertices[0], GL_STATIC_DRAW);

    // enable the INDEX attribut we want / POSITION is index 0 by default
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_COLOR    = 1;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_COLOR);

    // rebind the vbo to specify vao attribute
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)(offsetof(Vertex2DColor, m_position)));
    glVertexAttribPointer(VERTEX_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)(offsetof(Vertex2DColor, m_color)));
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

        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        // END OF MY DRAW CODE//
    };

    ctx.start();

    // Clear vbo & vao
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return 0;
}
