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
    
    enum class PEACH3D_DLL DrawMode
    {
        ePoint,     // node draw with point model
        eLine,      // node draw with line model
        eTriangle,  // node draw with triangle model
    };
    
    class PEACH3D_DLL Node : public ActionImplement
    {
    public:
        //! add node to child node list
        virtual void addChild(Node* childNode);
        //! mark need delete, waiting for next loop
        virtual void deleteFromParent(bool cleanNode = true);
        //! mark node need delete, waiting for next loop
        virtual void deleteChild(Node* childNode, bool cleanNode = true);
        /** Mark children need delete, waiting for next loop. */
        virtual void deleteAllChildren();
        //! return parent node
        Node* getParentNode() { return mParentNode; }
        
        //! is point in node zone, node must visibled
        virtual bool isPointInZone(const Vector2& point) {return mVisible;}
        /** get is need rendering */
        virtual bool isNeedRender() {return mNeedRender;}
        
        virtual void setVisible(bool visible) {mVisible = visible;}
        bool getVisible() {return mVisible;}
        
        void setName(const std::string& name) { mName = name; }
        const std::string& getName() { return mName; }
        
        virtual void setAlpha(float alpha) { mAlpha = alpha; }
        float getAlpha() { return mAlpha; }
        
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
        
        /** Set Node rendering attribute need update (pos, size, rotate...). */
        void setNeedUpdateRenderingAttributes();
        /** Judge is need render. */
        void prepareForRender(float lastFrameTime);
        
    protected:
        //! user can't call constructor function.
        Node(const std::string& name = "") : mName(name), mParentNode(nullptr), mVisible(true), mAlpha(1.f), mSwallowEvents(true), mIsRenderDirty(true), mSignDeleted(false), mSignClean(true) {}
        //! clean node and all child node, user can't call destructor function.
        virtual ~Node();
        /** When it's mark for delete, exit() will be called. */
        virtual void exit() { for (auto child : mChildNodeList) child->exit(); }
        /** Set parent node, only called for addChildNode */
        virtual void setParentNode(Node* pNode) { mParentNode = pNode; }
        
        /* Update rendering attributes. */
        virtual void updateRenderingAttributes(float lastFrameTime) = 0;
        
        /** Sign node need deleted next frame. */
        void signNeedDeleted(bool needClean);
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
        float              mAlpha;          // node alpha
        
        bool               mIsRenderDirty;  // is render attribute need update
        bool               mNeedRender;     // is node need render. (alpha==0, visible==false, will cause didn't render)
        
        bool               mSignDeleted;    // sign to need deleted next frame
        bool               mSignClean;      // sign to need clean next frame
        std::map<std::string, std::string>  mCustomData;        // scene node custom data
    };
}

#endif
