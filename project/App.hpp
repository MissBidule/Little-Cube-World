#include <cstddef>
#include <glimac/ParticleManager.hpp>
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

private:
    int const shadow_size = 4096;
    int const LODdistance = 500;

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
    SkinnedObjectManager*             m_Thingy = nullptr;
    SimpleObjectManager*              m_earth  = nullptr;
    const int                         MOON_NB  = 32;
    std::vector<SimpleObjectManager*> m_moons;
    std::vector<glm::vec3>            m_MoonRotDir;
    std::vector<glm::vec3>            m_MoonRotAxes;
    TexObjectManager*                 m_floor = nullptr;
    TexObjectManager*                 m_limit = nullptr;
};