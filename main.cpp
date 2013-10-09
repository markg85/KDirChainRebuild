#include <QCoreApplication>
//#include <QTreeView>
#include <QDebug>
#include <QUrl>


//#include <KDirModel>
//#include "dirmodel.h"
#include "kdirlisterv2.h"
#include "kdirectoryentry.h"
#include <sys/stat.h>
#include <limits.h>


//#include <KDirLister>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Here is the case, some classes (KIO for example) register in the event loop.
    // Once they are done they stop existing and the event loop maintains that.
    // if that was the only running object then the eventloop itselfs makes the app shut down
    // because there is nothing running. Setting below value to fale prevents that from hapening.
//    a.setQuitLockEnabled(false);

//    QString url("file:///home/kde-devel/50k_files/");
    QString url("file:///home/kde-devel/massive_folder_test/");
//    QString url("file:///home/kde-devel/");
//    QString url("smb:///nootdorp-server/");
//    QString url("/home/mark/");

//    KDirLister lister;
//    lister.openUrl(url);

//    QUrl homeFolder(url);

//    KDirModel model;
//    model.setDirLister(&lister);


//    DirModel model;
//    model.openUrl(url);

    KDirListerV2::DirectoryFetchDetails dirFetchDetails;
    dirFetchDetails.url = url;
    dirFetchDetails.details = "0";


    KDirListerV2 lister;
    lister.openUrl(dirFetchDetails);
//    lister.openUrl("~");

//    QObject::connect(&lister, SIGNAL(completed(KDirectory*)), qApp, SLOT(quit()));


//    QTreeView* tree = new QTreeView();
//    tree->setModel(&model);
//    tree->show();

    return a.exec();
}
