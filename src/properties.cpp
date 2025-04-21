#include <torchcs_properties/properties.hpp>
#include <fstream>
#include <algorithm>

namespace torchcs::utils {

    properties::properties(const std::filesystem::path& file_name)
        : file_name(file_name) {}

    void properties::create() {
        if (!std::filesystem::exists(file_name)) {
            std::ofstream file(file_name);
        }
    }

    void properties::load() {
        properties_map.clear();
        ordered_entries.clear();
        comments_before_key.clear();
    
        if (!std::filesystem::exists(file_name)) return;
    
        std::ifstream file(file_name);
        std::string line;
        std::string last_comment;
    
        while (std::getline(file, line)) {
            if (line.empty()) continue;
    
            if (line[0] == '#') {
                last_comment = line;
                continue;
            }
    
            size_t delimiter_pos = line.find('=');
            if (delimiter_pos != std::string::npos) {
                std::string key = line.substr(0, delimiter_pos);
                std::string value = line.substr(delimiter_pos + 1);
    
                properties_map[key] = value;
                ordered_entries.emplace_back(key, value);
    
                if (!last_comment.empty()) {
                    comments_before_key[key] = last_comment;
                    last_comment.clear();
                }
            }
        }
    }

    void properties::save() {

        std::ofstream file(file_name);
    
        for(size_t i = 0; i < ordered_entries.size(); ++i) {
            const auto& [key, value] = ordered_entries[i];
    
            if (comments_before_key.find(key) != comments_before_key.end()) {
                file << comments_before_key[key] << "\n";
            }
    
            file << key << "=" << value << "\n";
    
            if (i < ordered_entries.size() - 1) {
                file << "\n";
            }
        }
    }
    
    void properties::remove() {
        if (std::filesystem::exists(file_name)) {
            std::filesystem::remove(file_name);
        }
    }

    bool properties::exists() const {
        return std::filesystem::exists(file_name);
    }

    void properties::clear() {
        properties_map.clear();
    }

    std::string properties::get(const std::string& key) const {
        auto it = properties_map.find(key);
        if (it != properties_map.end()) {
            return it->second;
        }
        return {};
    }

    void properties::set(const std::string& key, const std::string& value) {
        properties_map[key] = value;
    
        auto it = std::find_if(ordered_entries.begin(), ordered_entries.end(), [&key](const auto& pair) { return pair.first == key; });

        if (it == ordered_entries.end()) {
            ordered_entries.emplace_back(key, value);
        } else {
            it->second = value;
        }
    }
    
    void properties::erase(const std::string& key) {
        properties_map.erase(key);
    }

    bool properties::has(const std::string& key) const {
        return properties_map.find(key) != properties_map.end();
    }

    void properties::add_comment(const std::string& key, const std::string& comment) {
        comments_before_key[key] = "# " + comment;
    }

    bool properties::parseBoolean(std::string str) const {

        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        if(str == "true" || str == "1") {
            return true;
        } else if(str == "false" || str == "0") {
            return false;
        }

        return false;
    }

    int properties::parseInt(std::string str) const {
        return std::stoi(str);
    }

    float properties::parseFloat(std::string str) const {
        return std::stof(str);
    }

    double properties::parseDouble(std::string str) const {
        return std::stod(str);
    }
}
