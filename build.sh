platform=$1

if [ $platform == "android" ]
then
cd ./platform/android/jni

ndk-build clean && ndk-build

# cd ../libs

# for i in $(ls -d */); do
# 	ff=$(echo ${i%%/});
# 	mkdir -p ../../../../Assets/Plugins/Android-"$ff"
# 	cp -r "$ff"/* ../../../../Assets/Plugins/Android-"$ff"
# done

elif [ $platform == "macos" ]
then
#xcodebuild something
rm -rf ./platform/macos/bin/XmmEngine.bundle
xcodebuild -project ./platform/macos/XmmEngine.xcodeproj

# mkdir -p ../Assets/Plugins/apple
# cp -r ./platform/apple/bin/XMMEngine.bundle ../Assets/Plugins/apple
# rm -rf ./platform/apple/build

fi