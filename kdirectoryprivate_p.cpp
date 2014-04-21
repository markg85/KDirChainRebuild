#include "kdirectoryprivate_p.h"

#include <QUrl>
#include <QDebug>

#include <KIO/StatJob>

KDirectoryPrivate::KDirectoryPrivate(KDirectory *dir, const QString& directory)
  : QObject(dir)
  , q(dir)
  , m_directory(directory)
  , m_filteredEntries()
  , m_filteredEntriesCount(0)
  , m_unusedEntries()
  , m_emptyEntry()
  , m_lastEntry()
  , m_lastEntryId(-1)
  , m_statInProgress()
  , m_job(0)
  , m_watch(KDirWatch::self())
  , m_details()
  , m_sortFlags(QDir::NoSort)
  , m_filterFlags(QDir::NoFilter)
{
    QUrl goodUrl(m_directory);
    m_directory = goodUrl.url();
    m_job = KIO::listDir(goodUrl, KIO::HideProgressInfo);
    m_job->setUiDelegate(0);

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
    // No need to check of m_lastEntryId is within the m_filteredEntriesCount because it will only be set if it does
    if(m_lastEntryId >= 0 && m_lastEntryId == index) {
        return m_lastEntry;
    } else if(index >= 0 && index < m_filteredEntriesCount) {
        m_lastEntryId = index;
        m_lastEntry = m_filteredEntries.at(index);
        return m_lastEntry;
    } else {
        m_lastEntryId = -1;

        // No known entry so we return the empty entry
        return m_emptyEntry;
    }
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
    if(entry.isHidden() && (m_filterFlags & QDir::Hidden)) {
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

    return true;
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

    for(const KIO::UDSEntry entry : entries) {
        KDirectoryEntry e(entry, m_details);
        if(keepEntryAccordingToFilter(e)) {
            m_filteredEntries.append(e); // Move the item to m_usableEntries
        } else {
            m_unusedEntries.append(e); // Hidden entries or for whatever reason not being used.
        }
    }
    m_filteredEntriesCount = m_filteredEntries.count();
}

void KDirectoryPrivate::loadEntryDetails(int id)
{
    // Prevent needless stat calls when a stat call for the requested file is currently in progress.
    if(m_statInProgress.contains(id)) {
        return;
    }

    // If we end up here then details are fetched with an id that isn't in out m_statInProgress list yet.
    m_statInProgress.append(id);

    if(id >= 0 && id < m_filteredEntriesCount) {
        QUrl newUrl = QUrl(m_directory + QDir::separator() + m_filteredEntries.at(id).name());

        KIO::StatJob* sjob = KIO::stat(newUrl, KIO::HideProgressInfo);
        sjob->setUiDelegate(0);
        sjob->setProperty("id", id);
        connect(sjob, &KIO::StatJob::result, [&](KJob* job){
            KIO::StatJob* statJob = qobject_cast<KIO::StatJob*>(job);
            if(statJob->error()) {
                // failed to stat this file..
                qDebug() << "Failed to stat the file:" << statJob->url();
            } else {
//                qDebug() << "Failed to stat the file:" << statJob->url() << "id:" << statJob->property("id").toInt();
                int id = statJob->property("id").toInt();
                m_filteredEntries[id].setUDSEntry(statJob->statResult(), "2");
                if(m_filteredEntries[id].detailsLoaded()) {
                    emit entryDetailsChanged(id);
                } else {
                    qDebug() << "Details where loaded, but failed to actually set in the KDirectoryEntry object.";
                }

                // Remove the id from m_statInProgress since we're now done stat calling this file.
                m_statInProgress.removeOne(id);
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
