#include "kdirectoryentry.h"

#include <sys/stat.h>
#include <KMimeType>

#include <QDebug>

class KDirectoryEntryPrivate
{
public:

    // This class contains the more exotic stat data that might be needed for file entries.
    class FullStatData
    {
    public:

        FullStatData()
            : m_isReadable(false)
            , m_isWritable(false)
        {

        }

        bool m_isReadable;
        bool m_isWritable;
    };

    KDirectoryEntryPrivate()
        : m_name()
        , m_fullDataAvailable(false)
        , m_fileMode()
        , m_statData(0)
    {
    }

    void parseUDSEntryBare(const KIO::UDSEntry& entry)
    {
        m_fullDataAvailable = false;
        m_name = entry.stringValue(KIO::UDSEntry::UDS_NAME);
        m_fileMode = entry.numberValue( KIO::UDSEntry::UDS_FILE_TYPE );
    }

    void parseUDSEntryFull(const KIO::UDSEntry& entry)
    {
        m_fullDataAvailable = true;
        parseUDSEntryBare(entry);

        // Full UDSEntry not supported - yet!

        m_fullDataAvailable = true;
    }

    void statCall()
    {
        if(!m_fullDataAvailable && !m_statData) {
            m_statData = new FullStatData();

            // Parse stat data and put it in some class fields.


            m_fullDataAvailable = true;
        }
    }

    const QString basename()
    {
        int dotPosition = m_name.lastIndexOf(QString("."));
        return m_name.left(dotPosition);
    }

    const QString extension()
    {
        if(m_name[0] != '.') {
            int lastDot = m_name.lastIndexOf(QString("."));

            if(lastDot > 0) {
                int dotPosition = m_name.length() - lastDot - 1;
                return m_name.right(dotPosition);
            }

        }
        return QString();
    }

    bool isReadable()
    {
        statCall();
        if(m_fullDataAvailable && m_statData) {
            return m_statData->m_isReadable;
        }
        return false;
    }

    bool isWritable()
    {
        return false;
    }

    QString m_name;
    bool m_fullDataAvailable;
    mode_t m_fileMode;
    FullStatData* m_statData;
};

KDirectoryEntry::KDirectoryEntry(const KIO::UDSEntry &entry, const QString &details)
    : d(new KDirectoryEntryPrivate())
{
    // Details comes from the directory lister. If it's 0 then we only have very few details in the entry object.
    if(details == "0") {
        d->parseUDSEntryBare(entry);
    } else {
        d->parseUDSEntryFull(entry);
    }
}

const QString &KDirectoryEntry::name()
{
    return d->m_name;
}

const QString KDirectoryEntry::basename()
{
    return d->basename();
}

const QString KDirectoryEntry::extension()
{
    return d->extension();
}

const QString KDirectoryEntry::iconName()
{
    KMimeType::Ptr mime;
    if(isDir()) {
        mime = KMimeType::findByPath("/", 0, true);
    } else {
        mime = KMimeType::findByPath(name(), 0, true);
    }
    return mime->iconName();
}

const QString KDirectoryEntry::mimeComment()
{
    KMimeType::Ptr mime;
    if(isDir()) {
        mime = KMimeType::findByPath("/", 0, true);
    } else {
        mime = KMimeType::findByPath(name(), 0, true);
    }
    return mime->comment();
}

bool KDirectoryEntry::isLink() const
{
    return S_ISLNK(d->m_fileMode);
}

bool KDirectoryEntry::isDir() const
{
    return S_ISDIR(d->m_fileMode);
}

bool KDirectoryEntry::isFile() const
{
    return !isDir();
}

bool KDirectoryEntry::isReadable() const
{
    return d->isReadable();
}

bool KDirectoryEntry::isWritable() const
{
    return d->isWritable();
}

bool KDirectoryEntry::isHidden() const
{
    if(d->m_name == 0) {
        return false;
    } else {
        if(d->m_name[0] == '.') {
            return true;
        } else {
            return false;
        }
    }
}
