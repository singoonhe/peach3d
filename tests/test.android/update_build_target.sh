# delete old files
rm -f AndroidManifest.xml
rm -f ./../../Peach3D/Android.mk

if [[ $1 == 3 ]]; then
    echo "update build min to android 4.3, use opengl es 3.0 default"
    ln -s AndroidManifest-V3.xml AndroidManifest.xml
    cp ./../../pro.android/Android-V3.mk ./../../Peach3D/Android.mk
    cp Application-V3.mk ./jni/Application.mk
else
    echo "update build min to android 4.0.3, use opengl es 2.0 default"
    ln -s AndroidManifest-V2.xml AndroidManifest.xml
    cp ./../../pro.android/Android-V2.mk ./../../Peach3D/Android.mk
    cp Application-V2.mk ./jni/Application.mk
fi