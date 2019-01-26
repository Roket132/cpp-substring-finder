#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include "progress_bar_style.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_browseButton_clicked();

    void detuch_index_bar();

    void indexing_completed();

    void error_indexing_complited();

    void on_run_button_clicked();

    void take_file(std::string);

    void on_actionOpen_Directory_triggered();

    void on_actionOpen_File_triggered();

    void on_actionDelete_File_triggered();

    void on_stopSearchButton_clicked();

    void set_cnt_found_files(int);

    void search_complited();

    void inc_search_bar(int);

    void on_pauseButton_clicked();

signals:
    void stop_search();

    void stop_index();

    void set_search_pause(bool);

private:
    Ui::MainWindow *ui;

    void index_task();

    void show_in_folder(const QString &path);

    QString get_select_item();

    bool search_is_paused;
};

#endif // MAINWINDOW_H
