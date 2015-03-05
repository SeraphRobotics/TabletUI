#ifndef MASTERCONTROLUNIT_H
#define MASTERCONTROLUNIT_H

#include <QObject>
#include <DataStructures/manipulations.h>
#include "View/UI_structs.h"

class ScannerWatcher;
class ScannerController;

class MasterControlUnit : public QObject
{
    Q_OBJECT
public:
    explicit MasterControlUnit(QObject *parent = 0);
    ~MasterControlUnit();

    QList< UI_Shell_Modification> getShellModificationsList();
signals:
    void shellModifications(QList< UI_Shell_Modification> mods);

    void USBPlugedIn();//choose patient
    void USBItemsChanged(QList < UI_USB_Item > items);
    void USBUnPluged();//choose patient
    void scanLoaded(Foot_Type foottype, QImage scanimage);//settings
    void view3D(Foot_Type foot, View_3D_Item mesh);//settings and review
    void connectedToInternet(); // setup
    void disconnectedFromInternet(); // setup

public slots:
    void getShellModifications();
    void setFrontEdge(Front_Edge points, Foot_Type footside);//settings-rx1
    void setTopCoat(Foot_Type foot,Top_Coat coating);//settings-rx2-topcoat
    void addModifications(QList< UI_Shell_Modification> modifications, Foot_Type foot);//settings-rx2-shellmods
    void removeAllModifications(Foot_Type foot);//settings-rx2-shellmods
    void newTypeofShellModification(UI_Shell_Modification modification);
    void setPosting(Posting posting, Foot_Type foot);//settings-rx2-posting
    void saveCurrentRx(); //review
    void saveCurrentRxAndLoadToUSB(); //review
    void loadCustomerNumber(QString); //setup

private slots:

    void test3d(Foot_Type foot);
    void testScanLoads(int num);

private:
    ScannerWatcher *scanWatcher;
    ScannerController *scanController;
};


int randInt(int low, int high);
#endif // MASTERCONTROLUNIT_H
