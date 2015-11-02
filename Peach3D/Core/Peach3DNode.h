//
//  Peach3DNode.h
//  TestPeach3D
//
//  Created by singoon.he on 12-10-14.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_NODE_H
#define PEACH3D_NODE_H

#include "Peach3DCompile.h"
#include "Peach3DVector2.h"
#include "Peach3DMatrix4.h"
#include "Peach3DIProgram.h"
#include "Peach3DActionImplement.h"

namespace Peach3D
{
    enum class PEACH3D_DLL TranslateRelative
    {
        eLocal,     // local rotation diff from parent
        eWorld,     // translate relative to world node
    };
    
    class PEACH3D_DLL Node : public ActionImplement
    {
    public:
        //! add node to child node list
        virtual void addChild(Node* childNode);
        //! delete self from parent
        virtual void deleteFromParent(bool cleanNode = true);
        //! delete node from child node list
        virtual void deleteChild(Node* childNode, bool cleanNode = true);
        /** Delete all child node and clean them. */
        virtual void deleteAllChildren();
        //! return parent node
        Node* getParentNode() { return mParentNode; }
        
        //! is point in node zone, node must visibled
        virtual bool isPointInZone(const Vector2& point) {return mVisible;}
        /** get is need rendering */
        virtual bool isNeedRender() {return mNeedRender && mRenderProgram;}
        /** Get render hash. */
        uint getRenderStateHash()const {return mRenderStateHash;}
        
        virtual void setVisible(bool visible) {mVisible = visible;}
        bool getVisible() {return mVisible;}
        
        void setName(const std::string& name) { mName = name; }
        std::string getName() { return mName; }
        
        virtual void setAlpha(float alpha) { mAlpha = alpha; }
        float getAlpha() { return mAlpha; }
        
        void useProgramForRender(IProgram* program);
        IProgram* getProgramForRender() {return mRenderProgram;}
        
        void setSwallowEvents(bool swallow) {mSwallowEvents = swallow;}
        bool isSwallowEvents() { return mSwallowEvents; }
        
        void setCustomData(const std::string& key, const std::string& value);
        std::string getCustomData(const std::string& key);
        
        /** Find child node by tag name, will iterative search children. */
        Node* findChildByName(const std::string& name);
        /** Traverse child nodes, will auto call lambda func. */
        void tranverseChildNode(std::function<void(size_t, Node*)> callFunc);
        uint getChildrenCount() { return (uint)mChildNodeList.size(); }
        const std::vector<Node*>& getChildrenList() {return mChildNodeList;}
        
        /** Judge is need render. */
        void prepareForRender(float lastFrameTime);
    protected:
        //! user can't call constructor function.
        Node(const std::string& name = "") : mName(name), mParentNode(nullptr), mVisible(true), mSwallowEvents(true), mIsRenderDirty(true), mSignDeleted(false), mSignClean(true), mAlpha(1.0f), mRenderProgram(nullptr), mRenderStateHash(0), mIsRenderHashDirty(true) {}
        //! clean node and all child node, user can't call destructor function.
        virtual ~Node();
        /** Set Node rendering attribute need update (pos, size, rotate...). */
        void setNeedUpdateRenderingAttributes();
        /** Set parent node, only called for addChildNode */
        virtual void setParentNode(Node* pNode) { mParentNode = pNode; }
        
        /* Update rendering attributes. */
        virtual void updateRenderingAttributes(float lastFrameTime) = 0;
        /** Get render state calc hash string. */
        virtual std::string getRenderStateString() = 0;
        /** Get preset program when not set program. */
        virtual void setPresetProgram() = 0;
        
        /** Sign node need deleted next frame. */
        void signNeedDeleted(bool needClean) { mSignDeleted = true; mSignClean = needClean; }
        /** Is node need delete. */
        bool isNeedDelete() { return mSignDeleted; }
        /** Is node need clean when deleted. */
        bool isNeedClean() { return mSignClean; }

    protected:
        std::vector<Node*> mChildNodeList;  // child node list
        Node*              mParentNode;
        std::string        mName;           // node target name
        bool               mSwallowEvents;  // is swallow events
        bool               mVisible;        // is Node visible
        float              mAlpha;          // node alpha, SceneNode diffuse color in material
        
        bool               mIsRenderDirty;  // is render attribute need update
        bool               mNeedRender;     // is node need render. (alpha==0, visible==false, will cause didn't render)
        Matrix4            mModelMatrix;    //*********** ui or SceneNode transpose matrix, will be discard
        
        IProgram*          mRenderProgram;  // render program
        uint               mRenderStateHash;// render state hash, used for instancing render
        bool               mIsRenderHashDirty; // is render state hash need update
        
        bool               mSignDeleted;    // sign to need deleted next frame
        bool               mSignClean;      // sign to need clean next frame
        std::map<std::string, std::string>  mCustomData;        // scene node custom data
    };
}

#endif
