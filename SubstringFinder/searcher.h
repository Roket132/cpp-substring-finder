#ifndef SEARCHER_H
#define SEARCHER_H

#include "index_help.h"

#include <string>
#include <fstream>
#include <QObject>
#include <QString>
#include <vector>
#include <qmutex.h>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

class searcher:public QObject
{
    Q_OBJECT
public:
    searcher(std::vector<fs::path> candidate, std::string text);

    bool isPause() {
        return pause_;
    }

public slots:
    void run();

    void stop_search();

    void set_pause(bool);

signals:
    void send_file(std::string);

    void inc_cnt_found_files(int);

    void search_complited();

    void inc_search_bar(int);

private:
    std::vector<fs::path> candidate_;

    std::string text_;

    int cnt_found_files;    

    bool pause_;

    QMutex PAUSE;

    bool STOP_;
};

bool findInputStringInFile(std::string &inputString, fs::path p);

#endif // SEARCHER_H
