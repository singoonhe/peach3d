#!/usr/bin/env bash
size_target=("32" "./../tests/test.android/res/drawable-ldpi/icon.png"
    "48" "./../tests/test.android/res/drawable-mdpi/icon.png"
    "72" "./../tests/test.android/res/drawable-hdpi/icon.png"
    "96" "./../tests/test.android/res/drawable-xhdpi/icon.png"
    "144" "./../tests/test.android/res/drawable-xxhdpi/icon.png"
    "29" "./../tests/test.mac_ios/test_ios/Images.xcassets/AppIcon.appiconset/Icon-29.png"
    "58" "./../tests/test.mac_ios/test_ios/Images.xcassets/AppIcon.appiconset/Icon-29@2x.png"
    "87" "./../tests/test.mac_ios/test_ios/Images.xcassets/AppIcon.appiconset/Icon-29@3x.png"
    "40" "./../tests/test.mac_ios/test_ios/Images.xcassets/AppIcon.appiconset/Icon-40.png"
    "80" "./../tests/test.mac_ios/test_ios/Images.xcassets/AppIcon.appiconset/Icon-40@2x.png"
    "120" "./../tests/test.mac_ios/test_ios/Images.xcassets/AppIcon.appiconset/Icon-40@3x.png"
    "120" "./../tests/test.mac_ios/test_ios/Images.xcassets/AppIcon.appiconset/Icon-60@2x.png"
    "180" "./../tests/test.mac_ios/test_ios/Images.xcassets/AppIcon.appiconset/Icon-60@3x.png"
    "76" "./../tests/test.mac_ios/test_ios/Images.xcassets/AppIcon.appiconset/Icon-76.png"
    "152" "./../tests/test.mac_ios/test_ios/Images.xcassets/AppIcon.appiconset/Icon-76@2x.png"
    "16" "./../tests/test.mac_ios/test_mac/Images.xcassets/AppIcon.appiconset/Icon-16.png"
    "32" "./../tests/test.mac_ios/test_mac/Images.xcassets/AppIcon.appiconset/Icon-16@2x.png"
    "32" "./../tests/test.mac_ios/test_mac/Images.xcassets/AppIcon.appiconset/Icon-32.png"
    "64" "./../tests/test.mac_ios/test_mac/Images.xcassets/AppIcon.appiconset/Icon-32@2x.png"
    "128" "./../tests/test.mac_ios/test_mac/Images.xcassets/AppIcon.appiconset/Icon-128.png"
    "256" "./../tests/test.mac_ios/test_mac/Images.xcassets/AppIcon.appiconset/Icon-128@2x.png"
    "256" "./../tests/test.mac_ios/test_mac/Images.xcassets/AppIcon.appiconset/Icon-256.png"
    "512" "./../tests/test.mac_ios/test_mac/Images.xcassets/AppIcon.appiconset/Icon-256@2x.png"
    "512" "./../tests/test.mac_ios/test_mac/Images.xcassets/AppIcon.appiconset/Icon-512.png"
    "1024" "./../tests/test.mac_ios/test_mac/Images.xcassets/AppIcon.appiconset/Icon-512@2x.png"
    "150" "./../tests/test.winstore/test/test.Windows/Assets/Logo.scale-100.png"
    "30" "./../tests/test.winstore/test/test.Windows/Assets/SmallLogo.scale-100.png"
    "50" "./../tests/test.winstore/test/test.Windows/Assets/StoreLogo.scale-100.png"
    "360" "./../tests/test.winstore/test/test.WindowsPhone/Assets/Logo.scale-240.png"
    "106" "./../tests/test.winstore/test/test.WindowsPhone/Assets/SmallLogo.scale-240.png"
    "170" "./../tests/test.winstore/test/test.WindowsPhone/Assets/Square71x71Logo.scale-240.png"
    "120" "./../tests/test.winstore/test/test.WindowsPhone/Assets/StoreLogo.scale-240.png")

# 生成目标资源函数
function update_icon()
 {
    size=$1
    res_name=$2
    out_name=$3
    sips -s format png -z $size $size $res_name --out $out_name
 }

# 拷贝各目标
for key in ${!size_target[@]}; do
    if [[ `expr $key % 2` == "1" ]]; then
        size=`expr $key - 1`
        update_icon ${size_target[$size]} "peach-icon.png" ${size_target[$key]}
    fi
done
