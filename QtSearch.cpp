#include "QtSearch.h"

QtSearch::QtSearch(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.lineEdit, &QLineEdit::textEdited, this, &QtSearch::handleTextChanged);
}
QtSearch::~QtSearch()
{
    searcherThread.quit();
    searcherThread.wait();
}

void QtSearch::handleTextChanged()
{
    totalMatches = 0;
    ui.labelMatches->setText("Total matches: 0");
    QString newText = ui.lineEdit->text();
    ui.textEdit->clear();
    searcherThread.quit();
    searcherThread.wait();
    Searcher* searcher = new Searcher();
    searcher->moveToThread(&searcherThread);
    connect(&searcherThread, &QThread::finished, searcher, &QObject::deleteLater);
    connect(this, &QtSearch::startComputation, searcher, &Searcher::startSearching);
    connect(searcher, &Searcher::partialWorkDone, this, &QtSearch::handlePartialWorkDone);
    connect(searcher, &Searcher::searchFinished, this, &QtSearch::handleSearchFinished);
    searcherThread.start();
    if (newText == "")
    {
        ui.label->setText("Idle");
        return;
    }
    ui.label->setText("Processing request...");
    ui.labelMatches->setText("Total matches: 0");
    emit(startComputation(newText));
}

void QtSearch::handlePartialWorkDone(QString result, int numMatches, QString searchString)
{
    if (searchString != ui.lineEdit->text()) return;
    int oldMatches = totalMatches;
    if (numMatches == 0) return;
    totalMatches += numMatches;
    ui.labelMatches->setText("Total matches: " + QString::number(totalMatches));
    if (oldMatches > maxMatchesShown) return;
    if (oldMatches <= maxMatchesShown && totalMatches > maxMatchesShown)
    {
        ui.textEdit->setPlainText(ui.textEdit->toPlainText() + ", " + result + " and more...");
        return;
    }
    QString addition = "", current = ui.textEdit->toPlainText();
    if (current.length() > 0) addition = ", ";
    ui.textEdit->setPlainText(current + addition + result);
}

void QtSearch::handleSearchFinished()
{
    ui.label->setText("Search finished!");
}