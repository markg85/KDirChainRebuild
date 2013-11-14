#ifndef KDIRECTORYPRIVATE_P_H
#define KDIRECTORYPRIVATE_P_H

#include <QObject>
#include <QDir>
#include <QVector>

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
    const KDirectoryEntry& entry(int index);
    
    QDir::Filters filter();
    void setFilter(QDir::Filters filters);
    QDir::SortFlags sorting();
    void setSorting(QDir::SortFlags sort);

    bool keepEntryAccordingToFilter(KDirectoryEntry entry);
    void processSortFlags();
    void processFilterFlags(const KIO::UDSEntryList &entries);

    void loadEntryDetails(int id);

    // Pointer to the actual KDirectory object.
    KDirectory* q;

    // The full path to the current directory including the current directory name.
    // It's a QString, but can be directly inserted in a QUrl. This string has the format:
    // <protocol>://<path>
    QString m_directory;

    // A list of all entries in this directory.
    QVector<KDirectoryEntry> m_filteredEntries;
    int m_filteredEntriesCount;
    QVector<KDirectoryEntry> m_unusedEntries;
    KDirectoryEntry m_emptyEntry;
    KDirectoryEntry m_lastEntry;
    int m_lastEntryId;
    QList<int> m_statInProgress;

    KIO::ListJob * m_job;

    KDirWatch* m_watch;

    QString m_details;

    QDir::SortFlags m_sortFlags;
    QDir::Filters m_filterFlags;

signals:
    void entriesProcessed();
    void completed();
    void entryDetailsChanged(int id);
    
public slots:
    void slotEntries(KIO::Job *, const KIO::UDSEntryList &entries);
    void slotResult(KJob *);
};

#endif // KDIRECTORYPRIVATE_P_H
