# slime-vr-interface
This repository provides a Godot plugin for interfacing with the SlimeVR Server. It uses the SolarXR Protocol to communicate directly with the server. The source is writen in C++ via godot-cpp (the C++ library for GDExtension).

## Contents
* slime-vr-interface source files ([src/](./src/))
* A demo godot project showcasing basic usage of the plugin [demo/](./demo), or to test that the plugin works.
* godot-cpp as a submodule (`godot-cpp/`)
* SolarXR-Protocol as a submodule (`SolarXR-Protocol/`)
* IXWebSocket as a submodule (`IXWebSocket/`)
* GitHub Issues template ([.github/ISSUE_TEMPLATE.yml](./.github/ISSUE_TEMPLATE.yml))

## Using the plugin

### Adding the plugin to your Godot project
todo: write plugin setup instructions

### Demo project example
todo: write demo example

## Contributing
If you would like to contribute to the slime-vr-interface plugin, you must clone the respository to you local computer. See the following sections for details on how to set up the project for building and developing.

### Building

#### Cmake
todo: write build instructions
```shell
# tbd
```

If the build command worked, you can test it with the [demo](./demo) project. 
* import [demo/project.godot](./demo/project.godot) into Godot and open it
* make sure SlimeVR Server is running
* launch the main scene
> **NOTE:** The demo requires SlimeVR Server to successfully run.

You should see it print the following line in the console:
```
expected console output here
```

### Configuring an IDE

#### Visual Studio 2026
todo: write ide config instructions
```shell
# tbd
```

### SlimeVR Tracking Demo
todo: write demo instructions

## Credits
Thanks to the following for their work and assistance. This project would not be possible without them!
* **SlimeVR**
	* SlimeVR Server, SolarXR Protocol, Hardware for testing
* **SlimeVR Community**
	* Coding and debug assitance
* **Godot Founation**
	* Godot Engine and GDExtension

<https://slimevr.dev/>
<https://godotengine.org/license/>
