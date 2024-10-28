#include "Serializer.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

namespace Serializer {
std::string Serialize(Color color) {
	return std::to_string(color.r) + ',' +
		   std::to_string(color.g) + ',' +
		   std::to_string(color.b) + ',' +
		   std::to_string(color.a);
}
Color DeserializeRaylibColor(std::string& colorString) {
    Color color = { 0, 0, 0, 255 }; // Default color (black with full alpha)

    // Use a single parse operation
    int r, g, b, a;
    if (sscanf(colorString.c_str(), "%d,%d,%d,%d", &r, &g, &b, &a) == 4) {
        color.r = static_cast<unsigned char>(r);
        color.g = static_cast<unsigned char>(g);
        color.b = static_cast<unsigned char>(b);
        color.a = static_cast<unsigned char>(a);
    }

    return color;
}
void SaveNote(std::string fileName, std::string text, Texture2D texture) {
    // Open file in binary mode
    fileName = std::filesystem::path(fileName).replace_extension(".note").string();
    std::ofstream file(fileName, std::ios::binary);
    
    // Save string length followed by the string data
    size_t stringLength = text.size();
    file.write(reinterpret_cast<char*>(&stringLength), sizeof(size_t));
    file.write(text.c_str(), stringLength);
    
    // Save texture metadata: width, height, and format
    file.write(reinterpret_cast<char*>(&texture.width), sizeof(int));
    file.write(reinterpret_cast<char*>(&texture.height), sizeof(int));
    file.write(reinterpret_cast<char*>(&texture.format), sizeof(int));

    // Save texture pixel data
    Image image = LoadImageFromTexture(texture); // Extract the texture data as an Image
    ImageFormat(&image, PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA);
    ImageFlipVertical(&image);
    file.write(reinterpret_cast<char*>(image.data), image.width * image.height * 4); // Assuming RGBA format 
    // Clean up
    UnloadImage(image);
    file.close();
    std::cout << "Saved note: " << fileName << std::endl;
}

bool LoadNote(std::string fileName, std::string& text, Texture2D& texture) {
    // Try to open the file in binary mode
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return false;
    }


    // Read string length and string data
    size_t stringLength;
    file.read(reinterpret_cast<char*>(&stringLength), sizeof(size_t));
    if (file.fail()) {
        std::cerr << "Failed to read string length." << std::endl;
        return false;
    }
    
    char* buffer = new char[stringLength + 1];
    file.read(buffer, stringLength);
    if (file.fail()) {
        std::cerr << "Failed to read string data." << std::endl;
        delete[] buffer;
        return false;
    }
    buffer[stringLength] = '\0';
    text = std::string(buffer);
    delete[] buffer;

    // Read texture metadata (width, height, format)
    int width, height, format;
    file.read(reinterpret_cast<char*>(&width), sizeof(int));
    file.read(reinterpret_cast<char*>(&height), sizeof(int));
    file.read(reinterpret_cast<char*>(&format), sizeof(int));
    if (file.fail()) {
        std::cerr << "Failed to read texture metadata." << std::endl;
        return false;
    }

    // Allocate memory for the pixel data and read it
    unsigned char* pixelData = new unsigned char[width * height * 4]; // Assuming RGBA format
    file.read(reinterpret_cast<char*>(pixelData), width * height * 4);
    if (file.fail()) {
        std::cerr << "Failed to read pixel data." << std::endl;
        delete[] pixelData;
        return false;
    }

    // Create the texture from the pixel data
    Image image = { pixelData, width, height, 1, format };
    texture = LoadTextureFromImage(image);
    // Clean up
    delete[] pixelData;
    file.close();
    std::cout << "Loaded note data successfully" << std::endl;
    return true;
}
}
