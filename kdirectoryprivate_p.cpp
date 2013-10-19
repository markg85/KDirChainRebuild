#include "kdirectoryprivate_p.h"

#include <QUrl>
#include <QDebug>

#include <KIO/StatJob>

KDirectoryPrivate::KDirectoryPrivate(KDirectory *dir, const QString& directory)
  : QObject(dir)
  , q(dir)
  , m_directory(directory)
  , m_filteredEntries()
  , m_unusedEntries()
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

    connect(m_job, &KIO::ListJob::entries, this, &KDirectoryPrivate::slotEntries);
    connect(m_job, &KJob::result, this, &KDirectoryPrivate::slotResult);
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
    if(index >= 0 && index < m_filteredEntries.count()) {
        return m_filteredEntries.at(index);
    }
    return KDirectoryEntry();
}

int KDirectoryPrivate::count()
{
    return m_filteredEntries.count();
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
    if(m_filterFlags & QDir::Dirs && entry.isDir()) {
        return true;
    }

    if(m_filterFlags & QDir::Files && entry.isFile()) {
        return true;
    }

    if(m_filterFlags & QDir::Readable && entry.isReadable()) {
        return true;
    }

    if(m_filterFlags & QDir::Writable && entry.isWritable()) {
        return true;
    }

    if(m_filterFlags & QDir::Executable && entry.isExecutable()) {
        return true;
    }

    if(m_filterFlags & QDir::Modified && entry.isModified()) {
        return true;
    }

    if(m_filterFlags & QDir::System && entry.isSystem()) {
        return true;
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
        // TODO: restructure this.
    } else if(m_sortFlags & QDir::DirsLast) {
        // TODO: restructure this.
    } else {
        // TODO: restructure this.
    }
}

// This filter just creates a new list with the entries that we are interested in.
void KDirectoryPrivate::processFilterFlags(const KIO::UDSEntryList &entries)
{
    // Move the entries that we want to use to a new list. The remaining entries that we
    // - for whatever reason - don't use move to m_unusedEntries.

    foreach(const KIO::UDSEntry entry, entries) {
        KDirectoryEntry e(entry);
        if(keepEntryAccordingToFilter(e)) {
            m_filteredEntries.append(e); // Move the item to m_usableEntries
        } else {
            m_unusedEntries.append(e); // Hidden entries or for whatever reason not being used.
        }
    }
}

void KDirectoryPrivate::loadEntryDetails(int id)
{
    if(id >= 0 && id < m_unusedEntries.count()) {
        QUrl newUrl = QUrl(m_directory);

        QString name = m_unusedEntries.at(id).name();
        if(m_unusedEntries.at(id).isDir()) {
            name += QDir::separator();
        }
        newUrl.setPath(newUrl.path() + QDir::separator() + name);

        KIO::StatJob* sjob = KIO::stat(newUrl, KIO::HideProgressInfo);
        connect(sjob, &KIO::StatJob::result, [&](){
            if(sjob->error()) {
                // failed to stat this file..
                qDebug() << "Failed to stat the file:" << newUrl.url();
            } else {
                m_unusedEntries[id].setUDSEntry(sjob->statResult(), "2");
                if(m_unusedEntries[id].entryDetailsLoaded()) {
                    emit entryDetailsLoaded(id);
                } else {
                    qDebug() << "Details where loaded, but failed to actually set in the KDirectoryEntry object.";
                }
            }
        });
    }
}

void KDirectoryPrivate::slotEntries(KIO::Job *, const KIO::UDSEntryList &entries)
{
    if(entries.count() > 0) {

//        qDebug() << "Entries received:" << entries.count();

        // Apply filters. Count just so that we filter the last # of entries that we received though this function
        processFilterFlags(entries);

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
    qDebug() << "Filtered entries:" << m_filteredEntries.count();
    qDebug() << "Unused entries:" << m_unusedEntries.count();

    // Thought: since we're emitting it directly, perhaps just remove this slot completely and emit the signal from KDirListerV2?
    emit completed();

}
