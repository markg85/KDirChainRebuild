#include "kdirectoryprivate_p.h"

#include <QUrl>
#include <QDebug>

KDirectoryPrivate::KDirectoryPrivate(KDirectory *dir, const QString& directory)
  : QObject(dir)
  , q(dir)
  , m_directory(directory)
  , m_dirEntries()
  , m_fileEntries()
  , m_allEntries()
  , m_disabledEntries() // these are the entries that are not shown in the view.
  , m_job(0)
  , m_watch(KDirWatch::self())
  , m_details()
  , m_sortFlags(QDir::NoSort)
  , m_filterFlags(QDir::NoFilter)
{
    QUrl goodUrl(m_directory);
    m_directory = goodUrl.url();
    m_job = KIO::listDir(goodUrl, KIO::HideProgressInfo);

    // If any details are set, pass them along to the listener.
    if(!m_details.isEmpty()) {
        m_job->addMetaData("details", m_details);
    }

    connect(m_job, SIGNAL(entries(KIO::Job*,KIO::UDSEntryList)), this, SLOT(slotEntries(KIO::Job*,KIO::UDSEntryList)));
    connect(m_job, SIGNAL(result(KJob*)), this, SLOT(slotResult(KJob*)));

}

void KDirectoryPrivate::setDetails(const QString &details)
{
    m_details = details;
    if(m_job && !m_details.isEmpty()) {
        m_job->addMetaData("details", m_details);
    }
}

const KDirectoryEntry &KDirectoryPrivate::entry(int index)
{
    if(index >= 0 && index < m_allEntries.count()) {
        return m_allEntries.at(index);
    }
    return KDirectoryEntry();
}

int KDirectoryPrivate::count()
{
    return m_allEntries.count();
}

QDir::Filters KDirectoryPrivate::filter()
{
    return m_filterFlags;
}

void KDirectoryPrivate::setFilter(QDir::Filters filters)
{
    m_filterFlags = filters;
}

QDir::SortFlags KDirectoryPrivate::sorting()
{
    return m_sortFlags;
}

void KDirectoryPrivate::setSorting(QDir::SortFlags sort)
{
    m_sortFlags = sort;
}

bool KDirectoryPrivate::keepEntryAccordingToFilter(KDirectoryEntry entry)
{
    // The NoFilter flag rules. If that flag is set all other flags will be ignored.
    if(m_filterFlags == QDir::NoFilter) {
        return true;
    }

    // Now process the flags that should return false. If one of those flags is set and the entry matches that flag then false should be returned
    if(m_filterFlags & QDir::NoDotAndDotDot) {
        QString name = entry.name();
        if(name == "." || name == "..") {
            return false;
        }
    }

    if(m_filterFlags & QDir::NoDot) {
        QString name = entry.name();
        if(name == ".") {
            return false;
        }
    }

    if(m_filterFlags & QDir::NoDotDot) {
        QString name = entry.name();
        if(name == "..") {
            return false;
        }
    }

    // Hidden entries are just files/folders only starting with a "." (on *nix). If the QDir::Hidden flag is not set then it should not be shown!
    if(entry.isHidden() && !(m_filterFlags & QDir::Hidden)) {
        return false;
    }

    //qDebug() << "name: " << entry.name() << "Flags:" << m_filterFlags;

    // The following flags (if set and if the entry matches it) want to keep the current entry
    if(m_filterFlags & QDir::Dirs) {
        if(entry.isDir()) {
            return true;
        }
    }

    if(m_filterFlags & QDir::Files) {
        if(entry.isFile()) {
            return true;
        }
    }

    return false;
}

void KDirectoryPrivate::processSortFlags()
{
    // If we have no sort filter, abort!
    if(m_sortFlags == QDir::NoSort) {
        return;
    }

    if(m_sortFlags & QDir::DirsFirst) {

        // For now we simply merge the two lists to one..
        m_allEntries = m_dirEntries + m_fileEntries;

    } else if(m_sortFlags & QDir::DirsLast) {

        // For now we simply merge the two lists to one..
        m_allEntries = m_fileEntries + m_dirEntries;

    } else {

    }
}

void KDirectoryPrivate::slotEntries(KIO::Job *, const KIO::UDSEntryList &entries)
{
    if(entries.count() > 0) {

        // Any filter can cause "some" items (matching the filter) to not be shown.
        if(m_filterFlags != QDir::NoFilter) {
            QVector<KDirectoryEntry> dirEntries;
            QVector<KDirectoryEntry> fileEntries;
            foreach(const KIO::UDSEntry entry, entries) {
                KDirectoryEntry newEntry(entry);
                if(keepEntryAccordingToFilter(newEntry)) {
                    if(newEntry.isDir()) {
                        dirEntries.append(newEntry);
                    } else {
                        fileEntries.append(newEntry);
                    }
                } else {
                    m_disabledEntries.append(entry);
                }
            }
            m_dirEntries += dirEntries;
            m_fileEntries += fileEntries;
        } else {
            QVector<KDirectoryEntry> allEntries;
            foreach(const KIO::UDSEntry entry, entries) {
                KDirectoryEntry newEntry(entry);
                if(keepEntryAccordingToFilter(newEntry)) {
                    allEntries.append(newEntry);
                } else {
                    m_disabledEntries.append(entry);
                }
            }
            m_allEntries += allEntries;
        }

        // Apply the sorting filters
        processSortFlags();

        emit entriesProcessed();
    } else {
        qDebug() << "Entries are not added because the KDirectory object doesn't exist or no entries where received. Entries received:" << entries.count();
    }

//    foreach(KDirectoryEntry entry, m_allEntries) {
//        qDebug() << "------------------------------";
//        qDebug() << "Entry:" << &entry;
//        qDebug() << "Name:" << entry.name();
//        qDebug() << "Basename:" << entry.basename();
//        qDebug() << "Extension:" << entry.extension();
//        qDebug() << "IconName:" << entry.iconName();
//        qDebug() << "MimeComment:" << entry.mimeComment();
//        qDebug() << "------------------------------";
//    }

//    qDebug() << "Entries! Count with filter:" << m_allEntries.count() << "count without filter:" << entries.count();
}

void KDirectoryPrivate::slotResult(KJob *)
{
    qDebug() << "Dir entries:" << m_dirEntries.count();
    qDebug() << "File entries:" << m_fileEntries.count();
    qDebug() << "Total entries:" << m_allEntries.count();

    // Thought: since we're emitting it directly, perhaps just remove this slot completely and emit the signal from KDirListerV2?
    emit completed();

}
