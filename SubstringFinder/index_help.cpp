#include "index_help.h"

static trigram NULL_T(0, 0, 0);
unsigned int CNT_FILES = 0;

trigram split(std::vector<trigram> &tri, char *buffer, trigram last) {
    trigram t(last);
    for (int i = 0; i < BUFFER_SIZE_; i++) {
        /*if (int(buffer[i]) < 0) {
            i++;
        }*/
        t.add(buffer[i]);
        if (t != NULL_T)
            tri.push_back(t);
    }
    return t;
}

void split_file_on_trigram(const std::string &path, std::vector<trigram> &ans) {
    std::ifstream in(path, std::ios::in);
    ans.clear();
    char buffer[BUFFER_SIZE_];
    memset(buffer, 0, BUFFER_SIZE_);
    trigram last;
    while (in.read(buffer, sizeof(buffer)).gcount() > 0) {
        last = split(ans, buffer, last);
    }
}

void split_str_on_trigram(const std::string &str, std::vector<trigram> &ans) {
    ans.clear();
    if (str.size() < 3)
        return;
    trigram trig(1, str[0], str[1]);
    for (size_t i = 2; i < str.length(); i++) {
        /*if (int(str[i]) < 0) {
            i++;
        }*/
        trig.add(str[i]);
        ans.push_back(trig);
    }
}

unsigned int get_int_from_arr_byte(const std::array<unsigned char, BYTE_COUNT_IN_INT> &charsArray) {
    unsigned int result = charsArray[0];
    for (size_t i = 1; i <= BYTE_COUNT_IN_INT - 1; ++i) {
        result <<= BITS_COUNT_IN_BYTE;
        result |= charsArray[i];
    }
    return result;
}

std::array<unsigned char, BYTE_COUNT_IN_INT> get_char_arr_from_int(unsigned int value) {
    std::array<unsigned char, BYTE_COUNT_IN_INT> result{};
    for (size_t i = 0; i <= BYTE_COUNT_IN_INT - 1; ++i) {
        result[BYTE_COUNT_IN_INT - i - 1] = static_cast<unsigned char>(value);
        value >>= BITS_COUNT_IN_BYTE;
    }
    return result;
}

void get_files_with_same_trigram(std::string text, std::vector<fs::path> &files)
{
    std::ifstream in(path_index_file, std::ios::in);

    files.clear();
    std::vector<fs::path> vec;
    std::vector<trigram> vec_tri;
    split_str_on_trigram(text, vec_tri);

    data &data = data.get_instance();

    bool first = true;
    for (auto tri : vec_tri) {
        vec.clear();
        int pos = data.ptr_dir[tri];
        int cnt = data.cnt_tri[tri];
        if (cnt == 0)
            continue;



        in.seekg(pos);
        unsigned char buffer[cnt * 4];
        memset(buffer, 0, size_t(cnt * 4));
        in.read((char*)buffer, sizeof(buffer));

        int ind = 0;
        for (int i = 0; i < cnt; i++) {
            std::array<unsigned char, BYTE_COUNT_IN_INT> arr = {buffer[ind], buffer[ind + 1], buffer[ind + 2],
                                                                buffer[ind + 3]};
            unsigned int file_number = get_int_from_arr_byte(arr);
            vec.push_back(data.path_from_number[file_number]);

            ind += 4;
        }


        sort(vec.begin(), vec.end());
        vec.erase(unique(vec.begin(), vec.end()), vec.end());
        if (first) {
            swap(files, vec);
            first = false;
        } else {
            std::vector<fs::path> intersection;
            std::set_intersection(vec.begin(), vec.end(),
                                 files.begin(), files.end(),
                                 std::back_inserter(intersection));
            swap(files, intersection);
        }
    }

}
