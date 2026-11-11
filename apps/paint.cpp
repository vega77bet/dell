#include "windows.h"
Canvas::Canvas(QWidget *p):QWidget(p){reset({960,540});setAttribute(Qt::WA_StaticContents);}
void Canvas::reset(QSize size){image=QImage(size,QImage::Format_RGB32);image.fill(Qt::white);setFixedSize(size);history.clear();future.clear();modified=false;update();}
bool Canvas::load(const QString &path){QImageReader reader(path);auto size=reader.size();if(size.width()>4096||size.height()>4096||!size.isValid())return false;QImage i=reader.read();if(i.isNull())return false;reset(i.size());image=i.convertToFormat(QImage::Format_RGB32);update();return true;}
bool Canvas::save(const QString &p){if(!image.save(p))return false;modified=false;return true;}
void Canvas::checkpoint(){history.append(image);while(history.size()>12)history.removeFirst();future.clear();modified=true;}
void Canvas::undo(){if(history.isEmpty())return;future.append(image);image=history.takeLast();setFixedSize(image.size());modified=true;update();}
void Canvas::redo(){if(future.isEmpty())return;history.append(image);image=future.takeLast();setFixedSize(image.size());modified=true;update();}
void Canvas::drawShape(QPainter &p,QPoint end){p.setRenderHint(QPainter::Antialiasing);p.setPen(QPen(color,width,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));QRect r=QRect(start,end).normalized();if(tool=="linha")p.drawLine(start,end);else if(tool=="retângulo")p.drawRect(r);else if(tool=="elipse")p.drawEllipse(r);}
void Canvas::paintEvent(QPaintEvent*){QPainter p(this);p.drawImage(0,0,image);if(drawing)drawShape(p,position);}
void Canvas::mousePressEvent(QMouseEvent *e){if(e->button()!=Qt::LeftButton)return;checkpoint();start=last=position=e->position().toPoint();drawing=true;if(tool=="texto"){QPainter p(&image);p.setPen(color);p.setFont(QFont("Noto Sans",width*3+8));p.drawText(start,text);drawing=false;}else if(tool=="pincel"||tool=="borracha"){QPainter p(&image);p.setPen(QPen(tool=="borracha"?Qt::white:color,width,Qt::SolidLine,Qt::RoundCap));p.drawPoint(start);}update();}
void Canvas::mouseMoveEvent(QMouseEvent *e){if(!drawing)return;position=e->position().toPoint();if(tool=="pincel"||tool=="borracha"){QPainter p(&image);p.setRenderHint(QPainter::Antialiasing);p.setPen(QPen(tool=="borracha"?Qt::white:color,width,Qt::SolidLine,Qt::RoundCap));p.drawLine(last,position);last=position;}update();}
void Canvas::mouseReleaseEvent(QMouseEvent *e){if(!drawing)return;QPainter p(&image);drawShape(p,e->position().toPoint());drawing=false;update();}
PaintWindow::PaintWindow(const QString &path){setWindowTitle("Paint · Dell OS");resize(1080,720);canvas=new Canvas;auto *scroll=new QScrollArea;scroll->setWidget(canvas);setCentralWidget(scroll);auto *b=addToolBar("Desenho");
    Dell::action(b,"Novo","document-new",[this]{if(!canDiscard())return;bool ok;int w=QInputDialog::getInt(this,"Novo desenho","Largura",960,32,2048,1,&ok);if(!ok)return;int h=QInputDialog::getInt(this,"Novo desenho","Altura",540,32,2048,1,&ok);if(ok)canvas->reset({w,h});},QKeySequence::New);
    Dell::action(b,"Abrir","document-open",[this]{if(!canDiscard())return;QString p=QFileDialog::getOpenFileName(this,"Abrir imagem",{},"Imagens (*.png *.jpg *.jpeg *.bmp)");if(!p.isEmpty()&&!canvas->load(p))Dell::error(this,"Não foi possível abrir a imagem (máximo 4096 × 4096).");},QKeySequence::Open);
    Dell::action(b,"Exportar","document-save",[this]{save();},QKeySequence::Save);
    Dell::action(b,"Desfazer","edit-undo",[this]{canvas->undo();},QKeySequence::Undo);Dell::action(b,"Refazer","edit-redo",[this]{canvas->redo();},QKeySequence::Redo);
    auto *tool=new QComboBox;tool->addItems({"pincel","borracha","linha","retângulo","elipse","texto"});b->addWidget(tool);connect(tool,&QComboBox::currentTextChanged,this,[this](const QString &t){canvas->tool=t;if(t=="texto")canvas->text=QInputDialog::getText(this,"Texto","Digite o texto");});
    auto *width=new QSpinBox;width->setRange(1,40);width->setValue(5);b->addWidget(width);connect(width,&QSpinBox::valueChanged,this,[this](int v){canvas->width=v;});
    Dell::action(b,"Cor","color-picker",[this]{QColor c=QColorDialog::getColor(canvas->color,this);if(c.isValid())canvas->color=c;});
    Dell::action(b,"Redimensionar","transform-scale",[this]{bool ok;int w=QInputDialog::getInt(this,"Redimensionar","Largura",canvas->image.width(),32,2048,1,&ok);if(!ok)return;int h=QInputDialog::getInt(this,"Redimensionar","Altura",canvas->image.height(),32,2048,1,&ok);if(ok){canvas->checkpoint();canvas->image=canvas->image.scaled(w,h,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);canvas->setFixedSize(w,h);canvas->update();}});
    statusBar()->showMessage("Desenhe com o botão esquerdo · PNG e JPEG · histórico de até 12 ações");if(!path.isEmpty()&&!canvas->load(path))Dell::error(this,"Não foi possível abrir a imagem.");
}
bool PaintWindow::save(){QString p=QFileDialog::getSaveFileName(this,"Exportar imagem","desenho.png","PNG (*.png);;JPEG (*.jpg)");if(p.isEmpty())return false;if(QFileInfo(p).suffix().isEmpty())p+=".png";if(!canvas->save(p)){Dell::error(this,"Erro ao exportar imagem.");return false;}return true;}
bool PaintWindow::canDiscard(){if(!canvas->modified)return true;auto r=QMessageBox::question(this,"Paint","Salvar desenho?",QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);return r==QMessageBox::Discard||(r==QMessageBox::Save&&save());}
void PaintWindow::closeEvent(QCloseEvent *e){canDiscard()?e->accept():e->ignore();}
