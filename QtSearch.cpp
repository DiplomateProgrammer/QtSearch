#include "QtSearch.h"

QtSearch::QtSearch(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    Searcher* searcher = new Searcher();
    searcher->moveToThread(&searcherThread);
    connect(&searcherThread, &QThread::finished, searcher, &QObject::deleteLater);
    connect(this, &QtSearch::startComputation, searcher, &Searcher::startSearching);
    connect(this, &QtSearch::stopComputation, searcher, &Searcher::handleStop);
    connect(searcher, &Searcher::partialWorkDone, this, &QtSearch::handlePartialWorkDone);
    connect(searcher, &Searcher::searchFinished, this, &QtSearch::handleSearchFinished);
    searcherThread.start();
    connect(ui.lineEdit, &QLineEdit::textEdited, this, &QtSearch::handleTextChanged);
    connect(ui.checkBox, &QCheckBox::stateChanged, this, &QtSearch::handleCheckedChanged);
}
QtSearch::~QtSearch()
{
    searcherThread.quit();
    searcherThread.wait();
}

void QtSearch::startSearch()
{
    emit(stopComputation());
    bool nonconsecutive = false;
    if (ui.checkBox->checkState() == Qt::Checked) nonconsecutive = true;
    totalMatches = 0;
    ui.labelMatches->setText("Total matches: 0");
    QString newText = ui.lineEdit->text();
    ui.textEdit->clear();
    if (newText == "")
    {
        ui.label->setText("Idle");
        return;
    }
    ui.label->setText("Processing request...");
    ui.labelMatches->setText("Total matches: 0");
    emit(startComputation(newText, nonconsecutive));
}

void QtSearch::handleTextChanged()
{
    startSearch();
}

void QtSearch::handleCheckedChanged()
{
    startSearch();
}

void QtSearch::handlePartialWorkDone(const QString& result, const int numMatches, const QString& searchString, const bool nonconsecutive)
{
    if (searchString != ui.lineEdit->text()) return;
    bool curNonconsecutive = false;
    if (ui.checkBox->checkState() == Qt::Checked) curNonconsecutive = true;
    if (nonconsecutive != curNonconsecutive) return;
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