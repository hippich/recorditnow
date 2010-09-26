#ifndef RECORDITNOW_KIPIPAGE_H
#define RECORDITNOW_KIPIPAGE_H


// own
#include "ui_kipipage.h"

// Qt
#include <QtGui/QWizardPage>


namespace KIPI {
    class Interface;
};


class KAction;
namespace RecordItNow {


class KIPIInterface;
class KIPIPage : public QWizardPage, Ui::KIPIPage
{
    Q_OBJECT


public:
    explicit KIPIPage(KIPI::Interface *interface, QWidget *parent = 0);
    ~KIPIPage();

    bool isComplete() const;

    void initializePage();


private:
    RecordItNow::KIPIInterface *m_interface;
    KAction *m_plugin;

    void setPlugin(KAction *plugin);


private slots:
    void currentSelectionChanged();


};


} // namespace RecordItNow


#endif // RECORDITNOW_KIPIPAGE_H
