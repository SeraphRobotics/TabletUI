#ifndef USBDATAMODEL_H
#define USBDATAMODEL_H

#include <QAbstractListModel>

#include "UI_tester/UI_structs.h"

class UsbDataModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum DataRoles {
        TypeRole = Qt::UserRole + 1,
        TimestampRole,
        CommentRole,
        StatusRole
    };

    explicit UsbDataModel(QObject *parent = 0);

    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    void setItems(const QList<UI_USB_Item> &items);
    void addItem(const UI_USB_Item &item);
    QString removeItem(int row);

    Q_INVOKABLE QString comment(int row) const;
    Q_INVOKABLE QString id(int row) const;
    Q_INVOKABLE bool isScan(int row) const;

private:
    QList<UI_USB_Item> m_Items;
};

#endif // USBDATAMODEL_H
