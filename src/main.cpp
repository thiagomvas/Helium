#include "Application.hpp"
#include "Configuration.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>

int main() {
    Helium::Configuration::getInstance().deserialize(Helium::Configuration::getInstance().serialize());
    Helium::Application app;
    app.Start();
    app.Stop();
}
