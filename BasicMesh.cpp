#include "SkinnedMesh.hpp"
#include "assimp/material.h"
#include "assimp/vector3.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/type_ptr.hpp"

#define POSITION_LOCATION  0
#define NORMAL_LOCATION    1
#define TEX_COORD_LOCATION 2

BasicMesh::BasicMesh()
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
};

BasicMesh::~BasicMesh()
{
    Clear();
}

void BasicMesh::Clear()
{
    // for (auto& i : m_Textures)
    // {
    //     glDeleteTextures(1, &i->m_textureObj);
    // }

    if (m_buffers[0] != 0)
    {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_buffers), m_buffers);
    }

    if (m_VAO != 0)
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}

bool BasicMesh::LoadMesh(const std::string& Filename)
{
    // Release the previously loaded mesh (if it exists)
    Clear();

    // Create the VAO
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Create the buffers for the vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_buffers), m_buffers);

    bool             Ret = false;
    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(Filename.c_str(), ASSIMP_LOAD_FLAGS);

    if (pScene)
    {
        Ret = InitFromScene(pScene);
    }
    else
    {
        std::cerr << "Error parsing " << Filename.c_str() << " : " << Importer.GetErrorString() << std::endl;
    }

    // Make sure the VAO is not changed from the outside
    glad_glBindVertexArray(0);

    return Ret;
}

bool BasicMesh::InitFromScene(const aiScene* pScene)
{
    m_Meshes.resize(pScene->mNumMeshes);
    m_Ka.resize(pScene->mNumMaterials);
    m_Kd.resize(pScene->mNumMaterials);
    m_Ks.resize(pScene->mNumMaterials);
    m_Ni.resize(pScene->mNumMaterials);
    m_D.resize(pScene->mNumMaterials);

    unsigned int NumVertices = 0;
    unsigned int NumIndices  = 0;

    CountVerticesAndIndices(pScene, NumVertices, NumIndices);

    ReserveSpace(NumVertices, NumIndices);

    InitAllMeshes(pScene);

    if (!InitMaterials(pScene))
    {
        return false;
    }

    PopulateBuffers();

    return GLCheckError();
}

void BasicMesh::CountVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices)
{
    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        m_Meshes[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
        m_Meshes[i].NumIndices    = pScene->mMeshes[i]->mNumFaces * 3;
        m_Meshes[i].BaseVertex    = NumVertices;
        m_Meshes[i].BaseIndex     = NumIndices;

        NumVertices += pScene->mMeshes[i]->mNumVertices;
        NumIndices += m_Meshes[i].NumIndices;
    }
}

void BasicMesh::ReserveSpace(unsigned int NumVertices, unsigned int NumIndices)
{
    m_Positions.reserve(NumVertices);
    m_Normals.reserve(NumVertices);
    m_TexCoords.reserve(NumVertices);
    m_Indices.reserve(NumIndices);
}

void BasicMesh::InitAllMeshes(const aiScene* pScene)
{
    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitSingleMesh(paiMesh);
    }
}

void BasicMesh::InitSingleMesh(const aiMesh* paiMesh)
{
    const aiVector3D Zero3D(0.f, 0.f, 0.f);

    // Populate the vertex attribute vectors
    for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
    {
        const aiVector3D& pPos      = paiMesh->mVertices[i];
        const aiVector3D& pNormal   = paiMesh->mNormals[i];
        const aiVector3D& pTexCoord = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : Zero3D;

        m_Positions.emplace_back(pPos.x, pPos.y, pPos.z);
        m_Normals.emplace_back(pNormal.x, pNormal.y, pNormal.z);
        m_TexCoords.emplace_back(pTexCoord.x, pTexCoord.y);
    }

    // Populate the index buffer
    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
    {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        m_Indices.emplace_back(Face.mIndices[0]);
        m_Indices.emplace_back(Face.mIndices[1]);
        m_Indices.emplace_back(Face.mIndices[2]);
    }
}

