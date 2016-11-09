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
#include <vector>
#include "tinyxml2.h"
#include "document.h"
#include "filereadstream.h"

using namespace tinyxml2;
using namespace rapidjson;

typedef struct _AnimRange {
    int start;
    int end;
}AnimRange;

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

XMLElement* depthClone(XMLElement* parent)
{
    XMLElement* resEle = (XMLElement*)parent->ShallowClone(nullptr);
    if (parent->GetText()) {
        resEle->SetText(parent->GetText());
    }
    auto childEle = parent->FirstChildElement();
    while (childEle) {
        auto cloneEle = depthClone(childEle);
        resEle->LinkEndChild(cloneEle);
        childEle = childEle->NextSiblingElement();
    }
    return resEle;
}

int main(int argc, const char * argv[]) {
    if (argc < 3) {
        // print params
        std::cout << "skeleton_upgrade [Peach3D skeleton file] [(c3t file)|(animate split xml)]\n"
        "Animate split xml example:\n"
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<Animations version=\"0.1\">\n"
        "   <Animation name=\"idle\">1, 5</Animation>\n"
        "   <Animation name=\"kick\">6, 20</Animation>\n"
        "</Animations>\n";
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
        do {
            auto rootEle = splitDoc.FirstChildElement();
            if (strcmp(rootEle->Name(), "Animations") !=0 ) {
                std::cout << "animation split file parse error" <<std::endl;
                break;
            }
            // parse animation ranges
            int maxFrameCount = 0;
            std::map<std::string, AnimRange> rangeList;
            auto animEle = rootEle->FirstChildElement();
            while (animEle) {
                // read animation name and range
                auto animName = animEle->Attribute("name");
                AnimRange curRange;
                sscanf(animEle->GetText(), "%d,%d", &curRange.start, &curRange.end);
                rangeList[animName] = curRange;
                if (curRange.end > maxFrameCount) {
                    maxFrameCount = curRange.end;
                }
                animEle = animEle->NextSiblingElement();
            }
            
            // check is frame count match with plist
            auto psRootEle = psDoc.FirstChildElement("Skeleton");
            auto paAnimEle = psRootEle->FirstChildElement("Animation");
            std::vector<XMLElement*> oldAnimList;
            std::vector<XMLElement*> maxFrameList;
            while (paAnimEle) {
                std::vector<XMLElement*> frameList;
                int maxFrameCount = 0;
                auto frameEle = paAnimEle->FirstChildElement();
                while (frameEle) {
                    frameList.push_back(frameEle);
                    maxFrameCount++;
                    frameEle = frameEle->NextSiblingElement();
                }
                // find max frame list
                if (maxFrameCount > maxFrameList.size()) {
                    maxFrameList = frameList;
                }
                oldAnimList.push_back(paAnimEle);
                paAnimEle = paAnimEle->NextSiblingElement();
            }
            if (maxFrameCount != maxFrameList.size()) {
                std::cout << "split xml frame count not match as skeleton file" <<std::endl;
                break;
            }
            std::cout << "start read split file for animations" <<std::endl;
            
            // add split animation
            for (auto iter : rangeList) {
                XMLElement* animEle = psDoc.NewElement("Animation");
                // calc animation time length
                auto startEle = maxFrameList[iter.second.start-1];
                float startTime = startEle->FloatAttribute("time");
                auto endEle = maxFrameList[iter.second.end-1];
                float endTime = endEle->FloatAttribute("time");
                animEle->SetAttribute("length", endTime - startTime);
                animEle->SetAttribute("name", iter.first.c_str());
                std::cout <<"add animation \""<<iter.first<<"\" from "<<iter.second.start<<" to "<<iter.second.end<<std::endl;
                
                // add frames to new animation
                for (auto i=iter.second.start; i<=iter.second.end; ++i) {
                    auto copyEle = depthClone(maxFrameList[i-1]);
                    float oldTime = copyEle->FloatAttribute("time");
                    copyEle->SetAttribute("time", oldTime - startTime);
                    animEle->LinkEndChild(copyEle);
                }
                psRootEle->LinkEndChild(animEle);
            }
            
            // delete all old animation
            for (auto ele : oldAnimList) {
                psRootEle->DeleteChild(ele);
            }
            
            psDoc.SaveFile(psPath);
            std::cout << "split animation over" <<std::endl;
        } while(0);
    }
    return 0;
}
