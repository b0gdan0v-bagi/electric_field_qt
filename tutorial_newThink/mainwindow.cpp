#include <QtWidgets>
#include <QObject>
#include "qobject.h"


#include "mainwindow.h"



// MainWindow constructor
MainWindow::MainWindow()
{
    //centralWidget = new CentralWidget(this);
   // setCentralWidget(centralWidget);
    sListWidget = new QListWidget(this);
    scribbleArea = new ScribbleArea(this);
    // Create the ScribbleArea widget and make it
    // the central widget
    
    
    //potMapBut = new QPushButton("show potencial map", this);
    QToolBar* toolbar = addToolBar("main toolbar");
    toolbar->setMinimumHeight(50);
    QAction* scribble = toolbar->addAction("Scribble", this, [=]() {scribbleArea->setScribbleMode(); updateListWidget(); scribbleArea->setMouseTracking(true); });
    toolbar->addSeparator();
    QAction* createPoint = toolbar->addAction("Create Point",this, [=]() {scribbleArea->scribblemodes = ScribbleArea::ScribbleModes::POINT; updateListWidget(); scribbleArea->setMouseTracking(true);  });
    QAction* createLine = toolbar->addAction("Create Line",this, [=]() {scribbleArea->scribblemodes = ScribbleArea::ScribbleModes::LINE; updateListWidget(); scribbleArea->setMouseTracking(true);  });
    QAction* createMovingPoint = toolbar->addAction("Create moving Point",this, [=]() {scribbleArea->scribblemodes = ScribbleArea::ScribbleModes::MOVING_POINT; updateListWidget(); scribbleArea->setMouseTracking(true);  });
    toolbar->addSeparator();
    QAction* mouseMode = toolbar->addAction("Mouse mode", this, [=]() {scribbleArea->scribblemodes = ScribbleArea::ScribbleModes::TRACKING; });
    toolbar->addSeparator();
    QAction* drawEqPotLine = toolbar->addAction("Store eqpotencial lines", this, [=]() {scribbleArea->scribblemodes = ScribbleArea::ScribbleModes::EQPOTLINES;  scribbleArea->updateShapes(); });
    toolbar->addSeparator();
    QAction* reverseCharge = toolbar->addAction("Reverse charge",this, [=]() {chargeLE->setText(QString::number(chargeLE->text().toInt() * -1)); });
    toolbar->addSeparator();
    QAction* startSimulate = toolbar->addAction("Start simulating", this, [=]() {scribbleArea->scribblemodes = ScribbleArea::ScribbleModes::SIMULATE;/*scribbleArea->simulateMovement(); */});
    QAction* stopSimulate = toolbar->addAction("Stop simulating", this, [=]() {scribbleArea->scribblemodes = ScribbleArea::ScribbleModes::TRACKING; scribbleArea->updateShapes(); });


    chargeLabel = new QLabel("charge to add = k* ");
    chargeLE = new QLineEdit(this);
    chargeLE->setValidator(new QIntValidator(-1000000.f, 1000000.f, this));
    chargeLE->setText(QString::number(1));
    reverseChargeBut = new QPushButton("Reverse charge", this);

    deleteShapeBut = new QPushButton("Delete selected shape", this);
    deleteAllShapeBut = new QPushButton("Delete all shapes", this);

    scaleOfDrawElFieldS = new QSlider(Qt::Orientation::Horizontal,this);
    scaleOfDrawElFieldS->setMaximum(100);
    scaleOfDrawElFieldS->setMinimum(5);
    scaleOfDrawElFieldS->setValue(50);
    drawElFieldCB = new QCheckBox("Show electric field in all area; scale: " + QString::number(scaleOfDrawElFieldS->value()), this);
    

    drawPotMapS = new QSlider(Qt::Orientation::Horizontal, this);
    drawPotMapS->setMaximum(10);
    drawPotMapS->setMinimum(1);
    drawPotMapS->setValue(2);
    drawPotMapCB = new QCheckBox("Show potencial map in all area; scale: " + QString::number(drawPotMapS->value()), this);

    //showEqBut = new QPushButton("show equipotential", this);
    
    precisionEqPtS = new QSlider(Qt::Orientation::Horizontal, this);
    precisionEqPtS->setMaximum(100);
    precisionEqPtS->setMinimum(1);
    precisionEqPtS->setValue(10);
    showEqCB = new QCheckBox("show equipotential " + QString::number(precisionEqPtS->value() * 0.001) + " %,  precision", this);
    clearStorageEqPtsBut = new QPushButton("clear points", this);
   
    showTrajectoriesCB = new QCheckBox("show trajectories ", this);

    drawPowerLinesCB = new QCheckBox("show power lines", this);

    QWidget* testQW = new QWidget(this);
    QGridLayout* controlsLayout = new QGridLayout();
    QHBoxLayout* hbox = new QHBoxLayout(testQW);
    QVBoxLayout* vbox1 = new QVBoxLayout();


    controlsLayout->addWidget(chargeLabel, 0, 1);
    controlsLayout->addWidget(chargeLE, 0, 2, 1, 2);
    controlsLayout->addWidget(reverseChargeBut, 0, 0);

    controlsLayout->addWidget(showEqCB, 1, 0);
    controlsLayout->addWidget(precisionEqPtS ,1,1,1,2);
    controlsLayout->addWidget(clearStorageEqPtsBut,1,4);

    controlsLayout->addWidget(drawPotMapCB, 2, 0);
    controlsLayout->addWidget(drawPotMapS, 2, 1,1,2);
    
    controlsLayout->addWidget(drawElFieldCB,3,0);
    controlsLayout->addWidget(scaleOfDrawElFieldS,3,1,1,2);

    controlsLayout->addWidget(drawPowerLinesCB, 4, 0, 1, 1);
    controlsLayout->addWidget(showTrajectoriesCB, 4, 1, 1, 1);
    
    controlsLayout->addWidget(sListWidget, 5, 0, 4, 3);
    controlsLayout->addWidget(deleteAllShapeBut, 9, 1);
    controlsLayout->addWidget(deleteShapeBut, 9, 0);
    

    vbox1->addWidget(scribbleArea);

    hbox->addLayout(vbox1);
    hbox->addLayout(controlsLayout);

    connect(chargeLE, &QLineEdit::textChanged,this, [=]() {scribbleArea->chargeToAdd = chargeLE->text().toInt(); });
    connect(scribbleArea, &ScribbleArea::dataReady, this, [=]() {updateListWidget(); });
 
    connect(drawPowerLinesCB, &QCheckBox::clicked, this, [=]() {scribbleArea->drawPowerLines = drawPowerLinesCB->isChecked(); });
 
    connect(reverseChargeBut, &QPushButton::clicked, this, [=]() {chargeLE->setText(QString::number(chargeLE->text().toInt()*-1)); });

    connect(deleteShapeBut, &QPushButton::clicked, this, &MainWindow::deleteShape );
    connect(deleteAllShapeBut, &QPushButton::clicked, this, [=]() { scribbleArea->shapes.clear(); sListWidget->clear(); scribbleArea->updateShapes(); });
    
    connect(drawElFieldCB, &QCheckBox::clicked, this, [=]() { scribbleArea->drawElField = drawElFieldCB->isChecked(); scribbleArea->updateShapes(); });
    connect(scaleOfDrawElFieldS, QOverload<int>::of(&QSlider::valueChanged), this, [=]() {scribbleArea->drawElFieldScale = scaleOfDrawElFieldS->value(); scribbleArea->updateShapes(); drawElFieldCB->setText("Show electric field in all area; scale: " + QString::number(scaleOfDrawElFieldS->value())); });
    
    connect(drawPotMapCB, &QCheckBox::clicked, this, [=]() { scribbleArea->drawPotMap = drawPotMapCB->isChecked(); scribbleArea->updateShapes(); });
    connect(drawPotMapS, QOverload<int>::of(&QSlider::valueChanged), this, [=]() {scribbleArea->potScale = drawPotMapS->value(); scribbleArea->updateShapes(); drawPotMapCB->setText("Show potencial map in all area; scale: " + QString::number(drawPotMapS->value())); });

    connect(showEqCB, &QCheckBox::clicked, this, [=]() { scribbleArea->drawEqPotLines = showEqCB->isChecked(); scribbleArea->setMouseTracking(true); scribbleArea->updateShapes(); });
    connect(precisionEqPtS, QOverload<int>::of(&QSlider::valueChanged), this, [=]() {scribbleArea->precisionFindEqPot = precisionEqPtS->value()*0.001f;  showEqCB->setText("show equipotential " + QString::number(precisionEqPtS->value() * 0.001) + " %,  precision"); });
    connect(clearStorageEqPtsBut, &QPushButton::clicked, this, [=]() {scribbleArea->storageEqPts.clear(); scribbleArea->updateShapes(); });

    connect(showTrajectoriesCB, &QCheckBox::clicked, this, [=]() { scribbleArea->showTrajectories = showTrajectoriesCB->isChecked();});

    setCentralWidget(testQW);

    createActions();
    createMenus();

    // Set the title
    setWindowTitle(tr("Modulating charges in electric field"));

    // Size the app
    //resize(1000, 500);
}

