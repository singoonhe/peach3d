//
//  Peach3DResourceManager.cpp
//  Peach3DLib
//
//  Created by singoon.he on 9/4/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include <stdio.h>
#include "Peach3DResourceManager.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DIPlatform.h"
#include "Peach3DObjLoader.h"
#include "Peach3DShaderCode.h"
#include "xxhash/xxhash.h"
#include "tinyxml2/tinyxml2.h"
#if (PEACH3D_CURRENT_RENDER != PEACH3D_RENDER_DX)
#include "Peach3DImageParse.h"
#endif
#if PEACH3D_CURRENT_PLATFORM == PEACH3D_PLATFORM_ANDROID
#include <android/native_activity.h>
#endif

namespace Peach3D
{
    IMPLEMENT_SINGLETON_STATIC(ResourceManager);
    // defind static all vertex attr info
    std::vector<VertexAttrInfo> ResourceManager::mVertexAttrList;
    
    ResourceManager::ResourceManager() : mTexFilter(TextureFilter::eLinear), mTexMipMapEnabled(false)
    {
        mPresetShader = new ShaderCode();
        // init vertex attri list
        if (mVertexAttrList.size() == 0) {
            mVertexAttrList.push_back(VertexAttrInfo(VertexType::Point2, 2 * sizeof(float), DefaultAttrLocation::eVertex ,pdShaderVertexAttribName));
            mVertexAttrList.push_back(VertexAttrInfo(VertexType::Point3, 3 * sizeof(float), DefaultAttrLocation::eVertex ,pdShaderVertexAttribName));
            mVertexAttrList.push_back(VertexAttrInfo(VertexType::Normal, 3 * sizeof(float), DefaultAttrLocation::eNormal, pdShaderNormalAttribName));
            mVertexAttrList.push_back(VertexAttrInfo(VertexType::PSize, sizeof(float), DefaultAttrLocation::ePSize, pdShaderPSizeAttribName));
            mVertexAttrList.push_back(VertexAttrInfo(VertexType::UV, 2 * sizeof(float), DefaultAttrLocation::eUV, pdShaderUVAttribName));
        }
        /* add default search dir in platform instance, most platform use different dir */
    }
    
    ResourceManager::~ResourceManager()
    {
        // release all resource
        IRender* render = IRender::getSingletonPtr();
        for (auto iter=mTextureMap.begin(); iter!=mTextureMap.end(); iter++) {
            render->deleteTexture(iter->second);
        }
        for (auto iter=mMeshMap.begin(); iter!=mMeshMap.end(); iter++) {
            delete iter->second;
        }
        for (auto iter=mProgramMap.begin(); iter!=mProgramMap.end(); iter++) {
            render->deleteProgram(iter->second);
        }
        // free shader code data
        mPresetShader->freeShaderCodeData();
    }
    
    ITexture* ResourceManager::addTexture(const char* file)
    {
        ITexture* newTexture = nullptr;
        if (mTextureMap.find(file) != mTextureMap.end()) {
            newTexture = mTextureMap[file];
        }
        else {
            ulong texLength = 0;
            // get texture file data
            uchar *texData = getFileData(file, &texLength);
            if (texLength > 0 && texData) {
                // webp header max length = 12
                if (texLength > 12) {
                    // create texutre from memory
                    newTexture = createTexture(file, texData, texLength);
                }
                else {
                    Peach3DLog(LogLevel::eError, "Texture data is too short, create texture failed");
                }
                // release memory data
                free(texData);
            }
        }
        return newTexture;
    }
    
