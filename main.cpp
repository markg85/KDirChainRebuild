#include <QCoreApplication>
#include <QApplication>
#include <QDebug>
#include <QUrl>


#include "models/dirlistmodel.h"
#include "kdirlisterv2.h"
#include "kdirectory.h"
#include "kdirectoryentry.h"
//#include "simpleview.h"

#include <QListView>
#include <QTreeView>


#include <KDirModel>
#include <KDirLister>
#include <QStringList>
#include <QStringListModel>
#include <QTimer>

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);
    QApplication a(argc, argv);

    // Here is the case, some classes (KIO for example) register in the event loop.
    // Once they are done they stop existing and the event loop maintains that.
    // if that was the only running object then the eventloop itselfs makes the app shut down
    // because there is nothing running. Setting below value to fale prevents that from hapening.
    a.setQuitLockEnabled(false);

    QString url("file:///home/kde-devel/50k_files/");
//    QString url("file:///home/kde-devel/massive_folder_test/");
//    QString url("file:///home/kde-devel/");

//    KDirLister lister;
//    lister.openUrl(QUrl(url));

//    KDirModel model;
//    model.setDirLister(&lister);

//    QFileSystemModel model;
//    model.setRootPath(QDir::currentPath());

    DirListModel model;
    model.setPath(url);

    QObject::connect(&model, &QAbstractItemModel::rowsInserted, [&](QModelIndex index,int start,int end){
        qDebug() << "On rows inserted: " << index.isValid() << start << end;
    });

    /*
    int entryCount = 0;
    QStringListModel *model = new QStringListModel();
    QStringList list;

    QTimer *timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, [&](){
        if(entryCount >= 5000) {
            timer->stop();
        } else {
            for(int i = entryCount; i < entryCount + 200; i++) {
                qDebug() << i;
                list << QString::number(i) + ".txt";
            }
            model->insertRows(entryCount, 200);
            entryCount += 200;
        }
    });

    timer->start(100);

    model->setStringList(list);
    */


//    QListView* view = new QListView();
//    SimpleView* view = new SimpleView();
    QTreeView* view = new QTreeView();
    view->setModel(&model);
    view->show();


//    KDirListerV2::DirectoryFetchDetails dirFetchDetails;
//    dirFetchDetails.url = url;
//    dirFetchDetails.details = "0";
//    dirFetchDetails.filters = QDir::Files;

//    KDirListerV2 lister;
//    lister.openUrl(dirFetchDetails);
//    lister.openUrl("~");

//    QObject::connect(&lister, SIGNAL(completed(KDirectory*)), qApp, SLOT(quit()));


//    QTreeView* tree = new QTreeView();
//    tree->setModel(&model);
//    tree->show();

    return a.exec();
}
