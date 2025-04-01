#include <unordered_map>
#include <string>
#include <filesystem>
#include <string>
#include <vector>
#include <utility>

namespace torchcs::utils {
    class properties {
    private:
        std::vector<std::pair<std::string, std::string>> ordered_entries;
        std::unordered_map<std::string, std::string> properties_map;
        std::unordered_map<std::string, std::string> comments_before_key;
        std::filesystem::path file_name;

    public:
        explicit properties(const std::filesystem::path& file_name);

        void create();
        void load();
        void save();
        void remove();
        bool exists() const;
        void clear();

        void add_comment(const std::string& key, const std::string& comment);

        std::string get(const std::string& key) const;
        void set(const std::string& key, const std::string& value);
        void erase(const std::string& key);
        bool has(const std::string& key) const;
    };

}
