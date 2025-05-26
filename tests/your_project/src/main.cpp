#include <torchcs_properties/properties.hpp>
#include <iostream>

int main() {

    torchcs::properties hello;

    hello.load_file("hello.properties");
    bool test = hello.parseBoolean("true");

    std::cout << test << std::endl;

    if(hello.exists()) {
        hello.load();
        return 0;
    }

    hello.create();

    hello.set("name", "Hello World");


    hello.save();

    return 0;
}