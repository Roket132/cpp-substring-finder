#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>

static std::string DIRECTORY_NAME = "";
static QProgressBar *INDEX_BAR;

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

    void on_pushButton_2_clicked();

    void detuch_index_bar();

    void indexing_completed();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
