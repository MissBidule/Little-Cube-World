#ifndef SKINNING_MESH_HPP
#define SKINNING_MESH_HPP

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <map>
#include <string>
#include <vector>
#include "glm/fwd.hpp"
#include "img/src/Image.h"
#include "p6/p6.h"

#define ASSIMP_LOAD_FLAGS         (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a) / sizeof(a[0]))
#define GLCheckError()            (glGetError() == GL_NO_ERROR)

class SkinnedMesh {
public:
    inline SkinnedMesh() = default;

    ~SkinnedMesh();

    bool LoadMesh(const std::string& Filename);

    void render();

    void render(float seconds, GLint uBoneTransforms, GLint uKa, GLint uKd, GLint uKs, GLint uShininess, GLint uOpacity);

    inline int NumBones() { return m_BoneNameToIndexMap.size(); }

    void GetBoneTransforms(float AnimationTimeSec, std::vector<glm::mat4>& Transforms);

private:
#define MAX_NUM_BONES_PER_VERTEX 4

    void Clear();

    bool InitFromScene(const aiScene* pScene);

    void CountVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices);

    void ReserveSpace(unsigned int NumVertices, unsigned int NumIndices);

    void InitAllMeshes(const aiScene* pScene);

    void InitSingleMesh(unsigned int MeshIndex, const aiMesh* paiMesh);

    bool InitMaterials(const aiScene* pScene);

    void PopulateBuffers();

    struct VertexBoneData {
        unsigned int BoneIDs[MAX_NUM_BONES_PER_VERTEX] = {0};
        float        Weights[MAX_NUM_BONES_PER_VERTEX] = {0.f};

        VertexBoneData() {}

        void AddBoneData(unsigned int BoneID, float Weight)
        {
            for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(BoneIDs); i++)
            {
                if (Weights[i] == 0.0)
                {
                    BoneIDs[i] = BoneID;
                    Weights[i] = Weight;
                    return;
                }
            }
            // Should neer get here - more bones than we have space for
            assert(0);
        }
    };

    void LoadMeshBones(unsigned int MeshIndex, const aiMesh* paiMesh);
    void LoadSingleBone(unsigned int MeshIndex, const aiBone* pBone);
    int  GetBoneId(const aiBone* pBone);
    void ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* pNode, const glm::mat4& ParentTransform);

    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
    unsigned int      FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
    void              CalcInterpolatedPosition(aiVector3D& out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
    unsigned int      FindRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
    void              CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
    unsigned int      FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
    void              CalcInterpolatedScaling(aiVector3D& out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);

#define INVALID_MATERIAL 0xFFFFFFFF

    enum BUFFER_TYPE {
        INDEX_BUFFER = 0,
        POS_VB       = 1,
        TEXCOORD_VB  = 2,
        NORMAL_VB    = 3,
        BONE_VB      = 4,
        NUM_BUFFERS  = 5
    };

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
    };

    Assimp::Importer            Importer;
    const aiScene*              pScene = NULL;
    glm::mat4                   m_GlobalInverseTransform;
    std::vector<BasicMeshEntry> m_Meshes;
    std::vector<glm::vec3>      m_Ka;
    std::vector<glm::vec3>      m_Kd;
    std::vector<glm::vec3>      m_Ks;
    std::vector<float>          m_Ni;
    std::vector<float>          m_D;

    std::vector<glm::vec3>      m_Positions;
    std::vector<glm::vec3>      m_Normals;
    std::vector<glm::vec2>      m_TexCoords;
    std::vector<unsigned int>   m_Indices;
    std::vector<VertexBoneData> m_Bones;

    std::map<std::string, unsigned int> m_BoneNameToIndexMap;

    struct BoneInfo {
        glm::mat4 OffsetMatrix;
        glm::mat4 FinalTransformation;

        BoneInfo(const glm::mat4& Offset)
        {
            OffsetMatrix        = Offset;
            FinalTransformation = glm::mat4();
        }
    };

    std::vector<BoneInfo> m_BoneInfo;
};

#endif