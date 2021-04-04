#pragma once

#include <QtWidgets/QMainWindow>
#include "stdafx.h"
#include "ui_QtSearch.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

const std::string DICTIONARY_FILE_NAME = "words.txt";
const size_t WORD_BATCH_SIZE = 100;
const int num_workers = 4;


class ProcessSearchThread : public QThread
{
    Q_OBJECT

public:
    void run();
    void startSearching();
    void stopSearching();
    std::string readString();
    void writeString(std::string newValue);
    void handleFinished(int numThread);
private:
    std::string seachString = "";
    std::ifstream file;
    QFuture<void> futures[num_workers];
    QFutureWatcher<void> watchers[num_workers];
    std::vector<std::string> words[num_workers];
signals:
    void workDone();
    void workStopped();
};

class QtSearch : public QMainWindow
{
    Q_OBJECT

public:
    QtSearch(QWidget *parent = Q_NULLPTR);
private slots:
    void handleTextChanged();
private:
    Ui::MainWindow ui;
    std::atomic<bool> working = false;
    ProcessSearchThread* searcherThread = nullptr;
};
