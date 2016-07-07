#ifndef NAMEOBJECT_H
#define NAMEOBJECT_H

#include <QObject>

class NameObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString firstName READ firstName
               NOTIFY sigFirstNameChanged)
    Q_PROPERTY(QString lastName READ lastName
               NOTIFY sigLastNameChanged)
    Q_PROPERTY(QString title READ title
               NOTIFY sigTitleChanged)

public:
    explicit NameObject(const QString &firstName = QString(),
                        const QString &lastName = QString(),
                        const QString &title = QString(),
                        QObject *parent = 0);

    QString firstName() const;
    QString lastName() const;
    QString title() const;

    void setFirstName(const QString &firstName);
    void setLastName(const QString &lastName);
    void setTitle(const QString &title);

signals:
    void sigFirstNameChanged() const;
    void sigLastNameChanged() const;
    void sigTitleChanged() const;

private:
    QString m_Title;
    QString m_FirstName;
    QString m_LastName;
};

#endif // NAMEOBJECT_H
