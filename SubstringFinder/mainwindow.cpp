#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "index.h"

#include <qfiledialog.h>
#include <iostream>
#include <thread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_browseButton_clicked()
{
    QString str;
    try {
        str = QFileDialog::getExistingDirectory(nullptr, "Directory Dialog", "");
        if (str == "") {
            throw "todo";
        }
    } catch (...) {
        std::cout << "stop" << std::endl;
        str = QString::fromStdString(DIRECTORY_NAME);
    }

    DIRECTORY_NAME = str.toStdString();
    ui->pathEdit->setText("Directory: " + QString::fromStdString(DIRECTORY_NAME));
}


void MainWindow::on_indexButton_clicked()
{
    QProgressBar *bar = new QProgressBar();
    ui->statusBar->addPermanentWidget(bar);
    try {
        index(DIRECTORY_NAME, bar);
        ui->statusBar->removeWidget(bar);
        delete bar;
    } catch (...) {
        std::cout << "failed index" << std::endl;
        ui->statusBar->removeWidget(bar);
        delete bar;
    }
}



void MainWindow::on_pushButton_2_clicked()
{
    using namespace std;
    QString QStr = ui->textEdit->text();
    std::string str = QStr.toStdString();
    cout << "str = " << str << endl;
    std::vector<fs::path> files;
    std::cout << "SZ1" << " " << files.size() << std::endl;
    get_files_with_same_trigram(str, files);

    std::cout << "SZ2" << " " << files.size() << std::endl;
    for (auto it : files) {
        std::cout << it << std::endl;
    }

}
