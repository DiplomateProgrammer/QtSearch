#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <QtConcurrent/QtConcurrentRun>
#include <QWaitCondition>
#include <QFutureWatcher>

using stringVector = std::vector<std::string>;
Q_DECLARE_METATYPE(stringVector)

//Path to the dictionary file
const std::string DICTIONARY_FILE_NAME = "words.txt";
//How many words one worker thread handles at a time
const size_t WORD_BATCH_SIZE = 500;
//How many worker threads are processing word batches simultaneously
const int numWorkers = 2;

//Processes a vector of strings and returns a QString to add to GUI and number of matches
class Worker : public QObject
{
    Q_OBJECT

public slots:
    void filterWords(stringVector words);
signals:
    void finishedWork(QString result, int numMatches);
};


//Class that performs the actual work of searching the file, runs in another thread, spawns worker threads
class Searcher : public QObject
{
    Q_OBJECT

public:
    Searcher();
    ~Searcher();
private:
    void startOneWorkerThread(int numThread);
    std::ifstream file;
    stringVector words[numWorkers];
    Worker* workers[numWorkers];
    QThread workerThreads[numWorkers];
    int currentlyWorking = 0;
    QString searchString;
public slots:
    void startSearching(QString searchString);
    void handleWorkerFinished(QString result, int numMatches, int numThread);
signals:
    void partialWorkDone(QString result, int numMatches, QString searchString); //0'th is the search string
    void searchFinished();
};

