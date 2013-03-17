#include "kdirectoryprivate_p.h"

// Only required for KIO::listDir
#include <KUrl>

#include <QDebug>

KDirectoryPrivate::KDirectoryPrivate(KDirectory *dir, const QString& directory)
  : QObject(dir)
  , q(dir)
  , m_directory(directory)
  , m_dirEntries()
  , m_job(0)
  , m_watch(KDirWatch::self())
  , m_details()
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

void KDirectoryPrivate::slotEntries(KIO::Job *job, const KIO::UDSEntryList &entries)
{
    if(entries.count() > 0) {
        QList<KDirectoryEntry> currentList;

        foreach(const KIO::UDSEntry entry, entries) {
            currentList.append(KDirectoryEntry(entry));
        }

        m_dirEntries += currentList;

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

    qDebug() << "Entries! Count:" << entries.count();
}

void KDirectoryPrivate::slotResult(KJob *job)
{
    Q_UNUSED(job)

    // Thought: since we're emitting it directly, perhaps just remove this slot completely and emit the signal from KDirListerV2?
    emit completed();

}
