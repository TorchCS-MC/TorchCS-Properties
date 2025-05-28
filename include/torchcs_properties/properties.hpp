#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <filesystem>

namespace torchcs
{

    class properties
    {
    private:
        std::unordered_map<std::string, std::string> props_map;
        std::vector<std::string> raw_lines;
        std::filesystem::path file_name;

        inline std::string trim(const std::string &s) const;
        std::string get_property_and_value(const std::string &data) const;
        std::string scann_next_block_in_line_and_slice(const std::string &line) const;

    public:
        std::string save_to_string() const;

        void load_file(const std::filesystem::path &path);
        void load_string(const std::string &content);
        void create();
        void load();
        void save();
        void remove();
        bool exists() const;
        void clear();

        std::string get(const std::string &key) const;
        void set(const std::string &key, const std::string &value);
        void erase(const std::string &key);
        bool has(const std::string &key) const;

        bool parseBoolean(const std::string &key) const;
        int parseInt(const std::string &key) const;
        float parseFloat(const std::string &key) const;
        double parseDouble(const std::string &key) const;
    };

} // namespace torchcs
