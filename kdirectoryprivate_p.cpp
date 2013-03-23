#include "kdirectoryprivate_p.h"

// Only required for KIO::listDir
#include <KUrl>

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
  , m_sortFlags()
  , m_filterFlags(QDir::NoFilter)
{
    KUrl goodUrl(m_directory);
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

const QList<KDirectoryEntry> &KDirectoryPrivate::entryInfoList(QDir::Filters filters, QDir::SortFlags sort)
{
    // A LOT more has to be checked here before something is returned..
    return m_dirEntries;
}

const KDirectoryEntry &KDirectoryPrivate::entryLookup(int index)
{
    if(index < m_allEntries.count()) {
        return m_allEntries.at(index);
    }
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
    if(m_filterFlags.testFlag(QDir::NoFilter)) {
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

    qDebug() << "name: " << entry.name() << "Flags:" << m_filterFlags;

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
    if(m_sortFlags & QDir::DirsFirst) {

        // For now we simply merge the two lists to one..
        m_allEntries = m_dirEntries + m_fileEntries;

    } else if(m_sortFlags & QDir::DirsLast) {

        // For now we simply merge the two lists to one..
        m_allEntries = m_fileEntries + m_dirEntries;

    } else {

    }
}

void KDirectoryPrivate::slotEntries(KIO::Job *job, const KIO::UDSEntryList &entries)
{
    if(entries.count() > 0) {

        // If DirsFirst or DirsLast is provided as sort flag then we need to create two different lists. One for the files, one for the folders.
        if(m_sortFlags & QDir::DirsFirst || m_sortFlags & QDir::DirsLast) {
            QList<KDirectoryEntry> dirEntries;
            QList<KDirectoryEntry> fileEntries;
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
            QList<KDirectoryEntry> allEntries;
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

//    foreach(KDirectoryEntry entry, m_dirEntries) {
//        qDebug() << "------------------------------";
//        qDebug() << "Entry:" << &entry;
//        qDebug() << "Name:" << entry.name();
//        qDebug() << "Basename:" << entry.basename();
//        qDebug() << "Extension:" << entry.extension();
//        qDebug() << "IconName:" << entry.iconName();
//        qDebug() << "MimeComment:" << entry.mimeComment();
//        qDebug() << "------------------------------";
//    }

    qDebug() << "Entries! Count:" << m_allEntries.count();
    qDebug() << "Entries! Count:" << entries.count();
}

void KDirectoryPrivate::slotResult(KJob *job)
{
    Q_UNUSED(job)

    // Thought: since we're emitting it directly, perhaps just remove this slot completely and emit the signal from KDirListerV2?
    emit completed();

}
