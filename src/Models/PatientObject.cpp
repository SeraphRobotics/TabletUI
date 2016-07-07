#include "PatientObject.h"
#include "NameObject.h"
#include "PatientDataObject.h"

#include <QDebug>

/*!
 * \class PatientObject
 * \brief Class to store all patient data including Scans and Rx's
 */

PatientObject::PatientObject(NameObject *name,
                             const QString &id,
                             const QString &doctorId,
                             QObject *parent) :
    QObject(parent),
    m_Id(id),
    m_DoctorId(doctorId),
    m_NameObject(name)

{
    m_NameObject->setParent(this);
}

QString PatientObject::id() const
{
    return m_Id;
}

QString PatientObject::doctorId() const
{
    return m_DoctorId;
}

NameObject* PatientObject::name()
{
    return m_NameObject;
}

void PatientObject::setId(const QString &id)
{
    if (m_Id != id)
    {
        m_Id = id;
        emit sigIdChanged();
    }
}

void PatientObject::setName(NameObject *name)
{
    if (name != m_NameObject && name != nullptr)
    {
        m_NameObject = name;
        emit sigNameChanged();
    }
}

QQmlListProperty<QObject> PatientObject::scansList()
{
    return QQmlListProperty<QObject>(this, m_ScansList);
}

QQmlListProperty<QObject> PatientObject::rxList()
{
    return QQmlListProperty<QObject>(this, m_RxList);
}

void PatientObject::appendDataObject(const UI_USB_Item &object)
{
    PatientDataObject *patientDataObject = new PatientDataObject(this);

    patientDataObject->setDataTime(object.datetime);

    patientDataObject->setComment(object.comment);
    patientDataObject->setId(object.id);
    patientDataObject->setFoot(object.foot);
    patientDataObject->setPatientName(new NameObject(object.patient.first,
                                                     object.patient.last,
                                                     object.patient.title,
                                                     this)
                                      );

    if (object.type == UI_USB_Item::kScan)
    {
        m_ScansList.append(patientDataObject);
        m_ScansMap.insert(patientDataObject->id(), patientDataObject);
        qDebug()<<"Scan added, Scan list size:"<<m_ScansList.size(); /// @todo
        emit sigScansListChanged();
    }
    else if(object.type == UI_USB_Item::kRx)
    {
        m_RxList.append(patientDataObject);
        m_RxToScansMap.insert(patientDataObject->id(), object.parentId);
        qDebug()<<"Rx added, Rx list size:"<<m_RxList.size(); /// @todo
        emit sigRxListChanged();
    }
    else if(object.type == UI_USB_Item::kAssociatedScan)
    {
        m_ScansMap.insert(patientDataObject->id(), patientDataObject);
    }
}

void PatientObject::removeDataObject(const QString &objectId, const UI_USB_Item::Type &type)
{
    if (type == UI_USB_Item::kScan) {
        for (int i = 0; i < m_ScansList.count(); i++) {
            auto patientData = qobject_cast<PatientDataObject*>(m_ScansList.at(i));

            if (patientData->id() == objectId) {
                m_ScansList.removeAt(i);
                m_ScansMap.remove(objectId);
                break;
            }
        }

        qDebug() << "Scan removed, Scan list size:" << m_ScansList.size(); /// @todo

        emit sigScansListChanged();
    } else if (type == UI_USB_Item::kRx) {
        for (int i = 0; i < m_RxList.count(); i++) {
            auto patientData = qobject_cast<PatientDataObject*>(m_RxList.at(i));

            if (patientData->id() == objectId) {
                m_RxList.removeAt(i);
                m_RxToScansMap.remove(objectId);
                break;
            }
        }

        qDebug()<<"Rx removed, Rx list size:"<<m_RxList.size(); /// @todo

        emit sigRxListChanged();
    }
}

int PatientObject::scansListCurrentIndex() const
{
    return m_ScansListCurrentIndex;
}

int PatientObject::rxListCurrentIndex() const
{
    return m_RxListCurrentIndex;
}

void PatientObject::setScansListCurrentIndex(const int &currentIndex)
{
    if (m_ScansListCurrentIndex != currentIndex)
        m_ScansListCurrentIndex = currentIndex;
}

void PatientObject::setRxListCurrentIndex(const int &currentIndex)
{
    if (m_RxListCurrentIndex != currentIndex)
        m_RxListCurrentIndex = currentIndex;
}

void PatientObject::sortRxViaDate()
{
    std::sort(m_RxList.begin(), m_RxList.end(), PatientObject::_dateTimeSortFunction);
}

bool PatientObject::_dateTimeSortFunction(QObject *o1, QObject *o2)
{
    PatientDataObject *first = qobject_cast<PatientDataObject*>(o1);
    PatientDataObject *second = qobject_cast<PatientDataObject*>(o2);

    return first->dateTime() < second->dateTime();
}

QDateTime PatientObject::dateTime() const
{
    if (m_RxList.isEmpty())
        return QDateTime();

    return qobject_cast<PatientDataObject*>(m_RxList.first())->dateTime();
}

/*!
 * \brief Return scan id from the same <Rx>
 * where the specified orthoId is
 * \param orthoId
 * \return
 */
QString PatientObject::getScanIdByOrthoticId(const QString &orthoId) const
{
    if (m_RxToScansMap.contains(orthoId))
        return m_RxToScansMap.value(orthoId);

    return QString();
}
