#include <QApplication>
#include <QScreen>
#include <QPixmap>
int main(int argc,char**argv){QApplication a(argc,argv);if(argc!=2)return 2;return a.primaryScreen()->grabWindow(0).save(QString::fromLocal8Bit(argv[1]))?0:1;}
