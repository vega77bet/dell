#include "windows.h"
#include "fileops.h"
namespace {
class Files final : public QMainWindow {
    QFileSystemModel *model=new QFileSystemModel(this);
    QTreeView *view=new QTreeView;
    QLineEdit *address=new QLineEdit;
    QString location;QStringList clipboard;bool cut=false;bool busy=false;
    void closeEvent(QCloseEvent *e)override{if(busy)e->ignore();else e->accept();}
    void go(QString p){p=QDir::cleanPath(p);if(!QFileInfo(p).isDir()){Dell::error(this,"Pasta não encontrada: "+p);return;}location=p;view->setRootIndex(model->setRootPath(p));address->setText(p);setWindowTitle(QFileInfo(p).fileName()+" · Arquivos");statusBar()->showMessage(p);}
    QStringList selected(){QStringList paths;for(const auto &i:view->selectionModel()->selectedRows())paths<<model->filePath(i);return paths;}
    void open(const QString &p){QFileInfo f(p);if(f.isDir())go(p);else if(QStringList{"txt","md","log","ini","conf","cpp","h","json","yaml","sh"}.contains(f.suffix().toLower()))Dell::launch("text",{p});else if(QStringList{"png","jpg","jpeg","bmp"}.contains(f.suffix().toLower()))Dell::launch("paint",{p});else QDesktopServices::openUrl(QUrl::fromLocalFile(p));}
public:
    explicit Files(QString p){resize(980,650);auto *bar=addToolBar("Arquivos");
        Dell::action(bar,"Pasta pessoal","user-home",[this]{go(QDir::homePath());},QKeySequence("Alt+Home"));
        Dell::action(bar,"Acima","go-up",[this]{go(QFileInfo(location).dir().absolutePath());},QKeySequence("Alt+Up"));bar->addWidget(address);connect(address,&QLineEdit::returnPressed,this,[this]{go(address->text());});
        auto *split=new QSplitter;auto *places=new QListWidget;places->setMaximumWidth(190);
        const QList<QPair<QString,QString>> bookmarks{{"Pasta pessoal",QDir::homePath()},{"Documentos",QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)},{"Downloads",QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)},{"Imagens",QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)},{"Lixeira",QDir::homePath()+"/.local/share/Trash/files"},{"Dispositivos USB","/media"},{"Sistema","/"}};
        for(const auto &pair:bookmarks){auto *i=new QListWidgetItem(pair.first,places);i->setData(Qt::UserRole,pair.second);}
        connect(places,&QListWidget::itemClicked,this,[this](QListWidgetItem*i){QString p=i->data(Qt::UserRole).toString();if(p.startsWith(QDir::homePath()))QDir().mkpath(p);go(p);});
        model->setReadOnly(false);model->setFilter(QDir::AllEntries|QDir::NoDotAndDotDot);view->setModel(model);view->setSelectionMode(QAbstractItemView::ExtendedSelection);view->setSortingEnabled(true);view->sortByColumn(0,Qt::AscendingOrder);view->setColumnWidth(0,340);view->setEditTriggers(QAbstractItemView::NoEditTriggers);split->addWidget(places);split->addWidget(view);setCentralWidget(split);
        connect(view,&QTreeView::doubleClicked,this,[this](const QModelIndex &i){open(model->filePath(i));});
        auto *ops=addToolBar("Operações");ops->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        Dell::action(ops,"Nova pasta","folder-new",[this]{bool ok;QString n=QInputDialog::getText(this,"Nova pasta","Nome",QLineEdit::Normal,{},&ok);if(ok&&!n.isEmpty()&&!n.contains('/')&&n!="."&&n!=".."&&!QDir(location).mkdir(n))Dell::error(this,"Não foi possível criar a pasta.");},QKeySequence("Ctrl+Shift+N"));
        Dell::action(ops,"Copiar","edit-copy",[this]{clipboard=selected();cut=false;},QKeySequence::Copy);
        Dell::action(ops,"Recortar","edit-cut",[this]{clipboard=selected();cut=true;},QKeySequence::Cut);
        Dell::action(ops,"Colar","edit-paste",[this]{if(clipboard.isEmpty())return;auto sources=clipboard;const QString dest=location;const bool moving=cut;busy=true;setEnabled(false);auto *thread=QThread::create([this,sources,dest,moving]{QString errors;for(const QString &src:sources){QString e;QString dst=dest+"/"+QFileInfo(src).fileName();if(!(moving?FileOps::move(src,dst,&e):FileOps::copy(src,dst,&e)))errors+=e+"\n";}QMetaObject::invokeMethod(this,[this,errors,moving]{busy=false;setEnabled(true);if(moving)clipboard.clear();if(!errors.isEmpty())Dell::error(this,errors);},Qt::QueuedConnection);});connect(thread,&QThread::finished,thread,&QObject::deleteLater);thread->start();},QKeySequence::Paste);
        Dell::action(ops,"Renomear","edit-rename",[this]{auto paths=selected();if(paths.size()!=1)return;QString src=paths.first();bool ok;QString n=QInputDialog::getText(this,"Renomear","Novo nome",QLineEdit::Normal,QFileInfo(src).fileName(),&ok);if(ok&&!n.isEmpty()&&!n.contains('/')&&n!="."&&n!=".."){QString e;if(!FileOps::move(src,QFileInfo(src).dir().filePath(n),&e))Dell::error(this,e);}},QKeySequence("F2"));
        Dell::action(ops,"Lixeira","user-trash",[this]{auto paths=selected();if(paths.isEmpty())return;if(QMessageBox::question(this,"Mover para lixeira",QString("Mover %1 item(ns) para a lixeira?").arg(paths.size()))!=QMessageBox::Yes)return;for(const auto&p:paths)if(!QFile::moveToTrash(p))Dell::error(this,"Não foi possível mover para a lixeira: "+p);},QKeySequence::Delete);
        Dell::action(ops,"Restaurar","edit-undo",[this]{for(const auto&p:selected()){const QString root=QDir::homePath()+"/.local/share/Trash/";if(QFileInfo(p).dir().absolutePath()!=root+"files")continue;const QString info=root+"info/"+QFileInfo(p).fileName()+".trashinfo";QSettings s(info,QSettings::IniFormat);QString dest=QUrl::fromPercentEncoding(s.value("Trash Info/Path").toByteArray());if(dest.isEmpty()||!dest.startsWith('/')){Dell::error(this,"Metadados da lixeira inválidos.");continue;}QString e;if(FileOps::move(p,dest,&e))QFile::remove(info);else Dell::error(this,e);}});
        Dell::action(ops,"Ocultos","view-hidden",[this]{bool h=model->filter().testFlag(QDir::Hidden);model->setFilter(QDir::AllEntries|QDir::NoDotAndDotDot|(h?QDir::Filters():QDir::Hidden));},QKeySequence("Ctrl+H"));
        Dell::action(ops,"Propriedades","document-properties",[this]{auto paths=selected();if(paths.size()!=1)return;QFileInfo f(paths.first());QMessageBox::information(this,"Propriedades",QString("%1\n%2 bytes\nModificado: %3\nLeitura: %4 · Escrita: %5").arg(f.absoluteFilePath()).arg(f.size()).arg(f.lastModified().toString()).arg(f.isReadable()?"sim":"não",f.isWritable()?"sim":"não"));});
        Dell::action(ops,"Desmontar USB","media-eject",[]{Dell::external("udiskie-umount",{"--all"});});go(p.isEmpty()?QDir::homePath():p);
    }
};
}
QWidget *filesWindow(const QString &path){return new Files(path);}
