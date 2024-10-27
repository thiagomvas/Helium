#ifndef UTILS_HPP
#define UTILS_HPP

#include "Configuration.hpp"
#include <memory>
#include <string>
#include <vector>
namespace Utils {
void WrapText(std::string text, std::shared_ptr<std::vector<std::string>> output, std::shared_ptr<Helium::Configuration> _config);
}

#endif

