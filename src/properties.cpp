#include <torchcs_properties/properties.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_set>
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
        std::vector<std::string> current_block;

        auto flush_block = [&](const std::vector<std::string> &block)
        {
            if (block.empty())
                return;

            std::string key, value;
            bool is_invisible = false;
            std::vector<std::string> comments;

            for (const std::string &l : block)
            {
                std::string trimmed = l;
                trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
                trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);

                if (trimmed.empty())
                    continue;

                if (trimmed[0] == '#' && trimmed.find('=') != std::string::npos)
                {
                    std::string line_content = trimmed.substr(1);
                    size_t eq = line_content.find('=');
                    if (eq != std::string::npos)
                    {
                        key = line_content.substr(0, eq);
                        value = line_content.substr(eq + 1);
                        is_invisible = true;
                    }
                }
                else if (trimmed.find('=') != std::string::npos && trimmed[0] != '#')
                {
                    size_t eq = trimmed.find('=');
                    key = trimmed.substr(0, eq);
                    value = trimmed.substr(eq + 1);
                    is_invisible = false;
                }
                else
                {
                    comments.push_back(trimmed);
                }
            }

            if (!key.empty())
            {
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                ordered_entries.emplace_back(key, value);

                if (is_invisible)
                {
                    invisible_properties_map[key] = value;
                }
                else
                {
                    properties_map[key] = value;
                }

                if (!comments.empty())
                {
                    std::ostringstream comment_text;
                    for (const auto &c : comments)
                    {
                        comment_text << c << "\n";
                    }
                    comments_after_key[key] = comment_text.str();
                }
            }
        };

        while (std::getline(stream, line))
        {
            if (line.empty())
            {
                flush_block(current_block);
                current_block.clear();
            }
            else
            {
                current_block.push_back(line);
            }
        }

        flush_block(current_block);
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

            bool is_invisible = invisible_properties_map.find(key) != invisible_properties_map.end();
            out << (is_invisible ? "# " : "") << key << "=" << value << "\n";

            auto comment_it = comments_after_key.find(key);
            if (comment_it != comments_after_key.end() && !comment_it->second.empty())
            {
                std::istringstream comment_stream(comment_it->second);
                std::string comment_line;
                while (std::getline(comment_stream, comment_line))
                {
                    out << comment_line << "\n";
                }
            }

            out << "\n";
        }

        return out.str();
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
        properties_map.clear();
        ordered_entries.clear();
        invisible_properties_map.clear();
        comments_after_key.clear();
    }

    void properties::add_comment(const std::string &key, const std::string &comment)
    {
        auto &existing = comments_after_key[key];
        if (!existing.empty())
            existing += "\n";
        existing += "# " + comment;
    }

    std::string properties::get(const std::string &key) const
    {
        auto it = properties_map.find(key);
        return it != properties_map.end() ? it->second : "";
    }

    std::string properties::get_invisible(const std::string &key) const
    {
        auto it = invisible_properties_map.find(key);
        return it != invisible_properties_map.end() ? it->second : "";
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
        invisible_properties_map.erase(key);
    }

    void properties::erase(const std::string &key)
    {
        properties_map.erase(key);
        invisible_properties_map.erase(key);
        comments_after_key.erase(key);

        ordered_entries.erase(std::remove_if(ordered_entries.begin(), ordered_entries.end(),
                                             [&key](const auto &pair)
                                             {
                                                 return pair.first == key;
                                             }),
                              ordered_entries.end());
    }

    void properties::delete_comment(const std::string &key, size_t index)
    {
        auto it = comments_after_key.find(key);
        if (it == comments_after_key.end())
            return;

        std::istringstream stream(it->second);
        std::vector<std::string> lines;
        std::string line;

        while (std::getline(stream, line))
        {
            if (!line.empty())
                lines.push_back(line);
        }

        if (index >= lines.size())
            return;

        lines.erase(lines.begin() + index);

        std::ostringstream rebuilt;
        for (size_t i = 0; i < lines.size(); ++i)
        {
            rebuilt << lines[i];
            if (i + 1 < lines.size())
                rebuilt << "\n";
        }

        if (lines.empty())
        {
            comments_after_key.erase(key);
        }
        else
        {
            comments_after_key[key] = rebuilt.str();
        }
    }

    void properties::replace_comment(const std::string &key, size_t index, const std::string &text)
    {
        auto it = comments_after_key.find(key);
        if (it == comments_after_key.end())
            return;

        std::istringstream stream(it->second);
        std::vector<std::string> lines;
        std::string line;

        while (std::getline(stream, line))
        {
            if (!line.empty())
                lines.push_back(line);
        }

        if (index >= lines.size())
            return;

        std::string formatted_text = text;
        if (formatted_text.rfind("#", 0) != 0)
        {
            formatted_text = "# " + formatted_text;
        }

        lines[index] = formatted_text;

        std::ostringstream rebuilt;
        for (size_t i = 0; i < lines.size(); ++i)
        {
            rebuilt << lines[i];
            if (i + 1 < lines.size())
                rebuilt << "\n";
        }

        comments_after_key[key] = rebuilt.str();
    }

    bool properties::has(const std::string &key) const
    {
        return properties_map.find(key) != properties_map.end();
    }

    bool properties::parseBoolean(const std::string &str) const
    {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c)
                       { return static_cast<char>(std::tolower(c)); });
        return (lower == "true" || lower == "1" || lower == "yes");
    }

    int properties::parseInt(const std::string &str) const
    {
        return std::stoi(str);
    }

    float properties::parseFloat(const std::string &str) const
    {
        return std::stof(str);
    }

    double properties::parseDouble(const std::string &str) const
    {
        return std::stod(str);
    }

}
