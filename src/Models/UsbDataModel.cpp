#include "UsbDataModel.h"

/*!
 * \class UsbDataModel
 * \brief Class perpesents usb flash filesystem
 */

UsbDataModel::UsbDataModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

/*!
 * \brief Set filesystem items from backend
 * \param items
 */
void UsbDataModel::setItems(const QList<UI_USB_Item> &items)
{
    m_Items = items;
}

/*!
 * \brief Add new filesystem item
 * \param item
 */
void UsbDataModel::addItem(const UI_USB_Item &item)
{
    beginInsertRows(QModelIndex(), m_Items.count(), m_Items.count());
    m_Items.append(item);
    endInsertRows();
}

/*!
 * \brief Remove filesystem item
 * \param row
 * \return
 */
QString UsbDataModel::removeItem(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    QString id = m_Items.at(row).id;
    m_Items.removeAt(row);
    endRemoveRows();
    return id;
}

/*!
 * \brief Return model role names
 * \return
 */
QHash<int, QByteArray> UsbDataModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TypeRole] = "type";
    roles[TimestampRole] = "timeStamp";
    roles[CommentRole] = "comment";
    roles[StatusRole] = "status";
    return roles;
}

/*!
 * \brief Return items count
 * \param parent
 * \return
 */
int UsbDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_Items.count();
}

/*!
 * \brief Return item data by index and role
 * \param index
 * \param role
 * \return
 */
QVariant UsbDataModel::data(const QModelIndex &index, int role) const
{
    UI_USB_Item item = m_Items.at(index.row());

    if (role == TypeRole)
        return item.type == UI_USB_Item::kScan ? "Scan" : "Rx";

    if (role == TimestampRole)
        return item.datetime.toString("M/d/yy H:mm");

    if (role == CommentRole)
        return item.comment;

    //if none, then not assigned, if exists, then status is "[Name]'s orthotic is ready for transfer to printer"
    if (role == StatusRole)
    {
        if (item.patient.first.isEmpty()
                && item.patient.last.isEmpty()
                && item.patient.title.isEmpty())
        {
            return QStringLiteral("Not assigned");
        }

        return QString("%1 %2 %3's orthotic is ready for transfer to printer")
                .arg(item.patient.title,
                     item.patient.first,
                     item.patient.last);
    }

    return QStringLiteral("");
}

/*!
 * \brief Return item comment
 * \param row
 * \return
 */
QString UsbDataModel::comment(int row) const
{
    if (row < 0 || row >= m_Items.count())
        return QStringLiteral("");
    return m_Items.at(row).comment;
}

/*!
 * \brief Return item id
 * \param row
 * \return
 */
QString UsbDataModel::id(int row) const
{
    return m_Items.at(row).id;
}

/*!
 * \brief Return whether specified item is scan
 * \param row
 * \return
 */
bool UsbDataModel::isScan(int row) const
{
    return m_Items.at(row).type == UI_USB_Item::kScan;
}
