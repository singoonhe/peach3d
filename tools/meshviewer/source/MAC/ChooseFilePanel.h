//
//  ChooseFilePanel.h
//  meshviewer
//
//  Created by singoon.he on 1/30/15.
//  Copyright (c) 2015 singoon. All rights reserved.
//

#ifndef __meshviewer__ChooseFilePanel__
#define __meshviewer__ChooseFilePanel__

#include "Peach3DCompile.h"

// open choose file pannel, callback when file selected
void openChooseFilePannel(const std::vector<std::string>& extList, std::function<void(const char* file)> chooseCallFunc);

#endif /* defined(__meshviewer__ChooseFilePanel__) */
