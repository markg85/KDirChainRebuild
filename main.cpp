#include <QCoreApplication>
//#include <QTreeView>
#include <QDebug>


//#include <KDirModel>
//#include "dirmodel.h"
#include "kdirlisterv2.h"
#include "kdirectoryentry.h"


//#include <KDirLister>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString url("file:///home/kde-devel/");
//    QString url("smb:///nootdorp-server/");
//    QString url("/home/mark/");

//    KDirLister lister;
//    lister.openUrl(url);

//    QUrl homeFolder(url);

//    KDirModel model;
//    model.setDirLister(&lister);


//    DirModel model;
//    model.openUrl(url);
    qDebug() << "aaabbbccc";

    KDirListerV2 lister;
    lister.setDetails("0");
    lister.openUrl(url);
//    lister.openUrl("~");


//    QObject::connect(&lister, SIGNAL(completed(KDirectory*)), qApp, SLOT(quit()));


//    QTreeView* tree = new QTreeView();
//    tree->setModel(&model);
//    tree->show();


    return a.exec();
}
