#include "windows.h"
int main(int argc,char **argv) {
    QApplication app(argc,argv); Dell::theme();
    const QString mode=app.arguments().value(1,"welcome");
    app.setApplicationName("dell-"+mode);
    const QString path=Dell::openPath(app.arguments().value(2));
    QWidget *w=nullptr;
    if(mode=="text")w=new TextWindow(path);
    else if(mode=="paint")w=new PaintWindow(path);
    else if(mode=="files")w=filesWindow(path);
    else if(mode=="terminal")w=terminalWindow(app.arguments().value(2)=="-e"?app.arguments().mid(3):QStringList{});
    else if(mode=="media")w=mediaWindow(path);
    else if(mode=="capture")w=captureWindow();
    else if(mode=="settings" || mode=="welcome")w=settingsWindow(mode=="welcome");
    else {Dell::error(nullptr,"Aplicativo desconhecido: "+mode);return 1;}
    w->setAttribute(Qt::WA_DeleteOnClose); w->show();
    return app.exec();
}
