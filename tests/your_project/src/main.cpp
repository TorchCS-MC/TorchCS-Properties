#include <torchcs_properties/properties.hpp>

int main() {

    torchcs::utils::properties hello("hello.txt");

    if(hello.exists()) {
        hello.load();
        return 0;
    }

    hello.create();

    hello.set("name", "Hello World");

    hello.save();

    return 0;
}