    bool ResourceManager::addTextureFrames(const char* file, std::vector<TextureFrame>* outList)
    {
        bool loadRes = false;
        if (mTexFrameMap.find(file) != mTexFrameMap.end()) {
            if (outList) {
                *outList = mTexFrameMap[file];
            }
            loadRes = true;
        }
        else {
            ulong len = 0;
            // get texture file data
            uchar *content = getFileData(file, &len);
            if (content && len > 0) {
                tinyxml2::XMLDocument readDoc;
                if (readDoc.Parse((const char*)content, len) == tinyxml2::XML_SUCCESS) {
                    do {
                        std::vector<TextureFrame> frameList;
                        tinyxml2::XMLElement* layoutEle = readDoc.FirstChildElement("TextureAtlas");
                        IF_BREAK(!layoutEle, "Texture packer file \"%s\" format error", file);
                        
                        // check texture loading and size
                        auto imageFile = layoutEle->Attribute("imagePath");
                        auto tex = addTexture(imageFile);
                        auto tw = atoi(layoutEle->Attribute("width"));
                        auto th = atoi(layoutEle->Attribute("height"));
                        IF_BREAK(!tex || tw!=tex->getWidth() || th!=tex->getHeight(), "Texture packer file \"%s\" data error", file);
                        
                        auto childElement = layoutEle->FirstChildElement();
                        while (childElement) {
                            auto name = childElement->Attribute("n");
                            auto x = atoi(childElement->Attribute("x"));
                            auto y = atoi(childElement->Attribute("y"));
                            auto w = atoi(childElement->Attribute("w"));
                            auto h = atoi(childElement->Attribute("h"));
                            // save one frame
                            frameList.push_back(TextureFrame(tex, Rect(x/(float)tw, (th - y - h)/(float)th, w/(float)tw, h/(float)th), name));
                            childElement = childElement->NextSiblingElement();
                        }
                        
                        mTexFrameMap[file] = frameList;
                        if (outList) {
                            *outList = frameList;
                        }
                        Peach3DLog(LogLevel::eInfo, "Load new texture packer %s success", file);
                        loadRes = true;
                    } while (0);
                }
                // release memory data
                free(content);
            }
        }
        return loadRes;
    }
    
