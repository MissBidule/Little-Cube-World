#include "SkinnedMesh.hpp"
#include <cassert>
#include <vector>
#include "assimp/anim.h"
#include "assimp/material.h"
#include "assimp/quaternion.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "assimp/vector3.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/matrix.hpp"

#define POSITION_LOCATION    0
#define NORMAL_LOCATION      1
#define TEX_COORD_LOCATION   2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

glm::mat4 convertMatrix(const aiMatrix4x4& aiMat)
{
    return {
        aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
        aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
        aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
        aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4};
}

SkinnedMesh::SkinnedMesh()
    : m_Program(p6::load_shader("shaders/skinning.vs.glsl", "shaders/dirPosLight.fs.glsl"))
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
    uBoneTransforms = glGetUniformLocation(m_Program.id(), "uBoneTransforms[0]");
    // FOR SKIN DEBUG
    uDisplayBoneIndex = glGetUniformLocation(m_Program.id(), "uDisplayBoneIndex");
};

void SkinnedMesh::UpdateDebug(int DisplayBoneIndex)
{
    glUniform1i(uDisplayBoneIndex, DisplayBoneIndex);
}

SkinnedMesh::~SkinnedMesh()
{
    Clear();
}

void SkinnedMesh::Clear()
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

bool SkinnedMesh::LoadMesh(const std::string& Filename)
{
    // Release the previously loaded mesh (if it exists)
    Clear();

    // Create the VAO
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Create the buffers for the vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_buffers), m_buffers);

    bool Ret = false;

    pScene = Importer.ReadFile(Filename.c_str(), ASSIMP_LOAD_FLAGS);

    if (pScene)
    {
        m_GlobalInverseTransform = convertMatrix(pScene->mRootNode->mTransformation);
        glm::inverse(m_GlobalInverseTransform);
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

bool SkinnedMesh::InitFromScene(const aiScene* pScene)
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

void SkinnedMesh::CountVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices)
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

void SkinnedMesh::ReserveSpace(unsigned int NumVertices, unsigned int NumIndices)
{
    m_Positions.reserve(NumVertices);
    m_Normals.reserve(NumVertices);
    m_TexCoords.reserve(NumVertices);
    m_Indices.reserve(NumIndices);
    m_Bones.resize(NumVertices);
}

void SkinnedMesh::InitAllMeshes(const aiScene* pScene)
{
    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitSingleMesh(i, paiMesh);
    }
}

void SkinnedMesh::InitSingleMesh(unsigned int MeshIndex, const aiMesh* paiMesh)
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

    LoadMeshBones(MeshIndex, paiMesh);

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

void SkinnedMesh::LoadMeshBones(unsigned int MeshIndex, const aiMesh* pMesh)
{
    for (unsigned int i = 0; i < pMesh->mNumBones; i++)
    {
        LoadSingleBone(MeshIndex, pMesh->mBones[i]);
    }
}

void SkinnedMesh::LoadSingleBone(unsigned int MeshIndex, const aiBone* pBone)
{
    int BoneId = GetBoneId(pBone);

    if (BoneId == m_BoneInfo.size())
    {
        BoneInfo bi(convertMatrix(pBone->mOffsetMatrix));
        m_BoneInfo.push_back(bi);
    }

    for (unsigned int i = 0; i < pBone->mNumWeights; i++)
    {
        const aiVertexWeight& vw             = pBone->mWeights[i];
        unsigned int          GlobalVertexID = m_Meshes[MeshIndex].BaseVertex + pBone->mWeights[i].mVertexId;
        m_Bones[GlobalVertexID].AddBoneData(BoneId, vw.mWeight);
    }
}

int SkinnedMesh::GetBoneId(const aiBone* pBone)
{
    int         BoneIndex = 0;
    std::string BoneName(pBone->mName.C_Str());

    if (m_BoneNameToIndexMap.find(BoneName) == m_BoneNameToIndexMap.end())
    {
        // Allocate an index for new bone
        BoneIndex                      = m_BoneNameToIndexMap.size();
        m_BoneNameToIndexMap[BoneName] = BoneIndex;
    }
    else
    {
        BoneIndex = m_BoneNameToIndexMap[BoneName];
    }

    return BoneIndex;
}

