#include "async_index.h"

QMutex INDEXING_MUTEX;
bool STOP = false;
trigram NULL_T(0, 0, 0);

void async_index::do_index() {
    INDEXING_MUTEX.lock();
    bool ok = true;
    try {
        index(DIRECTORY_NAME, my_signal);
        my_signal->detuch_index_bar();
    } catch (...) {
        std::cerr << "index was failed" << std::endl;
        ok = false;
    }
    if (ok) {
        emit my_signal->indexing_completed();
    }
    INDEXING_MUTEX.unlock();
}

void async_index::stop_index()
{
    STOP_ = true;
}

async_index::async_index(std::string str, my_signals* index_s_) : DIRECTORY_NAME(str), my_signal(index_s_), STOP_(false) {}

void split_file(const std::string &path, std::vector<trigram> &ans) {
    std::ifstream in(path, std::ios::in);

    ans.clear();

    char buffer[BUFFER_SIZE_];
    memset(buffer, 0, BUFFER_SIZE_);
    trigram last;
    while (in.read(buffer, sizeof(buffer)).gcount() > 0) {
        last = split(ans, buffer, last);
    }
}

#define DO_STOP if (STOP_) { throw "sopped index"; }

void async_index::index(std::string from, my_signals* my_signal)
{
    CNT_FILES = 0;
    std::ofstream out;
    try {
        out = std::ofstream(path_pair_file, std::ios::out);
    } catch (std::ofstream::failure e) {
        std::cout << e.what() << std::endl;
        throw e;
    }

    data &data = data.get_instance();

    data.ptr_dir.clear();
    data.cnt_tri.clear();
    data.indexed.clear();

    size_t cnt = 0;
    double add_progress = 100.0 / number_of_files_in_directory(from) * 1.0;

    emit my_signal->send_index_bar(int(cnt * add_progress));

    for (const auto& entry : fs::recursive_directory_iterator(from)) {
        DO_STOP
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

        data.path_from_number[++CNT_FILES] = path;
        data.number_from_path[path] = CNT_FILES;

        std::vector<trigram> tri;

        split_file(path, tri);
        if (tri.size() > 200000) {
            continue;
        }
        DO_STOP
        sort(tri.begin(), tri.end());
        tri.erase(unique(tri.begin(), tri.end()), tri.end());

        //this cycle - bottle neck, but not thread safe

        for (auto it : tri) {
            DO_STOP
            unsigned int file_name = CNT_FILES;
            std::array<unsigned char, BYTE_COUNT_IN_INT> byte = get_char_arr_from_int(file_name);
            out << it << " " << byte[0] << byte[1] << byte[2] << byte[3];
            data.cnt_tri[it]++;

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
    for (auto it : data.cnt_tri) {
        DO_STOP
        if (!data.ptr_dir.count(it.first))
                data.ptr_dir[it.first] = pos;


        for (int i = 0; i < it.second; i++) {
            DO_STOP
            pos += 4;
        }
    }

    std::vector<std::pair<trigram, unsigned int>> v_tri;

    //not thread safe

    unsigned char buffer[BUFFER_SIZE_];
    memset(buffer, 0, BUFFER_SIZE_);

    DO_STOP
    try {
        while (in.read((char *) buffer, sizeof(buffer)).gcount() > 0) {
            DO_STOP
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


        DO_STOP
        std::sort(v_tri.begin(), v_tri.end());
        v_tri.erase(unique(v_tri.begin(), v_tri.end()), v_tri.end());

        //not thread safe, but, maybe? We reading in diff pos...

        for (auto it : v_tri) {
            DO_STOP
            trigram trig = it.first;

            int pos = data.ptr_dir[trig] + (4 * data.indexed[trig]);
            out.seekp(pos);
            data.indexed[trig]++;
            std::array<unsigned char, BYTE_COUNT_IN_INT> byte = get_char_arr_from_int(it.second);
            out << byte[0] << byte[1] << byte[2] << byte[3];
        }
   } catch (...) {
       std::cerr << "index was failed" << std::endl;
    }
}
