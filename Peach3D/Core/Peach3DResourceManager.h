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
        ITexture* addTexture(const char* file);
        /** Create texture from encode memory, auto read file format and compress state */
        ITexture* createTexture(const char* name, void* data, ulong size);
        /**
         * Create Cube texture from six file, return texture if file loaded.
         * Texture name must be set manually.
         */
        ITexture* addCubeTexture(const char* name, const char* file[6]);
        /** Create texture from six chunk encode memory, auto read file format and compress state */
        ITexture* createCubeTexture(const char* name, void* dataList[6], ulong sizeList[6]);
        /** Create texture from decompressed data */
        ITexture* createTexture(void* data, uint size, int width, int height, TextureFormat format);
        /** Delete texture */
        void deleteTexture(ITexture* tex);
        
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
        Mesh* addMesh(const char* file);
        //! create empty mesh
        // will created with system name when name==nullptr
        Mesh* createMesh(const char* name=nullptr);
        //! delete mesh
        void deleteMesh(Mesh* mesh);
        
        /** Add program from file, windows need set isCompiled=true if use vs compiled.
         * @params vertexType Used to bind attribute or create layout for DX.
         * @params uniformList Use program needed uniforms info.
         * @params isCompiled Is shader had compiled, *.cso file need set true.
        */
        IProgram* addProgram(const char* vsFile, const char* psFile, uint vertexType, const std::vector<ProgramUniform>& uniformList, bool isCompiled = false);
        /** Create program from memory, params see @addProgram. */
        IProgram* createProgram(const char* vs, const char* ps, uint vertexType, const std::vector<ProgramUniform>& uniformList, ulong vsSize=0, ulong psSize=0, bool isCompiled=false);
        //! delete program
        void deleteProgram(IProgram* program);
        
        //! get preset object program in Peach3D system, program will create if not find
        IProgram* getPresetProgram(uint verType, const std::string& verName, const std::string& fragName = "");
        
        //! set assert manager just for android
        void setAssetsManager(void* manager) { mAssetsManager=manager; }
        //! add resource search directory
        void addSearchDirectory(const char* path);
        //! get file data and length, nullptr will return if file not exist
        //! Notice: you should free buffer after using.
        uchar* getFileData(const char* relativePath, ulong* size);
        
        //! get default vertex attr info list
        static const std::vector<VertexAttrInfo>& getVertexAttrInfoList() { return mVertexAttrList; }
        
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
        std::map<std::string, ITexture*>    mTextureMap;    // texture list
        std::map<std::string, Mesh*>        mMeshMap;       // mesh list
        std::map<uint, IProgram*>           mProgramMap;    // program list
        
        static std::vector<VertexAttrInfo> mVertexAttrList; // all vertex attr info, same in all objects
        void*   mAssetsManager; // save read android apk assert manager
        friend class IPlatform; //! IPlatform can call destructor function.
    };
}

#endif // PEACH3D_RESOURCEMANAGER_H
