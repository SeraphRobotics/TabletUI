#include "PatientsListModel.h"
#include "NameObject.h"

#include "DataStructures/patientmanager.h"
#include "PatientObject.h"

#include <QDebug>

/*!
 * \class PatientsListModel
 * \brief Class contains information about all patients
 */

PatientsListModel::PatientsListModel(QObject *parent) :
    QObject(parent),
    m_CurrentIndex(0)
{
    m_PatientManager = new PatientManager(this);
}

/*!
 * \brief Return patients list
 * \return
 */
QList<UI_Patient> PatientsListModel::getPatientsList()
{
    QList<PatientSearch> patients = m_PatientManager->getPatients();
    QList< UI_Patient> ui_patients;

    for(int i=0;i<patients.size();i++) {
        PatientSearch ps = patients.at(i);
        UI_Patient uip;
        uip.name = ps.name;
        uip.id=ps.id;
        uip.doctorid = ps.docIds.at(0);
        Patient p = m_PatientManager->getByID(ps.id);

        QList<Rx> rxs=p.rxs;

        /// create orthotic
        for(int j=0;j<rxs.size();j++){
            Rx rx = rxs.at(j);

            if (!rx.orthoticAvailable) {
                continue;
            }

            UI_USB_Item item;
            if (rx.leftOrthoticId.isEmpty())
            {
                item.id = rx.rightOrthoticId;
                item.foot = kRight;
                item.parentId = rx.rightScanId;
            }
            else
            {
                item.id = rx.leftOrthoticId;
                item.foot = kLeft;
                item.parentId = rx.leftScanId;
            }

            item.type=UI_USB_Item::kRx;
            item.datetime= QDateTime(rx.date);
            item.patient=uip.name;
            item.comment=rx.note;
            uip.item_list.append(item);
        }

        /// create scan
        ///
        for(int j=0;j<rxs.size();j++){
            Rx rx = rxs.at(j);

            UI_USB_Item item;
            if (rx.leftScanId.isEmpty())
            {
                item.id = rx.rightScanId;
                item.foot = kRight;
            }
            else
            {
                item.id = rx.leftScanId;
                item.foot = kLeft;
            }

            if (rx.orthoticAvailable) {
                item.type=UI_USB_Item::kAssociatedScan;
            } else {
                item.type=UI_USB_Item::kScan;
            }

            item.datetime= QDateTime(rx.date);
            item.patient=uip.name;
            item.comment=rx.note+"This is comment from scan data from ";
            item.parentId=QString();
            uip.item_list.append(item);
        }

        ui_patients.append(uip);


    }
    return ui_patients;
}

/*!
 * \brief Set patients list from backend
 */
void PatientsListModel::updatePatientsList()
{
    clear();

    QList<UI_Patient> patients = getPatientsList();

    for (int i = 0; i < patients.size(); i++)
    {
        UI_Patient patient = patients.at(i);
        auto nameObject = new NameObject(patient.name.first,
                                         patient.name.last,
                                         patient.name.title);
        PatientObject *patientObject = new PatientObject(nameObject,
                                                         patient.id,
                                                         patient.doctorid,
                                                         this);

        for (UI_USB_Item obj : patient.item_list)
        {
            patientObject->appendDataObject(obj);
        }
        patientObject->sortRxViaDate();
        append(patientObject);
    }
    sortWithSpecificOrder("A-Z");
    qDebug()<<"PatientsList size is "<< size();
    emit sigCurrentIndexChanged();
}

/*!
 * \brief Return patients list as QmlListProperty
 * \return
 */
QQmlListProperty<QObject> PatientsListModel::patientsList()
{
    return QQmlListProperty<QObject>(this, m_PatientsList);
}

/*!
 * \brief Return found patients list as QmlListProperty
 * \return
 */
QQmlListProperty<QObject> PatientsListModel::searchPatientsList()
{
    return QQmlListProperty<QObject>(this, m_SearchPatientsList);
}

/*!
 * \brief Set search list via specific key
 * \param key
 */
void PatientsListModel::setSearchListViaSpecificKey(const QString &key)
{
    clearSearchList();

    if (key == "")
        return;

    for (int i = 0; i < m_PatientsList.size(); i++)
    {
        PatientObject *obj = qobject_cast<PatientObject*> (m_PatientsList.at(i));

        if (obj->name()->firstName().contains(key, Qt::CaseInsensitive)
                || obj->name()->lastName().contains(key, Qt::CaseInsensitive)
                || obj->name()->title().contains(key, Qt::CaseInsensitive))
        {
            m_SearchPatientsList.append(obj);
            emit sigSearchListModificate();
        }
    }
}

/*!
 * \brief Clear search list
 */
void PatientsListModel::clearSearchList()
{
    //// @todo we need to create one class for list model and store this function in this class
    ///  or at least use qqmllistproperty.html and add speciics function. ClearFunction,etc
    ///
    m_SearchPatientsList.clear();
    emit sigSearchListModificate();
}

