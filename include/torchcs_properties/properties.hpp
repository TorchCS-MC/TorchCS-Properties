#pragma once

#include <unordered_map>
#include <string>
#include <filesystem>
#include <vector>
#include <utility>

namespace torchcs {
    class properties {
    private:
        std::vector<std::pair<std::string, std::string>> ordered_entries;
        std::unordered_map<std::string, std::string> properties_map;
        std::unordered_map<std::string, std::string> invisible_properties_map;
        std::unordered_map<std::string, std::string> comments_after_key; 
        std::filesystem::path file_name;

    public:
        void load_file(const std::filesystem::path& path);
        void load_string(const std::string& content);
        std::string save_to_string() const;

        void create();
        void load();
        void save();
        void remove();
        bool exists() const;
        void clear();

        void add_comment(const std::string& key, const std::string& comment);

        std::string get_invisible(const std::string& key) const; 
        std::string get(const std::string& key) const;
        void set(const std::string& key, const std::string& value);
        void erase(const std::string& key);
        bool has(const std::string& key) const;

        bool parseBoolean(const std::string& str) const;
        int parseInt(const std::string& str) const;
        float parseFloat(const std::string& str) const;
        double parseDouble(const std::string& str) const;
    };
}
