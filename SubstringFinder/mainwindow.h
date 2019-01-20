#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>

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

    void on_run_button_clicked();

    void take_file(std::string);

private:
    Ui::MainWindow *ui;

    void index_task();
};

#endif // MAINWINDOW_H
