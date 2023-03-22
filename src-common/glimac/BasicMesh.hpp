#ifndef BASIC_MESH_HPP
#define BASIC_MESH_HPP

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <string>
#include <vector>
#include "assimp/mesh.h"
#include "img/src/Image.h"
#include "p6/p6.h"

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a) / sizeof(a[0]))

#define GLCheckError() (glGetError() == GL_NO_ERROR)

class BasicMesh {
public:
    BasicMesh();

    ~BasicMesh();

    bool LoadMesh(const std::string& Filename);

    void Render();

    void Render(glm::mat4 camera, glm::mat4 ProjMatrix, glm::vec3 LightDir, glm::vec3 LightPos, glm::mat4 LightCamera, glm::mat4 LightProjMatrix);

    inline void MeshProgramUse()
    {
        m_Program.use();
    }

    // void Render(unsigned int NumInstances, const glm::mat4* MVPMats, const glm::mat4* ModeleMats);

    // inline glm::mat4& GetModelTransform() { return m_modelTransform; }

private:
    void Clear();

    bool InitFromScene(const aiScene* pScene);

    void CountVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices);

    void ReserveSpace(unsigned int NumVertices, unsigned int NumIndices);

    void InitAllMeshes(const aiScene* pScene);

    void InitSingleMesh(const aiMesh* paiMesh);

    bool InitMaterials(const aiScene* pScene);

    void PopulateBuffers();

#define INVALID_MATERIAL 0xFFFFFFFF

    enum BUFFER_TYPE {
        INDEX_BUFFER = 0,
        POS_VB       = 1,
        TEXCOORD_VB  = 2,
        NORMAL_VB    = 3,
        MVP_MAT_VB   = 4,
        MODEL_MAT_VB = 5,
        NUM_BUFFERS  = 6
    };

    // glm::mat4 m_modelTransform;
    GLuint m_VAO                  = 0;
    GLuint m_buffers[NUM_BUFFERS] = {0};

    struct BasicMeshEntry {
        BasicMeshEntry()
        {
            NumIndices    = 0;
            BaseVertex    = 0;
            BaseIndex     = 0;
            MaterialIndex = INVALID_MATERIAL;
        }

        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
        unsigned int MaterialIndex;
    };

    struct Texture {
        std::string m_filename;
        GLenum      m_textureTarget;
        GLuint      m_textureObj = 0;

        Texture(GLenum TextureTarget, const std::string& Filename)
        {
            m_textureTarget = TextureTarget;
            m_filename      = Filename;
        }

        bool Load()
        {
            img::Image image_data = p6::load_image_buffer(m_filename);

            glGenTextures(1, &m_textureObj);
            glBindTexture(m_textureTarget, m_textureObj);
            if (m_textureTarget == GL_TEXTURE_2D)
            {
                // specify the texture
                glTexImage2D(m_textureTarget, 0, GL_RGBA, image_data.width(), image_data.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data.data());
            }
            else
            {
                std::cerr << "Support for texture target " << m_textureTarget << " is not implemented" << std::endl;
            }

            // needed for pixel overlapping (I guess)
            glTexParameteri(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindTexture(m_textureTarget, 0);

            return true;
        }

        void Bind(GLenum TextureUnit)
        {
            glActiveTexture(TextureUnit);
            glBindTexture(m_textureTarget, m_textureObj);
        }
    };

    std::vector<BasicMeshEntry> m_Meshes;
    std::vector<glm::vec3>      m_Ka;
    std::vector<glm::vec3>      m_Kd;
    std::vector<glm::vec3>      m_Ks;
    std::vector<float>          m_Ni;
    std::vector<float>          m_D;

    std::vector<glm::vec3>    m_Positions;
    std::vector<glm::vec3>    m_Normals;
    std::vector<glm::vec2>    m_TexCoords;
    std::vector<unsigned int> m_Indices;

    p6::Shader m_Program;

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
};

#endif