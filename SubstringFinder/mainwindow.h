#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

static std::string DIRECTORY_NAME = "";

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
    void on_browseButton_pressed();

    void on_browseButton_clicked();

    void on_pushButton_clicked();

    void on_indexButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
