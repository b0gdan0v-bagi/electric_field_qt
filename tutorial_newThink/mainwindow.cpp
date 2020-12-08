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
    
    clearBut = new QPushButton("Clear", this);
    potMapBut = new QPushButton("show potencial map", this);
    showEqBut = new QPushButton("show equipotential", this);
    updateBut = new QPushButton("Update", this);
    crtLineBut = new QPushButton("Create line", this);
    crtScribbleBut = new QPushButton("Scribble", this);
    crtSingleBut = new QPushButton("Create 1 point", this);
    dirBut = new QPushButton("Show directions", this);
    avaliableNodesL = new QLabel(QString::number(scribbleArea->nNodes), this);
    chargeLabel = new QLabel("charge = k* ");
    chargeLE = new QLineEdit(this);
    chargeLE->setValidator(new QIntValidator(-1000000.f, 1000000, this));
    reverseChargeBut = new QPushButton("Reverse charge", this);
    drawElFieldMapBut = new QPushButton("draw field vectors", this);
    deleteShapeBut = new QPushButton("Delete selected shape", this);

    chargeLE->setText(QString::number(1));
    
    //nNodes = &scribbleArea->nNodes;
    //later - connect label to nNodes in area;
    //connect(nNodes, &QObject::variableChanged, [=](int i) {
    //    avaliableNodesL->setText("Number = " + QString::number(i));

    testQSB = new QSpinBox(this);

    QWidget* testQW = new QWidget(this);
    QGridLayout* controlsLayout = new QGridLayout();
    QHBoxLayout* hbox = new QHBoxLayout(testQW);

    QVBoxLayout* vbox1 = new QVBoxLayout();

    controlsLayout->addWidget(crtScribbleBut, 0, 0);
    controlsLayout->addWidget(crtSingleBut, 1, 0);
    controlsLayout->addWidget(crtLineBut, 2, 0);

    controlsLayout->addWidget(potMapBut,3,0);
    controlsLayout->addWidget(showEqBut,4,0);
    controlsLayout->addWidget(testQSB,5,0);
    controlsLayout->addWidget(clearBut,6,0);
    controlsLayout->addWidget(updateBut,7,0);
    controlsLayout->addWidget(dirBut,8,0);
    controlsLayout->addWidget(chargeLabel,9,0);
    controlsLayout->addWidget(chargeLE,9,1,1,2);
    controlsLayout->addWidget(reverseChargeBut,9,4);
    controlsLayout->addWidget(drawElFieldMapBut,10,0);
    controlsLayout->addWidget(deleteShapeBut,7,2);
    controlsLayout->addWidget(sListWidget,0,2,7,3);
    

    vbox1->addWidget(scribbleArea);
    //hbox->addWidget(sListWidget);
    hbox->addLayout(vbox1);
    hbox->addLayout(controlsLayout);
    //hbox->addLayout(vbox3);
    connect(chargeLE, &QLineEdit::textChanged,this, [=]() {scribbleArea->chargeToAdd = chargeLE->text().toInt(); });
    connect(crtScribbleBut, &QPushButton::clicked, this, [=]() {scribbleArea->setScribbleMode(); updateListWidget(); scribbleArea->setMouseTracking(true); });
    connect(crtSingleBut, &QPushButton::clicked, this, [=]() {scribbleArea->setSingleMode(); updateListWidget(); scribbleArea->setMouseTracking(true);  });
    connect(crtLineBut, &QPushButton::clicked, this, [=]() {scribbleArea->setLineMode(); updateListWidget(); scribbleArea->setMouseTracking(true);  });
    connect(updateBut, &QPushButton::clicked, this, [=]() {scribbleArea->updateShapes(); updateListWidget(); scribbleArea->setMouseTracking(true); });
    connect(clearBut, &QPushButton::clicked, this, [=]() { scribbleArea->clearImage(); scribbleArea->setMouseTracking(true); });
    connect(scribbleArea, &ScribbleArea::dataReady, this, [=]() {updateListWidget(); });
    connect(potMapBut, &QPushButton::clicked, this, [=]() {scribbleArea->calculatePotencial(); scribbleArea->setMouseTracking(false); });
    connect(showEqBut, &QPushButton::clicked, this, [=]() { scribbleArea->setEqPotLinesMode(); scribbleArea->setMouseTracking(true); });
    connect(dirBut, &QPushButton::clicked, this, [=]() { scribbleArea->setDirectionsMode() ; scribbleArea->setMouseTracking(true); });
    connect(reverseChargeBut, &QPushButton::clicked, this, [=]() {chargeLE->setText(QString::number(chargeLE->text().toInt()*-1)); });
    connect(drawElFieldMapBut, &QPushButton::clicked, this, [=]() {scribbleArea->drawElFieldAllArea(); });
    connect(deleteShapeBut, &QPushButton::clicked, this, &MainWindow::deleteShape );

    
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
}
