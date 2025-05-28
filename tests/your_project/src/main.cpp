#include <torchcs_properties/properties.hpp>
#include <iostream>


std::string properties() {
    return R"(
name=Test
#This is a test

version=1.0.0
#this is a version
    )";
}

int main()
{
    torchcs::properties props;

    props.load_file("server.properties");
    props.set("server-name", "TorchCS");

    std::cout << props.parseInt("max-players") << std::endl;

    props.save();

    return 0;
}