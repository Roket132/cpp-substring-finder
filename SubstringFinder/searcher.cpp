#include "searcher.h"
#include "mainwindow.h"

extern std::string DIRECTORY_NAME;
extern bool INDEXED;

void searcher::run()
{
    std::set<fs::path> check_files;
    if (!INDEXED) {

        for (const auto& entry : fs::recursive_directory_iterator(DIRECTORY_NAME)) {
            fs::path path = entry.path();
            check_files.insert(path);
            if (findInputStringInFile(text_, path)) {
                emit send_file(path);
            }

            //index already ready (in async_index)
            if (INDEXED) {
                break;
            }
        }
    }

    for (auto file : candidate_) {
        if (check_files.size() > 0 && check_files.count(file)) {
            continue;
        }
        if (findInputStringInFile(text_, file)) {
            emit send_file(file);
        }
    }
}

searcher::searcher(std::vector<std::experimental::filesystem::__cxx11::path> candidate, std::string text)
{
    swap(candidate, candidate_);
    swap(text, text_);
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


