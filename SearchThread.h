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
    void filterWords(const stringVector& words, const bool nonconsecutive);
signals:
    void finishedWork(const QString& result, const int numMatches);
};


//Class that performs the actual work of searching the file, runs in another thread, spawns worker threads
class Searcher : public QObject
{
    Q_OBJECT

public:
    Searcher();
    ~Searcher();
private:
    void startOneWorkerThread(const int numThread);
    std::ifstream file;
    stringVector words[numWorkers];
    Worker* workers[numWorkers];
    QThread workerThreads[numWorkers];
    int currentlyWorking = 0;
    bool nonconsecutive;
    QString searchString;
public slots:
    void startSearching(const QString& searchString, const bool nonconsecutive);
    void handleWorkerFinished(const QString& result, const int numMatches, const int numThread);
signals:
    void partialWorkDone(const QString& result, const int numMatches, const QString& searchString, const bool nonconsecutive);
    void searchFinished();
};

