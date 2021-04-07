#include "SearchThread.h"
#include <functional>

//This function filters out the words that don't contain the substring, returns a QString to add to UI and number of matches.
void Worker::filterWords(stringVector words, const bool nonconsecutive) //0'th element is the substring.
{
	int numMatches = 0;
	QString res = "";
	std::string& searchString = words[0];
	//Naive algorithm is implemented since words are small and mostly don't have ugly patterns
	for (int i = 1; i < words.size(); i++)
	{
		bool fits = false;
		if (nonconsecutive)
		{
			int curFits = 0;
			for (int cur = 0; curFits < searchString.length() && cur < words[i].length(); cur++)
			{
				if (words[i][cur] == searchString[curFits]) curFits++;
			}
			if (curFits == searchString.length()) fits = true;
		}
		else
		{
			for (int start = 0; !fits && start < (int)words[i].length() - (int)searchString.length() + 1; start++)
			{
				fits = true;
				for (int cur = start; fits && cur < start + (int)searchString.length(); cur++)
				{
					if (words[i][cur] != searchString[cur - start]) fits = false;
				}
			}
		}
		if (fits)
		{
			if (numMatches > 0) res += ", ";
			res += QString::fromStdString(words[i]);
			numMatches++;
		}
	}
	emit(finishedWork(res, numMatches));
}

Searcher::Searcher()
{
	qRegisterMetaType<stringVector>("stringVector");
	try
	{
		file.open(DICTIONARY_FILE_NAME);
	}
	catch (...)
	{
		std::cerr << "Failed to open the file";
		exit(EXIT_FAILURE);
	}
}

Searcher::~Searcher()
{
	for (int i = 0; i < numWorkers; i++)
	{
		workerThreads[i].quit();
	}
	for (int i = 0; i < numWorkers; i++)
	{
		workerThreads[i].wait();
	}
}

void Searcher::startOneWorkerThread(int numThread)
{
	if (file.eof())
	{
		return;
	}
	int i = 1;
	words[numThread].resize(WORD_BATCH_SIZE + 1);
	words[numThread][0] = searchString.toStdString();
	std::string cur_word;
	while (file >> cur_word && i < WORD_BATCH_SIZE + 1)
	{
		words[numThread][i] = cur_word;
		i++;
	}
	if (i < WORD_BATCH_SIZE + 1) words[numThread].resize(i + 1);
	if (i > 1)
	{
		currentlyWorking++;
		QMetaObject::invokeMethod(workers[numThread], "filterWords", Qt::QueuedConnection,
			Q_ARG(stringVector, words[numThread]), Q_ARG(bool, nonconsecutive));
	}
}

void Searcher::handleWorkerFinished(QString result, int numMatches, int numThread)
{
	currentlyWorking--;
	emit(partialWorkDone(result, numMatches, searchString, nonconsecutive));
	if (file.eof())
	{
		if (currentlyWorking == 0)
		{
			for (int i = 0; i < numWorkers; i++)
			{
				workerThreads[i].quit();
			}
			for (int i = 0; i < numWorkers; i++)
			{
				workerThreads[i].wait();
			}
			emit(searchFinished());
		}
	}
	startOneWorkerThread(numThread);
}

void Searcher::startSearching(QString searchString, bool nonconsecutive)
{
	currentlyWorking = 0;
	file.seekg(0);
	this->searchString = searchString;
	this->nonconsecutive = nonconsecutive;
	for (int i = 0; i < numWorkers; i++)
	{
		workers[i] = new Worker();
		workers[i]->moveToThread(&workerThreads[i]);
		connect(&workerThreads[i], &QThread::finished, workers[i], &QObject::deleteLater);
		auto handler = [=](QString result, int numMatches)
		{
			handleWorkerFinished(result, numMatches, i);
		};
		connect(workers[i], &Worker::finishedWork, this, handler);
		workerThreads[i].start();
	}
	for (int thr = 0; thr < numWorkers; thr++)
	{
		startOneWorkerThread(thr);
	}
}