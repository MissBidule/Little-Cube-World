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
    auto ctx = p6::Context{{window_width, window_height, "TP3"}};
    ctx.maximize_window();

    // BEGINNING OF MY INIT CODE//

    p6::Shader Shader = p6::load_shader("shaders/mandelbrot.vs.glsl", "shaders/mandelbrot.fs.glsl");

    // init ONE vbo with coord data
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    // array_buffer is for vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // triangle coords
    std::vector<Vertex2DColor> vertices = {
        Vertex2DColor(glm::vec2(-1, 1), glm::vec3(1, 1, 1)),  // 0
        Vertex2DColor(glm::vec2(1, -1), glm::vec3(1, 1, 1)),  // 1
        Vertex2DColor(glm::vec2(-1, -1), glm::vec3(1, 1, 1)), // 2
        Vertex2DColor(glm::vec2(1, 1), glm::vec3(1, 1, 1))    // 3
    };

    // fill those coords in the vbo / Static is for constant variables
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex2DColor), vertices.data(), GL_STATIC_DRAW);

    // init ONE vao with info data
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // init ONE vao with vertices data
    GLuint ibo = 0;
    glGenBuffers(1, &ibo);
    // element_array is for ibo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // vbo vertices for the triangles
    std::vector<uint32_t> indices = {
        0, 1, 2,
        0, 1, 3};

    // fill those indexes in the ibo
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    // enable the INDEX attribut we want / POSITION is index 0 by default
    const GLuint VERTEX_ATTR_POSITION = 0;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);

    // rebind the vbo to specify vao attribute
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)(offsetof(Vertex2DColor, m_position)));

    // debind the vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // debind the vao
    glBindVertexArray(0);

    // debind the ibo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // END OF MY INIT CODE//

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;

    /* Loop until the user closes the window */
    ctx.update = [&]() {
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // BEGIN OF MY DRAW CODE//

        Shader.use();

        glBindVertexArray(vao);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);

        // END OF MY DRAW CODE//
    };

    ctx.start();

    // Clear vbo & vao & ibo
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return 0;
}
