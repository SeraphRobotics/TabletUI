#ifndef PATIENTSLISTMODEL_H
#define PATIENTSLISTMODEL_H

#include <QObject>
#include <QQmlListProperty>

#include "DataStructures/patientdata.h"
#include "View/UI_structs.h"

class PatientManager;
class PatientObject;

class PatientsListModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<QObject> list READ patientsList
               NOTIFY sigPatientsListModificate())
    Q_PROPERTY(QQmlListProperty<QObject> searchPatientsList READ searchPatientsList
               NOTIFY sigSearchListModificate())
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex
               NOTIFY sigCurrentIndexChanged)

public:
    explicit PatientsListModel(QObject *parent = 0);

    QList<UI_Patient> getPatientsList();
    void updatePatientsList();

    QQmlListProperty<QObject> patientsList();
    QQmlListProperty<QObject> searchPatientsList();

    Q_INVOKABLE PatientObject *getSpecificItem(const int &index) const;

    Q_INVOKABLE void append(PatientObject *patient);
    Q_INVOKABLE void setSearchListViaSpecificKey(const QString &key);
    Q_INVOKABLE void restartListModel();
    Q_INVOKABLE void sortWithSpecificOrder(const QString &order);
    Q_INVOKABLE bool contains(const QString &patientName) const;

    int currentIndex() const;
    Q_INVOKABLE int size() const;

    void setCurrentIndex(int index);

    void clear();

    PatientObject *getCurrentPatient() const;

    QString getScanIdByOrthoticId(const QString &orthoId) const;
    QString getSiblingScanIdByScan(const QString &scanId) const;
    QString getSiblingOrthoticByOrthoticId(const QString &orthoId) const;

    void addRxToPatient(const QString &patientId, const Rx &rx);
    void replaceRxAtPatient(const QString &patientId, const QString &scanId, const Rx &rx);
    void removeRxFromPatient(const QString &patientId, const QString &rxId);
    Rx getRxOfPatient(const QString &patientId, const QString &rxId) const;
    void addScansToPatient(const QString &leftScanId,
                           const QString &rightScanId,
                           const QString &patientId,
                           const QString &docId);//choose patient
    void removeScanFromPatient(const QString &patientId, const QString &scanId);
    void newPatientFromScans(const QString &leftScanId,
                             const QString &rightScanId,
                             const QString &patientName,
                             const QString &comments,
                             const QString &docId);//choose patient

signals:
    void sigPatientsListModificate() const;
    void sigCurrentIndexChanged() const;
    void sigSearchListModificate() const;

private:
    PatientManager *m_PatientManager;
    QList<QObject*> m_PatientsList;
    QList<QObject*> m_SearchPatientsList;
    int m_CurrentIndex;

private:
    /// "Sorting routine looks painfully slow.
    ///  It's not a problem if you need to sort a small list,
    ///  but this will quickly start to be a performance hit I think.
    ///  Use SortFilterProxyModel "
    ///  But in our application, especially in first poc form it should be enough to use sorting this way instead of QAbstractItemModel.
    static bool _sortViaNameAscending(QObject *o1, QObject *o2);
    static bool _sortViaNameDescending(QObject *o1, QObject *o2);
    static bool _sortViaDateTimeAscending(QObject *o1, QObject *o2);
    static bool _sortViaDateTimeDescending(QObject *o1, QObject *o2);

    void clearSearchList();
};

#endif // PATIENTSLISTMODEL_H
