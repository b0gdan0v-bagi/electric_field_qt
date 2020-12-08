#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <qpushbutton.h>
#include "scribblearea.h"
#include "sShape.h"
#include "centralWidget.h"
#include <QListWidget>

//#include "vec2D.h"



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

class MainWindow : public QMainWindow
{
    // Declares our class as a QObject which is the base class
    // for all Qt objects
    // QObjects handle events
    Q_OBJECT

public:
    MainWindow();

protected:
    // Function used to close an event
    void closeEvent(QCloseEvent* event) override;

    // The events that can be triggered
private slots:
    void open();
    void save();
    void penColor();
    void penWidth();
    void about();
    void updateListWidget();

private:

    void deleteShape();
    QPoint vec2DtoQPoint(const vd2D& v) { return QPoint(v.x, v.y); }
    CentralWidget* centralWidget;
    QListWidget* sListWidget;
    //QList<sShape*> shapes;

    QLabel* avaliableNodesL;
    QPushButton* crtLineBut;
    QPushButton* crtSingleBut;
    QPushButton* crtScribbleBut;
    QPushButton* updateBut;
    QPushButton* dirBut;
    QPushButton* reverseChargeBut;
    QPushButton* drawElFieldMapBut;
    QPushButton* deleteShapeBut;
    QLabel* chargeLabel;
    QLineEdit* chargeLE;
    int nNodes;

    QPushButton* potMapBut;
    QPushButton* showEqBut;
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

#endif