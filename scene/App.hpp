#include <cstddef>
#include <glimac/ParticleManager.hpp>
#include <glimac/SimpleObjectManager.hpp>
#include <glimac/common.hpp>
#include <iostream>
#include <vector>
#include "glimac/FreeflyCamera.hpp"
#include "glimac/LightManager.hpp"
#include "glimac/ObjectManager.hpp"
#include "glimac/ShadowCubeMap.hpp"
#include "glimac/ShadowManager.hpp"
#include "glimac/ShadowMap.hpp"
#include "glimac/SkinnedMesh.hpp"
#include "glimac/SkinnedObjectManager.hpp"
#include "glimac/TexObjectManager.hpp"
#include "glimac/TrackballCamera.hpp"
#include "glimac/quad_vertices.hpp"
#include "glimac/sphere_vertices.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "p6/p6.h"

class App {
public:
    explicit App(int window_width = 1920, int window_height = 1080);
    void        init();
    void        update();
    inline void clean()
    {
        for (auto& i : m_ObjList)
        {
            i->clear();
        }
    }

private:
    // AppInit.cpp
    void initAllObject();
    void initViewProjection();
    void initShadow();

    // AppUpdate.cpp
    void keyEvent();
    void actionEvent();
    void loop();
    void shadowPass();
    void lightPass();
    int  LODtoShow(const ObjectManager* obj);

private:
    int const shadow_size = 4096;
    int const LODdistance = 300;

    bool Z = false;
    bool Q = false;
    bool S = false;
    bool D = false;

    p6::Context                 m_ctx;
    std::vector<ObjectManager*> m_ObjList;
    ParticleManager             m_Particles;
    std::vector<LightManager>   m_LightList;
    std::vector<ShadowManager>  m_ShadowProgList;
    FreeflyCamera               m_ViewMatrixCamera;
    glm::mat4                   m_ProjMatrix;

    // Objects of the scene
    SimpleObjectManager*               m_sun    = nullptr;
    SkinnedObjectManager*              m_ground = nullptr;
    SkinnedObjectManager*              m_river  = nullptr;
    const int                          WLILY_NB = 5;
    std::vector<SkinnedObjectManager*> m_wlily;
    TexObjectManager*                  m_floor = nullptr; // HELPER
    TexObjectManager*                  m_limit = nullptr;
};