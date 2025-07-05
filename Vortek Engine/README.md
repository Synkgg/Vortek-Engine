
![new_logo](https://github.com/user-attachments/assets/1c6b585d-6b34-4f38-a9e3-84af16954a2c)


# 🎮 VORTEK2D – A Simple 2D Game Engine for Learning and Teaching

VORTEK2D is a lightweight, educational 2D game engine built with the goal of **learning by doing** 🧠💡. It's part of my journey to understand game development from the ground up — and share everything I learn through my [YouTube channel]([https://www.youtube.com/](https://www.youtube.com/playlist?list=PL3HUvSWOJR7XRDwVVQqqWO-zyyscb8L-v)) 📺✨.

> This engine isn’t meant to compete with big names like Unity or Godot — and that’s okay!  
> It’s about exploring, building, breaking things, and **growing as a developer** 🚀.

Whether you're here to learn alongside me, peek at the source, or just curious about making your own engine, welcome! 👋

---

🛠 **Why VORTEK2D?**

- ✅ Made with ❤️ for education
- 🎓 Teach what I learn, and learn what I teach
- 🎮 Build real 2D games from scratch
- 💬 Collaborate, share ideas, and grow together

---

## 📖 Documentation
> **⚠️ Active Development Notice:** This engine and editor are currently under active development. The documentation is still a work in progress and will not be up to date.
[VORTEK2D Docs](https://dwjclark11.github.io/VORTEK2D_Docs/)

## 🚀 Features
-  **Cross-Platform**: Builds on Windows and Linux. Currently Mac is not supported.
-  **Modern C++20**: Use of modern c++ standards and functionality.
-  **Integrated Tilemap Editor**: Ability to create tilemaps, create game objects, and more.

---- 
### Hub
![VORTEKhub](https://github.com/user-attachments/assets/d9be3935-fa0f-470a-ad03-d1c31a241565)

----
#### VORTEK Editor
![neweditorpic](https://github.com/user-attachments/assets/147c74eb-82e7-42e5-a065-4b86b6cb1f88)

----

# Build
----
Requires [CMake 3.26](https://cmake.org/) and [vcpkg](https://github.com/microsoft/vcpkg)
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

	Open a terminal and type the following:
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
	vcpkg install fmt glm entt glad soil2 sdl2 sdl2-mixer lua sol2 stb tinyfiledialogs rapidjson
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
	```
	vcpkg install fmt glm entt glad soil2 sdl2[alsa] sdl2-mixer lua sol2 stb tinyfiledialogs rapidjson
	```
- ImGui Docking and SDL2-Binding
  * It seems like the ```ImGui[sdl2-binding]``` no longer exists in vcpkg. Also the current ```Imgui[docking-experimental]``` does not seem to be up to date either.
  * For now, we grabbed the latest from the docking imgui branch and build with the editor. I have added the necessary files under the [thirdparty](https://github.com/dwjclark11/VORTEK2D/tree/master/thirdparty/imgui_backends) folder.
  * These should already be setup in cmake.
  * Will move back to using vcpkg bindings once the correct version is available.
- Box2d Install
  * There has been a huge change in the latest box2d that is a breaking change to our codebase.
  * We are using Box2D 2.41, the latest 3.1, uses a C-API that is done in a completely different way.
  * Eventually we are going to have to update the code to support the latest; however, we currently have a work around.
  * Please see the ```Readme.md``` file here [VORTEK_PHYSICS](https://github.com/dwjclark11/VORTEK2D/tree/master/VORTEK_PHYSICS) for the workaround steps.
    
## Clone the repository 
```
git clone https://github.com/dwjclark11/VORTEK2D.git
cd VORTEK2D
cmake -S . -B build
```
 
* Also, in the main.lua file for the editor, comment out the loaded assets and files that don't exist. They will just error out.

## Use the built-in OpenGL debugger
Note: this requires a graphics adapter with OpenGL version >= 4.3 capabilities.

In VORTEK2D/CMakeLists.txt:
* Set the variable ```VORTEK_OPENGL_DEBUG_CALLBACK``` to ```ON```.
* Optionnaly, set the variable ```VORTEK_OPENGL_DEBUG_FORWARD_COMPATIBILITY``` to ```ON``` in order to enable warnings about deprecated OpenGL functions.

Activate the debugger in your code as soon as you have a valid OpenGL context made current:
* ```VORTEK_RENDERING::OpenGLDebugger::init()```.
* Optionnaly, you can opt out a list of warning wy doing the following:
	```
	std::vector<unsigned int> ignore{ 1281, 131169, 131185, 131204, 31218 };
	VORTEK_RENDERING::OpenGLDebugger::init( ignore );
	```

* To allow the debugger to break, call ```VORTEK_RENDERING::OpenGLDebugger::breakOnError( true/false )``` and/or ```VORTEK_RENDERING::OpenGLDebugger::breakOnWarning( true/false )```.
* To ignore a specific warning, call ```VORTEK_RENDERING::OpenGLDebugger::push( Id )```.
* To reinstate, call ```VORTEK_RENDERING::OpenGLDebugger::pop( Id )```.
* To set the severity level, call ```VORTEK_RENDERING::OpenGLDebugger::setSeverityLevel( VORTEK_RENDERING::OpenGLDebuggerSeverity::Disable/Notification/Low/Medium/High )```.

## Force the discrete GPU on Optimus laptops
In VORTEK2D/CMakeLists.txt:
* Set the variable ```VORTEK_OPENGL_FORCE_DISCRETE_GPU``` to ```ON```.
Note: Ids can differ between platforms or drivers.
Note for linux users: this is currently working only on nVidia GPUs.

## 🎯 Supported Platforms

| Platform | Compiler   | Status             |
| -------- | ---------- | ------------------ |
| Windows  | MSVC 2019+ | ✅ Fully Supported |
| Windows  | MinGW/GCC  | 🔧 To be tested    |
| Linux    | GCC 10+    | ✅ Partially Supported |
| Linux    | Clang 12+  | 🔧 To be tested    |

## 📝 License

This project is licensed under the [MIT License](LICENSE) - see the LICENSE file for details.

## 🙏 Acknowledgments
This project would not be possible without the help of all the contributors, the motivation to keep working forward through the wonderful comments and supporters from my YouTube Channel. 
Also from all the wonderful open source projects that I have been able to use in the creation of the engine.

### Open Source Dependencies
Check out these amazing open source projects that we are using in the engine. Make sure to give them all a star! for all of their amazing work.

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

_Thank you to all the contributors and maintainers of these projects!_ ❤️
