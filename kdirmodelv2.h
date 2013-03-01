#ifndef KDIRMODELV2_H
#define KDIRMODELV2_H

#include <QAbstractItemModel>
#include <QHash>
#include <QTime>

#include <KUrl>
#include <KFileItem>
#include <KDirLister>

#include "CNaturalString.h"

class KDirModelV2Private;
class KDirModelV2;

struct ItemData
{
    KFileItem* item;

    // This natural string should probably be added to KFileItem.
    CNaturalString* collationSequence;

    // I don't know where to put the model index.. In here for now..
    QModelIndex modelIndex;
};


class Node
{
public:
    Node();
    const ItemData& getItem(int row) { return m_itemList.at(row); }


private:
    QList<ItemData> m_itemList;
};


class KDirModelV2Private : public QObject
{
    Q_OBJECT

public:
    KDirModelV2Private(KDirModelV2* model);
    void setUrl(const KUrl& url);
    bool haveModelIndex(const QModelIndex &parent);

public slots:
    void newItemsFromLister(const KFileItemList& items);
    void listerCompleted();

public:
    KDirModelV2* m_model;
    KUrl m_url;
    KDirLister* m_lister;
    QModelIndex m_rootIndex;
    QModelIndex m_currentIndex;
    QList<ItemData> m_itemList;
    QTime m_time;
    int rowCount;
};

class KDirModelV2 : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit KDirModelV2(QObject *parent = 0);

    void setUrl(const KUrl& url);
    QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    void fetchMore(const QModelIndex &parent);
    bool canFetchMore(const QModelIndex &parent) const;
    bool hasChildren(const QModelIndex &parent) const;
    
    /***
     * Useful "default" columns. Views can use a proxy to have more control over this.
     */
    enum ModelColumns {
        Name = 0,
        Size,
        ModifiedTime,
        Permissions,
        Owner,
        Group,
        MimeComment,
        Url,
        MimeType,
        IconName,
        BaseName,
        Extension,
        TimeString,
        ColumnCount
    };

signals:

public slots:

private:
    friend class KDirModelV2Private;
    KDirModelV2Private *const d;

};

#endif // KDIRMODELV2_H