bool SkinnedMesh::InitMaterials(const aiScene* pScene)
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

    for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
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

void SkinnedMesh::GetBoneTransforms(float AnimationTimeSec, std::vector<glm::mat4>& Transforms)
{
    // PUT ANIMATION TICKS TO 0 TO STOP MOTION
    glm::mat4 Identity(1.f);

    float TicksPerSecond     = (float)(pScene->mAnimations[0]->mTicksPerSecond != 0 ? pScene->mAnimations[0]->mTicksPerSecond : 25.f);
    float TimeInTicks        = AnimationTimeSec * TicksPerSecond;
    float AnimationTimeTicks = fmod(TimeInTicks, (float)pScene->mAnimations[0]->mDuration);

    ReadNodeHierarchy(AnimationTimeTicks, pScene->mRootNode, Identity);
    Transforms.resize(m_BoneInfo.size());

    for (unsigned int i = 0; i < m_BoneInfo.size(); i++)
    {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}

const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
    for (unsigned int i = 0; i < pAnimation->mNumChannels; i++)
    {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == NodeName)
        {
            return pNodeAnim;
        }
    }

    return NULL;
}

unsigned int SkinnedMesh::FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumPositionKeys > 0);

    for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
    {
        float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;
        if (AnimationTimeTicks < t)
        {
            return i;
        }
    }
    return 0;
}

void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumPositionKeys == 1)
    {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    unsigned int PositionIndex     = FindPosition(AnimationTimeTicks, pNodeAnim);
    unsigned int NextPositionIndex = PositionIndex + 1;
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float t1        = (float)pNodeAnim->mPositionKeys[PositionIndex].mTime;
    float t2        = (float)pNodeAnim->mPositionKeys[NextPositionIndex].mTime;
    float DeltaTime = t2 - t1;
    float Factor    = (AnimationTimeTicks - t1) / DeltaTime;
    assert(Factor >= 0.f && Factor <= 1.f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D        Delta = End - Start;
    Out                     = Start + Factor * Delta;
}

unsigned int SkinnedMesh::FindRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
    {
        float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;
        if (AnimationTimeTicks < t)
        {
            return i;
        }
    }
    return 0;
}

void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1)
    {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    unsigned int RotationIndex     = FindRotation(AnimationTimeTicks, pNodeAnim);
    unsigned int NextRotationIndex = RotationIndex + 1;
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float t1        = (float)pNodeAnim->mRotationKeys[RotationIndex].mTime;
    float t2        = (float)pNodeAnim->mRotationKeys[NextRotationIndex].mTime;
    float DeltaTime = t2 - t1;
    float Factor    = (AnimationTimeTicks - t1) / DeltaTime;
    assert(Factor >= 0.f && Factor <= 1.f);
    const aiQuaternion& Start = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& End   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, Start, End, Factor);
    Out = Start;
    Out.Normalize();
}

unsigned int SkinnedMesh::FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
    {
        float t = (float)pNodeAnim->mScalingKeys[i + 1].mTime;
        if (AnimationTimeTicks < t)
        {
            return i;
        }
    }
    return 0;
}

