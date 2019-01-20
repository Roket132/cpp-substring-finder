#ifndef SEARCHER_H
#define SEARCHER_H

#include "index.h"

#include <string>
#include <fstream>
#include <QObject>
#include <QString>
#include <vector>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

class searcher:public QObject
{
    Q_OBJECT
public:
    searcher(std::vector<fs::path> candidate, std::string text);
public slots:
    void run();
signals:
    void send_file(std::string);
private:
    std::vector<fs::path> candidate_;
    std::string text_;
};

bool findInputStringInFile(std::string &inputString, fs::path p);

#endif // SEARCHER_H
