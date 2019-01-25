#include "searcher.h"
#include "mainwindow.h"

#include <qmutex.h>

static const int cnt_sends = 200;
extern std::string DIRECTORY_NAME;
extern bool INDEXED;

std::size_t number_of_files_in_directory(fs::path path)
{
    return std::distance(fs::recursive_directory_iterator(path), fs::recursive_directory_iterator{});
}

void searcher::run()
{
    std::vector<std::string> paths;
    try {
        bool promis = true;
        emit inc_cnt_found_files(0);

        size_t cnt = 0, cnt_files = number_of_files_in_directory(DIRECTORY_NAME);
        double add_progress = 100.0 / cnt_files * 1.0;
        emit inc_search_bar(0);

        std::set<fs::path> check_files;
        size_t min_sign = (int(text_[0]) > 0 ? 2 : 4);
        if (!INDEXED || text_.size() <= min_sign) {
            for (const auto& entry : fs::recursive_directory_iterator(DIRECTORY_NAME)) {
                // pause
                PAUSE.lock();
                PAUSE.unlock();
                if (STOP_) {
                    emit search_complited();
                    return;
                }

                cnt++;
                fs::path path = entry.path();
                try {
                    if (fs::is_directory(path) || fs::is_empty(path) || fs::is_block_file(path)) {
                        continue;
                    }
                } catch (fs::filesystem_error e) {
                    std::cerr << e.what() << std::endl;
                }
                check_files.insert(path);
                try {
                    if (findInputStringInFile(text_, path)) {
                        paths.push_back(path);
                        if (promis) emit send_file(path);
                        if (promis && paths.size() > cnt_sends) {
                            promis = false;
                            paths.clear();
                        }
                        emit inc_cnt_found_files(++cnt_found_files);
                    }
                } catch (...) {
                    std::cerr << "search in file " << path << " was failed" << std::endl;
                }

                //index already ready (in async_index)
                if (INDEXED && text_.size() > min_sign) {
                    break;
                }
                inc_search_bar(cnt * add_progress);
            }
        }

        cnt = cnt_files - candidate_.size();
        inc_search_bar(cnt * add_progress);
        for (auto file : candidate_) {
            // pause
            PAUSE.lock();
            PAUSE.unlock();
            if (STOP_) {
                emit search_complited();
                return;
            }

            cnt++;
            if (check_files.size() > 0 && check_files.count(file)) {
                continue;
            }
            try {
                if (findInputStringInFile(text_, file)) {
                    paths.push_back(file);
                    if (promis) emit send_file(file);
                    if (promis && paths.size() > cnt_sends) {
                        promis = false;
                        paths.clear();
                    }
                    emit inc_cnt_found_files(++cnt_found_files);
                }
            } catch (...) {
                std::cerr << "search in file " << file << " was failed"  << std::endl;
            }
            inc_search_bar(cnt * add_progress);
        }
    } catch (...) {
        std::cerr << "search was failed" << std::endl;
    }
    for (auto file : paths) {
        emit send_file(file);
    }
    emit search_complited();
}

searcher::searcher(std::vector<std::experimental::filesystem::__cxx11::path> candidate, std::string text)
{
    swap(candidate, candidate_);
    swap(text, text_);
    STOP_ = false;
    pause_ = false;
    cnt_found_files = 0;
}

void searcher::stop_search()
{
    STOP_ = true;
}

void searcher::set_pause(bool value)
{
    if (value) {
        if (!pause_) {
            PAUSE.lock();
        }
    } else {
        if (pause_) {
            PAUSE.unlock();
        }
    }
    pause_ = !pause_;
}

bool findInputStringInFile(std::string &inputString, fs::path p) {
    std::ifstream file(p);
    if (!file.is_open()) {
        return false;
    }
    std::vector<size_t> prefInputString(inputString.length(), 0);
    for (size_t i = 1; i <inputString.length(); i++) {
        size_t k = prefInputString[i - 1];
        while (k > 0 && inputString[i] != inputString[k]) {
            k = prefInputString[k - 1];
        }
        if (inputString[i] == inputString[k]) {
            k++;
        }
        prefInputString[i] = k;
    }
    size_t prevPref = 0;
    size_t ind = 0;
    char buf[BUFFER_SIZE_];
    do {
        file.read(buf, sizeof(buf));
        auto sz = file.gcount();
        for (size_t i = 0; i < static_cast<size_t>(sz); i++, ind++) {
            size_t k = prevPref;
            while (k > 0 && buf[i] != inputString[k]) {
                k = prefInputString[k - 1];
            }
            if (buf[i] == inputString[k]) {
                k++;
            }
            prevPref = k;
            if (prevPref == inputString.length()) {
                file.close();
                return true;
            }
        }
    } while (file);
    file.close();
    return false;
}
