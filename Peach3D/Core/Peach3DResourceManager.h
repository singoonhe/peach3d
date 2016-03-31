//
//  Peach3DResourceManager.h
//  Peach3DLib
//
//  Created by singoon.he on 9/4/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_RESOURCEMANAGER_H
#define PEACH3D_RESOURCEMANAGER_H

#include "Peach3DCompile.h"
#include "Peach3DSingleton.h"
#include "Peach3DITexture.h"
#include "Peach3DMesh.h"
#include "Peach3DShaderCode.h"

namespace Peach3D
{
    // vertex attr info. Include type, shader name, vertex stride...
    struct PEACH3D_DLL VertexAttrInfo
    {
        VertexAttrInfo() {}
        VertexAttrInfo(uint _type, uint _size, DefaultAttrLocation _locate, const char* _name) :type(_type), size(_size), locate(_locate), name(_name) {}
        uint                type;   // vertex attr type
        uint                size;   // attr bit size
        DefaultAttrLocation locate; // ahder bind location
        std::string         name;   // shader bind name
    };

    class PEACH3D_DLL ResourceManager : public Singleton < ResourceManager >
    {        
    public:
        /** Create texture from file, return texture if file loaded. */
        TexturePtr addTexture(const char* file);
        /** Create texture from encode memory, auto read file format and compress state */
        TexturePtr createTexture(const char* name, void* data, ulong size);
        /** Create texture from decompressed data */
        TexturePtr createTexture(void* data, uint size, int width, int height, TextureFormat format);
        /** Create RTT(render to texture) texture, texture will auto release if not used.
         * @params isDepth Bind depth buffer if true, or bind colors.
         */
        TexturePtr createRenderTexture(int width, int height, bool isDepth = false);
        /**
         * Create Cube texture from six file, return texture if file loaded.
         * Texture name must be set manually.
         */
        TexturePtr addCubeTexture(const char* name, const char* file[6]);
        /** Create texture from six chunk encode memory, auto read file format and compress state */
        TexturePtr createCubeTexture(const char* name, void* dataList[6], ulong sizeList[6]);
        /** Delete texture. */
        void deleteTexture(const TexturePtr& tex);
        /** Return current all RTT(render to texture) textures, used in rendering loop.
         * This function will auto check RTT and release unsed texture.
         */
        const std::vector<TexturePtr>& getRenderTextureList();
        
        /** Create texture frames from file, return frame list in param.
         * Current support TexturePacker general xml export format.
         */
        bool addTextureFrames(const char* file, std::vector<TextureFrame>* outList);
        /** Get texture frame from cache by name, return frame in param. 
         * @params name Name must begin with '#', or return 'addTexture' result.
         */
        bool getTextureFrame(const char* name, TextureFrame* outFrame);
        
        //! set default texture filter, this will effect textures load later.
        // anisotropic need ext in GL, linear will enabled if anisotropic not supported
        void setDefaultTextureFilter(TextureFilter filter) { mTexFilter = filter; }
        //! get default texture filter
        TextureFilter getDefaultTextureFilter() { return mTexFilter; }
        //! set mipmap is enabled when texture created, this will effect textures load later.
        void enableTextureMipMap(bool enabled) { mTexMipMapEnabled = enabled; }
        //! get is mipmap enabled
        bool isTextureMipMapEnabled() { return mTexMipMapEnabled; }
        
        //! create mesh from file, return mesh if file loaded.
        MeshPtr addMesh(const char* file);
        //! create empty mesh
        // will created with system name when name==nullptr
        MeshPtr createMesh(const char* name=nullptr);
        //! delete mesh
        void deleteMesh(const MeshPtr& mesh);
        
        /** Add program from file, windows need set isCompiled=true if use vs compiled.
         * @params vertexType Used to bind attribute or create layout for DX.
         * @params uniformList Use program needed uniforms info.
         * @params isCompiled Is shader had compiled, *.cso file need set true.
        */
        ProgramPtr addProgram(const char* vsFile, const char* psFile, uint vertexType, const std::vector<ProgramUniform>& uniformList, bool isCompiled = false);
        /** Create program from memory, params see @addProgram. */
        ProgramPtr createProgram(const char* vs, const char* ps, uint vertexType, const std::vector<ProgramUniform>& uniformList, ulong vsSize=0, ulong psSize=0, bool isCompiled=false);
        /** Get preset object program in Peach3D system, program will create if not find. */
        ProgramPtr getPresetProgram(const PresetProgramFeatures& feature);
        void deleteProgram(const ProgramPtr& program);
        
        //! get file data and length, nullptr will return if file not exist
        //! Notice: you should free buffer after using.
        uchar* getFileData(const char* relativePath, ulong* size);
        //! set assert manager just for android
        void setAssetsManager(void* manager) { mAssetsManager=manager; }
        /** Add a directory for file search. 
         * @params isBack Push new dir to back or front.
         */
        void addSearchDirectory(const char* dir, bool isBack = true);
        /** Reture all search directorys. */
        const std::vector<std::string>& getSearchDirectory() { return mSearchDirs; }
        
        /** Get default vertex attr info list. */
        static const std::vector<VertexAttrInfo>& getVertexAttrInfoList() { return mVertexAttrList; }
        /** Purge texture and mesh which count is 1, no one hand it. */
        void purgeCachedData();
        
    protected:
        /** Read file data */
        uchar* readFileData(const char* fullPath, ulong* size);
#if PEACH3D_CURRENT_RENDER != PEACH3D_RENDER_DX
        /** Parse image data, return decoded or compressed image info. */
        uchar* parseImageData(void* orignData, uint orignSize, uint* outSize, TextureFormat* format, uint* width, uint* height, TextureDataStatus* status);
#endif
        
    private:
        ResourceManager();
        ~ResourceManager();
        
        ShaderCode*     mPresetShader;      // preset shader codes
        TextureFilter   mTexFilter;         // texture filter, default nearest
        bool            mTexMipMapEnabled;  // texture is mipmap enabled, default false
        
        std::vector<std::string>            mSearchDirs;    // resource search dir list
        std::map<std::string, MeshPtr>      mMeshMap;       // mesh list
        std::map<uint, ProgramPtr>          mProgramMap;    // program list
        std::map<std::string, TexturePtr>   mTextureMap;    // texture list
        std::vector<TexturePtr>             mRTTList;       // RTT texture list, have no name
        std::map<std::string, std::vector<TextureFrame>> mTexFrameMap;  // texture frame cache list
        
        static std::vector<VertexAttrInfo> mVertexAttrList; // all vertex attr info, same in all objects
        void*   mAssetsManager; // save read android apk assert manager
        friend class IPlatform; //! IPlatform can call destructor function.
    };
}

#endif // PEACH3D_RESOURCEMANAGER_H
