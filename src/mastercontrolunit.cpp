// qt
#include <QDebug>
#include <qglobal.h>
#include <QTime>

// seraphLibs
#include <libraries/shared/stl/stlfile.h>
//#include <DataStructures/orthoticmanager.h>
#include <ScannerMinder/scannerwatcher.h>
#include <ScannerMinder/scannercontroller.h>

// local
#include "mastercontrolunit.h"


MasterControlUnit::MasterControlUnit(QObject *parent) :
    QObject(parent)
{
    scanWatcher = new ScannerWatcher();
    scanController = new ScannerController();
    scanController->connect(scanWatcher,SIGNAL(scannerPort(QString)),scanController,SLOT(portSelected(QString)));
    scanController->connect(scanWatcher,SIGNAL(disconnected()), scanController,SLOT(disconnected()));
    connect(scanController, SIGNAL(scanError(QString)), this, SIGNAL(scanError(QString)));
    connect(scanController, SIGNAL(scanProgress(int)), this, SIGNAL(scanProgress(int)));
    connect(scanController, SIGNAL(scanComplete()), this, SIGNAL(scanCompleted()));


    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
}

MasterControlUnit::~MasterControlUnit()
{
    delete scanWatcher;
    delete scanController;
}

// public
void MasterControlUnit::beginScan()
{
    scanController->startScan();
}


void  MasterControlUnit::test3d(Foot_Type foot){
    STLFile f;
    View_3D_Item v3d;

    if(foot == kLeft){
        f.read("left.stl");

    }else{
        f.read("right.stl");
    }
    v3d.color = QColor(randInt(0,255),randInt(0,255),randInt(0,255));

    foreach(STLFacet* facet,f.GetMesh()->GetFacets()){
        STLFacet fct;
        fct.triangle = facet->triangle;
        fct.normal = facet->normal;
        v3d.mesh->AddFacet(fct);
    }

    emit view3D(foot,v3d);
}

void  MasterControlUnit::testScanLoads(int num){
    if (num >1){
        QImage i;
        i.load("left-scan.png");
        emit scanLoaded(kLeft,i);
    }

    QImage i2;
    i2.load("right-scan.png");
    emit scanLoaded(kRight,i2);
}

QList< UI_Shell_Modification> MasterControlUnit::getShellModificationsList(){
    QList< UI_Shell_Modification> mods;
    UI_Shell_Modification m1;
    m1.name="Hole";
    m1.height=0;
    m1.depth=100;
    m1.stiffness=0;
    Border b;
    b.append(QPointF(0,0));
    b.append(QPointF(10,0));
    b.append(QPointF(0,10));
    b.append(QPointF(10,10));
    m1.outer_border=b;

    UI_Shell_Modification m2;
    m2.name="box with hole";
    m2.height=10;
    m2.depth=0;
    m2.stiffness=110;
    Border b2;
    b2.append(QPointF(0,0));
    b2.append(QPointF(30,0));
    b2.append(QPointF(0,30));
    b2.append(QPointF(30,30));
    m2.outer_border=b2;
  //  m2.inner_borders.append(b);

    mods.append(m1);
    mods.append(m2);
    return mods;

}


void MasterControlUnit::getShellModifications(){
    emit shellModifications(getShellModificationsList());
}

void MasterControlUnit::setFrontEdge(Front_Edge points, Foot_Type footside){
    qDebug()<<"Front Edge for "<<footside<<" is: ";
    qDebug()<<"\t" << points.p1 << ", " << points.p2 << ", "<< points.p3 << ", " << points.p4;

    if(footside==kLeft){
        QImage i;
        i.load("left-top.jpg");
        emit scanLoaded(footside,i);
    }else if(footside ==kRight){
        QImage i;
        i.load("right-top.jpg");
        emit scanLoaded(footside,i);
    }
}//settings-rx1

void MasterControlUnit::setTopCoat(Foot_Type foot,Top_Coat coating){
    qDebug()<<"set Top coat for "<<foot<<".";
    qDebug()<<"\tdensity (l/m/h): " <<coating.density;
    qDebug()<<"\tdepth (mm): " <<coating.depth;
    qDebug()<<"\tthickness (mm): " <<coating.thickness;
    qDebug()<<"\tstyle: " <<coating.style;
}//settings-rx2-topcoat

void MasterControlUnit::addModifications(QList< UI_Shell_Modification> modifications, Foot_Type foot){
    qDebug()<<"added mods on "<<foot<<" : ";
    for(int i=0;i<modifications.size();i++){
        qDebug()<<"\t"<<modifications.at(i).name;
    }
    test3d(foot);

}//settings-rx2-shellmods

void MasterControlUnit::removeAllModifications(Foot_Type foot){
    qDebug()<<"remove mods "<<foot;
}//settings-rx2-shellmods

void MasterControlUnit::newTypeofShellModification(UI_Shell_Modification modification){
    qDebug()<<"new modification: "<<modification.name<<"\n\touter:";
    for(int i=0;i<modification.outer_border.size();i++){
        qDebug()<<"\t\t"<<modification.outer_border.at(i);
    }
    for(int i=0;i<modification.inner_borders.size();i++){
        qDebug()<<"\tinner border "<<i;
        //for(int j=0;j<modification.inner_borders.at(i).size();j++){
        //   ;// qDebug()<<"\t\t"<<modification.inner_borders.at(i).at(j);
        //}
    }
}
void MasterControlUnit::setPosting(Posting posting, Foot_Type foot){
    qDebug() << "set posting for foot:"<<foot;
    qDebug() << "\tangle: " << posting.angle;
    qDebug() << "\tverticle: " << posting.verticle;
    qDebug() << "\tvargus: " << posting.varus_valgus;
    qDebug() << "\tside: "<<posting.for_rear;

    test3d(foot);

}//settings-rx2-posting

void MasterControlUnit::saveCurrentRx(){qDebug()<<"save rx";} //review
void MasterControlUnit::saveCurrentRxAndLoadToUSB(){qDebug()<<"save rx and load to USB";} //review
void MasterControlUnit::loadCustomerNumber(QString number){qDebug()<<"customer number is "<<number;} //setup


int randInt(int low, int high){
    return qrand() % ((high + 1) - low) + low;
}
