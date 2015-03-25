# app-template
This template was planned as wrapper over graphical applications written with `C/C++` which allows to compile them for different platforms.
Now only two types of platforms are supported: `Desktop`(`Linux`, `Mac`, `Windows` and etc.) and `Android`.
For development for `Android` platform you need `android-sdk` and `android-ndk` tools.

You can build and run application using scripts from project directory: 
+ `desktop/build` or `android/build` builds the project. `.apk` file for `Android` platform is located in `android/bin/` directory. Binary file for `Desktop` platforms is in `desktop/` directory.
+ `desktop/run` or `android/run [device]` runs the builded application.

Source code must be placed in `source/` directory. Build settings (compiler flags, libraries, files to compile) must be written in `build-info.mk` file. `android/update` must be runned once to create necessary files.
