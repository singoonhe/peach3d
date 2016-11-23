//
//  Peach3DNode.cpp
//  TestPeach3D
//
//  Created by singoon.he on 12-10-14.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DNode.h"
#include "Peach3DIPlatform.h"

namespace Peach3D
{
    void Node::addChild(Node* childNode)
    {
        Peach3DAssert(childNode, "Can't add null node as a child node!");
        Peach3DAssert(!childNode->getParentNode(), "Child already add to another node!");

        // add child node
        if (!childNode->getParentNode()) {
            mChildNodeList.push_back(childNode);
            childNode->setParentNode(this);
        }
    }
    
    void Node::deleteChild(Node* childNode, bool cleanNode)
    {
        // find child node, and sign to delete
        if (std::find(mChildNodeList.begin(), mChildNodeList.end(), childNode) != mChildNodeList.end()) {
            childNode->signNeedDeleted(cleanNode);
        }
    }
    
    void Node::deleteAllChildren()
    {
        // set all children need deleted
        for (auto& iterNode : mChildNodeList) {
            iterNode->signNeedDeleted(true);
        }
    }
    
    void Node::deleteFromParent(bool cleanNode)
    {
        if (mParentNode) {
            mParentNode->deleteChild(this, cleanNode);
        }
        else if (cleanNode) {
            delete this;
        }
    }

    void Node::setCustomData(const std::string& key, const std::string& value)
    {
        if (key.size() > 0) {
            mCustomData[key] = value;
        }
    }

    std::string Node::getCustomData(const std::string& key)
    {
        if (key.size() > 0 && mCustomData.find(key) != mCustomData.end()) {
            return mCustomData[key];
        }
        else {
            return "";
        }
    }

    Node* Node::findChildByName(const std::string& name)
    {
        Node* findChild = nullptr;
        if (name.size() > 0 && mChildNodeList.size() > 0) {
            // find node in children
            for (auto& child : mChildNodeList) {
                if (child->getName() == name) {
                    findChild = child;
                    break;
                }
            }
            if (!findChild) {
                // find node in children of child
                for (auto& child : mChildNodeList) {
                    findChild = child->findChildByName(name);
                    if (findChild) {
                        break;
                    }
                }
            }
        }
        return findChild;
    }
    
    void Node::tranverseChildNode(std::function<void(size_t, Node*)> callFunc)
    {
        for (size_t i=0; i<mChildNodeList.size(); ++i) {
            callFunc(i, mChildNodeList[i]);
        }
    }
    
    void Node::prepareForRender(float lastFrameTime)
    {
        ActionImplement::prepareForRender(lastFrameTime);
        // calc is need rendering
        bool isNeedVisible = mVisible;
        Node* pNode = mParentNode;
        while (pNode && isNeedVisible) {
            isNeedVisible = pNode->getVisible();
            pNode = pNode->getParentNode();
        }
        mNeedRender = isNeedVisible && (mAlpha > FLT_EPSILON);
        
        // update render attributes
        // Notice: function "updateRenderingAttributes" may change render state.
        updateRenderingAttributes(lastFrameTime);
        // delete children node
        for (auto delIter = mChildNodeList.begin(); delIter != mChildNodeList.end();) {
            if ((*delIter)->isNeedDelete()) {
                if ((*delIter)->isNeedClean()) {
                    delete (*delIter);
                }
                delIter = mChildNodeList.erase(delIter);
            }
            else {
                delIter++;
            }
        }
    }
    
    void Node::setNeedUpdateRenderingAttributes()
    {
        mIsRenderDirty = true;
        // set children need update matrix
        std::for_each(mChildNodeList.begin(), mChildNodeList.end(), [](Node* child){
            child->setNeedUpdateRenderingAttributes();
        });
    }
    
    void Node::signNeedDeleted(bool needClean)
    {
        mSignDeleted = true;
        mSignClean = needClean;
        // node just unbind from parent if not clean
        if (mSignClean) {
            exit();
        }
    }
    
    Node::~Node()
    {
        // delete self event listener
        EventDispatcher::getSingletonPtr()->deleteClickEventListener(this);
        // clean all child node
        for (auto childNode = mChildNodeList.begin(); childNode != mChildNodeList.end(); childNode++) {
            delete (*childNode);
        }
    }
}
