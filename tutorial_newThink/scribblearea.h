#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H
#include "sShape.h"
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QObject>
#include <vector>
#include "qmatrix.h"

#include <QMessageBox>
#include <QtWidgets>
#include "qcolor.h"
#include <algorithm>
#include "qdebug.h"

#include <condition_variable>
#include <atomic>
#include <complex>
#include <cstdlib>
#include <immintrin.h>
#include <thread>

constexpr int nMaxThreads = 32;


class ScribbleArea : public QWidget, public QObject
{
    // Declares our class as a QObject which is the base class
    // for all Qt objects
    // QObjects handle events
    Q_OBJECT

public:
    ScribbleArea(QWidget* parent = 0) ;

    QTimer* TESTtimer;
    int TEST_timerId;

    // Handles all events
    bool openImage(const QString& fileName);
    bool saveImage(const QString& fileName, const char* fileFormat);
    void setPenColor(const QColor& newColor) { myPenColor = newColor; }
    void setPenWidth(int newWidth) {myPenWidth = newWidth;}

    // Has the image been modified since last save
    bool isModified() const { return modified; }
    bool isScribbling() const { return scribbling; }
    QColor penColor() const { return myPenColor; }
    int penWidth() const { return myPenWidth; }

private:
    int nNodes;
    QVector2D tempPoint; // for line

    QMap<int, QVector<QVector2D>> trajectoryMap;
    void drawTrajectoriesAllArea();
public:
    bool showTrajectories = { false };
public:
    
    enum ScribbleModes { NONE, LINE, POINT,MOVING_POINT, CYLINDER, TRACKING, EQPOTLINES, DIRECTIONS,SIMULATE } scribblemodes = NONE;
    QList<sShape*> shapes;
    void updateShapes();

    bool nodeHited = { false };
    bool clickNearShapeNode(QVector2D& mouseClick);
    bool mouseInboundArea();
    QVector2D inboundVector(QVector2D vectorToCheck, int boundSize = 5);
    //bool chargeIntersectChargeOrBound(QVector2D vectorToCheck);

    void setLineMode() { updateShapes();  nNodes = 2; scribblemodes = LINE;  }
    void setScribbleMode() { updateShapes(); scribblemodes = NONE; }
    void setSingleMode() { updateShapes(); scribblemodes = POINT; }
    void setCylinderMode() { updateShapes(); scribblemodes = CYLINDER; }
    void setEqPotLinesMode() { scribblemodes = EQPOTLINES; }
    void setDirectionsMode() { scribblemodes = DIRECTIONS; }

    void startSimulateMovement();
    void stopSimulateMovement();
    bool mChargeCrossChargeOrBorder(const sShape& s, const int boundSize = 5);
    QVector2D summaryForceInPoint(const sShape& curS, bool reverse = false);
private:

    bool vectorInbound(QVector2D v, int boundSize = 5);

    QPoint mousePoint;
    QVector2D mouseVector;
public:
    bool drawPowerLines = { false };
private:
    void drawAllPowerLines();

public: // for potencial
    bool drawPotMap = { false };
    bool potShouldReCalc = { true };
    int potScale = { 1 };
    float precisionFindEqPot = {0.01f};
    bool drawEqPotLines = { false };
    bool storePtsEqPotLines = { true }; // for test it is true
    
    QList<QPoint> storageEqPts;
    
private:
    void calculatePotencial();
    QColor floatToRgb(float minValue, float maxValue, float value);
    //QVector<QVector<float>> arrayOfPotencials;
    QVector<float> temp_vector;
    //QVector<QVector<float>> arrayOfPotencials;
    const int fieldYsize = 1080;
    const int fieldXsize = 1920;
    float minPot;
    float avgPot;
    float maxPot;
    void calcEqPot(QPoint& point);
    void drawPotencialAllArea();

public:
    float chargeToAdd = { 1 };
private:
    QVector2D summaryFieldInPoint(const QVector2D start, bool reverse = false);
    QVector2D plusFieldInPointByPoint(const QVector2D pos, const QVector2D chargePoint, const float charge);
    bool vecCrossChargeOrBorder(const QVector2D v, const int boundSize = 5);
    QVector2D plusForceInPointByPoint(const QVector2D curPoint, const QVector2D chargePoint, const float curCharge, const float chargeCharge);

signals:
    void dataReady(const QList<sShape*>& shapes_);

public slots:

    // Events to handle
    void clearImage();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void timerEvent(QTimerEvent* event) override;

    // Updates the scribble area where we are painting
    void paintEvent(QPaintEvent* event) override;

    // Makes sure the area we are drawing on remains
    // as large as the widget
    void resizeEvent(QResizeEvent* event) override;

public: // for test
    void drawPowerLine(QVector2D& startPos, const int maxPts = 10000, const QColor& lineColor = Qt::black, const bool reverse = false);
    void drawLines(QVector<QVector2D>& pointsToDraw, const QColor& lineColor = Qt::black);
    bool drawPowerLinesAroundCharge(const QVector2D& chargePoint);
    void drawText(const QPoint& point, const QString& s, const QColor c = Qt::black);
    void drawCylTo(QPoint &point, qreal WIDTH);
    void drawRectangle(QPoint& point, QColor pointColor = Qt::green, qreal pointWidth = 15);
    void drawLineTo(const QPoint& endPoint);
    void drawLineBetween(const QPoint& startPoint, const QPoint& endPoint, const QColor lineColor = Qt::black);
    void drawArrow(const QVector2D& fromPoint, const QVector2D& toPoint, const float head_length = 10.f, const float head_width = 5.f, const QColor lineColor = Qt::black, const bool drawBody = true);
    void drawInfo(const QPoint& point);
    void drawToolTip(QMouseEvent* event);
public:
    void drawElFieldAllArea();
    bool drawElField = { false };
    int drawElFieldScale = { 50 };

    
private:
    QVector2D findIntersectLineNormal(QVector2D line_p1, QVector2D line_p2, QVector2D outer_p);
    void resizeImage(QImage* image, const QSize& newSize);

