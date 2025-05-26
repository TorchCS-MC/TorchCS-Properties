#include <torchcs_properties/properties.hpp>
#include <iostream>

int main()
{
    const std::string raw_properties = R"(
# This is a comment
# Another comment
cool=true

# name=TorchCS
# nameme is cool
# cool or not?

test=123
)";

    torchcs::properties props;

    props.load_string(raw_properties);
    

    props.add_comment("new_key", "This is a comment for the test key");
    props.set("new_key", "new_value");

    props.add_comment("test", "This is a comment for the test key");
    props.add_comment("test", "This is a comment for the test key");
    props.add_comment("test", "This is a comment for the test key");

    props.set("#nein", "nein");
    props.set("#hackstest", "456");
    
    props.add_comment("#nein", "das ist ein Kommentar");

    std::cout << props.save_to_string() << std::endl;

    props.load_file("test.properties");
    props.save();


    return 0;
}
