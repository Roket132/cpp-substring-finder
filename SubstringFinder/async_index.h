#ifndef ASYNC_INDEX_H
#define ASYNC_INDEX_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "my_signals.h"

#include <QObject>
#include <QProgressBar>
#include <QString>
#include "index.h"


class async_index:public QObject
{
    Q_OBJECT
public:
    async_index(std::string str, my_signals* index_s);
public slots:
    void do_index();
signals:
    void send_bar(double);
private:
    std::string DIRECTORY_NAME;
    my_signals *my_signal;
};


#endif // ASYNC_INDEX_H
