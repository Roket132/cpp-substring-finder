#include "async_index.h"


void async_index::do_index() {
    std::cout << DIRECTORY_NAME << std::endl;

    bool ok = true;
    try {
        index(DIRECTORY_NAME, my_signal);
        my_signal->detuch_index_bar();
    } catch (...) {
        std::cout << "failed index" << std::endl;
        my_signal->detuch_index_bar();
        ok = false;
    }
    if (ok) {
        emit my_signal->indexing_completed();
    }

}

async_index::async_index(std::string str, my_signals* index_s_) : DIRECTORY_NAME(str), my_signal(index_s_) {}
