#include "windows.h"
TextWindow::TextWindow(const QString &path) {
    setWindowTitle("Texto · Dell OS");resize(900,620);
    editor=new QPlainTextEdit;editor->setObjectName("editor");editor->setFont(QFont("DejaVu Sans Mono",11));setCentralWidget(editor);
    auto *bar=addToolBar("Arquivo");bar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    Dell::action(bar,"Novo","document-new",[this]{if(canDiscard()){editor->clear();file.clear();editor->document()->setModified(false);}},QKeySequence::New);
    Dell::action(bar,"Abrir","document-open",[this]{if(canDiscard()){QString p=QFileDialog::getOpenFileName(this,"Abrir texto");if(!p.isEmpty())load(p);}},QKeySequence::Open);
    Dell::action(bar,"Salvar","document-save",[this]{save();},QKeySequence::Save);
    Dell::action(bar,"Salvar como","document-save-as",[this]{save(true);},QKeySequence::SaveAs);
    Dell::action(bar,"Desfazer","edit-undo",[this]{editor->undo();},QKeySequence::Undo);
    Dell::action(bar,"Refazer","edit-redo",[this]{editor->redo();},QKeySequence::Redo);
    Dell::action(bar,"Buscar / substituir","edit-find",[this]{
        QDialog d(this);d.setWindowTitle("Buscar e substituir");QFormLayout l(&d);QLineEdit search,replacement;QCheckBox cases("Diferenciar maiúsculas");QPushButton next("Próxima"),replace("Substituir todas");
        l.addRow("Buscar",&search);l.addRow("Substituir por",&replacement);l.addRow(&cases);l.addRow(&next,&replace);
        connect(&next,&QPushButton::clicked,&d,[&]{QTextDocument::FindFlags flags;if(cases.isChecked())flags|=QTextDocument::FindCaseSensitively;if(!editor->find(search.text(),flags)){auto c=editor->textCursor();c.movePosition(QTextCursor::Start);editor->setTextCursor(c);editor->find(search.text(),flags);}});
        connect(&replace,&QPushButton::clicked,&d,[&]{if(search.text().isEmpty())return;QString t=editor->toPlainText();t.replace(search.text(),replacement.text(),cases.isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive);auto c=editor->textCursor();c.select(QTextCursor::Document);c.insertText(t);});d.exec();
    },QKeySequence::Find);
    connect(editor,&QPlainTextEdit::cursorPositionChanged,this,[this]{auto c=editor->textCursor();statusBar()->showMessage(QString("Linha %1 · Coluna %2 · %3 caracteres · UTF-8").arg(c.blockNumber()+1).arg(c.positionInBlock()+1).arg(editor->toPlainText().size()));});
    connect(editor->document(),&QTextDocument::modificationChanged,this,[this](bool m){setWindowTitle((m?"● ":"")+(file.isEmpty()?"Sem título":QFileInfo(file).fileName())+" · Texto");});
    if(!path.isEmpty())load(path);
}
bool TextWindow::load(const QString &p) {
    QFile f(p);if(!f.open(QIODevice::ReadOnly)){Dell::error(this,f.errorString());return false;}
    if(f.size()>16*1024*1024){Dell::error(this,"Limite de 16 MB para manter o editor leve.");return false;}
    editor->setPlainText(QString::fromUtf8(f.readAll()));file=p;editor->document()->setModified(false);setWindowTitle(QFileInfo(file).fileName()+" · Texto");return true;
}
bool TextWindow::saveTo(const QString &p) {
    QSaveFile f(p);if(!f.open(QIODevice::WriteOnly)){Dell::error(this,f.errorString());return false;}
    const auto data=editor->toPlainText().toUtf8();if(f.write(data)!=data.size() || !f.commit()){Dell::error(this,f.errorString());return false;}
    file=p;editor->document()->setModified(false);setWindowTitle(QFileInfo(file).fileName()+" · Texto");return true;
}
bool TextWindow::save(bool as) {QString p=file;if(as||p.isEmpty())p=QFileDialog::getSaveFileName(this,"Salvar texto",p,"Texto (*.txt);;Todos os arquivos (*)");return !p.isEmpty()&&saveTo(p);}
bool TextWindow::canDiscard() {if(!editor->document()->isModified())return true;auto r=QMessageBox::question(this,"Texto","Salvar alterações?",QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);return r==QMessageBox::Discard||(r==QMessageBox::Save&&save());}
void TextWindow::closeEvent(QCloseEvent *e) {canDiscard()?e->accept():e->ignore();}