void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumScalingKeys == 1)
    {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    unsigned int ScalingIndex     = FindScaling(AnimationTimeTicks, pNodeAnim);
    unsigned int NextScalingIndex = ScalingIndex + 1;
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float t1        = (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime;
    float t2        = (float)pNodeAnim->mScalingKeys[NextScalingIndex].mTime;
    float DeltaTime = t2 - t1;
    float Factor    = (AnimationTimeTicks - t1) / DeltaTime;
    assert(Factor >= 0.f && Factor <= 1.f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D        Delta = End - Start;
    Out                     = Start + Factor * Delta;
}

void SkinnedMesh::ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* pNode, const glm::mat4& ParentTransform)
{
    std::string NodeName(pNode->mName.data);

    const aiAnimation* pAnimation = pScene->mAnimations[0];

    glm::mat4 NodeTransformation(convertMatrix(pNode->mTransformation));

    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

    if (pNodeAnim)
    {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTimeTicks, pNodeAnim);
        glm::mat4 ScalingM = glm::scale(glm::mat4(1.f), glm::vec3(Scaling.x, Scaling.y, Scaling.z));

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTimeTicks, pNodeAnim);
        glm::mat4 RotationM = convertMatrix(aiMatrix4x4(RotationQ.GetMatrix()));

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTimeTicks, pNodeAnim);
        glm::mat4 TranslationM = glm::translate(glm::mat4(1.f), glm::vec3(Translation.x, Translation.y, Translation.z));

        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

    if (m_BoneNameToIndexMap.find(NodeName) != m_BoneNameToIndexMap.end())
    {
        unsigned int BoneIndex                    = m_BoneNameToIndexMap[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].OffsetMatrix;
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; i++)
    {
        ReadNodeHierarchy(AnimationTimeTicks, pNode->mChildren[i], GlobalTransformation);
    }
}

void SkinnedMesh::PopulateBuffers()
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

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[BONE_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Bones[0]) * m_Bones.size(), m_Bones.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(BONE_ID_LOCATION);
    glVertexAttribIPointer(BONE_ID_LOCATION, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
    glVertexAttribPointer(BONE_WEIGHT_LOCATION, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)(MAX_NUM_BONES_PER_VERTEX * sizeof(int32_t)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), m_Indices.data(), GL_STATIC_DRAW);
}

void SkinnedMesh::Render()
{
    glBindVertexArray(m_VAO);

    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        glDrawElementsBaseVertex(GL_TRIANGLES, m_Meshes[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_Meshes[i].BaseIndex), m_Meshes[i].BaseVertex);
    }

    glBindVertexArray(0);
}

void SkinnedMesh::Render(glm::mat4 camera, glm::mat4 ProjMatrix, glm::vec3 LightDir, glm::vec3 LightPos, glm::mat4 LightCamera, glm::mat4 LightProjMatrix, float seconds)
{
    std::vector<glm::mat4> Transforms;
    GetBoneTransforms(seconds, Transforms);

    for (unsigned int i = 0; i < Transforms.size(); i++)
    {
        glUniformMatrix4fv(uBoneTransforms + i * 4, 1, GL_FALSE, glm::value_ptr(Transforms[i]));
    }

    // LIGHT CALC
    glm::mat4 MVMatrix = LightCamera;
    MVMatrix           = glm::translate(MVMatrix, glm::vec3(3.f, -3.f, 0));
    // BCS IT'S AN FBX OBJECT
    MVMatrix = glm::rotate(MVMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));

    glUniformMatrix4fv(uMVPLight, 1, GL_FALSE, glm::value_ptr(LightProjMatrix * MVMatrix));

    // OBJET CALC
    MVMatrix = camera;
    MVMatrix = glm::translate(MVMatrix, glm::vec3(3.f, -3.f, 0.f));
    // BCS IT'S AN FBX OBJECT
    MVMatrix               = glm::rotate(MVMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
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

        glUniform3fv(uKa, 1, glm::value_ptr(m_Kd[MaterialIndex] * 0.25f));
        glUniform3fv(uKd, 1, glm::value_ptr(m_Kd[MaterialIndex]));
        glUniform3fv(uKs, 1, glm::value_ptr(m_Ks[MaterialIndex]));
        glUniform1f(uShininess, m_Ni[MaterialIndex]);

        glDrawElementsBaseVertex(GL_TRIANGLES, m_Meshes[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_Meshes[i].BaseIndex), m_Meshes[i].BaseVertex);
    }

    glBindVertexArray(0);
}
