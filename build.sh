platform=$1

if [ $platform == "android" ]
then

cd ./platform/android/jni
ndk-build clean && ndk-build

elif [ $platform == "macos" ] || [ $platform == "ios" ]
then

rm -rf ./platform/$platform/bin/*
xcodebuild -project ./platform/$platform/XmmEngine.xcodeproj

fi