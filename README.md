# xmm-unity

Native Unity plugin wrapping the [XMM library](https://github.com/ircam-rnd/xmm)
with build scripts for Android and OSX.

#### installation

Clone this repository with the `--recursive` option to grab XMM as a submodule :  
`git clone --recursive https://github.com/Ircam-RnD/xmm-unity.git`,
or clone it normally and add a symlink to XMM's folder at the root of this repository.

#### compilation

Use the build script with the target platform as an argument :  
`./build.sh android` or `./build.sh macos`  

To build for Android you need to have the Android NDK installed,
and for MacOSX you need to have XCode installed.

#### credits / license

This plugin has been written by the ISMM team at IRCAM, within the context of
the RAPID-MIX project, funded by the European Union’s Horizon 2020 research and
innovation programme.  
The original XMM code has been authored by Jules Françoise.  
See the [XMM repository](https://github.com/ircam-rnd/xmm) for detailed credits
and licensing information (dual GPL).  
