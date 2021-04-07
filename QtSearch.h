#pragma once

#include <QtWidgets/QMainWindow>
#include "SearchThread.h"
#include "stdafx.h"
#include "ui_QtSearch.h"

//How many matches will be shown
const int maxMatchesShown = 500;

class QtSearch : public QMainWindow
{
    Q_OBJECT

public:
    QtSearch(QWidget *parent = Q_NULLPTR);
    ~QtSearch();
private slots:
    void handleTextChanged();
    void handleCheckedChanged();
    void handlePartialWorkDone(const QString& result, const int numMatches, const QString& searchString, const bool nonconsecutive);
    void handleSearchFinished();
private:
    void startSearch();
    int totalMatches;
    Ui::MainWindow ui;
    QThread searcherThread;
signals:
    void startComputation(const QString& searchString, const bool nonconsecutive);
    void stopComputation();
};
