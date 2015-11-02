# delete old files
rm -f AndroidManifest.xml
rm -f ./../../Peach3D/Android.mk

if [[ $1 == 18 ]]; then
    echo "update build target to android 4.3, use opengl es 3.0 default"
    ln -s AndroidManifest-18.xml AndroidManifest.xml
    cp ./../../pro.android/Android-18.mk ./../../Peach3D/Android.mk
    android update project --path . --target android-18
else
    echo "update build target to android 4.0.3, use opengl es 2.0 default"
    ln -s AndroidManifest-15.xml AndroidManifest.xml
    cp ./../../pro.android/Android-15.mk ./../../Peach3D/Android.mk
    android update project --path . --target android-15
fi