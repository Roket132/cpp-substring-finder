#ifndef MY_SIGNALS_H
#define MY_SIGNALS_H

#include <QObject>

class my_signals:public QObject
{
    Q_OBJECT
public:
    my_signals();
signals:
    void send_index_bar(int i);
    void detuch_index_bar();
    void indexing_completed();
};

#endif // MY_SIGNALS_H
