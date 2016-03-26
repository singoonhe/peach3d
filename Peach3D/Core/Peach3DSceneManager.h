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
        Light* addNewLight(const char* name = "");
        Light* getLight(const char* name);
        void deleteLight(const char* name);
        void deleteAllLights();
        void tranverseLights(std::function<void(const std::string& name, Light* l)> callFunc, bool onlyEnabled = false);
        /** Update all SceneNode light state, will be called when light changed. */
        void updateAllNodesLighting();
        
        /** Set perspective projection. */
        void setPerspectiveProjection(float fovY, float asPect, float zNear=1.0f, float zFar=1000.0f);
        /** Set ortho projection. */
        void setOrthoProjection(float left, float right, float bottom, float top, float nearVal, float farVal);
        const Matrix4& getProjectionMatrix() {return mProjectionMatrix;}
        
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
        /** Render once pass, update-draw. 
         * @params isMain main pass will cache clicked node list.
         */
        virtual void renderOncePass(float lastFrameTime, bool isMain = false);
        
        /** Create draw stats node. */
        void createDrawStatsNode();
        /** Create preset widget object and OBB object. */
        void createPresetObjects();
        /** Update SceneNode and children. */
        void updateSceneNodeLighting(Node* sNode);
        /** Add render and picking scene node to cache list, also prepare for render.
         * @params isMain main pass will cache clicked node list.
         */
        void addSceneNodeToCacheList(Node* node, float lastFrameTime, bool isMain);
        /** Add render widget to cache list, also prepare for render. */
        void addWidgetToCacheList(int* zOrder, Widget* widget, float lastFrameTime);

    protected:
        SceneNode*              mRootSceneNode;     // root scene node
        Widget*                 mRootWidget;        // root Widget
        Matrix4                 mProjectionMatrix;  // used projective matrix for object
        int                     mLightMax;          // lighting  max supported count
        
        Scheduler*              mDrawUpdateSchedule;// draw stats update schedule
        Widget*                 mDebugDrawNode;     // draw stats widget
        IObject*                mWidgetObject;      // object for rendering widget
        IObject*                mOBBObject;         // object for rendering OBB
        
        Camera*                 mActiveCamera;      // current active camera
        std::vector<Camera*>    mCameraList;        // scene camera list
        
        std::map<std::string, Light*>       mLightList;         // scene light list
        std::vector<Widget*>                mRenderWidgetList;  // cache widget list
        std::vector<OBB*>                   mRenderOBBList;     // cache OBB render node list
        std::multimap<uint, RenderNode*>    mRenderNodeMap;     // cache 3d render node map, using "multimap" to reduce sort
        std::vector<SceneNode*>             mPickSceneNodeList; // cache picking scene node list
    };
}

#endif // PEACH3D_SCENEMANAGER_H
