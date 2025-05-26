#include <torchcs_properties/properties.hpp>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <unordered_set>
#include <iostream>

namespace torchcs
{

    void properties::create()
    {
        if (!std::filesystem::exists(file_name))
        {
            std::ofstream file(file_name);
        }
    }

    void properties::load_file(const std::filesystem::path &path)
    {
        this->file_name = path;
        load();
    }

    void properties::load()
    {
        properties_map.clear();
        ordered_entries.clear();
        comments_before_key.clear();

        if (!std::filesystem::exists(file_name))
            return;

        std::ifstream file(file_name);
        std::string line;
        std::string last_comment;

        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            if (line[0] == '#')
            {
                if (!last_comment.empty())
                    last_comment += "\n";
                last_comment += line;
                continue;
            }

            size_t delimiter_pos = line.find('=');
            if (delimiter_pos != std::string::npos)
            {
                std::string key = line.substr(0, delimiter_pos);
                std::string value = line.substr(delimiter_pos + 1);

                properties_map[key] = value;
                ordered_entries.emplace_back(key, value);

                if (!last_comment.empty())
                {
                    comments_before_key[key] = last_comment;
                    last_comment.clear();
                }
            }
        }
    }

    std::string properties::save_to_string() const
    {
        std::ostringstream out;
        std::unordered_set<std::string> written_keys;

        for (const auto &[key, value] : ordered_entries)
        {
            if (written_keys.count(key))
                continue;
            written_keys.insert(key);

            if (comments_before_key.find(key) != comments_before_key.end())
            {
                out << comments_before_key.at(key) << "\n";
            }

            out << key << "=" << value << "\n\n";
        }

        return out.str();
    }

    void properties::load_string(const std::string &content)
    {
        properties_map.clear();
        ordered_entries.clear();
        comments_before_key.clear();

        std::istringstream stream(content);
        std::string line;
        std::string last_comment;

        while (std::getline(stream, line))
        {
            if (line.empty())
                continue;

            if (line[0] == '#')
            {
                if (!last_comment.empty())
                    last_comment += "\n";
                last_comment += line;
                continue;
            }

            size_t delimiter_pos = line.find('=');
            if (delimiter_pos != std::string::npos)
            {
                std::string key = line.substr(0, delimiter_pos);
                std::string value = line.substr(delimiter_pos + 1);

                properties_map[key] = value;
                ordered_entries.emplace_back(key, value);

                if (!last_comment.empty())
                {
                    comments_before_key[key] = last_comment;
                    last_comment.clear();
                }
            }
        }
    }

    void properties::save()
    {
        std::ofstream file(file_name);
        std::unordered_set<std::string> written_keys;

        for (const auto &[key, value] : ordered_entries)
        {
            if (written_keys.count(key))
                continue;
            written_keys.insert(key);

            if (comments_before_key.find(key) != comments_before_key.end())
            {
                file << comments_before_key[key] << "\n";
            }

            file << key << "=" << value << "\n\n";
        }
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
        properties_map.clear();
        ordered_entries.clear();
        comments_before_key.clear();
    }

    std::string properties::get(const std::string &key) const
    {
        auto it = properties_map.find(key);
        if (it != properties_map.end())
        {
            return it->second;
        }
        return {};
    }

    void properties::set(const std::string &key, const std::string &value)
    {
        auto it = std::find_if(ordered_entries.begin(), ordered_entries.end(),
                               [&key](const auto &pair)
                               { return pair.first == key; });

        if (it != ordered_entries.end())
        {
            it->second = value;
        }
        else
        {
            ordered_entries.emplace_back(key, value);
        }

        properties_map[key] = value;
    }

    void properties::erase(const std::string &key)
    {
        properties_map.erase(key);
        comments_before_key.erase(key);

        ordered_entries.erase(
            std::remove_if(ordered_entries.begin(), ordered_entries.end(),
                           [&key](const auto &pair)
                           { return pair.first == key; }),
            ordered_entries.end());
    }

    bool properties::has(const std::string &key) const
    {
        return properties_map.find(key) != properties_map.end();
    }

    void properties::add_comment(const std::string &key, const std::string &comment)
    {
        comments_before_key[key] = "# " + comment;
    }

    bool properties::parseBoolean(std::string str) const
    {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c)
                       { return static_cast<char>(std::tolower(c)); });

        return (str == "true" || str == "1");
    }

    int properties::parseInt(std::string str) const
    {
        return std::stoi(str);
    }

    float properties::parseFloat(std::string str) const
    {
        return std::stof(str);
    }

    double properties::parseDouble(std::string str) const
    {
        return std::stod(str);
    }

}
