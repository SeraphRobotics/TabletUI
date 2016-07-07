#include "UsersListModel.h"
#include "NameObject.h"
#include "UserObject.h"

#include "DataStructures/usermanager.h"

/*!
 * \class UsersListModel
 * \brief Class contains information about all users
 */

UsersListModel::UsersListModel(QObject *parent) :
    QObject(parent),
    m_CurrentIndex(-1)
{
    m_UserDataManager = new UserDataManager(this);
}

/*!
 * \brief Remove last user
 */
void UsersListModel::removeLast()
{
    if (m_UsersList.size() == 0)
        return;

    m_UsersList.removeLast();
    emit sigUsersListModificate();
}

/*!
 * \brief Return users list
 * \return
 */
QList<User> UsersListModel::getUsersList() const
{
    return m_UserDataManager->getUsers();
}

/*!
 * \brief Add new user
 * \param user
 */
void UsersListModel::appendNewUser(UserObject *user)
{
    m_UsersList.append(user);
    emit sigUsersListModificate();
}

/*!
 * \brief Add new user
 * \param firstName
 * \param lastName
 * \param title
 * \param id
 * \param password
 * \param iconPath
 * \param degree
 */
void UsersListModel::appendNewUser(const QString &firstName,
                                   const QString &lastName,
                                   const QString &title,
                                   const QString &id,
                                   const QString &password,
                                   const QString &iconPath,
                                   const QString &degree)
{
    m_UsersList.append(new UserObject(
                           new NameObject(firstName,lastName,title,this),
                           id,
                           password,
                           iconPath,
                           degree,
                           this)
                       );
    emit sigUsersListModificate();
}

/*!
 * \brief Update user data
 * \param i
 * \param firstName
 * \param lastName
 * \param title
 * \param id
 * \param password
 * \param iconPath
 * \param degree
 */
void UsersListModel::updateUser(unsigned i,
                                const QString &firstName,
                                const QString &lastName,
                                const QString &title,
                                const QString &id,
                                const QString &password,
                                const QString &iconPath,
                                const QString &degree)
{
    m_UsersList[i] = new UserObject(new
                                    NameObject(firstName,
                                               lastName,
                                               title,
                                               this),
                                    id,
                                    password,
                                    iconPath,
                                    degree,
                                    this);
    emit sigUsersListModificate();
}

/*!
 * \brief Return users list as QmlListProperty
 * \return
 */
QQmlListProperty<QObject> UsersListModel::usersList()
{
    return QQmlListProperty<QObject>(this, m_UsersList);
}

/*!
 * \brief Return user specified by index
 * \param index
 * \return
 */
UserObject* UsersListModel::getSpecificItem(const int &index) const
{
    if (index < 0 || index > m_UsersList.size() - 1)
    {
        qDebug() << "Out of list range";
        return nullptr;
    }
    return qobject_cast<UserObject*>(m_UsersList.at(index));
}

/*!
 * \brief Function searchs for value of valueName type
 * (id, password, iconPath, firstName, lastName, title) in m_UsersList.
 * \param value
 * \param valueName
 * \return
 *
 * Returns true if value is in list.
 * Returns false if value isn't in the list.
*/
bool UsersListModel::checkIfValueExist(const QString &value, const QString &valueName)
{
    if (value.isEmpty())
        return true;

    int index = -1;
    for (QObject *userObject: m_UsersList)
    {
        index++;
        if (qobject_cast<UserObject*>(userObject)->getSpecificValue(valueName) == value)
            return true;
    }
    return false;
}

/*!
 * \brief Return current user index
 * \return
 */
int UsersListModel::currentIndex() const
{
    return m_CurrentIndex;
}

void UsersListModel::setCurrentIndex(const int &index)
{
    m_CurrentIndex = index;
    emit sigCurrentIndexChanged();
}

/*!
 * \brief Return users count
 * \return
 */
int UsersListModel::size() const
{
    return m_UsersList.size();
}

/*!
 * \brief Return user name by id
 * \param id
 * \return
 */
QString UsersListModel::getNameById(const QString &id)
{
    if (checkIfValueExist(id, "id"))
    {
        UserObject *obj = getSpecificItem(m_CurrentIndex);

        if (obj == nullptr)
            return QString();

        return obj->name()->title() + " "
                + obj->name()->firstName() + " "
                + obj->name()->lastName();
    }

    return QString();
}

/*!
 * \brief Remove user by index
 * \param i
 */
void UsersListModel::removeAt(int i)
{
    m_UsersList.removeAt(i);
    emit sigUsersListModificate();
}

/*!
 * \brief Reset users list
 */
void UsersListModel::resetList()
{
    emit sigUsersListReset();
    emit sigUsersListModificate();
}

/*!
 * \brief Save users list
 */
void UsersListModel::saveUserList()
{
    //TODO: implement saving users list
}

