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
#include "Peach3DScheduler.h"

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
        void setGlobalAmbient(const Color4& newColor) { mAmbientColor = newColor; }
        const Color4& getGlobalAmbient() {return mAmbientColor;}
        
        /** Set perspective projection. */
        void setPerspectiveProjection(float fovY, float asPect, float zNear=1.0f, float zFar=1000.0f);
        /** Set ortho projection. */
        void setOrthoProjection(float left, float right, float bottom, float top, float nearVal, float farVal);
        const Matrix4& getProjectionMatrix() {return mProjectionMatrix;}
        
        /**
         * @brief Get clicked SceneNode, this will use a ray(camera ray) to check crossing with SceneNodes.
         * @params clickedPos The clicked point.
         * @return Return nullptr if no SceneNode crossed with camera ray.
         */
        SceneNode* getWindowClickedNode(const Vector2& clickedPos);
        

    protected:
        //! create object by IPlatform, user can't call constructor function.
        SceneManager() : mRootSceneNode(nullptr), mRootWidget(nullptr), mActiveCamera(nullptr), mDebugDrawNode(nullptr), mDrawUpdateSchedule(nullptr)  {}
        //! delete object by IPlatform, user can't call destructor function.
        virtual ~SceneManager();
        //! init
        void init();
        //! render
        virtual void render(float lastFrameTime);
        
        /** Create draw stats node. */
        void createDrawStatsNode();
        /** Auto generate widget render object, each program need IObject on GL3. */
        IObject* autoGenerateWidgetObject(uint program);
        /** Add render and picking scene node to cache list, also prepare for render. */
        void addSceneNodeToCacheList(Node* node, float lastFrameTime);
        /** Add render widget to cache list, also prepare for render. */
        void addWidgetToList(int* zOrder, Widget* widget, float lastFrameTime);

    protected:
        SceneNode*              mRootSceneNode;    // root scene node
        Widget*                 mRootWidget;       // root Widget
        Matrix4                 mProjectionMatrix; // used projective matrix for object
        Color4                  mAmbientColor;     // global ambient color
        
        Scheduler*              mDrawUpdateSchedule;// draw stats update schedule
        Widget*                 mDebugDrawNode;     // draw stats widget
        std::vector<Widget*>    mRenderWidgetList;  // cache widget list
        std::map<uint, IObject*>    mWidgetObject;  // object use to render widget map, key is render state hash
        
        std::vector<Camera*>    mCameraList;        // scene camera list
        Camera*                 mActiveCamera;      // current active camera
        
        std::vector<SceneNode*> mRenderSceneNodeList;   // cache render scene node list
        std::vector<SceneNode*> mPickSceneNodeList;     // cache picking scene node list
    };
}

#endif // PEACH3D_SCENEMANAGER_H
