#include <torchcs_properties/properties.hpp>
#include <iostream>

int main()
{
    const std::string raw_properties = R"(
# Server Einstellungen
# Server Einstellungen
# Server Einstellungen
# Server Einstellungen
# Server Einstellungen
# Server Einstellungen
# Server Einstellungen
server-name=Test Server
# Gamemode Einstellung
gamemode=creative
# Maximale Spieler
max-players=20
)";

    torchcs::properties props;

    props.load_string(raw_properties);

    std::string server_name = props.get("server-name");
    std::string gamemode = props.get("gamemode");
    int max_players = props.parseInt(props.get("max-players"));

    std::cout << "Server Name: " << server_name << std::endl;
    std::cout << "Gamemode: " << gamemode << std::endl;
    std::cout << "Max Players: " << max_players << std::endl;

    props.set("level-name", "TestLevel");
    props.add_comment("level-name", "Welt");

    std::string new_file_content = props.save_to_string();
    std::cout << new_file_content << std::endl;

    return 0;
}