/*!
 * \brief Clear search list and show patients sorted alphabetically
 */
void PatientsListModel::restartListModel()
{
    clearSearchList();
    sortWithSpecificOrder("A-Z");
}

/*!
 * \brief Append new patient
 * \param patient
 */
void PatientsListModel::append(PatientObject *patient)
{
    m_PatientsList.append(patient);
    emit sigPatientsListModificate();
}

/*!
 * \brief Return patient by index
 * \param index
 * \return
 */
PatientObject* PatientsListModel::getSpecificItem(const int &index) const
{
    if (index < 0 || index > m_PatientsList.size() - 1)
    {
        qDebug() << "Out of list range";
        return nullptr;
    }
    return qobject_cast<PatientObject*>(m_PatientsList.at(index));
}

/*!
 * \brief Return current patient index
 * \return
 */
int PatientsListModel::currentIndex() const
{
    return m_CurrentIndex;
}

/*!
 * \brief Set current patient index
 * \param index
 */
void PatientsListModel::setCurrentIndex(int index)
{
    m_CurrentIndex = index;
    emit sigCurrentIndexChanged();
}

/*!
 * \brief Perform sorting with specific order
 * \param order - "A-Z" - ascending , "Z-A" - descending sort
 */
void PatientsListModel::sortWithSpecificOrder(const QString &order)
{
    if (order == "A-Z")
        std::sort(m_PatientsList.begin(), m_PatientsList.end(), PatientsListModel::_sortViaNameAscending);
    else if (order == "Z-A")
        std::sort(m_PatientsList.begin(), m_PatientsList.end(), PatientsListModel::_sortViaNameDescending);
    else if (order == "newest-oldest")
        std::sort(m_PatientsList.begin(), m_PatientsList.end(), PatientsListModel::_sortViaDateTimeDescending);
    else if (order == "oldest-newest")
        std::sort(m_PatientsList.begin(), m_PatientsList.end(), PatientsListModel::_sortViaDateTimeAscending);

    emit sigPatientsListModificate();
}

bool PatientsListModel::_sortViaNameAscending(QObject *o1, QObject *o2)
{
    PatientObject *first = qobject_cast<PatientObject*>(o1);
    PatientObject *second = qobject_cast<PatientObject*>(o2);

    return first->name()->firstName().toLower()
            < second->name()->firstName().toLower();
}

bool PatientsListModel::_sortViaNameDescending(QObject *o1, QObject *o2)
{
    PatientObject *first = qobject_cast<PatientObject*>(o1);
    PatientObject *second = qobject_cast<PatientObject*>(o2);

    return first->name()->firstName().toLower()
            > second->name()->firstName().toLower();
}

bool PatientsListModel::_sortViaDateTimeAscending(QObject *o1, QObject *o2)
{
    PatientObject *first = qobject_cast<PatientObject*>(o1);
    PatientObject *second = qobject_cast<PatientObject*>(o2);

    return first->dateTime() < second->dateTime();
}

bool PatientsListModel::_sortViaDateTimeDescending(QObject *o1, QObject *o2)
{
    PatientObject *first = qobject_cast<PatientObject*>(o1);
    PatientObject *second = qobject_cast<PatientObject*>(o2);

    return first->dateTime() > second->dateTime();
}

/*!
 * \brief Return patients count
 * \return
 */
int PatientsListModel::size() const
{
    return m_PatientsList.size();
}

/*!
 * \brief Clear patients list
 */
void PatientsListModel::clear()
{
    while (!m_PatientsList.isEmpty())
        m_PatientsList.takeFirst()->deleteLater();
    emit sigPatientsListModificate();

    while (!m_SearchPatientsList.isEmpty())
        m_SearchPatientsList.takeFirst()->deleteLater();
    emit sigSearchListModificate();
}

/*!
 * \brief Return scan id which is in the same Rx that specified ortho id
 * \param orthoId
 * \return
 */
QString PatientsListModel::getScanIdByOrthoticId(const QString &orthoId) const
{
    return getSpecificItem(m_CurrentIndex)->getScanIdByOrthoticId(orthoId);
}

/*!
 * \brief Return scan id which is in the same Rx that specified scan id
 * \param scanId
 * \return
 */
QString PatientsListModel::getSiblingScanIdByScan(const QString &scanId) const
{
    QString patientId = getSpecificItem(m_CurrentIndex)->id();
    const Patient patient = m_PatientManager->getByID(patientId);

    foreach (const Rx &rx, patient.rxs)
    {
        if (rx.leftScanId == scanId)
            return rx.rightScanId;
        if (rx.rightScanId == scanId)
            return rx.leftScanId;
    }

    return QString();
}

/*!
 * \brief Return ortho id which is in the same Rx that specified ortho id
 * \param orthoId
 * \return
 */
