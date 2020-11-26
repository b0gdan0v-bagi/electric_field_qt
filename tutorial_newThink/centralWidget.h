#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QList>
#include <QMainWindow>
#include <qpushbutton.h>
#include "scribblearea.h"
#include "sShape.h"
#include <QListWidget>
#include <QtWidgets>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
class QSessionManager;
class QListWidget;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QSpinBox;
class QStackedWidget;
QT_END_NAMESPACE

class CentralWidget : public QWidget
{

    Q_OBJECT
public:

    CentralWidget(QWidget* parent = 0);

private:

    QPoint vec2DtoQPoint(const vd2D& v) { return QPoint(v.x, v.y); }

    QListWidget* sListWidget;
    //QList<sShape*> shapes;

    QLabel* avaliableNodesL;
    QPushButton* crtLineBut;
    QPushButton* crtSingleBut;
    QPushButton* crtScribbleBut;
    QPushButton* updateBut;
    int nNodes;

    QPushButton* testBut;
    QPushButton* test2But;
    QSpinBox* testQSB;
    QPushButton* clearBut;

    void testColorBut();

    void testColorBut2();

    // Will tie user actions to functions
    void createActions();
    void createMenus();

    // Will check if changes have occurred since last save
    bool maybeSave();

    // Opens the Save dialog and saves
    bool saveFile(const QByteArray& fileFormat);

    // What we'll draw on
    ScribbleArea* scribbleArea;

    // The menu widgets
    QMenu* saveAsMenu;
    QMenu* fileMenu;
    QMenu* optionMenu;
    QMenu* helpMenu;

    // All the actions that can occur
    QAction* openAct;

    // Actions tied to specific file formats
    QList<QAction*> saveAsActs;
    QAction* exitAct;
    QAction* penColorAct;
    QAction* penWidthAct;
    QAction* printAct;
    QAction* clearScreenAct;
    QAction* aboutAct;
    QAction* aboutQtAct;



};








#endif // CENTRALWIDGET_H