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
    void handlePartialWorkDone(QString result, int numMatches, QString searchString);
    void handleSearchFinished();
private:
    int totalMatches;
    Ui::MainWindow ui;
    QThread searcherThread;
signals:
    void startComputation(QString searchString);
};
