

// own
#include "screenshotrecorder.h"
#include <screenshot.h>

// KDE
#include <klocalizedstring.h>
#include <kplugininfo.h>
#include <kdebug.h>

// Qt
#include <QtGui/QPixmap>
#include <QtGui/QX11Info>
#include <QtCore/QFile>
#include <QtGui/QPainter>

// X
#include <X11/extensions/Xfixes.h>


static const char format[][7] = { "PNG", "JPG", "BMP", "PPM", "TIFF", "XBM", "XPM"};


K_PLUGIN_FACTORY(myFactory, registerPlugin<ScreenshotRecorder>();)
K_EXPORT_PLUGIN(myFactory("screenshot_recorder"))
ScreenshotRecorder::ScreenshotRecorder(QObject *parent, const QVariantList &args)
    : AbstractRecorder(parent, args)
{



}



ScreenshotRecorder::~ScreenshotRecorder()
{



}


bool ScreenshotRecorder::hasFeature(const AbstractRecorder::Feature &feature) const
{

    switch (feature) {
    case AbstractRecorder::Sound:
    case AbstractRecorder::Fps:
    case AbstractRecorder::Pause:
    case AbstractRecorder::Stop:
    default: return false;
    }

}


void ScreenshotRecorder::record(const AbstractRecorder::Data &d)
{

    emit status(i18n("Cheese!"));

    // reload cfg
    Settings::self()->readConfig();

    const int x = d.geometry.x();
    const int y = d.geometry.y();

    const int w = d.geometry.width();
    const int h = d.geometry.height();

    const qlonglong window = d.winId != -1 ? d.winId : QX11Info::appRootWindow();

    QPixmap cheese = QPixmap::grabWindow(window, x, y, w, h);

    QFile outFile(d.outputFile);
    if (outFile.exists()) {
        if (!Settings::overwrite()) {
            while (outFile.exists()) {
                outFile.setFileName(outFile.fileName()+"_");
            }
        }
    }

    if (!outFile.open(QIODevice::WriteOnly)) {
        emit error(i18n("Cannot open output file: %1", outFile.errorString()));
        return;
    }

    kDebug() << "format:" << format[Settings::format()];
    if (!cheese.save(&outFile, format[Settings::format()])) {
        outFile.close();
        outFile.remove();
        emit error(i18n("Cannot save image."));
        return;
    }
    outFile.close();
    emit finished(AbstractRecorder::Normal);

}


void ScreenshotRecorder::pause()
{



}


void ScreenshotRecorder::stop()
{



}


