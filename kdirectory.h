#ifndef KDIRECTORY_H
#define KDIRECTORY_H

#include <QObject>
#include <KIO/Job>

#include "kdirectoryentry.h"

class KDirectory : public QObject
{
    Q_OBJECT
public:
    explicit KDirectory(const QString& directory, QObject *parent = 0);
    
    virtual const QList<KDirectoryEntry>& entries();
    virtual const QString& url();
    virtual int count();

signals:
    void entriesProcessed(KDirectory* dir);
    void completed(KDirectory* dir);
    
public slots:
    void slotEntries( KIO::Job *job, const KIO::UDSEntryList &entries );
    void slotResult( KJob *job );
    
private:
    QString m_directory;
    QList<KDirectoryEntry> m_dirEntries;
    int m_count;

};

#endif // KDIRECTORY_H
