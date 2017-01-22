# xmm-unity

Native Unity plugin wrapping the [XMM library](https://github.com/ircam-rnd/xmm)
with build scripts for Android, OSX and iOS.

#### installation

Clone this repository with the `--recursive` option to grab XMM as a submodule :  
`git clone --recursive https://github.com/Ircam-RnD/xmm-unity.git`  
Or clone it normally and add a symlink to XMM's folder at the root of this
repository.

#### compilation

Use the build script with the target platform as an argument :  
`./build.sh android`, `./build.sh macos` or `./build.sh ios`  

To build for Android you need to have the Android NDK installed.  
To build for MacOSX and iOS you need to have XCode installed.

#### usage

To use the plugin, drop the built binary file into the `Assets/Plugins`
folder of your Unity project, and the C# wrapper code into the `Assets/Scripts`
folder. You will then be able to instantiate `XmmTrainingSet` and `XmmModel`
classes.  
You can find some examples of how to use the plugin
[here](https://github.com/ircam-rnd/xmm-unity-examples).

#### credits

This plugin is developed by the [ISMM](http://ismm.ircam.fr/) team at IRCAM,
within the context of the [RAPID-MIX](http://rapidmix.goldsmithsdigital.com/)
project, funded by the European Union’s Horizon 2020 research and innovation
programme.  
Original XMM code authored by Jules Françoise, ported to Unity by
Joseph Larralde.  
See [github.com/Ircam-RnD/xmm](https://github.com/Ircam-RnD/xmm)
for detailed XMM credits.
