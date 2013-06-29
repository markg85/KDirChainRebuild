#ifndef KDIRECTORYPRIVATE_P_H
#define KDIRECTORYPRIVATE_P_H

#include <QObject>
#include <QDir>

// KDE includes
#include <KDirWatch>
#include <KIO/Job>

#include "kdirectoryentry.h"
#include "kdirectory.h"


class KDirectoryPrivate : public QObject
{
    Q_OBJECT
public:
    explicit KDirectoryPrivate(KDirectory* dir, const QString& directory);
    void setDetails(const QString& details);
    const QVector<KDirectoryEntry>& entryInfoList(QDir::Filters filters = QDir::NoFilter, QDir::SortFlags sort = QDir::NoSort);
    const KDirectoryEntry& entryLookup(int index);
    int count();
    
    QDir::Filters filter();
    void setFilter(QDir::Filters filters);
    QDir::SortFlags sorting();
    void setSorting(QDir::SortFlags sort);

    bool keepEntryAccordingToFilter(KDirectoryEntry entry);
    void processSortFlags();


    // Pointer to the actual KDirectory object.
    KDirectory* q;

    // The full path to the current directory including the current directory name.
    QString m_directory;

    // A list of all entries in this directory.
    QVector<KDirectoryEntry> m_dirEntries;
    QVector<KDirectoryEntry> m_fileEntries;
    QVector<KDirectoryEntry> m_allEntries;
    KIO::UDSEntryList m_disabledEntries;

    KIO::ListJob * m_job;

    KDirWatch* m_watch;

    QString m_details;

    QDir::SortFlags m_sortFlags;
    QDir::Filters m_filterFlags;


signals:
    void entriesProcessed();
    void completed();
    
public slots:
    void slotEntries(KIO::Job *job, const KIO::UDSEntryList &entries);
    void slotResult(KJob *job);
};

#endif // KDIRECTORYPRIVATE_P_H