    // Will be marked true or false depending on if
    // we have saved after a change
    bool modified;

    // Marked true or false depending on if the user
    // is drawing
    bool scribbling;

    // Holds the current pen width & color
    int myPenWidth;
    QColor myPenColor;

    // Stores the image being drawn
    QImage image;

    // Stores the location at the current mouse event
    QPoint lastPoint;

    // thread pool here (my tests)
public:

    //QVector<QVector<float>>* pArrayOfPotencials = nullptr;
    QList<sShape*>* pShapes = nullptr;
    int* pPotScale = nullptr;
    float* pArrayPot = new float[32]{ 0 };

    struct WorkerThread
    {
        int left_x = 0;
        int right_x = 0;
        int right_y = 0;
       

        std::condition_variable cvStart;\
        bool alive = true;
        std::mutex mux;
        int screen_width = 0;
        //QVector<QVector<float>>* pArrayOfPotencials = nullptr;
        QList<sShape*>* pShapes = nullptr;
        int* pPotScale = nullptr;

        float* arrayPot = nullptr;

        //int* fractal = nullptr;

        std::thread thread;

        void start(const int LEFT_X, const int RIGHT_X, const int RIGHT_Y)
        {
            left_x = LEFT_X;
            right_x = RIGHT_X;
            right_y = RIGHT_Y;


            //pArrayOfPotencials = & POTENCIAL;

            std::unique_lock<std::mutex> lm(mux);
            cvStart.notify_one();
        }

        void calculatePotencial()
        {
            while (alive)
            {
                std::unique_lock<std::mutex> lm(mux);
                cvStart.wait(lm);
                arrayPot[left_x] += left_x;
                //(*pArrayOfPotencials)[0][0] = 1.f;
                /*
                float radius;
                float minPot = 0;
                float avgPot = 0;
                float maxPot = 0;
                for (int y = 0; y < right_y; y += *pPotScale)

                {
                    //QVector<float> tempPot(width());
                    for (int x = left_x; x < right_x; x += *pPotScale)
                    {
                        //tempPot[x] = 0;
                        
                        (*pArrayOfPotencials)[y][x] = 0.f;
                        for (auto const& sh : *pShapes) {
                            switch (sh->shapeType)
                            {
                            case sShape::ShapeType::POINT:
                            {
                                radius = QVector2D(x, y).distanceToPoint(sh->vecNodes[0].pos);
                                //if (radius > 3) tempPot[x] += (sh->charge / radius); break;
                                if (radius > 3) (*pArrayOfPotencials)[y][x] += (sh->charge / radius); break;
                            }
                            case sShape::ShapeType::LINE:
                            {
                                for (auto const& pts : sh->allPoints)
                                {
                                    radius = QVector2D(x, y).distanceToPoint(*pts);
                                    //if (radius > 3) tempPot[x] += sh->charge / (radius * sh->allPoints.size());
                                    if (radius > 3) (*pArrayOfPotencials)[y][x] += sh->charge / (radius * sh->allPoints.size());
                                }
                                break;
                            }
                            case sShape::ShapeType::MOVING_POINT:
                            {
                                if (sh->interactable)
                                {
                                    radius = QVector2D(x, y).distanceToPoint(sh->movingPos);
                                    ///if (radius > 3) tempPot[x] += (sh->charge / radius); break;
                                    if (radius > 3) (*pArrayOfPotencials)[y][x] += (sh->charge / radius);
                                }
                                break;
                            }
                            }

                        }
                        //if (tempPot[x] > maxPot) maxPot = tempPot[x];
                        //if (tempPot[x] < minPot) minPot = tempPot[x];
                        //avgPot += tempPot[x];
                        //if ((*pArrayOfPotencials)[y][x] > maxPot) maxPot = (*pArrayOfPotencials)[y][x];
                        //if ((*pArrayOfPotencials)[y][x] < minPot) minPot = (*pArrayOfPotencials)[y][x];
                        //avgPot += (*pArrayOfPotencials)[y][x];
                    }
                    //arrayOfPotencials.push_back(tempPot);
                }
                */
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
            //workers[i].pArrayOfPotencials = pArrayOfPotencials;
            workers[i].pShapes = pShapes;
            workers[i].pPotScale = pPotScale;

            workers[i].arrayPot = pArrayPot;

            //workers[i].screen_width = ScreenWidth();
            workers[i].thread = std::thread(&WorkerThread::calculatePotencial, &workers[i]);
        }
    }


    void calculatePotencialThreadPool(const int width, const int height)
    {
        int nSectionWidth = width / nMaxThreads;
        //double dFractalWidth = (frac_br.x - frac_tl.x) / double(nMaxThreads);
        nWorkerComplete = 0;

        for (size_t i = 0; i < nMaxThreads; i++)
            workers[i].start(
               // nSectionWidth * i,
               // nSectionWidth * (i+1),
              //  height
                i, // test
                i,
                i
                //QVector2D(50 * (i + 1), height)

            );
        //QMessageBox hello;
        //hello.setText("hello");
        //hello.exec();


        while (nWorkerComplete < nMaxThreads) // Wait for all workers to complete
        {
        }
    }



};



#endif