#include <torchcs/properties/properties.hpp>
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

    if(!props.exists()) {
        props.create();
    }

    props.load_file("server.properties");
    props.load_string(properties());

    props.save();

    return 0;
}