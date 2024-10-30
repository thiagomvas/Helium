# Helium
Helium is a light-weight markdown-based note-taking app built with C++ using Raylib. It is designed to be simple and easy to use, with high customizability.

## Features
- **Markdown Support**: Write notes in markdown for easy formatting.
- **Drawing on note**: You can draw on top of your notes and save them as is
- **High customizability**: Helium has a focus on high customizability, where the user can customize the app as much as possible.
- **Text macros**: Text macros are "keywords" that whenever you press `[TAB]`, that keyword will be replaced with whatever text you configured it to.

## Installation
Currently, the only way to install it is by building it locally. 
> [!NOTE]
> Building requires that you already have CMake and a C++ compiler set up

1. Clone the repository
```
https://github.com/thiagomvas/Helium.git
cd Helium
```
2. Set up project using CMake
```
cmake -S . -B build
```
3. Build the project
```
cmake --build build
```
4. Start the application. Simply run the ``.exe`` file inside the build folder

## Contribution
Contributions are welcome! Please create a fork and submit a pull request with any features, fixes, or suggestions, or simply create an issue with a bug you found or a feature you think would fit Helium.

## License
Helium is released under the [MIT License](https://github.com/thiagomvas/Helium/blob/dev/LICENSE)
