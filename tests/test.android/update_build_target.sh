# delete old files
rm -f app/src/main/AndroidManifest.xml
rm -f ./../../Peach3D/Android.mk

if [[ $1 == 3 ]]; then
    echo "update build min to android 4.3, use opengl es 3.0 default"
    cp AndroidManifest-V3.xml app/src/main/AndroidManifest.xml
    cp ./../../pro.android/Android-V3.mk ./../../Peach3D/Android.mk
    cp Application-V3.mk ./jni/Application.mk
else
    echo "update build min to android 4.0.3, use opengl es 2.0 default"
    cp AndroidManifest-V2.xml app/src/main/AndroidManifest.xml
    cp ./../../pro.android/Android-V2.mk ./../../Peach3D/Android.mk
    cp Application-V2.mk ./jni/Application.mk
fi