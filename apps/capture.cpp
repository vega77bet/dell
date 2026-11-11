#include "windows.h"
#include <QCamera>
#include <QMediaDevices>
#include <QMediaCaptureSession>
#include <QMediaRecorder>
#include <QMediaFormat>
#include <QAudioInput>
#include <QAudioSource>
#include <QAudioDevice>
#include <QImageCapture>
#include <QVideoWidget>
#include <cmath>
QWidget *captureWindow(){
    auto *w=new QMainWindow;w->setWindowTitle("Captura · Dell OS");w->resize(850,620);auto *tabs=new QTabWidget;w->setCentralWidget(tabs);
    auto *screen=new QWidget;auto *sl=new QVBoxLayout(screen);sl->addWidget(new QLabel("Capture a tela inteira ou a janela ativa com uma espera de 3 segundos."));auto *shot=new QPushButton("Capturar tela");auto *windowShot=new QPushButton("Capturar janela ativa");sl->addWidget(shot);sl->addWidget(windowShot);sl->addStretch();tabs->addTab(screen,"Tela");
    auto take=[w](bool active){QString dest=QFileDialog::getSaveFileName(w,"Salvar captura","captura.png","PNG (*.png)");if(dest.isEmpty())return;w->hide();QTimer::singleShot(3000,w,[w,dest,active]{QScreen *s=QGuiApplication::primaryScreen();WId id=0;if(active){QProcess p;p.start("xdotool",{"getactivewindow"});p.waitForFinished(1500);id=p.readAllStandardOutput().trimmed().toULong();}QPixmap pic=s->grabWindow(id);w->show();if(pic.isNull()||!pic.save(dest,"PNG"))Dell::error(w,"Falha ao salvar captura.");});};
    QObject::connect(shot,&QPushButton::clicked,w,[take]{take(false);});QObject::connect(windowShot,&QPushButton::clicked,w,[take]{take(true);});
    auto *mic=new QWidget;auto *ml=new QVBoxLayout(mic);auto *inputs=new QComboBox;auto devices=QMediaDevices::audioInputs();for(const auto&d:devices)inputs->addItem(d.description());ml->addWidget(new QLabel(devices.isEmpty()?"Nenhum microfone detectado.":"Microfone"));ml->addWidget(inputs);auto *level=new QProgressBar;level->setRange(0,100);ml->addWidget(new QLabel("Nível de entrada"));ml->addWidget(level);auto *test=new QPushButton("Testar microfone");ml->addWidget(test);auto *record=new QPushButton("Gravar áudio");ml->addWidget(record);auto *state=new QLabel("Pronto");ml->addWidget(state);ml->addStretch();tabs->addTab(mic,"Microfone");
    auto *session=new QMediaCaptureSession(w);auto *input=new QAudioInput(w);session->setAudioInput(input);auto *recorder=new QMediaRecorder(w);session->setRecorder(recorder);QMediaFormat format;format.setFileFormat(QMediaFormat::Wave);recorder->setMediaFormat(format);recorder->setQuality(QMediaRecorder::NormalQuality);
    auto *source=new QAudioSource(QMediaDevices::defaultAudioInput(),QMediaDevices::defaultAudioInput().preferredFormat(),w);source->setObjectName("audioTest");
    QObject::connect(test,&QPushButton::clicked,w,[w,test,level]{auto*source=w->findChild<QAudioSource*>("audioTest");if(source->state()==QAudio::ActiveState){source->stop();test->setText("Testar microfone");level->setValue(0);return;}QIODevice *io=source->start();if(!io)return;test->setText("Parar teste");QObject::connect(io,&QIODevice::readyRead,source,[source,io,level]{QByteArray b=io->readAll();float peak=0;QAudioFormat f=source->format();for(int i=0;i+f.bytesPerSample()<=b.size();i+=f.bytesPerSample())peak=qMax(peak,std::abs(f.normalizedSampleValue(b.constData()+i)));level->setValue(qRound(peak*100));});});
    QObject::connect(inputs,&QComboBox::currentIndexChanged,w,[w,input,devices,level,test](int i){if(i>=0&&i<devices.size()){delete w->findChild<QAudioSource*>("audioTest");auto*source=new QAudioSource(devices[i],devices[i].preferredFormat(),w);source->setObjectName("audioTest");input->setDevice(devices[i]);test->setText("Testar microfone");level->setValue(0);}});
    QObject::connect(record,&QPushButton::clicked,w,[w,recorder]{if(recorder->recorderState()==QMediaRecorder::RecordingState){recorder->stop();return;}QString p=QFileDialog::getSaveFileName(w,"Salvar gravação","gravacao.wav","WAV (*.wav)");if(p.isEmpty())return;w->findChild<QAudioSource*>("audioTest")->stop();recorder->setOutputLocation(QUrl::fromLocalFile(p));recorder->record();});
    QObject::connect(recorder,&QMediaRecorder::recorderStateChanged,w,[recorder,record,state]{bool r=recorder->recorderState()==QMediaRecorder::RecordingState;record->setText(r?"Parar e salvar":"Gravar áudio");state->setText(r?"Gravando…":("Salvo em "+recorder->actualLocation().toLocalFile()));});QObject::connect(recorder,&QMediaRecorder::errorOccurred,w,[w,recorder]{Dell::error(w,recorder->errorString());});test->setEnabled(!devices.isEmpty());record->setEnabled(!devices.isEmpty());
    auto *camPage=new QWidget;auto *cl=new QVBoxLayout(camPage);auto cams=QMediaDevices::videoInputs();auto *combo=new QComboBox;for(const auto&c:cams)combo->addItem(c.description());cl->addWidget(combo);auto *view=new QVideoWidget;cl->addWidget(view,1);auto *photo=new QPushButton("Salvar foto");cl->addWidget(photo);tabs->addTab(camPage,"Câmera");
    auto *cs=new QMediaCaptureSession(w);auto *camera=new QCamera(w);cs->setCamera(camera);cs->setVideoOutput(view);auto *image=new QImageCapture(w);cs->setImageCapture(image);if(!cams.isEmpty())camera->setCameraDevice(cams.first());else {photo->setEnabled(false);cl->addWidget(new QLabel("Nenhuma câmera encontrada. Conecte uma webcam e reabra o aplicativo."));}
    QObject::connect(combo,&QComboBox::currentIndexChanged,w,[camera,cams](int i){if(i>=0&&i<cams.size()){camera->stop();camera->setCameraDevice(cams[i]);camera->start();}});
    QObject::connect(tabs,&QTabWidget::currentChanged,w,[w,camera,cams](int i){if(i==2&&!cams.isEmpty())camera->start();else camera->stop();if(i!=1)w->findChild<QAudioSource*>("audioTest")->stop();});
    QObject::connect(photo,&QPushButton::clicked,w,[w,image]{QString p=QFileDialog::getSaveFileName(w,"Salvar foto","foto.jpg","JPEG (*.jpg)");if(!p.isEmpty())image->captureToFile(p);});QObject::connect(camera,&QCamera::errorOccurred,w,[w,camera]{Dell::error(w,camera->errorString());});QObject::connect(image,&QImageCapture::errorOccurred,w,[w](int,QImageCapture::Error,const QString&s){Dell::error(w,s);});return w;
}
