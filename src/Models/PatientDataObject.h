#ifndef PATIENTDATAOBJECT_H
#define PATIENTDATAOBJECT_H

#include <QObject>
#include <QDateTime>
#include <QString>

class NameObject;

class PatientDataObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id NOTIFY sigIdChanged)
    Q_PROPERTY(QDateTime dateTime READ dateTime NOTIFY sigDateTimeChanged)
    Q_PROPERTY(QString comment READ comment NOTIFY sigCommentChanged)
    Q_PROPERTY(int foot READ foot WRITE setFoot)
    
public:
    explicit PatientDataObject(QObject *parent = 0);

    int foot() const;
    void setFoot(int foot);

signals:
    void sigIdChanged() const;
    void sigCommentChanged() const;
    void sigDateTimeChanged() const;
    void sigPatientNameChanged() const;

public slots:
    QDateTime dateTime() const;
    QString id() const;
    QString comment() const;
    NameObject *patientName() const;

    void setDataTime(const QDateTime &dateTime);
    void setId(const QString &id);
    void setComment(const QString &comment);
    void setPatientName(NameObject *name);

private:
    QDateTime m_DateTime;
    QString m_Id;
    QString m_Comment; /*! Value used to store the doctor's comments about the patient. !*/
    NameObject *m_PatientName;
    int m_Foot;
};

#endif // PATIENTDATAOBJECT_H
