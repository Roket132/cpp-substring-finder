#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "index.h"
#include "async_index.h"
#include "my_signals.h"
#include "searcher.h"

#include <qfiledialog.h>
#include <iostream>
#include <thread>
#include <QThread>

std::string DIRECTORY_NAME = "";
bool INDEXED = false;
bool INDEX_IN_PROGRESS = false;
QProgressBar *INDEX_BAR = nullptr;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //add lable
    ui->setupUi(this);
    ui->index_status->setText("indexing didn't start");
    ui->index_status->setStyleSheet("color: rgb(204, 6, 5)");
    ui->statusBar->addWidget(ui->index_status);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 *  not SLOT, just func
 *  indexing path "DIRECTORY_NAME"
 */
void MainWindow::index_task() {
    INDEX_IN_PROGRESS = true;

    //status indexing on GUI
    ui->index_status->setText("indexing not completed");
    ui->index_status->setStyleSheet("color: rgb(204, 6, 5)");

    INDEX_BAR = new QProgressBar();
    ui->statusBar->addPermanentWidget(INDEX_BAR);
    try {
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

void MainWindow::on_browseButton_clicked()
{
    QString str;
    try {
        str = QFileDialog::getExistingDirectory(nullptr, "Directory Dialog", "");
        INDEXED = false;
        if (str == "") {
            throw "todo";
        }
    } catch (...) {
        std::cout << "stop" << std::endl;
        str = QString::fromStdString(DIRECTORY_NAME);
    }

    DIRECTORY_NAME = str.toStdString();
    ui->pathEdit->setText("Directory: " + QString::fromStdString(DIRECTORY_NAME));

    index_task();
}


void MainWindow::detuch_index_bar()
{
    ui->statusBar->removeWidget(INDEX_BAR);
    delete INDEX_BAR;
}

void MainWindow::indexing_completed()
{
    ui->index_status->setText("indexing completed");
    ui->index_status->setStyleSheet("color: rgb(4, 204, 37)");
    INDEXED = true;
    INDEX_IN_PROGRESS = false;
}


void MainWindow::on_run_button_clicked()
{
    if (INDEXED) {
        if (fs::last_write_time(DIRECTORY_NAME) > fs::last_write_time(path_index_file)) {
            INDEXED = false;
        }
    }
    if (!INDEXED && !INDEX_IN_PROGRESS) {
        index_task();
    }

    //clear outList
    ui->listWidget->clear();

    QString QStr = ui->textEdit->text();
    std::string str = QStr.toStdString();
    std::vector<fs::path> files;
    get_files_with_same_trigram(str, files);

    //
    for(auto it : files) {
        std::cout << it << std::endl;
    }

    std::cout << std::endl;
    //

    QThread *thread= new QThread;
    searcher *my_searcher = new searcher(files, str);

    my_searcher->moveToThread(thread);

    qRegisterMetaType<std::string>("std::string");
    connect(my_searcher, SIGNAL(send_file(std::string)), this, SLOT(take_file(std::string)));
    connect(thread, SIGNAL(started()), my_searcher, SLOT(run()));

    thread->start();
}

void MainWindow::take_file(std::string str)
{
    QString QStr = QString::fromStdString(str);
    QListWidgetItem *item = new QListWidgetItem(QStr);
    ui->listWidget->addItem(item);
}
