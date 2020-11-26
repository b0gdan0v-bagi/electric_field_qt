#include <QtWidgets>
#include <QObject>
#include "qobject.h"


#include "mainwindow.h"


// MainWindow constructor
MainWindow::MainWindow()
{
    sListWidget = new QListWidget(this);
    scribbleArea = new ScribbleArea(this);
    // Create the ScribbleArea widget and make it
    // the central widget
    QGridLayout* controlsLayout = new QGridLayout(this);
    clearBut = new QPushButton("Clear", this);
    testBut = new QPushButton("TEST", this);
    test2But = new QPushButton("TEST 2", this);
    updateBut = new QPushButton("Update", this);
    crtLineBut = new QPushButton("Create line", this);
    crtScribbleBut = new QPushButton("Scribble", this);
    crtSingleBut = new QPushButton("Create 1 point", this);
    avaliableNodesL = new QLabel(QString::number(scribbleArea->nNodes), this);
    
    //nNodes = &scribbleArea->nNodes;
    //later - connect label to nNodes in area;
    //connect(nNodes, &QObject::variableChanged, [=](int i) {
    //    avaliableNodesL->setText("Number = " + QString::number(i));

    testQSB = new QSpinBox(this);
    
    //shapes.push_back(new sLine());
    //shapes.back()->GetNextNode(vf2D(50, 50));
    //shapes.back()->GetNextNode(vf2D(100, 100));
    //testBut->setText(QString::number(shapes.back()->vecNodes.size()));
    //shapes.back()->vecNodes[0].pos = vf2D(50, 50);

    
    //setCentralWidget(scribbleArea);
    //QWidget* testQW = new QWidget(this);

    //QHBoxLayout* hbox = new QHBoxLayout(testQW);
    QHBoxLayout* hbox = new QHBoxLayout(this);
    QVBoxLayout* vbox3 = new QVBoxLayout();
    QVBoxLayout* vbox1 = new QVBoxLayout();
    vbox3->addWidget(sListWidget);
    controlsLayout->addWidget(crtScribbleBut, 0, 0);
    controlsLayout->addWidget(crtSingleBut, 1, 0);
    controlsLayout->addWidget(crtLineBut, 2, 0);

    controlsLayout->addWidget(testBut,3,0);
    controlsLayout->addWidget(test2But,4,0);
    controlsLayout->addWidget(testQSB,5,0);
    controlsLayout->addWidget(clearBut,6,0);
    controlsLayout->addWidget(updateBut,7,0);
    controlsLayout->addWidget(avaliableNodesL,8,0);
    //controlsLayout->addWidget(sListWidget);
    

    vbox1->addWidget(scribbleArea);
    //hbox->addWidget(sListWidget);
    hbox->addLayout(vbox1);
    hbox->addLayout(controlsLayout);
    hbox->addLayout(vbox3);
    connect(crtScribbleBut, &QPushButton::clicked, this, [=]() {scribbleArea->setScribbleMode(); });
    connect(crtSingleBut, &QPushButton::clicked, this, [=]() {scribbleArea->setSingleMode(); });
    connect(crtLineBut, &QPushButton::clicked, this, [=]() {scribbleArea->setLineMode();});
    connect(updateBut, &QPushButton::clicked, this, [=]() {scribbleArea->updateShapes(); });

    connect(testBut, &QPushButton::clicked, this, [=]() {
        //QPoint p1 = vec2DtoQPoint(shapes.back()->vecNodes[0].pos);
        //QPoint p2 = vec2DtoQPoint(shapes.back()->vecNodes[1].pos);
        //scribbleArea->drawLineBetween(p1, p2);
        });
    connect(test2But, &QPushButton::clicked, this, [=]() { 
        scribbleArea->drawCylTo(QPoint(5 * testQSB->value(), 5 * testQSB->value()), 10); 
        });

    connect(clearBut, &QPushButton::clicked, this, [=]() { scribbleArea->clearImage(); });
    setLayout(hbox);
    //setCentralWidget(scribbleArea);
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

void MainWindow::testColorBut()
{
    scribbleArea->setPenColor(QColor(Qt::GlobalColor::blue));
    //scribbleArea->drawLineTo(QPoint(50, 50));
    scribbleArea->drawCylTo(QPoint(50, 50),10);
}
void MainWindow::testColorBut2()
{
    scribbleArea->setPenColor(QColor(Qt::GlobalColor::blue));
    //scribbleArea->drawLineTo(QPoint(50, 50));
    
    scribbleArea->drawCylTo(QPoint(5* testQSB->value(), 50), 10);
}
// Define menu actions that call functions

void MainWindow::createActions()
{
    // Create the action tied to the menu
    openAct = new QAction(tr("&Open..."), this);

    // Define the associated shortcut key
    openAct->setShortcuts(QKeySequence::Open);

    // Tie the action to MainWindow::open()
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    // Get a list of the supported file formats
    // QImageWriter is used to write images to files
    foreach(QByteArray format, QImageWriter::supportedImageFormats()) {
        QString text = tr("%1...").arg(QString(format).toUpper());

        // Create an action for each file format
        QAction* action = new QAction(text, this);

        // Set an action for each file format
        action->setData(format);

        // When clicked call MainWindow::save()
        connect(action, SIGNAL(triggered()), this, SLOT(save()));

        // Attach each file format option menu item to Save As
        saveAsActs.append(action);
    }

    // Create print action and tie to MainWindow::print()
    printAct = new QAction(tr("&Print..."), this);
    connect(printAct, SIGNAL(triggered()), scribbleArea, SLOT(print()));

    // Create exit action and tie to MainWindow::close()
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    // Create pen color action and tie to MainWindow::penColor()
    penColorAct = new QAction(tr("&Pen Color..."), this);
    connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));

    // Create pen width action and tie to MainWindow::penWidth()
    penWidthAct = new QAction(tr("Pen &Width..."), this);
    connect(penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));

    // Create clear screen action and tie to MainWindow::clearImage()
    clearScreenAct = new QAction(tr("&Clear Screen"), this);
    clearScreenAct->setShortcut(tr("Ctrl+L"));
    connect(clearScreenAct, SIGNAL(triggered()),
        scribbleArea, SLOT(clearImage()));

    // Create about action and tie to MainWindow::about()
    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    // Create about Qt action and tie to MainWindow::aboutQt()
    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

// Create the menubar
void MainWindow::createMenus()
{
    // Create Save As option and the list of file types
    saveAsMenu = new QMenu(tr("&Save As"), this);
    foreach(QAction * action, saveAsActs)
        saveAsMenu->addAction(action);

    // Attach all actions to File
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addMenu(saveAsMenu);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    // Attach all actions to Options
    optionMenu = new QMenu(tr("&Options"), this);
    optionMenu->addAction(penColorAct);
    optionMenu->addAction(penWidthAct);
    optionMenu->addSeparator();
    optionMenu->addAction(clearScreenAct);

    // Attach all actions to Help
    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    // Add menu items to the menubar
    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addMenu(helpMenu);
}

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