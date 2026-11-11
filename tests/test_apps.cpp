#include "windows.h"
#include "fileops.h"
#include <QtTest>
class Tests:public QObject {
 Q_OBJECT
private slots:
 void textRoundTrip(){QTemporaryDir d;TextWindow w;QString p=d.path()+"/olá.txt";w.editor->setPlainText("Olá, mundo!\nAcentuação: ç ã ê\n");QVERIFY(w.saveTo(p));w.editor->clear();QVERIFY(w.load(p));QCOMPARE(w.editor->toPlainText(),QString("Olá, mundo!\nAcentuação: ç ã ê\n"));QVERIFY(!w.editor->document()->isModified());}
 void atomicTextOverwrite(){QTemporaryDir d;TextWindow w;QString p=d.path()+"/a.txt";w.editor->setPlainText("anterior");QVERIFY(w.saveTo(p));w.editor->setPlainText("novo");QVERIFY(w.saveTo(p));QFile f(p);QVERIFY(f.open(QIODevice::ReadOnly));QCOMPARE(f.readAll(),QByteArray("novo"));}
 void paintHistoryAndExport(){QTemporaryDir d;Canvas c;c.reset({100,80});c.checkpoint();c.image.setPixelColor(3,4,Qt::red);c.undo();QCOMPARE(c.image.pixelColor(3,4),QColor(Qt::white));c.redo();QCOMPARE(c.image.pixelColor(3,4),QColor(Qt::red));QString p=d.path()+"/paint.png";QVERIFY(c.save(p));Canvas second;QVERIFY(second.load(p));QCOMPARE(second.image.pixelColor(3,4),QColor(Qt::red));QCOMPARE(second.image.size(),QSize(100,80));}
 void recursiveCopyAndMove(){QTemporaryDir d;QDir().mkdir(d.path()+"/source");QFile f(d.path()+"/source/a.txt");QVERIFY(f.open(QIODevice::WriteOnly));f.write("real content");f.close();QString error;QVERIFY(FileOps::copy(d.path()+"/source",d.path()+"/copy",&error));QVERIFY(QFileInfo::exists(d.path()+"/copy/a.txt"));QVERIFY(!FileOps::copy(d.path()+"/source",d.path()+"/source/nested",&error));QVERIFY(!FileOps::copy(d.path()+"/source",d.path()+"/copy",&error));QVERIFY(FileOps::move(d.path()+"/copy",d.path()+"/moved",&error));QVERIFY(!QFileInfo::exists(d.path()+"/copy"));QFile moved(d.path()+"/moved/a.txt");QVERIFY(moved.open(QIODevice::ReadOnly));QCOMPARE(moved.readAll(),QByteArray("real content"));}
 void copyDoesNotFollowDirectoryLinks(){QTemporaryDir d;QDir().mkdir(d.path()+"/src");QVERIFY(QFile::link(d.path()+"/src",d.path()+"/src/loop"));QString error;QVERIFY(FileOps::copy(d.path()+"/src",d.path()+"/dst",&error));QVERIFY(QFileInfo(d.path()+"/dst/loop").isSymLink());}
};
QTEST_MAIN(Tests)
#include "test_apps.moc"
