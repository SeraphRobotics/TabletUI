//qt
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QUrl>

// seraphLibs

// local
#include "QmlCppWrapper.h"
#include "XmlManager/XmlFileManager.h"
#include "ImageProcessingClass/ImageContoursDetector.h"


QmlCppWrapper::QmlCppWrapper(QObject *parent) :
    MasterControlUnit(parent),
    //QObject(parent),
    c_ImagesFolderPath(qApp->applicationDirPath().append("/customPad"))
{
    m_XmlManager = new XmlFileManager(this);
    m_ImageContoursDetector = new ImageContoursDetector(this);

    /*
    qRegisterMetaType<NameObject*>("NameObject*");
    qRegisterMetaType<UserObject*>("UserObject*");
    qRegisterMetaType<PatientObject*>("PatientObject*");
    qRegisterMetaType<PatientDataObject*>("PatientDataObject*");
    qRegisterMetaType<QQmlListProperty<UserObject>>("QQmlListProperty<UserObject>");

    _setUsersList();
    _setPatientsList();

    QObject::connect(m_UsersList, &UsersListModel::sigUsersListReset,
                     this, &QmlCppWrapper::_setUsersList);
    QObject::connect(m_UsersList, SIGNAL(sigSaveUserList(QList<QObject*> *)),
                     &m_MasterControlUnit, SLOT(updateUserList(QList<QObject*>*)));
    */
    _createImagesDirectoryIfNotExist();
}

QmlCppWrapper::~QmlCppWrapper()
{
    QDir(c_ImagesFolderPath).removeRecursively();
}


// public
void QmlCppWrapper::beginScan()
{

}


QString QmlCppWrapper::iFrameUrl() const
{
    return m_iFrameUrl;
}

void QmlCppWrapper::setiFrameUrl(const QString& iFrameUrl)
{
    if(m_iFrameUrl != iFrameUrl)
    {
        m_iFrameUrl = iFrameUrl;
        emit sigiFrameUrlChanged();
    }
}

QString QmlCppWrapper::getApplicationPath() const
{
    return qApp->applicationDirPath();
}

QString QmlCppWrapper::resolveUrl(const QString &fileName)
{
    QString name = QUrl::fromLocalFile(fileName).toString();
    qDebug()<<__FUNCTION__<<"Got "<<name;
    return name;
}

void  QmlCppWrapper::saveShellModifications()
{
    if(m_LeftElementShells.size() > 0)
        m_XmlManager->saveShellModifications("left", m_LeftElementShells);
    if(m_RightElementShells.size() > 0)
        m_XmlManager->saveShellModifications("right", m_RightElementShells);

    m_LeftElementShells.clear();
    m_RightElementShells.clear();
}

void QmlCppWrapper::appendElementToShellList(const QString &direction,
                                             const QString &name,
                                             const double &depth,
                                             const double &height,
                                             const double &stiffness,
                                             const QImage &image,
                                             const int &startingXPos,
                                             const int &startingYPos)
{

    qDebug()<<__FUNCTION__<<" Direction "<<direction<<" Name "<<name;

    QVariantList outerBorder;
    UI_Shell_Modification element;
    element.name = name;
    element.depth = depth;
    element.height = height;
    element.stiffness = stiffness;

    m_ImageContoursDetector->detectContours(image,
                                            element.inner_borders,
                                            outerBorder,
                                            startingXPos,
                                            startingYPos);

    /// @note Convert qvariant to QPointF
    QListIterator<QVariant> itr(outerBorder);
    while (itr.hasNext())
    {
        element.outer_border << itr.next().toPointF();
    }

    if(direction == "left")
    {
        m_LeftElementShells.append(element);
        qDebug()<< m_LeftElementShells[m_LeftElementShells.size()-1].name;
    }
    else if(direction == "right")
    {
        m_RightElementShells.append(element);
        qDebug()<< m_RightElementShells[m_RightElementShells.size()-1].name;
    }
}

void QmlCppWrapper::_createImagesDirectoryIfNotExist() const
{
    QDir().mkdir(c_ImagesFolderPath);
}


