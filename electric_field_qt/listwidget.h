#pragma once

#include <QWidget>
#include "renderarea.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QLabel;
class QSpinBox;
QT_END_NAMESPACE
class RenderArea;

class ListWidget : public QWidget {

    Q_OBJECT

public:
    ListWidget(QWidget* parent = 0);

private slots:
    void shapeChanged();
    void penChanged();
    void brushChanged();

private:
    RenderArea* renderArea;
    QLabel* shapeLabel;
    QLabel* penWidthLabel;
    QLabel* penStyleLabel;
    QLabel* penCapLabel;
    QLabel* penJoinLabel;
    QLabel* brushStyleLabel;
    QLabel* otherOptionsLabel;
    QComboBox* shapeComboBox;
    QSpinBox* penWidthSpinBox;
    QComboBox* penStyleComboBox;
    QComboBox* penCapComboBox;
    QComboBox* penJoinComboBox;
    QComboBox* brushStyleComboBox;
    QCheckBox* antialiasingCheckBox;
    QCheckBox* transformationsCheckBox;

    //AnotherMenu* menu;



    void view_debug(const char* pszFileName);


};
