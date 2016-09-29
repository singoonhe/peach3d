//
//  Peach3DSceneManager.h
//  TestPeach3D
//
//  Created by singoon.he on 12-10-14.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_SCENEMANAGER_H
#define PEACH3D_SCENEMANAGER_H

#include "Peach3DCompile.h"
#include "Peach3DColor4.h"
#include "Peach3DSceneNode.h"
#include "Peach3DCamera.h"
#include "Peach3DMesh.h"
#include "Peach3DSingleton.h"
#include "Peach3DScheduler.h"
#include "Peach3DLight.h"

namespace Peach3D
{
    class Particle3D;
    struct PEACH3D_DLL Render3DPassContent
    {
        std::multimap<uint, RenderNode*> nodeMap;   // cache 3d render node map, using "multimap" to reduce sort
        std::vector<OBB*>               OBBList;    // cache OBB render node list
        std::vector<Particle3D*>        particles;  // cache render particles
        void clear() { nodeMap.clear(); OBBList.clear(); particles.clear(); }
    };
    
    class PEACH3D_DLL SceneManager : public Singleton < SceneManager >
    {
        //! declare class IPlatform is friend class, so IRender can call destructor function.
        friend class IPlatform;
    public:
        //! clear scenemanager widget and scenenode
        void reset();
        
        /** Create camera from camera position/rotation, this camera will be free. */
        virtual Camera* createFreeCamera(const Vector3& pos=Vector3(0.0f, 0.0f, 1.0f), const Vector3& rotate=Vector3Zero);
        /** Create camera from camera position/target/up, this camera will be locked to center. */
        virtual Camera* createLockedCamera(const Vector3& eye=Vector3(0.0f, 0.0f, 1.0f), const Vector3& center=Vector3Zero, const Vector3& up=Vector3(0.0f, 1.0f, 0.0f));
        
        virtual void deleteCamera(Camera* camera);
        inline void setActiveCamera(Camera* newCamera);
        Camera* getActiveCamera() { return mActiveCamera; }
        SceneNode* getRootSceneNode() { return mRootSceneNode; }
        Widget* getRootWidget() { return mRootWidget; }
        
        void setLightMax(int count) { if (count > 0) mLightMax = count; }
        int getLightMax() { return mLightMax; }
        /** Add new light, auto new name if null. */
        LightPtr addNewLight(const char* name = "");
        LightPtr getLight(const char* name);
        void deleteLight(const char* name);
        void deleteAllLights();
        void tranverseLights(std::function<void(const std::string& name, const LightPtr& l)> callFunc, bool onlyEnabled = false);
        
        /** Set perspective projection. */
        void setPerspectiveProjection(float fovY, float asPect, float zNear=1.0f, float zFar=1000.0f);
        /** Set ortho projection. */
        void setOrthoProjection(float left, float right, float bottom, float top, float nearVal, float farVal);
        /** Return current projection type, perspective mat[11] always is -1. */
        bool isOrthoProjection() { return mProjectionMatrix.mat[11] > -0.5f; }
        void setProjectionMatrix(const Matrix4& proj) { mProjectionMatrix = proj; }
        const Matrix4& getProjectionMatrix() {return mProjectionMatrix;}
        
        /** Update UI size and projection size. */
        void updateSceneSize(const Vector2& size);
        /**
         * @brief Get clicked SceneNode, this will use a ray(camera ray) to check crossing with SceneNodes.
         * @params clickedPos The clicked point.
         * @params name Return clicked RenderNode in clicked SceneNode.
         * @return Return nullptr if no SceneNode crossed with camera ray.
         */
        SceneNode* getWindowClickedNode(const Vector2& clickedPos, RenderNode** outNode = nullptr);
        

    protected:
        //! create object by IPlatform, user can't call constructor function.
        SceneManager() : mRootSceneNode(nullptr), mRootWidget(nullptr), mActiveCamera(nullptr), mDebugDrawNode(nullptr), mDrawUpdateSchedule(nullptr), mWidgetObject(nullptr), mOBBObject(nullptr), mLightMax(10)  {}
        //! delete object by IPlatform, user can't call destructor function.
        virtual ~SceneManager();
        //! init
        void init();
        /** Render one frame, executing all pass. */
        virtual void render(float lastFrameTime);
        /** Render once pass. */
        virtual void renderOncePass(Render3DPassContent* content);
        /** Render once pass for RTT, generate new content if content is nullptr. */
        virtual void renderForRTT(const TexturePtr& rtt, Render3DPassContent* content);
        
        /** Create draw stats node. */
        void createDrawStatsNode();
        /** Create preset widget object and OBB object. */
        void createPresetObjects();
        /** Add render and picking scene node to cache list, also prepare for render.
         * @params content where node and obb list saved.
         * @params pickEnabled pass will cache clicked node list.
         */
        void addSceneNodeToCacheList(Node* node, float lastFrameTime, Render3DPassContent* content, bool pickEnabled);
        /** Add render widget to cache list, also prepare for render. */
        void addWidgetToCacheList(int* zOrder, Node* child, float lastFrameTime);

    protected:
        SceneNode*              mRootSceneNode;     // root scene node
        Widget*                 mRootWidget;        // root Widget
        Matrix4                 mProjectionMatrix;  // used projective matrix for object
        int                     mLightMax;          // lighting  max supported count
        
        Scheduler*              mDrawUpdateSchedule;// draw stats update schedule
        Widget*                 mDebugDrawNode;     // draw stats widget
        ObjectPtr               mWidgetObject;      // object for rendering widget
        ObjectPtr               mOBBObject;         // object for rendering OBB
        ObjectPtr               mParticle2DObject;  // object for 2D particle
        ObjectPtr               mParticle3DObject;  // object for 3D particle
        
        Camera*                 mActiveCamera;      // current active camera
        std::vector<Camera*>    mCameraList;        // scene camera list
        
        std::map<std::string, LightPtr>     mLightList;         // scene light list
        std::vector<Node*>                  mRenderWidgetList;  // cache widget(may particle2d) list
        std::vector<SceneNode*>             mPickSceneNodeList; // cache picking scene node list
    };
}

#endif // PEACH3D_SCENEMANAGER_H
