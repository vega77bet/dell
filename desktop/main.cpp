#include "common.h"
#include "x11.h"
struct App {QString name,description,mode,icon;};
static const QList<App> apps{
    {"Arquivos","Pastas, documentos e pendrives","files","system-file-manager"},
    {"Texto","Suas ideias em palavras","text","accessories-text-editor"},
    {"Paint","Desenhe e crie imagens","paint","applications-graphics"},
    {"Mídia","Música e vídeo","media","multimedia-video-player"},
    {"Captura","Tela, microfone e câmera","capture","camera-photo"},
    {"Terminal","Uma linha, muitas possibilidades","terminal","utilities-terminal"},
    {"Configurações","Deixe do seu jeito","settings","preferences-system"},
    {"Navegador","Internet com Firefox ESR","browser","web-browser"},
    {"Ajuda","Seu guia do Dell OS","help","help-browser"}
};
static void run(const App&a){if(a.mode=="browser")Dell::external("firefox-esr");else if(a.mode=="help")Dell::external("firefox-esr",{"file:///usr/share/dell-os/start/index.html"});else Dell::launch(a.mode);}
class Desktop final:public QWidget {
    QPixmap bg;QString current;QSize previous;
protected:
    void paintEvent(QPaintEvent*)override{QSettings s("DellOS","desktop");QString style=s.value("wallpaper","aurora").toString();if(style!=current||size()!=previous){current=style;previous=size();bg=Dell::wallpaper(size(),style);}QPainter p(this);p.drawPixmap(0,0,bg);p.setPen(QColor("#f0e5cd"));p.setFont(QFont("Noto Sans",15,QFont::DemiBold));p.drawText(QRect(64,64,width()-128,40),"DELL / OS");p.setPen(QColor("#99adc7"));p.setFont(QFont("Noto Sans",11));p.drawText(64,132,"Menos peso. Mais possibilidades.");}
public: Desktop(){setWindowFlags(Qt::FramelessWindowHint);setGeometry(QGuiApplication::primaryScreen()->geometry());setWindowTitle("Dell OS Desktop");desktopWindow(winId());auto *layout=new QVBoxLayout(this);layout->setContentsMargins(60,180,0,90);layout->setAlignment(Qt::AlignLeft|Qt::AlignTop);for(int i:{0,1,2}){const auto a=apps[i];auto*b=new QToolButton;b->setText(a.name);b->setIcon(Dell::icon(a.icon));b->setIconSize({34,34});b->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);b->setFixedSize(90,78);b->setStyleSheet("color:#edf2fa;background:transparent;");connect(b,&QToolButton::clicked,this,[a]{run(a);});layout->addWidget(b);}if(QFile::exists("/run/live/medium/live/filesystem.squashfs")){auto*b=new QPushButton("Instalar Dell OS");b->setFixedWidth(180);layout->addWidget(b);connect(b,&QPushButton::clicked,this,[]{Dell::external("install-dell-os");});}auto*t=new QTimer(this);connect(t,&QTimer::timeout,this,qOverload<>(&QWidget::update));t->start(2000);show();lower();}
};
class Panel final:public QWidget {
    QWidget *menu=new QWidget(nullptr,Qt::Popup);QHBoxLayout *tasks;QLabel *clock;QProcess *scan=new QProcess(this);
public:
    Panel(){setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);setWindowTitle("Dell OS Panel");auto r=QGuiApplication::primaryScreen()->geometry();setGeometry(r.x(),r.bottom()-57,r.width(),58);dockWindow(winId(),58,r.width(),y());auto*l=new QHBoxLayout(this);l->setContentsMargins(14,5,14,5);auto *start=new QPushButton("◈  Dell OS");start->setObjectName("start");l->addWidget(start);buildMenu();connect(start,&QPushButton::clicked,this,[this]{menu->move(x()+12,y()-menu->height()-8);menu->show();menu->findChild<QLineEdit*>()->setFocus();});
        for(int i:{0,7}){auto a=apps[i];auto*b=new QToolButton;b->setIcon(Dell::icon(a.icon));b->setToolTip(a.name);b->setIconSize({24,24});l->addWidget(b);connect(b,&QToolButton::clicked,this,[a]{run(a);});}
        tasks=new QHBoxLayout;l->addLayout(tasks,1);auto*settings=new QPushButton("Rede / som");connect(settings,&QPushButton::clicked,this,[]{Dell::launch("settings");});l->addWidget(settings);clock=new QLabel;l->addWidget(clock);
        auto*t=new QTimer(this);connect(t,&QTimer::timeout,this,[this]{clock->setText(QDateTime::currentDateTime().toString("HH:mm\nddd, dd MMM"));if(scan->state()==QProcess::NotRunning)scan->start("wmctrl",{"-l"});});t->start(1500);
        connect(scan,&QProcess::finished,this,[this]{auto lines=QString::fromUtf8(scan->readAllStandardOutput()).split('\n',Qt::SkipEmptyParts);while(auto*i=tasks->takeAt(0)){delete i->widget();delete i;}for(const QString &line:lines){QStringList fields=line.simplified().split(' ');if(fields.size()<4)continue;QString title=fields.mid(3).join(' ');if(title.startsWith("Dell OS Desktop")||title=="Dell OS Panel")continue;QString id=fields[0];auto*b=new QPushButton(QFontMetrics(font()).elidedText(title,Qt::ElideRight,180));b->setToolTip(title);tasks->addWidget(b);connect(b,&QPushButton::clicked,this,[id]{Dell::external("wmctrl",{"-ia",id});});}tasks->addStretch();});show();
    }
    void buildMenu(){menu->setFixedSize(480,580);auto*l=new QVBoxLayout(menu);l->setContentsMargins(22,22,22,18);auto*brand=new QLabel("Seu espaço.");brand->setFont(QFont("Noto Sans",22,QFont::Bold));l->addWidget(brand);auto*search=new QLineEdit;search->setPlaceholderText("Buscar um aplicativo…");l->addWidget(search);auto*list=new QListWidget;list->setIconSize({30,30});for(int i=0;i<apps.size();++i){const auto&a=apps[i];auto*item=new QListWidgetItem(Dell::icon(a.icon),a.name+"\n"+a.description,list);item->setData(Qt::UserRole,i);item->setSizeHint({400,48});}l->addWidget(list);connect(search,&QLineEdit::textChanged,menu,[list](const QString&s){for(int i=0;i<list->count();++i)list->item(i)->setHidden(!list->item(i)->text().contains(s,Qt::CaseInsensitive));});connect(list,&QListWidget::itemClicked,menu,[this](QListWidgetItem*i){menu->hide();run(apps[i->data(Qt::UserRole).toInt()]);});connect(search,&QLineEdit::returnPressed,menu,[this,list]{for(int i=0;i<list->count();++i)if(!list->item(i)->isHidden()){menu->hide();run(apps[list->item(i)->data(Qt::UserRole).toInt()]);break;}});
        auto*power=new QHBoxLayout;l->addLayout(power);for(const QString &name:{"Bloquear","Reiniciar","Desligar"}){auto*b=new QPushButton(name);power->addWidget(b);connect(b,&QPushButton::clicked,menu,[this,name]{menu->hide();if(name=="Bloquear"){Dell::external("dm-tool",{"lock"});return;}if(QMessageBox::question(this,name,"Salve seus arquivos antes de continuar. "+name+" agora?")==QMessageBox::Yes)Dell::external("systemctl",{name=="Reiniciar"?"reboot":"poweroff"});});}}
    ~Panel(){delete menu;}
};
int main(int argc,char**argv){QApplication a(argc,argv);Dell::theme();a.setApplicationName("dell-shell");a.setQuitOnLastWindowClosed(false);Desktop desktop;Panel panel;return a.exec();}
