#include "index.h"


trigram NULL_T(0, 0, 0);
unsigned int CNT_FILES = 0;

trigram split(std::vector<trigram> &tri, char *buffer, trigram last) {
    trigram t(last);
    for (int i = 0; i < BUFFER_SIZE_; i++) {
        //if check box
        if (int(buffer[i]) == -48 || int(buffer[i]) == -47) {
            i++;
        }
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
        //if check box
        if (int(str[i]) == -48 || int(str[i]) == -47) {
            i++;
        }
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

std::size_t number_of_files_in_directory(fs::path path)
{
    return std::distance(fs::recursive_directory_iterator(path), fs::recursive_directory_iterator{});
}

void index(std::string from, my_signals* my_signal)
{
    CNT_FILES = 0;
    std::ofstream out;
    try {
        out = std::ofstream(path_pair_file, std::ios::out);
    } catch (std::ofstream::failure e) {
        std::cout << e.what() << std::endl;
        throw e;
    }

    ptr_dir.clear();
    cnt_tri.clear();
    indexed.clear();

    size_t cnt = 0;
    double add_progress = 100.0 / number_of_files_in_directory(from) * 1.0;

    emit my_signal->send_index_bar(int(cnt * add_progress));
    //bar->setValue(int(cnt * add_progress));

    for (const auto& entry : fs::recursive_directory_iterator(from)) {
        cnt++;
        fs::path path = entry.path();
        try {
            if (fs::is_directory(path) || fs::is_empty(path) || fs::is_block_file(path)) {
                continue;
            }
        } catch (fs::filesystem_error e) {
            std::cerr << e.what() << std::endl;
            continue;
        }

        std::ifstream in;
        try {
            in = std::ifstream(path, std::ios::in);
        } catch (std::ifstream::failure e) {
            std::cout << e.what() << std::endl;
            continue;
        }

        path_from_number[++CNT_FILES] = path;
        number_from_path[path] = CNT_FILES;

        std::vector<trigram> tri;

        split_file_on_trigram(path, tri);
        if (tri.size() > 200000) {
            continue;
        }
        sort(tri.begin(), tri.end());
        tri.erase(unique(tri.begin(), tri.end()), tri.end());

        //this cycle - bottle neck, but not thread safe

        for (auto it : tri) {
            unsigned int file_name = CNT_FILES;
            std::array<unsigned char, BYTE_COUNT_IN_INT> byte = get_char_arr_from_int(file_name);
            out << it << " " << byte[0] << byte[1] << byte[2] << byte[3];
            cnt_tri[it]++;

        }

        in.close();
        emit my_signal->send_index_bar(int(cnt * add_progress));
    }

    out.close();
    std::ifstream in;
    try {
        in = std::ifstream(path_pair_file, std::ios::in);
        out = std::ofstream(path_index_file, std::ios::out);
    } catch (std::ifstream::failure e) {
        std::cout << e.what() << std::endl;
        throw e;
    } catch (std::ofstream::failure e) {
        std::cout << e.what() << std::endl;
        throw e;
    }

    //also not thread safe

    int pos = 0;
    for (auto it : cnt_tri) {
        if (!ptr_dir.count(it.first))
                ptr_dir[it.first] = pos;


        for (int i = 0; i < it.second; i++) {
            pos += 4;
        }
    }

    std::vector<std::pair<trigram, unsigned int>> v_tri;

    //not thread safe

    unsigned char buffer[BUFFER_SIZE_];
    memset(buffer, 0, BUFFER_SIZE_);

    try {
        while (in.read((char *) buffer, sizeof(buffer)).gcount() > 0) {
            int ind = 0;
            while(ind < BUFFER_SIZE_) {
                trigram trig(buffer[ind], buffer[ind + 1], buffer[ind + 2]);
                if (trig == NULL_T)
                    break;
                std::array<unsigned char, BYTE_COUNT_IN_INT> arr = {buffer[ind + 4], buffer[ind + 5], buffer[ind + 6],
                                                                    buffer[ind + 7]};

                unsigned int file_number = get_int_from_arr_byte(arr);
                v_tri.emplace_back(trig, file_number);
                ind += 8;
            }
        }


        std::sort(v_tri.begin(), v_tri.end());
        v_tri.erase(unique(v_tri.begin(), v_tri.end()), v_tri.end());

        //not thread safe, but, maybe? We reading in diff pos...

        for (auto it : v_tri) {
            trigram trig = it.first;

            int pos = ptr_dir[trig] + (4 * indexed[trig]);
            out.seekp(pos);
            indexed[trig]++;
            std::array<unsigned char, BYTE_COUNT_IN_INT> byte = get_char_arr_from_int(it.second);
            out << byte[0] << byte[1] << byte[2] << byte[3];
        }
   } catch (...) {
       std::cout << "indexing faild exeption " << std::endl;
    }
}

void get_files_with_same_trigram(std::string text, std::vector<fs::path> &files)
{
    std::ifstream in(path_index_file, std::ios::in);

    files.clear();
    std::vector<fs::path> vec;
    std::vector<trigram> vec_tri;
    split_str_on_trigram(text, vec_tri);

    using namespace std;


    bool first = true;
    for (auto tri : vec_tri) {
        vec.clear();
        int pos = ptr_dir[tri];
        int cnt = cnt_tri[tri];

        //mutex lock

        if (cnt == 0)
            continue;
        in.seekg(pos);
        //Мы сможем создать массив на cnt * 4
        unsigned char buffer[cnt * 4];
        memset(buffer, 0, size_t(cnt * 4));
        in.read((char*)buffer, sizeof(buffer));

        //mutex lock or this

        int ind = 0;
        for (int i = 0; i < cnt; i++) {
            std::array<unsigned char, BYTE_COUNT_IN_INT> arr = {buffer[ind], buffer[ind + 1], buffer[ind + 2],
                                                                buffer[ind + 3]};
            unsigned int file_number = get_int_from_arr_byte(arr);
            vec.push_back(path_from_number[file_number]);

            ind += 4;
        }

        //mutex unlock

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
