#include "kdirmodelv2.h"
#include "CNaturalString.h"

#include <QModelIndex>
#include <QDebug>
#include <QTime>

#include <KIcon>
#include <KDirLister>
#include <klocale.h>
#include <algorithm>

inline bool
lessThan(const ItemData& s1, const ItemData& s2)
{
  return *s1.collationSequence < *s2.collationSequence;
}

KDirModelV2Private::KDirModelV2Private(KDirModelV2* model)
    : QObject(0)
    , m_model(model)
    , m_url()
    , m_lister()
    , m_rootIndex(model->createIndex(0, 0))
    , m_currentIndex(model->createIndex(0, 0))
    , m_itemList()
    , m_time()
    , rowCount(0)
{
    m_time.start();
}

void KDirModelV2Private::setUrl(const KUrl& url)
{
    m_url = url;
    m_lister = new KDirLister();

    connect(m_lister, SIGNAL(newItems(KFileItemList)), this, SLOT(newItemsFromLister(KFileItemList)));
    connect(m_lister, SIGNAL(completed()), this, SLOT(listerCompleted()));

    m_lister->openUrl(m_url);
}

bool KDirModelV2Private::haveModelIndex(const QModelIndex &)
{
    // check if we have the QModelIndex in out list somewhere
    return false;
}

void KDirModelV2Private::newItemsFromLister(const KFileItemList& items)
{
    qDebug() << "newItemsFromLister delay:" << m_time.elapsed();
    m_time.restart();
    const int itemCount = items.count();
    const int newItemListLength = m_itemList.count() + itemCount;

    // We can already determine the number of items in this list. Reserve them manyally.
    m_itemList.reserve(newItemListLength);

    m_model->beginInsertRows(m_currentIndex, rowCount, itemCount);

    QTime time;
    time.start();

    /*
    #pragma omp parallel for schedule(dynamic, 100)
    for(int i = 0; i < itemCount; i++) {

        ItemData newData;
        newData.item = new KFileItem(items.at(i));
//        newData.collationSequence = new CNaturalString(newData.item->name().toAscii().constData());
        newData.modelIndex = m_model->createIndex(i, 0);
        #pragma omp critical
        m_itemList.append(newData);
    }

    #pragma omp parallel for schedule(dynamic, 100)
    for(int i = 0; i < itemCount; i++) {

        ItemData * newData = &m_itemList[iBase + i];
        newData->collationSequence = new CNaturalString(newData->item->name().toAscii().constData());
    }
    */

    for(int i = 0; i < itemCount; i++) {

        ItemData newData;
        newData.item = new KFileItem(items.at(i));
        newData.collationSequence = new CNaturalString(newData.item->name().toAscii().constData());
        newData.modelIndex = m_model->createIndex(i, 0);
        m_itemList.append(newData);
    }

    qDebug() << "Time elapsed:" << time.elapsed() << "ms";
    rowCount = m_itemList.count();

//    std::sort(m_itemList.begin(), m_itemList.end(), lessThan);
//    qSort(m_itemList.begin(), m_itemList.end(), lessThan);
    m_model->endInsertRows();

    emit m_model->layoutChanged();


    // Move the current index to the last inserted item.
    ItemData lastItem = m_itemList.last();
    m_currentIndex = lastItem.modelIndex;

    qDebug() << "KDirModelV2Private::newItemsFromLister rowCount:" << rowCount;
}

void KDirModelV2Private::listerCompleted()
{

    QTime time;
    time.start();

    std::sort(m_itemList.begin(), m_itemList.end(), lessThan);

    qDebug() << "Sorting took:" << time.elapsed() << "ms";

    qApp->exit();
//    qSort(m_itemList.begin(), m_itemList.end(), lessThan);

//    QHash<QModelIndex, ItemData> swap;

//    foreach(ItemData data, m_itemList)
//    {
//        swap.insert(data.modelIndex, data);
////        qDebug() << data.item.name();
//    }

    qDebug() << "KDirModelV2Private::listerCompleted";
}

KDirModelV2::KDirModelV2(QObject *parent)
    : QAbstractItemModel(parent)
    , d(new KDirModelV2Private(this))
{
    QHash<int, QByteArray>roleNames;
    roleNames[Qt::DecorationRole]       = "Decoration";
    roleNames[Name]                     = "Name";
    roleNames[Size]                     = "Size";
    roleNames[ModifiedTime]             = "ModifiedTime";
    roleNames[Permissions]              = "Permissions";
    roleNames[Owner]                    = "Owner";
    roleNames[Group]                    = "Group";
    roleNames[MimeComment]              = "MimeComment"; // Type == item.mimeComment() ... yeah, go figure that one out.
    roleNames[ColumnCount]              = "ColumnCount";
    roleNames[Url]                      = "Url";
    roleNames[MimeType]                 = "MimeType";
    roleNames[IconName]                 = "IconName";
    roleNames[BaseName]                 = "BaseName";
    roleNames[Extension]                = "Extension";
    roleNames[TimeString]               = "TimeString";
    setRoleNames(roleNames);
}

