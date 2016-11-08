//
//  main.cpp
//  skeleton_upgrade
//
//  Created by singoon.he on 08/11/2016.
//  Copyright © 2016 supermotion. All rights reserved.
//

#include <iostream>
#include "tinyxml2.h"
#include "document.h"
#include "filereadstream.h"

using namespace tinyxml2;
using namespace rapidjson;

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
    if (psDoc.LoadFile(psPath) != XML_SUCCESS) {
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
            if (notC3t) {
                std::cout << "Param 2 not c3t file or split xml" <<std::endl;
                break;
            }
            
            // read c3t skeleton data
        } while(0);
    }
    else {
        // get animate split data
    }
    return 0;
}
