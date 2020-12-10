#include <QtWidgets>
#include <QObject>
#include "qobject.h"


#include "mainwindow.h"

#include "foo.h"


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


    crtLineBut = new QPushButton("Create line", this);
    crtScribbleBut = new QPushButton("Scribble", this);
    crtSingleBut = new QPushButton("Create 1 point", this);
    dirBut = new QPushButton("Show directions", this);
    //avaliableNodesL = new QLabel(QString::number(scribbleArea->nNodes), this);
    chargeLabel = new QLabel("charge to add = k* ");
    chargeLE = new QLineEdit(this);
    chargeLE->setValidator(new QIntValidator(-1000000.f, 1000000, this));
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
    //precisionEqPtL = new QLabel(QString::number(precisionEqPtS->value()*0.001) + " %,  precision");

    QWidget* testQW = new QWidget(this);
    QGridLayout* controlsLayout = new QGridLayout();
    QHBoxLayout* hbox = new QHBoxLayout(testQW);
    QVBoxLayout* vbox1 = new QVBoxLayout();

    controlsLayout->addWidget(crtScribbleBut, 0, 0,1,1);
    controlsLayout->addWidget(crtSingleBut, 1, 0);
    controlsLayout->addWidget(crtLineBut, 2, 0);
    controlsLayout->addWidget(dirBut, 3, 0);
    
    
    // 5 free
    controlsLayout->addWidget(deleteAllShapeBut,6,3);
    controlsLayout->addWidget(deleteShapeBut, 6, 2);

    controlsLayout->addWidget(showEqCB, 8, 0);
    controlsLayout->addWidget(precisionEqPtS ,8,1,1,2);
    controlsLayout->addWidget(clearStorageEqPtsBut,8,4);
    //controlsLayout->addWidget(precisionEqPtL, 8, 4);

    controlsLayout->addWidget(chargeLabel, 7, 1);
    controlsLayout->addWidget(chargeLE, 7, 2, 1, 2);
    controlsLayout->addWidget(reverseChargeBut, 7, 0);

    
    controlsLayout->addWidget(drawPotMapCB, 9, 0);
    controlsLayout->addWidget(drawPotMapS, 9, 1,1,2);
    
    controlsLayout->addWidget(drawElFieldCB,10,0);
    controlsLayout->addWidget(scaleOfDrawElFieldS,10,1,1,2);
    controlsLayout->addWidget(sListWidget,0,1,6,3);
    

    vbox1->addWidget(scribbleArea);
    //hbox->addWidget(sListWidget);
    hbox->addLayout(vbox1);
    hbox->addLayout(controlsLayout);
    //hbox->addLayout(vbox3);
    connect(chargeLE, &QLineEdit::textChanged,this, [=]() {scribbleArea->chargeToAdd = chargeLE->text().toInt(); });
    connect(crtScribbleBut, &QPushButton::clicked, this, [=]() {scribbleArea->setScribbleMode(); updateListWidget(); scribbleArea->setMouseTracking(true); });
    connect(crtSingleBut, &QPushButton::clicked, this, [=]() {scribbleArea->setSingleMode(); updateListWidget(); scribbleArea->setMouseTracking(true);  });
    connect(crtLineBut, &QPushButton::clicked, this, [=]() {scribbleArea->setLineMode(); updateListWidget(); scribbleArea->setMouseTracking(true);  });
    
   
    connect(scribbleArea, &ScribbleArea::dataReady, this, [=]() {updateListWidget(); });
    //connect(potMapBut, &QPushButton::clicked, this, [=]() {scribbleArea->calculatePotencial(); scribbleArea->setMouseTracking(false); });
    
    connect(dirBut, &QPushButton::clicked, this, [=]() { scribbleArea->setDirectionsMode() ; scribbleArea->setMouseTracking(true); });
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


    //setLayout(hbox);
    setCentralWidget(testQW);
    //setCentralWidget(hbox);
    // Create actions and menus
    createActions();
    createMenus();

    // Set the title
    setWindowTitle(tr("Scribble"));

    // Size the app
    //resize(500, 500);
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
        case 0: {toAddName = "point " + QString::number(count) + " charge " + QString::number(sh->charge); break; }
        case 1: {toAddName = "line " + QString::number(count) + " charge " + QString::number(sh->charge); break; }
        } 
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
