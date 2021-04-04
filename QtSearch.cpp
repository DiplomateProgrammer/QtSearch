#include "QtSearch.h"
#include <QtConcurrent/QtConcurrentRun>
#include <functional>

QMutex seachStringMutex;
QWaitCondition seachStringChanged;

QtSearch::QtSearch(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.lineEdit, &QLineEdit::textEdited, this, &QtSearch::handleTextChanged);
    searcherThread = new ProcessSearchThread();
    searcherThread->start();
}

//This function filters out the words that don't contain the substring. 0'th element is the substring.
void filterWords(std::vector<std::string>& words)
{
    return;
}

void ProcessSearchThread::run()
{
    startSearching();
}

void ProcessSearchThread::stopSearching()
{

}

void ProcessSearchThread::handleFinished(int numThread)
{

}

void ProcessSearchThread::startSearching()
{
    while (readString() == "")
    {
        seachStringChanged.wait(&seachStringMutex);
    }
    std::string stringToFind = readString();
    std::ifstream dict;
    dict.open(DICTIONARY_FILE_NAME);
    while (!dict.eof())
    {
        if (readString() != stringToFind)
        {
            stopSearching();
            startSearching();
        }
        for (int thr = 0; thr < num_workers; thr++)
        {
            int i = 0;
            words[thr].resize(WORD_BATCH_SIZE + 1);
            words[thr][0] = stringToFind;
            std::string cur_word;
            while (dict >> cur_word && i < WORD_BATCH_SIZE)
            {
                words[thr][i] = cur_word;
                i++;
            }
            if (i > 0)
            {
                futures[thr] = QtConcurrent::run(filterWords, std::ref(words[thr]));
                watchers[thr].setFuture(futures[thr]);
                auto handler = [=]()
                {
                    handleFinished(thr);
                };
                connect(&watchers[thr], &QFutureWatcher<std::vector<std::string>>::finished, handler);
            }
        }
    }
}
std::string ProcessSearchThread::readString()
{
    seachStringMutex.lock();
    return seachString;
    seachStringMutex.unlock();
}
void ProcessSearchThread::writeString(std::string newValue)
{
    seachStringMutex.lock();
    seachString = newValue;
    seachStringMutex.unlock();
}

void QtSearch::handleTextChanged()
{
    QString newText = ui.lineEdit->text();
    ui.textEdit->clear();
    if (newText == "")
    {
        ui.label->setText("Idle");
        searcherThread->writeString("");
        return;
    }
    ui.label->setText("Processing request...");
    searcherThread->writeString(newText.toStdString());
    seachStringChanged.wakeOne();
}