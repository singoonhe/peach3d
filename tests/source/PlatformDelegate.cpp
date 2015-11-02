//
//  PlatformDelegate.cpp
//  test
//
//  Created by singoon.he on 9/5/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3D.h"
#include "TestScene.h"
#include "BaseScene.h"
#include "PlatformDelegate.h"

using namespace Peach3D;

bool PlatformDelegate::appDidFinishLaunching()
{
    // show drawing stats, only valid on debug
    IPlatform::getSingleton().setIsDrawStats(true);
    // load program
    Peach3D::ResourceManager* resourceMgr = Peach3D::ResourceManager::getSingletonPtr();
//    std::vector<ProgramUniform> uniformList = {ProgramUniform(pdShaderModelMatrixUniformName, UniformType::eMatrix4)};
//#if PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_DX11
//    IProgram* program = resourceMgr->addProgram("demoVertexShader.cso", "demoPixelShader.cso", VertexTypePosition3 | VertexTypeColor4 | VertexTypeUV, uniformList);
//#else
//    IProgram* program = resourceMgr->addProgram("demoShader.vsh", "demoShader.fsh", VertexTypePosition3|VertexTypeColor4|VertexTypeUV, uniformList);
//#endif
    
    // create mesh
    Mesh* cube = resourceMgr->createMesh("pd_Mesh0");
    int step = 9;
    /*
    // create trianglestrip test object
    IObject* object1 = cube->createObject();
    float* vertex = (float*)malloc(4 * step*sizeof(float));
    for (int i=0; i<4*step; i++)
    {
        vertex[i] = 0.f;
    }
    vertex[0*step+0] = -0.8f;
    vertex[0*step+1] = 0.9f;
    vertex[1*step+0] = -0.9f;
    vertex[1*step+1] = 0.5f;
    vertex[2*step+0] = -0.7f;
    vertex[2*step+1] = 0.9f;
    vertex[3*step+0] = -0.7f;
    vertex[3*step+1] = 0.5f;
    // set color
    for (int i=0; i<3; ++i)
    {
        vertex[i*step+3] = (i==0 ? 0.5f : 0.0f);
        vertex[i*step+4] = (i==1 ? 0.5f : 0.0f);
        vertex[i*step+5] = (i==2 ? 0.5f : 0.0f);
    }
    object1->useProgramForRender(program);
    object1->setVertexBuffer(vertex, 4 * step*sizeof(float), VertexTypePosition3 | VertexTypeColor4);
    free(vertex);
    
    // create striangleslist test object
    IObject* object2 = cube->createObject();
    float* rectangle = (float*)malloc(step * 6 * sizeof(float));
    for (int i = 0; i<6 * step; i++)
    {
        rectangle[i] = -1.f;
    }
    rectangle[0 * step + 0] = 0.4f;
    rectangle[0 * step + 1] = 0.9f;
    rectangle[1 * step + 0] = 0.4f;
    rectangle[1 * step + 1] = 0.5f;
    rectangle[2 * step + 0] = 0.7f;
    rectangle[2 * step + 1] = 0.5f;
    rectangle[3 * step + 0] = 0.4f;
    rectangle[3 * step + 1] = 0.9f;
    rectangle[4 * step + 0] = 0.7f;
    rectangle[4 * step + 1] = 0.5f;
    rectangle[5 * step + 0] = 0.7f;
    rectangle[5 * step + 1] = 0.9f;
    // set color
    for (int i = 0; i<6; ++i)
    {
        rectangle[i*step + 3] = (i == 0 ? 0.5f : 0.0f);
        rectangle[i*step + 4] = (i == 1 ? 0.5f : 0.0f);
        rectangle[i*step + 5] = (i == 2 ? 0.5f : 0.0f);
    }
    object2->useProgramForRender(program);
    object2->setVertexBuffer(rectangle, 6 * step*sizeof(float), VertexTypePosition3 | VertexTypeColor4);
    free(rectangle);
    */
    
    // create cube index test object
    IObject* objectIndex = cube->createObject("CubeObject");
    float* eleVertex = (float*)malloc(8*step*sizeof(float));
    for (int i = 0; i<8 * step; i++)
    {
        eleVertex[i] = 0.f;
    }
    eleVertex[0 * step + 0] = 0.2f;
    eleVertex[0 * step + 1] = -0.2f;
    eleVertex[0 * step + 2] = -0.2f;
    eleVertex[0 * step + 7] = 0.0f;
    eleVertex[0 * step + 8] = 0.0f;
    
    eleVertex[1 * step + 0] = 0.2f;
    eleVertex[1 * step + 1] = -0.2f;
    eleVertex[1 * step + 2] = 0.2f;
    eleVertex[1 * step + 7] = 2.0f;
    eleVertex[1 * step + 8] = 2.0f;
    
    eleVertex[2 * step + 0] = -0.2f;
    eleVertex[2 * step + 1] = -0.2f;
    eleVertex[2 * step + 2] = 0.2f;
    eleVertex[2 * step + 7] = 2.0f;
    eleVertex[2 * step + 8] = 0.0f;
    
    eleVertex[3 * step + 0] = -0.2f;
    eleVertex[3 * step + 1] = -0.2f;
    eleVertex[3 * step + 2] = -0.2f;
    eleVertex[3 * step + 7] = 0.5f;
    eleVertex[3 * step + 8] = 2.0f;
    
    eleVertex[4 * step + 0] = 0.2f;
    eleVertex[4 * step + 1] = 0.2f;
    eleVertex[4 * step + 2] = -0.2f;
    eleVertex[4 * step + 7] = 2.0f;
    eleVertex[4 * step + 8] = 2.0f;
    
    eleVertex[5 * step + 0] = 0.2f;
    eleVertex[5 * step + 1] = 0.2f;
    eleVertex[5 * step + 2] = 0.2f;
    eleVertex[5 * step + 7] = 2.0f;
    eleVertex[5 * step + 8] = 0.0f;
    
    eleVertex[6 * step + 0] = -0.2f;
    eleVertex[6 * step + 1] = 0.2f;
    eleVertex[6 * step + 2] = 0.2f;
    eleVertex[6 * step + 7] = 0.0f;
    eleVertex[6 * step + 8] = 0.0f;
    
    eleVertex[7 * step + 0] = -0.2f;
    eleVertex[7 * step + 1] = 0.2f;
    eleVertex[7 * step + 2] = -0.2f;
    eleVertex[7 * step + 7] = 2.0f;
    eleVertex[7 * step + 8] = 0.0f;
    // set cube
    for (int i = 0; i<8; ++i)
    {
        eleVertex[i*step + 3] = ((i % 3) == 0 ? 0.9f : 0.0f);
        eleVertex[i*step + 4] = ((i % 3) == 1 ? 0.7f : 0.0f);
        eleVertex[i*step + 5] = ((i % 3) == 2 ? 0.8f : 0.5f);
        eleVertex[i*step + 6] = 1.0f;
    }
//    objectIndex->useProgramForRender(program);
//    objectIndex->setVertexBuffer(eleVertex, 8 * step*sizeof(float), VertexTypePosition3 | VertexTypeColor4 | VertexTypeUV);
    free(eleVertex);
    // init index buffer
    ushort eleIndex[] = {0, 1, 2, 0, 2, 3,  4, 7, 6, 4, 6, 5,  0, 4, 5, 0, 5, 1,  1, 5, 6, 1, 6, 2,  2, 6, 7, 2, 7, 3,  4, 0, 3, 4, 3, 7};
//    objectIndex->setIndexBuffer(eleIndex, sizeof(eleIndex));

    // start with TestScene
    IPlatform::getSingleton().replaceWithNewScene(new TestScene());
    
    return true;
}

void PlatformDelegate::appDidEnterBackground()
{
    // pause platform render loop
    IPlatform::getSingletonPtr()->pauseAnimating();
}

void PlatformDelegate::appWillEnterForeground()
{
    // resume platform render loop
    IPlatform::getSingletonPtr()->resumeAnimating();
}