QString PatientsListModel::getSiblingOrthoticByOrthoticId(const QString &orthoId) const
{
    QString patientId = getSpecificItem(m_CurrentIndex)->id();
    const Patient patient = m_PatientManager->getByID(patientId);

    foreach (const Rx &rx, patient.rxs)
    {
        if (rx.leftOrthoticId == orthoId)
            return rx.rightOrthoticId;
        if (rx.rightOrthoticId == orthoId)
            return rx.leftOrthoticId;
    }

    return QString();
}

/*!
 * \brief Add Rx to specified patient
 * \param patientId
 * \param rx
 */
void PatientsListModel::addRxToPatient(const QString &patientId, const Rx &rx)
{
    m_PatientManager->updateRx(patientId, rx);
}

/*!
 * \brief Replace Rx in specified patient
 * \param patientId
 * \param scanId
 * \param rx
 */
void PatientsListModel::replaceRxAtPatient(const QString &patientId, const QString &scanId, const Rx &rx)
{
    m_PatientManager->replaceRxByScanId(patientId, scanId, rx);
}

/*!
 * \brief Remove Rx from patient
 * \param patientId
 * \param rxId
 */
void PatientsListModel::removeRxFromPatient(const QString &patientId, const QString &rxId)
{
    m_PatientManager->removeRx(patientId, rxId);
}

/*!
 * \brief Return Rx from specified patient by id
 * \param patientId
 * \param rxId
 * \return
 */
Rx PatientsListModel::getRxOfPatient(const QString &patientId, const QString &rxId) const
{
    const Patient patient = m_PatientManager->getByID(patientId);

    foreach (const Rx &rx, patient.rxs)
    {
        if (rx.leftOrthoticId == rxId || rx.rightOrthoticId == rxId)
            return rx;
    }

    return Rx();
}

/*!
 * \brief Add scans to existing patient
 * \param leftScanId
 * \param rightScanId
 * \param patientId
 * \param docId
 */
void PatientsListModel::addScansToPatient(const QString &leftScanId,
                                          const QString &rightScanId,
                                          const QString &patientId,
                                          const QString &docId)
{
    qDebug() << "add scan to patient";
    Rx rx;
    rx.docId = docId;
    rx.date = QDate::currentDate();
    rx.note = "";
    rx.shoesize = 9;
    rx.leftScanId = leftScanId;
    rx.rightScanId = rightScanId;
    rx.orthoticAvailable = false;
    m_PatientManager->updateRx(patientId, rx);
}

/*!
 * \brief Remove specified scan id from patient
 * \param patientId
 * \param scanId
 */
void PatientsListModel::removeScanFromPatient(const QString &patientId, const QString &scanId)
{
    m_PatientManager->removeScan(patientId, scanId);
}

/*!
 * \brief Create new patient from scans
 * \param leftScanId
 * \param rightScanId
 * \param patientName
 * \param comments
 * \param docId
 */
void PatientsListModel::newPatientFromScans(const QString &leftScanId,
                                            const QString &rightScanId,
                                            const QString &patientName,
                                            const QString &comments,
                                            const QString &docId)
{
    qDebug() << "new patient from scan";
    Rx rx;
    rx.docId = docId;
    rx.date = QDate::currentDate();
    rx.note = comments;
    rx.shoesize = 9;
    rx.leftScanId = leftScanId;
    rx.rightScanId = rightScanId;
    rx.orthoticAvailable = false;
    Patient patient;
    QStringList nameParts = patientName.split(" ");
    Name name;
    name.title = nameParts.count() > 0 ? nameParts.at(0) : QString();
    name.first = nameParts.count() > 1 ? nameParts.at(1) : QString();
    name.last = nameParts.count() > 2 ? nameParts.at(2) : QString();
    patient.name = name;
    patient.rxs.append(rx);
    m_PatientManager->addPatient(patient);
}

/*!
 * \brief Return selected patient object
 * \return
 */
PatientObject* PatientsListModel::getCurrentPatient() const
{
    return getSpecificItem(m_CurrentIndex);
}

/*!
 * \brief Compare patient title, firstName and lastName with other patients
 * \param patientName
 * \return
 */
bool PatientsListModel::contains(const QString &patientName) const
{
    foreach (QObject *patient, m_PatientsList)
    {
        PatientObject *obj = qobject_cast<PatientObject*>(patient);
        QStringList nameParts = patientName.split(" ");
        QString title = nameParts.count() > 0 ? nameParts.at(0) : QString();
        QString firstName = nameParts.count() > 1 ? nameParts.at(1) : QString();
        QString lastName = nameParts.count() > 2 ? nameParts.at(2) : QString();

        if (title.compare(obj->name()->title(), Qt::CaseInsensitive) == 0
                && firstName.compare(obj->name()->firstName(), Qt::CaseInsensitive) == 0
                && lastName.compare(obj->name()->lastName(), Qt::CaseInsensitive) == 0)
        {
            return true;
        }
    }
    return false;
}
