#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "index.h"
#include "async_index.h"
#include "my_signals.h"

#include <qfiledialog.h>
#include <iostream>
#include <thread>
#include <QThread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->index_status->setText("indexing didn't start");
    ui->index_status->setStyleSheet("color: rgb(204, 6, 5)");
    ui->statusBar->addWidget(ui->index_status);
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

    //status indexing on GUI
    ui->index_status->setText("indexing not completed");
    ui->index_status->setStyleSheet("color: rgb(204, 6, 5)");


    DIRECTORY_NAME = str.toStdString();
    ui->pathEdit->setText("Directory: " + QString::fromStdString(DIRECTORY_NAME));

    INDEX_BAR = new QProgressBar();
    ui->statusBar->addPermanentWidget(INDEX_BAR);
    try {
        //index(DIRECTORY_NAME, bar);
        QThread *thread= new QThread;
        my_signals *my_signal = new my_signals();
        async_index *my_index = new async_index(DIRECTORY_NAME, my_signal);

        my_index->moveToThread(thread);

        connect(my_signal, SIGNAL(send_index_bar(int)), INDEX_BAR, SLOT(setValue(int)));
        connect(my_signal, SIGNAL(detuch_index_bar()), this, SLOT(detuch_index_bar()));
        connect(my_signal, SIGNAL(indexing_completed()), this, SLOT(indexing_completed()));
        connect(thread, SIGNAL(started()), my_index, SLOT(do_index()));

        thread->start();
    } catch (...) {
        std::cout << "failed index" << std::endl;
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    using namespace std;
    QString QStr = ui->textEdit->text();
    std::string str = QStr.toStdString();
    cout << "str = " << str << endl;
    std::vector<fs::path> files;
    get_files_with_same_trigram(str, files);
    for (auto it : files) {
        std::cout << it << std::endl;
    }

}

void MainWindow::detuch_index_bar()
{
    std::cout << "TOP" << std::endl;
    ui->statusBar->removeWidget(INDEX_BAR);
    delete INDEX_BAR;
}

void MainWindow::indexing_completed()
{
    ui->index_status->setText("indexing completed");
    ui->index_status->setStyleSheet("color: rgb(4, 204, 37)");
}

