# Helium
Helium is a light-weight markdown-based note-taking app built with C++ using Raylib. It is designed to be simple and easy to use, with high customizability.

## Features
- **Markdown Support**: Write notes in markdown for easy formatting.
- **Drawing on note**: You can draw on top of your notes and save them as is
- **High customizability**: Helium has a focus on high customizability, where the user can customize the app as much as possible.
- **Text macros**: Text macros are "keywords" that whenever you press `[TAB]`, that keyword will be replaced with whatever text you configured it to.

## Usage
By default, the program is in `READ` mode. When left clicking on the note area, it switches to `WRITE` mode where you can write your markdown text. 
To see how the markdown is being rendered, press `ESC` to leave `WRITE` mode and return to `READ`.

To draw, simply press `ALT` to enter draw mode. `Left-Click` to draw and `Right-Click` to erase. `LSHIFT + SCROLLWHEEL` to resize the brush. Press `ESC` once again to enter `READ` more

### Macros
Macros are activated by pressing `TAB` on a keyword. You can define macro keys on the configuration file (`%appdata%/Helium` on Windows, `/Library/Application Support/Helium` on MAC and `/.local/share/Helium` on Linux ).

Macros must be written under `MACROS`, and cannot have empty lines in between them. They also must follow the correct syntax for their type:
#### Single line macros
```
Key:Value
MyMacroKeyword:My macro text which can be any length.
```
#### Multiline macros
```
Key:<<
Value1
Value2
>>
lipsum:<<
Lorem Ipsum
dolor sit amet,
consectetur adipiscing elit
>>
```
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
