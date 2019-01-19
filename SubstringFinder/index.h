#ifndef INDEX_H
#define INDEX_H

#include "my_signals.h"
#include "trigram.h"

#include <string>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <algorithm>
#include <QProgressBar>
#include <QObject>
#include <experimental/filesystem>


/// Должно быть кратно 8
const int BUFFER_SIZE_ = 8;

const size_t BYTE_COUNT_IN_INT = 4;
const size_t BITS_COUNT_IN_BYTE = 8;

const std::string path_pair_file = ".pair.txt";
const std::string path_index_file = ".index.txt";

namespace fs = std::experimental::filesystem;

static std::map<trigram, int> cnt_tri;
static std::map<trigram, int> ptr_dir;
static std::map<trigram, int> indexed;
static std::map<fs::path, unsigned int> number_from_path;
static std::map<unsigned int, fs::path> path_from_number;
static unsigned int CNT_FILES;

///Принимает файл, и отдает список trigram
void split_file_on_trigram(const std::string &path, std::vector<trigram> &ans);

unsigned int get_int_from_arr_byte(const std::array<unsigned char, BYTE_COUNT_IN_INT> &charsArray);
std::array<unsigned char, BYTE_COUNT_IN_INT> get_char_arr_from_int(unsigned int value);

void index(std::string from, my_signals* my_signal);

void get_files_with_same_trigram(std::string text, std::vector<fs::path> &files);



#endif // INDEX_H
