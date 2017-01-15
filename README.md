# xmm-unity

Unity wrapper of the [XMM library](https://github.com/ircam-rnd/xmm) with build
scripts for Android and OSX.

#### installation

Clone this repository with the `--recursive` option to grab XMM as a submodule :
`git clone --recursive https://github.com/Ircam-RnD/xmm-unity.git`,
or clone it normally and add a symlink to XMM's folder at the root of the repository.

#### compilation

Use the build script with the target platform as an argument :
`./build.sh android` or `./build.sh macos`  

To build for Android you need to have the Android NDK installed,
and for MacOSX you need to have XCode installed.