#ifndef KDIRECTORYPRIVATE_P_H
#define KDIRECTORYPRIVATE_P_H

#include <QObject>

#include "kdirectoryentry.h"

class KDirectory;

class KDirectoryPrivate : public QObject
{
    Q_OBJECT
public:
    explicit KDirectoryPrivate(KDirectory* dir, const QString& directory);
    
    // Pointer to the actual KDirectory object.
    KDirectory* q;

    // The full path to the current directory including the current directory name.
    QString m_directory;

    // A list of all entries in this directory.
    QList<KDirectoryEntry> m_dirEntries;


signals:
    
public slots:

};

#endif // KDIRECTORYPRIVATE_P_H
