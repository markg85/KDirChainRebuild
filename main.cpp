#include <QApplication>

#include <QTreeView>
#include <QDebug>

#include <KDirModel>
#include "dirmodel.h"
#include "kdirlisterv2.h"
#include "kdirectoryentry.h"


#include <KDirLister>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString url("~/test/");
//    QString url("/home/mark/");

//    KDirLister lister;
//    lister.openUrl(url);

//    KUrl homeFolder(url);

//    KDirModel model;
//    model.setDirLister(&lister);


    DirModel model;
    model.openUrl(url);

//    KDirListerV2 lister;
//    lister.setDetails("0");
//    lister.openUrl(url);

//    QObject::connect(&lister, SIGNAL(completed(KDirectory*)), qApp, SLOT(quit()));


    QTreeView* tree = new QTreeView();
    tree->setModel(&model);
    tree->show();




    return a.exec();
}
