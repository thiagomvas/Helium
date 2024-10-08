#include "Application.hpp"
#include "Configuration.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>

int main() {

    std::shared_ptr<Helium::Configuration> config = std::make_shared<Helium::Configuration>();

    config->deserialize(config->serialize());
    Helium::Application app(config);
    app.Start();
    
    app.Stop();
}
