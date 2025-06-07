#include <torchcs/properties/properties.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <cctype>
#include <iostream>

namespace torchcs
{
    void properties::create()
    {
        if (!std::filesystem::exists(file_name))
        {
            std::ofstream(file_name).close();
        }
    }

    void properties::load_file(const std::filesystem::path &path)
    {
        this->file_name = path;
        load();
    }

    void properties::load()
    {
        if (!std::filesystem::exists(file_name))
            return;

        std::ifstream file(file_name);
        std::stringstream buffer;
        buffer << file.rdbuf();
        load_string(buffer.str());
    }

    void properties::load_string(const std::string &content)
    {
        clear();
        std::istringstream stream(content);
        std::string line;

        while (std::getline(stream, line))
        {
            raw_lines.push_back(line);

            std::string trimmed = trim(line);

            if (trimmed.empty() || trimmed[0] == '#')
                continue;

            std::string prop = get_property_and_value(trimmed);
            if (prop.empty())
                continue;

            size_t equal_pos = prop.find('=');
            if (equal_pos != std::string::npos)
            {
                std::string key = trim(prop.substr(0, equal_pos));
                std::string value = trim(prop.substr(equal_pos + 1));
                set(key, value);
            }
        }
    }

    std::string properties::save_to_string() const
    {
        std::ostringstream out;
        for (const std::string &line : raw_lines)
        {
            std::string trimmed = trim(line);
            if (trimmed.empty() || trimmed[0] == '#')
            {
                out << line << "\n";
                continue;
            }

            size_t equal_pos = trimmed.find('=');
            if (equal_pos != std::string::npos)
            {
                std::string key = trim(trimmed.substr(0, equal_pos));
                auto it = props_map.find(key);
                if (it != props_map.end())
                {
                    out << key << "=" << it->second << "\n";
                    continue;
                }
            }

            out << line << "\n";
        }

        return out.str();
    }

    std::string properties::get_property_and_value(const std::string &data) const
    {
        std::string n_data = scann_next_block_in_line_and_slice(data);

        if (n_data.length() < 3 || n_data.empty())
            return "";

        size_t equal_pos = n_data.find('=');
        if (equal_pos == std::string::npos)
            return "";

        std::string key = trim(n_data.substr(0, equal_pos));
        std::string value = trim(n_data.substr(equal_pos + 1));

        return key + "=" + value;
    }

    std::string properties::scann_next_block_in_line_and_slice(const std::string &line) const
    {
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos)
            return "";

        size_t equal_pos = line.find('=', start);
        if (equal_pos == std::string::npos)
            return "";

        std::string key = trim(line.substr(start, equal_pos - start));
        size_t after_eq = equal_pos + 1;

        if (after_eq >= line.size())
            return key + "=";

        if (std::isspace(static_cast<unsigned char>(line[after_eq])))
            return "";

        return trim(line.substr(start));
    }

    inline std::string properties::trim(const std::string &s) const
    {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end = s.find_last_not_of(" \t\r\n");

        if (start == std::string::npos || end == std::string::npos)
            return "";

        return s.substr(start, end - start + 1);
    }

    void properties::save()
    {
        std::ofstream file(file_name);
        file << save_to_string();
    }

    void properties::remove()
    {
        if (std::filesystem::exists(file_name))
        {
            std::filesystem::remove(file_name);
        }
    }

    bool properties::exists() const
    {
        return std::filesystem::exists(file_name);
    }

    void properties::clear()
    {
        props_map.clear();
    }

    std::string properties::get(const std::string &key) const
    {
        auto it = props_map.find(key);
        return it != props_map.end() ? it->second : "";
    }

    void properties::set(const std::string &key, const std::string &value)
    {
        props_map[key] = value;
    }

    void properties::erase(const std::string &key)
    {
        props_map.erase(key);
    }

    bool properties::has(const std::string &key) const
    {
        return props_map.find(key) != props_map.end();
    }

    bool properties::parseBoolean(const std::string &key) const
    {
        std::string lower;
        for (char c : this->get(key))
            lower += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        if (lower == "true" || lower == "1" || lower == "yes" || lower == "on")
            return true;

        if (lower == "false" || lower == "0" || lower == "no" || lower == "off")
            return false;

        return false;
    }

    int properties::parseInt(const std::string &key) const
    {
        return std::stoi(this->get(key));
    }

    float properties::parseFloat(const std::string &key) const
    {
        return std::stof(this->get(key));
    }

    double properties::parseDouble(const std::string &key) const
    {
        return std::stod(this->get(key));
    }

}
