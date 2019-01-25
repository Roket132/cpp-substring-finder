#ifndef DATA_H
#define DATA_H

#include "trigram.h"
#include <experimental/filesystem>
#include <map>

namespace fs = std::experimental::filesystem;

class data
{
private:
    data();
    data(const data&);
    data& operator=(data&);

public:
    std::map<trigram, int> cnt_tri;
    std::map<trigram, int> ptr_dir;
    std::map<trigram, int> indexed;
    std::map<fs::path, unsigned int> number_from_path;
    std::map<unsigned int, fs::path> path_from_number;

    static data& get_instance();

};

/*
class file_data
{
private:
    file_data();
    file_data(const file_data&);
    file_data& operator=(file_data&);

public:
    std::map<std::pair<size_t, bits>, std::vector<fs::path>> _FILES;
    std::map<std::string, std::pair<size_t, bits>> _KEYS;
    std::map<fs::path, std::string> _SHA256;
    std::map<fs::path, std::string> _FAST_HASH;
    std::map<fs::path, fs::file_time_type> _LAST_MODIF;

    static file_data& get_instance();
};
*/
#endif // DATA_H
