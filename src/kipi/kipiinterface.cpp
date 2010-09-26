




// own
#include "kipiinterface.h"
#include "helper.h"
#include "mainwindow.h"

// KIPI
#include <libkipi/plugin.h>
#include <libkipi/imagecollectionselector.h>
#include <libkipi/uploadwidget.h>
#include <libkipi/pluginloader.h>
#include <libkipi/imagecollectionshared.h>
#include <libkipi/imageinfo.h>
#include <libkipi/imageinfoshared.h>

// KDE
#include <kaction.h>
#include <kdebug.h>


namespace RecordItNow {


class KIPIImageInfo : public KIPI::ImageInfoShared {


public:
    KIPIImageInfo(KIPI::Interface *interface, const KUrl &url)
        : KIPI::ImageInfoShared(interface, url)
    {}


    QString title()
    {

        return path().fileName();

    }


    QString description()
    {

        return m_description;

    }


    void setDescription(const QString &description)
    {

        m_description = description;

    }


    int angle() {

        return 0;

    }


    QMap<QString,QVariant> attributes()
    {

        return m_attributes;

    }


    void delAttributes(const QStringList &attributes)
    {

        foreach (const QString &attr, attributes) {
            m_attributes.remove(attr);
        }

    }


    void clearAttributes()
    {

        m_attributes.clear();

    }

    void addAttributes(const QMap<QString, QVariant> &attributes)
    {

        QMapIterator<QString, QVariant> it(attributes);
        while (it.hasNext()) {
            it.next();

            m_attributes.insert(it.key(), it.value());
        }

    }


private:
    QString m_description;
    QMap<QString, QVariant> m_attributes;


};


class KIPIImageCollection : public KIPI::ImageCollectionShared
{


public:
    KIPIImageCollection(const KUrl::List &images = KUrl::List())
        : KIPI::ImageCollectionShared()
    {

        m_images = images;

    }


    KUrl::List images()
    {

        return m_images;

    }


    QString name()
    {

        return QString();

    }


    void addImage(const KUrl &image)
    {

        m_images.append(image);

    }


    void removeImage(const KUrl &image)
    {

        m_images.removeAll(image);

    }


private:
    KUrl::List m_images;


};



KIPIInterface::KIPIInterface(QObject *parent)
    : KIPI::Interface(parent)
{

    m_pluginLoader = new KIPI::PluginLoader(QStringList(), this);
    KIPI::PluginLoader::PluginList pluginList = m_pluginLoader->pluginList();
    foreach (KIPI::PluginLoader::Info* pluginInfo, pluginList) {
        if (!pluginInfo->shouldLoad()) {
            continue;
        }


        KIPI::Plugin* plugin = pluginInfo->plugin();
        if (!plugin) {
            continue;
        }

        plugin->setup(Helper::self()->window());

        QList<KAction*> actions = plugin->actions();
        foreach (KAction* action, actions) {
            const KIPI::Category category = plugin->category(action);

            if (category == KIPI::ExportPlugin) {
                m_plugins.append(plugin);

                break;
            }
        }
        if (!m_plugins.contains(plugin)) {
            delete plugin;
        }
    }

}


KIPIInterface::~KIPIInterface()
{

    qDeleteAll(m_plugins);
    //delete m_pluginLoader;

}


KIPI::ImageCollection KIPIInterface::currentAlbum()
{

    return KIPI::ImageCollection();

}


KIPI::ImageCollection KIPIInterface::currentSelection()
{

    RecordItNow::KIPIImageCollection *collection = new RecordItNow::KIPIImageCollection();
    collection->addImage(KUrl(RecordItNow::Helper::self()->currentFile()));
    return KIPI::ImageCollection(collection);

}


QList<KIPI::ImageCollection> KIPIInterface::allAlbums()
{

    QList<KIPI::ImageCollection> list;
    list << currentAlbum() << currentSelection();
    return list;

}


KIPI::ImageInfo KIPIInterface::info(const KUrl& url)
{

    KIPI::ImageInfo info(new KIPIImageInfo(this, url));
    return info;

}


int KIPIInterface::features() const
{

    return KIPI::HostSupportsProgressBar;

}


QList<KAction*> KIPIInterface::exportPlugins()
{

    QList<KAction*> plugins;

    foreach (KIPI::Plugin *plugin, m_plugins) {
        foreach (KAction *action, plugin->actions()) {
            if (plugin->category(action) == KIPI::ExportPlugin) {
                plugins.append(action);
            }
        }
    }

    return plugins;

}


KIPI::ImageCollectionSelector* KIPIInterface::imageCollectionSelector(QWidget *parent)
{

    return new KIPI::ImageCollectionSelector(parent);

}


KIPI::UploadWidget* KIPIInterface::uploadWidget(QWidget *parent)
{

    return new KIPI::UploadWidget(parent);

}



} // namespace RecordItNow


#include "kipiinterface.moc"
