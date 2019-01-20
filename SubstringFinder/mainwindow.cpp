#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "index_help.h"
#include "async_index.h"
#include "my_signals.h"
#include "searcher.h"

#include <qfiledialog.h>
#include <iostream>
#include <thread>
#include <QThread>
#include <QProcess>
#include <QDesktopServices>
#include <QMessageBox>
#include <QMutex>

std::string DIRECTORY_NAME = "";
bool INDEXED = false;
bool INDEX_IN_PROGRESS = false;
size_t CNT_FOUND_FILES = 0;
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

    ui->searchBar->setStyleSheet(
        "QProgressBar {"
            "border: 2px solid grey;"
            "border-radius: 5px;"
            "text-align: center;"
        "}"
        "QProgressBar::chunk {"
            "background-color: rgb(84, 84, 84);"
            "width: 10px;"
            "margin: 0.5px;"
        "}"
    );

    /*
     * rgb(88, 245, 49); green
     *
    */

    ui->searchBar->setValue(100);

    //buttons connect
    connect (ui->openDirButton, SIGNAL( clicked() ), this, SLOT( on_actionOpen_Directory_triggered() ));
    connect (ui->openFileButton, SIGNAL( clicked() ), this, SLOT( on_actionOpen_File_triggered() ));
    connect (ui->deleteFileButton, SIGNAL( clicked() ), this, SLOT( on_actionDelete_File_triggered() ));
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
    if (INDEX_IN_PROGRESS) {
        std::cout << "liol" << std::endl;
        emit stop_index();
    }
    INDEX_IN_PROGRESS = true;

    //status indexing on GUI
    ui->index_status->setText("indexing not completed");
    ui->index_status->setStyleSheet("color: rgb(204, 6, 5)");

    if (INDEX_BAR != nullptr) {
        ui->statusBar->removeWidget(INDEX_BAR);
        delete INDEX_BAR;
        INDEX_BAR = nullptr;
    }
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
        connect(this, SIGNAL(stop_index()), my_index, SLOT(stop_index()), Qt::DirectConnection);
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
        if (str == "") throw "todo";
    } catch (...) {
        std::cerr << "Choose directory was canceled"<< std::endl;
        str = QString::fromStdString(DIRECTORY_NAME);
    }

    if (DIRECTORY_NAME != str.toStdString()) {
        DIRECTORY_NAME = str.toStdString();
        INDEXED = false;
        ui->pathEdit->setText(QString::fromStdString(DIRECTORY_NAME));

        index_task();
    }
}


void MainWindow::detuch_index_bar()
{
    ui->statusBar->removeWidget(INDEX_BAR);
    delete INDEX_BAR;
    INDEX_BAR = nullptr;
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
    connect (this, SIGNAL( stop_search() ), my_searcher, SLOT( stop_search() ),  Qt::DirectConnection);
    connect (my_searcher, SIGNAL( inc_cnt_found_files(int) ), this, SLOT( set_cnt_found_files(int) ));
    connect(thread, SIGNAL(started()), my_searcher, SLOT(run()));



    thread->start();

}

void MainWindow::take_file(std::string str)
{
    QString QStr = QString::fromStdString(str);
    QListWidgetItem *item = new QListWidgetItem(QStr);
    ui->listWidget->addItem(item);
}

void MainWindow::show_in_folder(const QString &path)
{
    QFileInfo info(path);
#if defined(Q_OS_WIN)
    QStringList args;
    if (!info.isDir())
     args << "/select,";
    args << QDir::toNativeSeparators(path);
    if (QProcess::startDetached("explorer", args))
     return;
#endif
    QDesktopServices::openUrl(QUrl::fromLocalFile(info.isDir()? path : info.path()));
}

QString MainWindow::get_select_item()
{
    QString file;
    for (auto it : ui->listWidget->selectedItems()) {
        file = it->text();
    }
    return file;
}

void MainWindow::on_actionOpen_Directory_triggered()
{
    QString file = get_select_item();
    fs::path path = file.toStdString();
    file = QString::fromStdString(path.parent_path());
    show_in_folder(file);
}

void MainWindow::on_actionOpen_File_triggered()
{
    QString file = get_select_item();
    if (file == "") {
        return;
    }
    fs::path path = file.toStdString();
    file = QString::fromStdString(path);
    QProcess* process = new QProcess;
    QStringList list;
    list << file;
    process->start("gedit", list);
}

void MainWindow::on_actionDelete_File_triggered()
{
    QString file = get_select_item();
    if (file == "") {
        return;
    }
    fs::path path = file.toStdString();
    QString file_name = QString::fromStdString(path.filename());
    QString message = "The file \"" + file_name + "\" will be deleted";
    int n = QMessageBox::warning(0,
                                 "Warning",
                                 message +
                                 "\n Do you want to delete file?",
                                 "Yes",
                                 "No",
                                 QString(),
                                 0,
                                 1
                                );
    if(!n) {
        std::vector<QListWidgetItem> items;
        for (int i = 0; i < ui->listWidget->count(); i++) {
            auto item = ui->listWidget->item(i);
            if (item->text().toStdString() == path.string()) {
                delete item;
            }
        }
        set_cnt_found_files(--CNT_FOUND_FILES);
        fs::remove(path);
    }
}

void MainWindow::on_stopSearchButton_clicked()
{
    std::cout << "tut ok" << std::endl;
    emit stop_search();
}

void MainWindow::set_cnt_found_files(int cnt)
{
    CNT_FOUND_FILES = cnt;
    QString str = "Found " + QString::number(CNT_FOUND_FILES) + " files";
    ui->foundFilesLable->setText(str);
}