    bool ResourceManager::getTextureFrame(const char* name, TextureFrame* outFrame)
    {
        bool isFind = false;
        if (strlen(name) > 0){
            const char* frameName = name;
            if (name[0] == '#') {
                frameName = name + 1;
                // find frame in cache
                for (auto list : mTexFrameMap) {
                    for (auto frame : list.second) {
                        if (frame.name == frameName) {
                            if (outFrame) {
                                *outFrame = frame;
                            }
                            isFind = true;
                            break;
                        }
                    }
                    if (isFind) {
                        break;
                    }
                }
            }
            else {
                ITexture* loadTex = addTexture(name);
                if (loadTex) {
                    isFind = true;
                    if (outFrame) {
                        *outFrame = TextureFrame(loadTex);
                    }
                }
            }
        }
        return isFind;
    }

#if PEACH3D_CURRENT_RENDER != PEACH3D_RENDER_DX
    uchar* ResourceManager::parseImageData(void* orignData, uint orignSize, uint* outSize, TextureFormat* format, uint* width, uint* height, TextureDataStatus* status)
    {
        uchar* texBuffer = nullptr;
        static const unsigned char JPG_SOI[] = { 0xFF, 0xD8 };
        static const unsigned char PNG_SIGNATURE[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
        if (memcmp(orignData, JPG_SOI, 2) == 0) {
            // add jpeg texture file
            texBuffer = jpegImageDataParse(orignData, (uint)orignSize, outSize, format, width, height);
        }
        else if (memcmp(PNG_SIGNATURE, orignData, sizeof(PNG_SIGNATURE)) == 0) {
            // add png texture file
            texBuffer = pngImageDataParse(orignData, (uint)orignSize, outSize, format, width, height);
        }
        else {
            Peach3DWarnLog("Texture size %d data format not supported!", orignSize);
        }
        /*
        else if (strncmp(name, "DDS", 3) == 0)
        {
            texBuffer = (uchar*)orignData;
            dataStatus = TextureDataStatus::eCompressed;
        }*/
        return texBuffer;
    }
#endif
    
    ITexture* ResourceManager::createTexture(const char* name, void* data, ulong size)
    {
        // release old texture
        if (mTextureMap.find(name) != mTextureMap.end()) {
            deleteTexture(mTextureMap[name]);
        }
        
        ITexture *texture = nullptr;
        bool isTexDataValid = false;
#if PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_DX
        bool isCompressTex = false;
        texture = IRender::getSingletonPtr()->createTexture(name);
        isTexDataValid = texture->setTextureData(data, (uint)size, TextureDataStatus::eEncoded);
#else
        TextureDataStatus dataStatus = TextureDataStatus::eDecoded;
        uchar* texBuffer = nullptr;
        TextureFormat texFormat = TextureFormat::eUnknow;
        uint texSize = 0, texWidth = 0, texHeight = 0;
        texBuffer = parseImageData(data, (uint)size, &texSize, &texFormat, &texWidth, &texHeight, &dataStatus);
        
        // create texture if parse data success
        if (texBuffer && texSize > 0) {
            texture = IRender::getSingletonPtr()->createTexture(name);
            texture->setFormat(texFormat);
            texture->setWidth(texWidth);
            texture->setHeight(texHeight);
            // create texture with buffer
            isTexDataValid = texture->setTextureData(texBuffer, texSize, dataStatus);
            free(texBuffer);
        }
#endif
        if (texture) {
            if (!isTexDataValid){
                IRender::getSingletonPtr()->deleteTexture(texture);
                texture = nullptr;
            }
            else {
                mTextureMap[name] = texture;
                Peach3DLog(LogLevel::eInfo, "Create new texture %s success, with:%d height:%d", name, texture->getWidth(), texture->getHeight());
            }
        }
        return texture;
    }
    
    ITexture* ResourceManager::addCubeTexture(const char* name, const char* file[6])
    {
        ITexture* newTexture = nullptr;
        if (mTextureMap.find(name) != mTextureMap.end()) {
            newTexture = mTextureMap[name];
        }
        else {
            bool loadSuccess = true;
            ulong texLength[6] = {0, 0, 0, 0, 0, 0};
            uchar *texData[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
            for (auto i = 0; i < 6; ++i) {
                // get texture file data
                texData[i] = getFileData(file[i], &texLength[i]);
                if (texLength[i] <= 12 || !texData[i] ) {
                    loadSuccess = false;
                    break;
                }
            }
            if (loadSuccess) {
                // create texutre from memory
                newTexture = createCubeTexture(name, (void**)texData, texLength);
            }
            // release all memory data
            for (auto i = 0; i < 6; ++i) {
                if (texData[i]) {
                    free(texData[i]);
                }
            }
        }
        return newTexture;
    }

    ITexture* ResourceManager::createCubeTexture(const char* name, void* dataList[6], ulong sizeList[6])
    {
        // release old texture
        if (mTextureMap.find(name) != mTextureMap.end()) {
            deleteTexture(mTextureMap[name]);
        }
        
        ITexture *texture = nullptr;
        bool isTexDataValid = false;
#if PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_DX
        bool isCompressTex = false;
        texture = IRender::getSingletonPtr()->createTexture(name);
        uint intsList[6] = { sizeList[0], sizeList[1], sizeList[2], sizeList[3], sizeList[4], sizeList[5]};
        isTexDataValid = texture->setTextureData(dataList, intsList, TextureDataStatus::eEncoded);
#else
        TextureDataStatus dataStatus = TextureDataStatus::eDecoded;
        uchar* texBuffer[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
        uint texSize[6] = {0, 0, 0, 0, 0, 0};
        TextureFormat texFormat = TextureFormat::eUnknow;
        uint texWidth = 0, texHeight = 0;
        bool isAllValid = true;
        for (auto i = 0; i < 6; ++i) {
            texBuffer[i] = parseImageData(dataList[i], (uint)sizeList[i], &texSize[i], &texFormat, &texWidth, &texHeight, &dataStatus);
            if (!texBuffer[i] || !texSize[i]) {
                isAllValid = false;
                break;
            }
        }
        // create texture if parse all data success
        if (isAllValid) {
            texture = IRender::getSingletonPtr()->createTexture(name);
            texture->setFormat(texFormat);
            texture->setWidth(texWidth);
            texture->setHeight(texHeight);
            // create texture with six chunk buffers
            isTexDataValid = texture->setTextureData((void**)texBuffer, texSize, dataStatus);
        }
        for (auto i = 0; i < 6; ++i) {
            if (texBuffer[i]) {
                free(texBuffer[i]);
            }
        }
#endif
        if (texture) {
            if (!isTexDataValid){
                IRender::getSingletonPtr()->deleteTexture(texture);
                texture = nullptr;
            }
            else {
                mTextureMap[name] = texture;
                Peach3DLog(LogLevel::eInfo, "Create new Cube texture %s success, with:%d height:%d", name, texture->getWidth(), texture->getHeight());
            }
        }
        return texture;
    }
    
    ITexture* ResourceManager::createTexture(void* data, uint size, int width, int height, TextureFormat format)
    {
        char pName[100] = { 0 };
        static uint texAutoCount = 0;
        sprintf(pName, "pd_Texture%d", texAutoCount++);
        // create texture with inner name
        ITexture *texture = IRender::getSingletonPtr()->createTexture(pName);
        texture->setFormat(format);
        texture->setWidth(width);
        texture->setHeight(height);
        // create texture with buffer, data have decoded
        if (texture->setTextureData(data, size, TextureDataStatus::eDecoded)) {
            mTextureMap[pName] = texture;
        }
        else {
            IRender::getSingletonPtr()->deleteTexture(texture);
            texture = nullptr;
        }
        return texture;
    }
    
    void ResourceManager::deleteTexture(ITexture* tex)
    {
        Peach3DAssert(tex, "Can't delete a null texture!");
        if (tex) {
            auto nameIter = mTextureMap.find(tex->getName());
            if (nameIter != mTextureMap.end()) {
                mTextureMap.erase(nameIter);
            }
            // delete texture always
            IRender::getSingletonPtr()->deleteTexture(tex);
        }
    }
    
    Mesh* ResourceManager::addMesh(const char* file)
    {
        if (mMeshMap.find(file) != mMeshMap.end()) {
            return mMeshMap[file];
        }
        else
        {
            Mesh* fileMesh = nullptr;
            ulong fileLength = 0;
            // get texture file data
            uchar *fileData = getFileData(file, &fileLength);
            if (fileLength > 0 && fileData) {
                // use file name to create mesh
                fileMesh = createMesh(file);
                // get file name suffix
                std::string fileName = file;
                std::string ext = fileName.substr(fileName.rfind('.') == std::string::npos ? fileName.length() : fileName.rfind('.') + 1);
                for (size_t i=0; i<ext.size(); ++i) {
                    ext[i] = tolower(ext[i]);
                }
                // load obj file data, add to mesh
                bool loadResult = false;
                if (ext.compare("obj") == 0) {
                    std::string fileDir = fileName.substr(0, fileName.rfind('/') == std::string::npos ? 0 : fileName.rfind('/')+1);
                    loadResult = ObjLoader::objMeshDataParse(fileData, fileLength, fileDir, fileMesh);
                }
                else {
                    Peach3DLog(LogLevel::eError, "Did't support the mesh format \"%s\"", ext.c_str());
                }
                if (loadResult) {
                    Peach3DLog(LogLevel::eInfo, "Load mesh from file \"%s\" success", file);
                }
                // release memory data
                free(fileData);
            }
            
            return fileMesh;
        }
    }
    
    Mesh* ResourceManager::createMesh(const char* name)
    {
        if (name && (mMeshMap.find(name) != mMeshMap.end())) {
            return mMeshMap[name];
        }
        else {
            const char* meshName = name;
            if (!meshName) {
                // generate mesh name if name==nullptr
                char pName[100] = { 0 };
                static uint meshAutoCount = 0;
                sprintf(pName, "pd_Mesh%d", meshAutoCount++);
                meshName = pName;
                Peach3DLog(LogLevel::eInfo, "Create mesh with default name \"%s\"", meshName);
            }
            Mesh* newMesh = new Mesh(meshName);
            mMeshMap[meshName] = newMesh;
            return newMesh;
        }
    }
    
    void ResourceManager::deleteMesh(Mesh* mesh)
    {
        Peach3DAssert(mesh, "Can't delete a null mesh!");
        if (mesh) {
            auto nameIter = mMeshMap.find(mesh->getName());
            if (nameIter != mMeshMap.end()) {
                mMeshMap.erase(nameIter);
                delete mesh;
            }
        }
    }
    
    IProgram* ResourceManager::addProgram(const char* vsFile, const char* psFile, uint vertexType, const std::vector<ProgramUniform>& uniformList, bool isCompiled)
    {
        IProgram* newPrograme = nullptr;
        
        ulong vsLength = 0, psLength = 0;
        // get vs file data
        uchar *vsData = getFileData(vsFile, &vsLength);
        if (vsLength > 0 && vsData) {
            // get ps file data
            uchar *psData = getFileData(psFile, &psLength);
            if (psLength > 0 && psData) {
                // return program
                newPrograme = createProgram((const char*)vsData, (const char*)psData, vertexType, uniformList, vsLength, psLength, isCompiled);
                // free pixel file data
                free(psData);
            }
            // free vertex file data
            free(vsData);
        }
        // return new program
        return newPrograme;
    }
    
    IProgram* ResourceManager::createProgram(const char* vs, const char* ps, uint vertexType, const std::vector<ProgramUniform>& uniformList, ulong vsSize, ulong psSize, bool isCompiled)
    {
        IRender* render = IRender::getSingletonPtr();
        // generate program unique id
        static uint programAutoCount = 0;
        char pName[100] = {0};
        sprintf(pName, "pd_Program%d", programAutoCount++);
        uint hashPID = XXH32((void*)pName, (int)strlen(pName), 0);
        
        // create program
        IProgram* program = render->createProgram(hashPID);
        // add shader to program
        program->setVertexShader(vs, (vsSize==0) ? (int)strlen(vs) : (int)vsSize, isCompiled);
        program->setPixelShader(ps, (psSize==0) ? (int)strlen(ps) : (int)psSize, isCompiled);
        // set vertex type
        program->setVertexType(vertexType);
        // set object uniform
        program->setProgramUniformsDesc(uniformList);
        
        if (program->isProgramValid()) {
            Peach3DLog(LogLevel::eInfo, "Create new user program %u success", hashPID);
        }
        mProgramMap[hashPID] = program;
        return program;
    }
    
    void ResourceManager::deleteProgram(IProgram* program)
    {
        Peach3DAssert(program, "Can't delete a null program!");
        if (program) {
            auto idIter = mProgramMap.find(program->getProgramId());
            if (idIter != mProgramMap.end()) {
                mProgramMap.erase(idIter);
                IRender::getSingletonPtr()->deleteProgram(program);
            }
        }
    }
    
    IProgram* ResourceManager::getPresetProgram(uint verType, const std::string& verName, const std::string& fragName)
    {
        ShaderCodeData verData = mPresetShader->getShaderCode(verName);
        ShaderCodeData fragData = mPresetShader->getShaderCode(fragName);
        if (verData.size > 0) {
            std::string name = verName + fragName;
            uint hashPID = XXH32((void*)name.c_str(), (int)name.size(), 0);
            
            if (mProgramMap.find(hashPID) != mProgramMap.end()) {
                return mProgramMap[hashPID];
            }
            else {
                // create a new preset program
                IProgram* program = IRender::getSingletonPtr()->createProgram(hashPID);
                // add shader code to program
                program->setVertexShader(verData.data, verData.size, false);
                program->setPixelShader(fragData.data, fragData.size, false);

                // set vertex type
                program->setVertexType(verType);
                // set object uniform
                program->setProgramUniformsDesc(mPresetShader->getProgramUniforms(verName));
                
                if (program->isProgramValid()) {
                    Peach3DLog(LogLevel::eInfo, "Create new preset program \"%s\" success", name.c_str());
                }
                mProgramMap[hashPID] = program;
                return program;
            }
        }
        return nullptr;
    }
        
    void ResourceManager::addSearchDirectory(const char* dir, bool isBack)
    {
        std::string newPath = dir;
        char last_char = newPath[newPath.length()-1];
        if (last_char!='/' && last_char!='\\') {
            newPath = newPath + "/";
        }
        for (auto iter=mSearchDirs.begin(); iter!=mSearchDirs.end(); iter++) {
            if (iter->compare(newPath) == 0) {
                return ;
            }
        }
        
        // add new dir to back or front
        if (isBack) {
            mSearchDirs.push_back(newPath);
        }
        else {
            mSearchDirs.insert(mSearchDirs.begin(), newPath);
        }
        Peach3DLog(LogLevel::eInfo, "Add search directory: %s", newPath.c_str());
    }
    
    uchar* ResourceManager::getFileData(const char* relativePath, ulong* size)
    {
        uchar* fileBuffer = nullptr;
        std::string filePath = relativePath;
        if (relativePath[0] == '/' || relativePath[2] == '\\') {
            // read file direct
            fileBuffer = readFileData(relativePath, size);
        }
        else {
            for (auto dir = mSearchDirs.begin(); dir != mSearchDirs.end(); dir++) {
                fileBuffer = readFileData((*dir+relativePath).c_str(), size);
                if (fileBuffer) {
                    // read file success, return
                    break ;
                }
                else {
                    // can't read file, try from other dir
                    continue ;
                }
            }
        }
        // is file readed ?
        if (!fileBuffer) {
            Peach3DLog(LogLevel::eError, "Can't find file \"%s\" in search dir list", relativePath);
        }
        return fileBuffer;
    }
    
    uchar* ResourceManager::readFileData(const char* fullPath, ulong* size)
    {
        uchar* fileBuffer = nullptr;
#if PEACH3D_CURRENT_PLATFORM == PEACH3D_PLATFORM_ANDROID
        if (strncmp(fullPath, "assets/", strlen("assets/")) == 0) {
            AAssetManager* assetManager = (AAssetManager*)mAssetsManager;
            if (!mAssetsManager) {
                // AsstsManager not inited, read file failed
                Peach3DLog(LogLevel::eError, "Can't find android assets manager, it's need set when PlatformAndroid init");
                return fileBuffer;
            }
            // get relative path
            std::string relativePath = std::string(fullPath).substr(strlen("assets/"));
            // read file from andriod assets
            AAsset* asset = AAssetManager_open(assetManager, relativePath.c_str(), AASSET_MODE_BUFFER);
            if (nullptr == asset) {
                return fileBuffer;
            }
            // get file length
            off_t fileSize = AAsset_getLength(asset);
            // read buffer
            fileBuffer = (unsigned char*) malloc(fileSize + 1);
            int bytesread = AAsset_read(asset, (void*)fileBuffer, fileSize);
            if (size) {
                *size = bytesread;
            }
            fileBuffer[fileSize] = '\0';
    
            AAsset_close(asset);
            return fileBuffer;
        }
#endif
        FILE *file = fopen(fullPath, "rb");
        if (file) {
            // get file length
            fseek(file, 0, SEEK_END);
            ulong fileSize = ftell(file);
            // read file buffer
            fseek(file,0,SEEK_SET);
            fileBuffer = (unsigned char*) malloc(fileSize + 1);
            fileSize = (ulong)fread(fileBuffer, sizeof(unsigned char), fileSize, file);
            fclose(file);
            if (size) {
                *size = fileSize;
            }
            fileBuffer[fileSize] = '\0';
        }
        return fileBuffer;
    }
}