// User tried to close the app
void MainWindow::closeEvent(QCloseEvent* event)
{
    // If they try to close maybeSave() returns true
    // if no changes have been made and the app closes
    if (maybeSave()) {
        event->accept();
    }
    else {

        // If there have been changes ignore the event
        event->ignore();
    }
}

// Check if the current image has been changed and then
// open a dialog to open a file
void MainWindow::open()
{
    // Check if changes have been made since last save
    // maybeSave() returns true if no changes have been made
    if (maybeSave()) {

        // Get the file to open from a dialog
        // tr sets the window title to Open File
        // QDir opens the current dirctory
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open File"), QDir::currentPath());

        // If we have a file name load the image and place
        // it in the scribbleArea
        if (!fileName.isEmpty())
            scribbleArea->openImage(fileName);
    }
}

// Called when the user clicks Save As in the menu
void MainWindow::save()
{
    // A QAction represents the action of the user clicking
    QAction* action = qobject_cast<QAction*>(sender());

    // Stores the array of bytes of the users data
    QByteArray fileFormat = action->data().toByteArray();

    // Pass it to be saved
    saveFile(fileFormat);
}

// Opens a dialog to change the pen color
void MainWindow::penColor()
{
    // Store the chosen color from the dialog
    QColor newColor = QColorDialog::getColor(scribbleArea->penColor());

    // If a valid color set it
    if (newColor.isValid())
        scribbleArea->setPenColor(newColor);
}