void KDirModelV2::setUrl(const KUrl &url)
{
    d->setUrl(url);
}

QModelIndex KDirModelV2::index(int row, int column, const QModelIndex & parent) const
{
    Q_UNUSED(column)
    if(parent.isValid()) {
        return QModelIndex();
    } else {
        return createIndex(row, column, d->m_itemList.at(row).item);
    }
}

QModelIndex KDirModelV2::parent(const QModelIndex &child) const
{
    if(!child.isValid()) {
        return QModelIndex();
    } else {

//        QModelIndex parent = d->m_itemList.at(child.row()).modelIndex;
//        qDebug() << "Parent: " << parent.parent() << "Child: " << child;
        // Ehhhh...????
        return QModelIndex();
    }

//    qDebug() << "KDirModelV2::parent row:" << child.row() << " column:" << child.column();
//    return d->m_itemList.at(child.row()).modelIndex;
//    return QModelIndex();
}

QVariant KDirModelV2::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        if(index.row() < d->rowCount) {
            const int column = index.column(); // We don't actually use "columns".
            const KFileItem* currentItem = static_cast<KFileItem*>(index.internalPointer());
            switch (role) {
            case Qt::DisplayRole:
                switch (column) {
                case Name:
                    return currentItem->name();
                case Size:
                    return KGlobal::locale()->formatNumber(currentItem->size(), 0);
                case ModifiedTime:
                    return KGlobal::locale()->formatDateTime(currentItem->time(KFileItem::ModificationTime));
                case Permissions:
                    return currentItem->permissions();
                case Owner:
                    return currentItem->user();
                case Group:
                    return currentItem->group();
                case MimeComment:
                    return currentItem->mimeComment();
                case Url:
                    return currentItem->url();
                case MimeType:
                    return currentItem->mimetype();
                case IconName:
                    return currentItem->iconName();
                case BaseName:
                    return currentItem->name(); // should be the item name without extension
                case Extension:
                    return currentItem->name(); // should be just the item extension (without the dot)
                case TimeString:
                    return currentItem->timeString();
                }
                break;
            case Qt::DecorationRole:
                switch (column) {
                case Name:
                    // By default we show whatever we can show.
                    return KIcon(currentItem->iconName(), 0, currentItem->overlays());
                }
                break;
            }
        }

//        qDebug() << "::data. filename:" << currentItem.item.name();
    } else {

        qDebug() << "::data. index is apparently not valid.." << role;
        return QVariant();
    }

//    qDebug() << "KDirModelV2::data";
    return QVariant();
}

QVariant KDirModelV2::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        switch (role) {
        case Qt::DisplayRole:
            switch (section) {
            case Name:
                return i18nc("@title:column","Name");
            case Size:
                return i18nc("@title:column","Size");
            case ModifiedTime:
                return i18nc("@title:column","Date");
            case Permissions:
                return i18nc("@title:column","Permissions");
            case Owner:
                return i18nc("@title:column","Owner");
            case Group:
                return i18nc("@title:column","Group");
            case MimeComment:
                return i18nc("@title:column","Comment");
            case Url:
                return i18nc("@title:column","URL");
            case MimeType:
                return i18nc("@title:column","MimeType");
            case IconName:
                return i18nc("@title:column","IconName");
            case BaseName:
                return i18nc("@title:column","BaseName");
            case Extension:
                return i18nc("@title:column","Extension");
            case TimeString:
                return i18nc("@title:column","Time");
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags KDirModelV2::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
}

int KDirModelV2::rowCount(const QModelIndex &parent) const
{
//    qDebug() << "KDirModelV2::rowCount" << d->rowCount;
    return d->rowCount;
}

int KDirModelV2::columnCount(const QModelIndex &parent) const
{
//    qDebug() << "KDirModelV2::columnCount" << ColumnCount;
    return ColumnCount;
}

void KDirModelV2::fetchMore(const QModelIndex &parent)
{
    qDebug() << "KDirModelV2::fetchMore";
}

bool KDirModelV2::canFetchMore(const QModelIndex &parent) const
{
    qDebug() << "KDirModelV2::canFetchMore";
    return false;
}

bool KDirModelV2::hasChildren(const QModelIndex &parent) const
{
//    qDebug() << "KDirModelV2::hasChildren";
    return true;
}
