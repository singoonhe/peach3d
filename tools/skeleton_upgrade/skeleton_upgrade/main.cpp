//
//  main.cpp
//  skeleton_upgrade
//
//  Created by singoon.he on 08/11/2016.
//  Copyright © 2016 supermotion. All rights reserved.
//

#include <iostream>
#include <map>
#include <limits>
#include <sstream>
#include <iomanip>
#include "tinyxml2.h"
#include "document.h"
#include "filereadstream.h"

using namespace tinyxml2;
using namespace rapidjson;

int totalBones = 0;
void fixBone(XMLElement* rootBoneEle, const char* name, float* value)
{
    rootBoneEle->SetText("\n");
    auto eleName = rootBoneEle->Attribute("name");
    if (strcmp(eleName, name) == 0) {
        std::cout << "fix bone " <<name<<std::endl;
        totalBones++;
        std::stringstream matStr;
        for (auto i=0; i<15; ++i) {
            matStr<<std::fixed<<std::setprecision(6)<<value[i]<<", ";
        }
        matStr<<std::fixed<<std::setprecision(6)<<value[15];
        
        auto invertEle = rootBoneEle->FirstChildElement("Inverted");
        invertEle->SetText(matStr.str().c_str());
    }
    else {
        auto nextEle = rootBoneEle->FirstChildElement("Bone");
        while (nextEle) {
            fixBone(nextEle, name, value);
            nextEle = nextEle->NextSiblingElement("Bone");
        }
    }
}

int main(int argc, const char * argv[]) {
    if (argc < 3) {
        // print params
        std::cout << "skeleton_upgrade [Peach3D skeleton file] [(c3t file)|(animate split xml)]\n"
        "Animate split xml example:\n"
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<AnimateSplit version=\"0.1\">\n"
        "    <idle>1, 5</idle>\n"
        "    <kick>6, 20</kick>\n"
        "</AnimateSplit>\n";
        return 0;
    }
    
    const char* psPath = argv[1];
    XMLDocument psDoc;
    // load Peach3D skeleton file
    if (psDoc.LoadFile(psPath) != XML_SUCCESS || !psDoc.FirstChildElement("Skeleton")) {
        std::cout << "load Peach3D skeleton file failed" <<std::endl;
        return 0;
    }
    const char* param2 = argv[2];
    XMLDocument splitDoc;
    bool isSplit = splitDoc.LoadFile(param2) == XML_SUCCESS;
    if (!isSplit) {
        FILE* fp = fopen(param2, "r");
        // get file size
        fseek(fp, 0L, SEEK_END);
        long sz = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        // alloc file memory
        char* fd = (char*)malloc(sz * sizeof(char));
        FileReadStream is(fp, fd, sz);
        Document c3tDoc;
        // parse c3t file
        bool notC3t = c3tDoc.ParseStream(is).HasParseError();
        free(fd);
        fclose(fp);
        do {
            if (notC3t && !c3tDoc.HasMember("nodes")) {
                std::cout << "Param 2 not c3t file or split xml" <<std::endl;
                break;
            }
            
            // read pst bone data
            auto rootEle = psDoc.FirstChildElement("Skeleton");
            auto rootBoneEle = rootEle->FirstChildElement("Bone");
            if (!rootBoneEle) {
                std::cout << "skeleton file parse \"Bone\" error" <<std::endl;
                break;
            }
            
            std::cout << "start read c3t file to fix bones invert matrix" <<std::endl;
            // read c3t bone data
            const Value& nodes = c3tDoc["nodes"];
            for (SizeType i = 0; i < nodes.Size(); i++) {
                const Value& nodeValue = nodes[i];
                if (nodeValue["skeleton"].GetBool() == false && nodeValue.HasMember("parts")) {
                    const Value& nodeParts = nodeValue["parts"];
                    for (SizeType k = 0; k < nodeParts.Size(); ++k) {
                        const Value& nodePart = nodeParts[k];
                        if (nodePart.HasMember("bones")) {
                            const Value& nodeBones = nodePart["bones"];
                            // read bone invert matrix
                            for (int j=0; j<nodeBones.Size(); j++) {
                                // read used bone name
                                auto boneName = nodeBones[j]["node"].GetString();
                                float invertMat[16];
                                // read used bone invert transform
                                const Value& boneTranValue = nodeBones[j]["transform"];
                                for (int n=0; n<16; n++) {
                                    invertMat[n] = (float)boneTranValue[n].GetDouble();
                                }
                                // fix Peach3D skeleton bone invert matrix
                                fixBone(rootBoneEle, boneName, invertMat);
                            }
                        }
                    }
                }
            }
            if (totalBones > 0) {
                std::cout << "end fix "<<totalBones<<" bones total" <<std::endl;
                psDoc.SaveFile(psPath);
            }
            else {
                std::cout << "there's no same bones" <<std::endl;
            }
        } while(0);
    }
    else {
        // get animate split data
        std::cout << "start read split file for animations" <<std::endl;
    }
    return 0;
}
