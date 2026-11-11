#include "windows.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
QWidget *mediaWindow(const QString &path){
    auto *w=new QMainWindow;w->setWindowTitle("Mídia · Dell OS");w->resize(900,650);auto *player=new QMediaPlayer(w);auto *audio=new QAudioOutput(w);audio->setVolume(.7);player->setAudioOutput(audio);
    auto *central=new QWidget;auto *layout=new QVBoxLayout(central);auto *video=new QVideoWidget;video->setMinimumSize(320,200);player->setVideoOutput(video);layout->addWidget(video,1);auto *seek=new QSlider(Qt::Horizontal);layout->addWidget(seek);auto *time=new QLabel("00:00 / 00:00");layout->addWidget(time);auto *list=new QListWidget;list->setMaximumHeight(120);layout->addWidget(list);w->setCentralWidget(central);
    auto play=[player,list](int i){if(i<0||i>=list->count())return;list->setCurrentRow(i);player->setSource(QUrl::fromLocalFile(list->item(i)->data(Qt::UserRole).toString()));player->play();};
    QObject::connect(list,&QListWidget::itemDoubleClicked,w,[play,list](QListWidgetItem*i){play(list->row(i));});
    auto append=[list,play](const QStringList &paths){for(const auto&p:paths){auto*i=new QListWidgetItem(QFileInfo(p).fileName(),list);i->setData(Qt::UserRole,p);}if(!paths.isEmpty())play(list->count()-paths.size());};
    auto *b=w->addToolBar("Mídia");b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);Dell::action(b,"Abrir","document-open",[w,append]{append(QFileDialog::getOpenFileNames(w,"Abrir mídia",{},"Áudio e vídeo (*.mp4 *.mkv *.webm *.avi *.mp3 *.ogg *.wav *.flac);;Todos (*)"));},QKeySequence::Open);
    Dell::action(b,"Anterior","media-skip-backward",[list,play]{play(list->currentRow()-1);});Dell::action(b,"Reproduzir / pausar","media-playback-start",[player]{player->playbackState()==QMediaPlayer::PlayingState?player->pause():player->play();},QKeySequence("Space"));Dell::action(b,"Próxima","media-skip-forward",[list,play]{play(list->currentRow()+1);});
    auto *volume=new QSlider(Qt::Horizontal);volume->setMaximumWidth(120);volume->setRange(0,100);volume->setValue(70);b->addWidget(new QLabel(" Volume "));b->addWidget(volume);QObject::connect(volume,&QSlider::valueChanged,audio,[audio](int v){audio->setVolume(v/100.f);});
    QObject::connect(player,&QMediaPlayer::durationChanged,w,[seek](qint64 d){seek->setRange(0,int(d/1000));});QObject::connect(seek,&QSlider::sliderMoved,player,[player](int s){player->setPosition(qint64(s)*1000);});
    QObject::connect(player,&QMediaPlayer::positionChanged,w,[seek,time,player](qint64 p){if(!seek->isSliderDown())seek->setValue(int(p/1000));auto fmt=[](qint64 ms){return QString("%1:%2").arg(ms/60000,2,10,QChar('0')).arg((ms/1000)%60,2,10,QChar('0'));};time->setText(fmt(p)+" / "+fmt(player->duration()));});
    QObject::connect(player,&QMediaPlayer::mediaStatusChanged,w,[list,play](QMediaPlayer::MediaStatus s){if(s==QMediaPlayer::EndOfMedia)play(list->currentRow()+1);});
    QObject::connect(player,&QMediaPlayer::errorOccurred,w,[w,player]{Dell::error(w,player->errorString());});if(!path.isEmpty())append({path});return w;
}
