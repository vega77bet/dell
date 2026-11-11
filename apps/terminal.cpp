#include "windows.h"
#include <qtermwidget6/qtermwidget.h>
QWidget *terminalWindow(const QStringList &command){
    auto *w=new QMainWindow;w->setWindowTitle("Terminal · Dell OS");w->resize(920,600);auto *tabs=new QTabWidget;tabs->setTabsClosable(true);w->setCentralWidget(tabs);
    auto add=[tabs,command]{auto *t=new QTermWidget(0);t->setTerminalFont(QFont("DejaVu Sans Mono",11));t->setScrollBarPosition(QTermWidget::ScrollBarRight);t->setHistorySize(5000);t->setShellProgram(command.isEmpty()?"/bin/bash":command.first());t->setArgs(command.isEmpty()?QStringList{"-l"}:command.mid(1));t->setWorkingDirectory(QDir::homePath());int i=tabs->addTab(t,"Terminal");tabs->setCurrentIndex(i);QObject::connect(t,&QTermWidget::finished,tabs,[tabs,t]{int n=tabs->indexOf(t);if(n>=0){tabs->removeTab(n);t->deleteLater();}if(!tabs->count())tabs->window()->close();});t->startShellProgram();t->setFocus();};
    auto *bar=w->addToolBar("Terminal");Dell::action(bar,"Nova aba","tab-new",add,QKeySequence("Ctrl+Shift+T"));
    Dell::action(bar,"Copiar","edit-copy",[tabs]{if(auto*t=qobject_cast<QTermWidget*>(tabs->currentWidget()))t->copyClipboard();},QKeySequence("Ctrl+Shift+C"));
    Dell::action(bar,"Colar","edit-paste",[tabs]{if(auto*t=qobject_cast<QTermWidget*>(tabs->currentWidget()))t->pasteClipboard();},QKeySequence("Ctrl+Shift+V"));
    QObject::connect(tabs,&QTabWidget::tabCloseRequested,w,[tabs,w](int i){if(QMessageBox::question(w,"Terminal","Encerrar esta sessão e seus comandos?")!=QMessageBox::Yes)return;QWidget*t=tabs->widget(i);tabs->removeTab(i);t->deleteLater();if(!tabs->count())w->close();});add();return w;
}
