#include "scribblearea.h"

// Used to load the image and place it in the widget
bool ScribbleArea::openImage(const QString& fileName)
{
    // Holds the image
    QImage loadedImage;

    // If the image wasn't loaded leave this function
    if (!loadedImage.load(fileName))
        return false;

    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    modified = false;
    update();
    return true;
}

// Save the current image
bool ScribbleArea::saveImage(const QString& fileName, const char* fileFormat)
{
    // Created to hold the image
    QImage visibleImage = image;
    resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        modified = false;
        return true;
    }
    else {
        return false;
    }
}


// Color the image area with white
void ScribbleArea::clearImage()
{
    image.fill(qRgb(255, 255, 255));
    modified = true;
    update();
}