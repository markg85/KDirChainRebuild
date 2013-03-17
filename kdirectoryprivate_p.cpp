#include "kdirectoryprivate_p.h"

KDirectoryPrivate::KDirectoryPrivate(KDirectory *dir, const QString& directory)
  : q(dir)
  , m_directory(directory)
  , m_dirEntries()
{
}
