#ifndef KDIRECTORYENTRY_H
#define KDIRECTORYENTRY_H

#include <QString>
#include <QBitArray>

#include <kio/udsentry.h>

class KDirectoryEntryPrivate;

class KDirectoryEntry
{
public:
    KDirectoryEntry(const KIO::UDSEntry& entry, const QString& details = "0");

    virtual const QString& name();
    const QString basename();
    const QString extension();
    const QString iconName();
    const QString mimeComment();

    /**
     * Returns true if this item represents a link in the UNIX sense of
     * a link.
     * @return true if the file is a link
     */
    bool isLink() const;

    /**
     * Returns true if this item represents a directory.
     * @return true if the item is a directory
     */
    bool isDir() const;

    /**
     * Returns true if this item represents a file (and not a a directory)
     * @return true if the item is a file
     */
    bool isFile() const;

    /**
     * Checks whether the file or directory is readable. In some cases
     * (remote files), we may return true even though it can't be read.
     * @return true if the file can be read - more precisely,
     *         false if we know for sure it can't
     */
    bool isReadable() const;

    /**
     * Checks whether the file or directory is writable. In some cases
     * (remote files), we may return true even though it can't be written to.
     * @return true if the file or directory can be written to - more precisely,
     *         false if we know for sure it can't
     */
    bool isWritable() const;

    /**
     * Checks whether the file is hidden.
     * @return true if the file is hidden.
     */
    bool isHidden() const;


private:
    KDirectoryEntryPrivate* d;

};

#endif // KDIRECTORYENTRY_H
