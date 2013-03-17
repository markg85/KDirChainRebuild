#ifndef KDIRECTORYPRIVATE_P_H
#define KDIRECTORYPRIVATE_P_H

#include <QObject>

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
    
    // Pointer to the actual KDirectory object.
    KDirectory* q;

    // The full path to the current directory including the current directory name.
    QString m_directory;

    // A list of all entries in this directory.
    QList<KDirectoryEntry> m_dirEntries;

    KIO::ListJob * m_job;

    KDirWatch* m_watch;

    QString m_details;


signals:
    void entriesProcessed();
    void completed();
    
public slots:
    void slotEntries(KIO::Job *job, const KIO::UDSEntryList &entries);
    void slotResult(KJob *job);
};

#endif // KDIRECTORYPRIVATE_P_H