bool BasicMesh::InitMaterials(const aiScene* pScene)
{
    // NOT USING FILENAME

    // Extract the directory part from the file name
    // std::string::size_type SlashIndex = Filename.find_last_of("/");
    // std::string            Dir;

    // if (SlashIndex == std::string::npos)
    // {
    //     Dir = ".";
    // }
    // else if (SlashIndex == 0)
    // {
    //     Dir = "/";
    // }
    // else
    // {
    //     Dir = Filename.substr(0, SlashIndex);
    // }

    bool Ret = true;

    // Initialize the materials
    // for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
    // {
    //     const aiMaterial* pMaterial = pScene->mMaterials[i];

    //     m_Textures[i] = NULL;

    //     if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    //     {
    //         aiString Path;

    //         if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path) == AI_SUCCESS)
    //         {
    //             std::string p(Path.data);

    //             if (p.substr(0, 2) == ".\\")
    //             {
    //                 p = p.substr(2, p.size() - 2);

    //                 std::string FullPath = Dir + "/" + p;

    //                 m_Textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

    //                 if (!m_Textures[i]->Load())
    //                 {
    //                     std::cerr << "Error loading texture " << FullPath.c_str() << std::endl;
    //                     delete m_Textures[i];
    //                     m_Textures[i] = NULL;
    //                     Ret           = false;
    //                 }
    //                 else
    //                 {
    //                     std::cout << "Loaded texture " << FullPath.c_str() << std::endl;
    //                 }
    //             }
    //         }
    //     }
    // }

    for (unsigned int i = 1; i < pScene->mNumMaterials; i++)
    {
        const aiMaterial* mtl = pScene->mMaterials[i];

        float shininess;
        if (AI_SUCCESS == aiGetMaterialFloat(mtl, AI_MATKEY_SHININESS, &shininess))
            m_Ni[i] = (shininess / 1000.f);

        aiColor4D ambient;
        if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
        {
            m_Ka[i] = glm::vec3(0.f, 0.f, 0.f);
            // m_Ka.back() = glm::normalize(m_Ka.back());
        }

        aiColor4D diffuse;
        if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
        {
            m_Kd[i] = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
            // m_Kd.back() = glm::normalize(m_Kd.back());
        }

        aiColor4D specular;
        if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
        {
            m_Ks[i] = glm::vec3(specular.r, specular.g, specular.b);
            // m_Ks[i] = glm::normalize(m_Ks[i]);
        }

        float opacity = 1.0f;
        if (AI_SUCCESS == aiGetMaterialFloat(mtl, AI_MATKEY_OPACITY, &opacity))
            m_D[i] = (opacity);
    }

    return Ret;
}

void BasicMesh::PopulateBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions[0]) * m_Positions.size(), m_Positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0]) * m_Normals.size(), m_Normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0]) * m_TexCoords.size(), m_TexCoords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), m_Indices.data(), GL_STATIC_DRAW);
}

void BasicMesh::Render()
{
    glBindVertexArray(m_VAO);

    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        glad_glDrawElementsBaseVertex(GL_TRIANGLES, m_Meshes[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_Meshes[i].BaseIndex), m_Meshes[i].BaseVertex);
    }

    glBindVertexArray(0);
}

void BasicMesh::Render(glm::mat4 camera, glm::mat4 ProjMatrix, glm::vec3 LightDir, glm::vec3 LightPos, glm::mat4 LightCamera, glm::mat4 LightProjMatrix)
{
    // LIGHT CALC
    glm::mat4 MVMatrix = LightCamera;
    MVMatrix           = glm::translate(MVMatrix, glm::vec3(3.f, -3.f, 0));

    glUniformMatrix4fv(uMVPLight, 1, GL_FALSE, glm::value_ptr(LightProjMatrix * MVMatrix));

    // OBJET CALC
    MVMatrix               = camera;
    MVMatrix               = glm::translate(MVMatrix, glm::vec3(3.f, -3.f, 0.f));
    glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

    glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
    glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
    glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

    glUniform3fv(uLightDir_vs, 1, glm::value_ptr(LightDir));
    glUniform3fv(uLightPos_vs, 1, glm::value_ptr(LightPos));
    glUniform3fv(uLightIntensity, 1, glm::value_ptr(glm::vec3(.8f)));

    glBindVertexArray(m_VAO);

    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        unsigned int MaterialIndex = m_Meshes[i].MaterialIndex;

        assert(MaterialIndex < m_Ka.size());

        glUniform3fv(uKa, 1, glm::value_ptr(m_Kd[MaterialIndex] * 0.5f));
        glUniform3fv(uKd, 1, glm::value_ptr(m_Kd[MaterialIndex]));
        glUniform3fv(uKs, 1, glm::value_ptr(m_Ks[MaterialIndex]));
        glUniform1f(uShininess, m_Ni[MaterialIndex]);

        glad_glDrawElementsBaseVertex(GL_TRIANGLES, m_Meshes[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_Meshes[i].BaseIndex), m_Meshes[i].BaseVertex);
    }

    glBindVertexArray(0);
}
