#ifndef RECORDITNOW_KIPIINTERFACE_H
#define RECORDITNOW_KIPIINTERFACE_H


// KIPI
#include <libkipi/interface.h>
#include <libkipi/imagecollectionshared.h>

// Qt
#include <QtCore/QWeakPointer>


namespace KIPI {
    class PluginLoader;
    class Plugin;
}

class KAction;
namespace RecordItNow {


class KIPIInterface : public KIPI::Interface
{
    Q_OBJECT


public:
    explicit KIPIInterface(QObject *parent = 0);
    ~KIPIInterface();

    KIPI::ImageCollection currentAlbum();
    KIPI::ImageCollection currentSelection();
    QList<KIPI::ImageCollection> allAlbums();
    KIPI::ImageInfo info( const KUrl& url);
    int features() const;
    QList<KAction*> exportPlugins();

    virtual KIPI::ImageCollectionSelector* imageCollectionSelector(QWidget *parent);
    virtual KIPI::UploadWidget* uploadWidget(QWidget *parent);


private:
    KIPI::PluginLoader *m_pluginLoader;
    QList<KIPI::Plugin*> m_plugins;


};


} // namespace RecordItNow


#endif // KIPIINTERFACE_H
