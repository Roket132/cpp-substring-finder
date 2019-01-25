#ifndef INDEX_H
#define INDEX_H

#include "my_signals.h"
#include "trigram.h"
#include "data.h"

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
const int BUFFER_SIZE_ = 32768;

const size_t BYTE_COUNT_IN_INT = 4;
const size_t BITS_COUNT_IN_BYTE = 8;

const std::string path_pair_file = ".pair.txt";
const std::string path_index_file = ".index.txt";

namespace fs = std::experimental::filesystem;

///Принимает файл, и отдает список trigram
void split_file_on_trigram(const std::string &path, std::vector<trigram> &ans);

unsigned int get_int_from_arr_byte(const std::array<unsigned char, BYTE_COUNT_IN_INT> &charsArray);

std::array<unsigned char, BYTE_COUNT_IN_INT> get_char_arr_from_int(unsigned int value);

void get_files_with_same_trigram(std::string text, std::vector<fs::path> &files);

trigram split(std::vector<trigram> &tri, char *buffer, trigram last);

void index_V(std::string from, my_signals* my_signal);

std::size_t number_of_files_in_directory(fs::path path);

#endif // INDEX_H
