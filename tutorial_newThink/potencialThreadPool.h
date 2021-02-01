#ifndef POTENCIALTHREADPOOL_H
#define POTENCIALTHREADPOOL_H

#include <condition_variable>
#include <atomic>
#include <complex>
#include <cstdlib>
#include <immintrin.h>

#include <QVector2D>
#include "sShape.h"
#include "qlist.h"
#include <thread>

constexpr int nMaxThreads = 32;

struct WorkerThread
{
	QVector2D left_start = { 0,0 };
	QVector2D right_end = { 0,0 };

	std::condition_variable cvStart;
	bool alive = true;
	std::mutex mux;
	int screen_width = 0;

	//int* fractal = nullptr;

	std::thread thread;

	void Start(const QVector2D LEFT_START, const QVector2D RIGHT_END, QVector<QVector<float>>& potArray, const QList<sShape*>& shapes)
	{
		left_start = LEFT_START;
		right_end = RIGHT_END;
		std::unique_lock<std::mutex> lm(mux);
		cvStart.notify_one();
	}

	void calculatePotencial()
	{
		while (alive)
		{
			std::unique_lock<std::mutex> lm(mux);
			cvStart.wait(lm);


			nWorkerComplete++;
		}
	}
};

bool destroyPool()
{
	for (int i = 0; i < nMaxThreads; i++)
	{
		workers[i].alive = false;		 // Allow thread exit
		workers[i].cvStart.notify_one(); // Fake starting gun
	}

	// Clean up worker threads
	for (int i = 0; i < nMaxThreads; i++)
		workers[i].thread.join();

	return true;
}

WorkerThread workers[nMaxThreads];
static std::atomic<int> nWorkerComplete;

void initialiseThreadPool()
{
	for (int i = 0; i < nMaxThreads; i++)
	{
		workers[i].alive = true;
		//workers[i].fractal = pFractal;
		//workers[i].screen_width = ScreenWidth();
		workers[i].thread = std::thread(&WorkerThread::calculatePotencial, &workers[i]);
	}
}


void CreatePotencialThreadPool(QVector<QVector<float>> & potArray, const QList<sShape*> & shapes, const int width, const int height)
{
	int nSectionWidth = width / nMaxThreads;
	//double dFractalWidth = (frac_br.x - frac_tl.x) / double(nMaxThreads);
	nWorkerComplete = 0;

	for (size_t i = 0; i < nMaxThreads; i++)
		workers[i].Start(
			QVector2D(nSectionWidth * i, height),
			QVector2D(nSectionWidth * (i+1), height)
			);


	while (nWorkerComplete < nMaxThreads) // Wait for all workers to complete
	{
	}
}




#endif