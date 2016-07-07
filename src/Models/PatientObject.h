#ifndef PATIENTOBJECT_H
#define PATIENTOBJECT_H

#include <QObject>
#include <QQmlListProperty>

#include "View/UI_structs.h"

class NameObject;

class PatientObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id
               NOTIFY sigIdChanged)
    Q_PROPERTY(QString doctorId READ doctorId
               NOTIFY sigDoctorIdChanged)
    Q_PROPERTY(NameObject* name READ name
               NOTIFY sigNameChanged)
    Q_PROPERTY(QQmlListProperty<QObject> scansList READ scansList
               NOTIFY sigScansListChanged)
    Q_PROPERTY(QQmlListProperty<QObject> rxList READ rxList
               NOTIFY sigRxListChanged)
    Q_PROPERTY(int scansListCurrentIndex READ scansListCurrentIndex
               WRITE setScansListCurrentIndex)
    Q_PROPERTY(int rxListCurrentIndex READ rxListCurrentIndex
               WRITE setRxListCurrentIndex)
    Q_PROPERTY(QDateTime dateTime READ dateTime
               NOTIFY sigDateTimeChanged)

public:
    explicit PatientObject(NameObject *name,
                           const QString &id = QString::null,
                           const QString &doctorId = QString::null,
                           QObject *parent = 0);

    QString id() const;
    QString doctorId() const;
    QDateTime dateTime() const;
    NameObject* name();

    void setId(const QString &id);
    void setName(NameObject *firstName);

    void appendDataObject(const UI_USB_Item &object);
    void removeDataObject(const QString &objectId, const UI_USB_Item::Type &type);

    QQmlListProperty<QObject> scansList();
    QQmlListProperty<QObject> rxList();

    int scansListCurrentIndex() const;
    int rxListCurrentIndex() const;

    void setScansListCurrentIndex(const int &currentIndex);
    void setRxListCurrentIndex(const int &currentIndex);
    void sortRxViaDate();

    QString getScanIdByOrthoticId(const QString &orthoId) const;

signals:
    void sigIdChanged() const;
    void sigNameChanged() const;
    void sigRxListChanged() const;
    void sigScansListChanged() const;
    void sigDoctorIdChanged() const;
    void sigDateTimeChanged() const;

private:
    static bool _dateTimeSortFunction(QObject *o1, QObject *o2);

private:
    QString m_Id;
    QString m_DoctorId;
    NameObject *m_NameObject;

    QList<QObject*> m_ScansList;
    QList<QObject*> m_RxList;
    QMap<QString,QObject*> m_ScansMap;
    QMap<QString,QString> m_RxToScansMap;
    
    int m_ScansListCurrentIndex;
    int m_RxListCurrentIndex;
};

#endif // PATIENTOBJECT_H
