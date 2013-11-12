#ifndef MIMEIMAGEPROVIDER_H
#define MIMEIMAGEPROVIDER_H

#include <KIconLoader>

#include <QIcon>
#include <QPixmap>
#include <QQuickImageProvider>
#include <QDebug>

/**
 * @brief The MimeImageProvider class
 * Perhaps this class should make use of some pixmap caching?
 */
class MimeImageProvider : public QQuickImageProvider
{
public:
    MimeImageProvider()
        : QQuickImageProvider(QQuickImageProvider::Pixmap)
    {
    }

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
    {
        if(size)
        {
            *size = QSize(256, 256);
        }

        int iconSize = 0;
        if(requestedSize.width() > -1) {
            iconSize = qMin(requestedSize.width(), requestedSize.height());
        } else {
            iconSize = qMin(size->width(), size->height());
        }

        return KIconLoader::global()->loadMimeTypeIcon(id, KIconLoader::Desktop, iconSize);
    }
};

#endif // MIMEIMAGEPROVIDER_H