// Opens a dialog that allows the user to change the pen width
void MainWindow::penWidth()
{
    // Stores button value
    bool ok;

    // tr("Scribble") is the title
    // the next tr is the text to display
    // Get the current pen width
    // Define the min, max, step and ok button
    int newWidth = QInputDialog::getInt(this, tr("Scribble"),
        tr("Select pen width:"),
        scribbleArea->penWidth(),
        1, 50, 1, &ok);
    // Change the pen width
    if (ok)
        scribbleArea->setPenWidth(newWidth);
}

// Open an about dialog
void MainWindow::about()
{
    // Window title and text to display
    QMessageBox::about(this, tr("About Scribble"),
        tr("<p>The <b>Scribble</b> example is awesome</p>"));
}


// Define menu actions that call functions
bool MainWindow::maybeSave()
{
    // Check for changes since last save
    if (scribbleArea->isModified()) {
        QMessageBox::StandardButton ret;

        // Scribble is the title
        // Add text and the buttons
        ret = QMessageBox::warning(this, tr("Scribble"),
            tr("The image has been modified.\n"
                "Do you want to save your changes?"),
            QMessageBox::Save | QMessageBox::Discard
            | QMessageBox::Cancel);

        // If save button clicked call for file to be saved
        if (ret == QMessageBox::Save) {
            return saveFile("png");

            // If cancel do nothing
        }
        else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool MainWindow::saveFile(const QByteArray& fileFormat)
{
    // Define path, name and default file type
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;

    // Get selected file from dialog
    // Add the proper file formats and extensions
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
        initialPath,
        tr("%1 Files (*.%2);;All Files (*)")
        .arg(QString::fromLatin1(fileFormat.toUpper()))
        .arg(QString::fromLatin1(fileFormat)));

    // If no file do nothing
    if (fileName.isEmpty()) {
        return false;
    }
    else {

        // Call for the file to be saved
        return scribbleArea->saveImage(fileName, fileFormat.constData());
    }
}

void MainWindow::updateListWidget()
{
    sListWidget->clear();
    int count = 0;
    for (auto const& sh : scribbleArea->shapes)
    {
        QString toAddName;
        switch (sh->shapeType)
        {
        case sShape::ShapeType::POINT: {
            toAddName = "point " + QString::number(count) + " charge " + QString::number(sh->charge); 
            break; }
        case sShape::ShapeType::LINE: {
            toAddName = "line " + QString::number(count) + " charge " + QString::number(sh->charge); 
            break; }
        case sShape::ShapeType::MOVING_POINT: {
            toAddName = "moving point " + QString::number(count) + " charge " + QString::number(sh->charge);
            break; }
        } 
        //toAddName += QString::number(sh->shapeType);
        sListWidget->addItem(toAddName);
        count++;
    }
}

void MainWindow::deleteShape()
{
    int r = sListWidget->currentRow();
    if (r != -1)
    {
        QListWidgetItem* item = sListWidget->takeItem(r);
        scribbleArea->shapes.erase(scribbleArea->shapes.begin() + r);
        delete item;
        scribbleArea->updateShapes();
    }
    scribbleArea->potShouldReCalc = true;
}
