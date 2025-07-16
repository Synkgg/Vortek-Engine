
![Image](https://github.com/user-attachments/assets/8e07697b-d2be-42e2-aefe-538ffa82f040)


# Overview
Vortek Engine is based on the Youtube Series "Scion2D" From 
[Dustin Clark](https://github.com/dwjclark11). His Youtube is,
[Here](https://www.youtube.com/playlist?list=PL3HUvSWOJR7XRDwVVQqqWO-zyyscb8L-v) I recommend leaving him a subscribe and joining his discord.

---- 

# How to setup
----
Requires [CMake 3.26 or Later](https://cmake.org/) and [vcpkg](https://github.com/microsoft/vcpkg), Make sure to open a powershell window and not a command prompt.
## Get VCPKG:
```ps
git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.bat -disableMetrics
```
### Make sure the following environment variables are set:
```
VCPKG_ROOT=[path_to_vcpkg]
VCPKG_DEFAULT_TRIPLET=x64-windows
```
- Windows

	Add the following line in your Path environnment variable:
	```
	<path_to_vcpkg_installation_folder>
	```

	In your powershell window paste/type:
	```
	vcpkg integrate install
	vcpkg integrate powershell
	```
 - Linux

	Edit your profile's bashrc file:
	```
	nano ~/.bashrc
	```
	Add the following lines at the end:
	```
	export PATH=<path_to_vcpkg_installation_folder>:$PATH
	export VCPKG_ROOT=<path_to_vcpkg_installation_folder>
	export VCPKG_DEFAULT_TRIPLET=x64-linux
	```
	Apply changes:
	```
	source ~/.bashrc
	```

	Open a terminal and type the following:
	```
	vcpkg integrate install
	vcpkg integrate bash
	```
	
## Install dependencies 
- Windows
	```
	vcpkg install fmt glm entt glad soil2 sdl2 sdl2-mixer[mpg123] lua sol2 stb tinyfiledialogs rapidjson
	```
- Linux[debian based]
	```
	sudo apt install python-jinja2 autoconf automake libtool pkg-config libibus-1.0-dev
	```
	* if[Xorg]
		 ```
		sudo apt install libx11-dev libxft-dev libxext-dev
		```
	* if[Wayland]
		```
		sudo apt install libwayland-dev libxkbcommon-dev libegl1-mesa-dev
		```
	* Optional but good practice
		```
		sudo apt install build-essential
		```
- ImGui Docking and SDL2-Binding
  * It seems like the ```ImGui[sdl2-binding]``` no longer exists in vcpkg. Also the current ```Imgui[docking-experimental]``` does not seem to be up to date either.
  * For now, we grabbed the latest from the docking imgui branch and build with the editor. I have added the necessary files under the [thirdparty](https://github.com/Synkgg/Vortek-Engine/tree/main/thirdparty/imgui/imgui_backends) folder.
  * These should already be setup in cmake.
  * Will move back to using vcpkg bindings once the correct version is available.
- Box2d Install
  * There has been a huge change in the latest box2d that is a breaking change to our codebase.
  * We are using Box2D 2.41, the latest 3.1, uses a C-API that is done in a completely different way.
  * Eventually we are going to have to update the code to support the latest; however, we currently have a work around.
  * Please see [dwjclark11 SCION_PHYSICS Readme.me](https://github.com/dwjclark11/Scion2D/tree/master/SCION_PHYSICS) file for the workaround steps.
    
## Clone the repository 
```
git clone [https://github.com/dwjclark11/Scion2D.git](https://github.com/Synkgg/Vortek-Engine.git)
cd Vortek-Engine
cmake -S . -B build
```

## Documentation
[Documentation](https://synkgg.github.io/Vortek-Engine/)

## Supported Platforms
- Windows
- Linux | Partially Supported

### Open Source Dependencies
List of all the open source dependencies we use in the engine.

-   **[EnTT](https://github.com/skypjack/entt)** - Fast and reliable Entity Component System.
-   **[SDL2](https://github.com/libsdl-org/SDL)** -  a cross-platform library that provides an abstraction layer for computer multimedia hardware components.
-   **[SDL_mixer](https://github.com/libsdl-org/SDL_mixer)** - An audio mixer that supports various file formats for Simple Directmedia Layer.
-   **[Dear ImGui](https://github.com/ocornut/imgui)** - Immediate mode GUI for C++.
-   **[Box2D](https://github.com/erincatto/box2d)** - 2D physics engine for games.
-   **[GLM](https://github.com/g-truc/glm)** - Mathematics library for graphics software.
-   **[RapidJSON](https://github.com/Tencent/rapidjson)** - Fast JSON parser and generator.
-   **[sol2](https://github.com/ThePhD/sol2)** - C++ Lua binding library.
-   **[stb](https://github.com/nothings/stb)** - Single-file public domain libraries.
-   **[Lua](https://www.lua.org/)** - Powerful, efficient scripting language.
-   **[tinyfiledialogs](https://github.com/native-toolkit/libtinyfiledialogs)** - Cross-Platform Native dialog library for WINDOWS MAC OSX GTK+ QT CONSOLE **[Official](https://sourceforge.net/projects/tinyfiledialogs/)**.
