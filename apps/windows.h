#pragma once
#include "common.h"
class TextWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit TextWindow(const QString &path = {});
    bool load(const QString &path);
    bool saveTo(const QString &path);
    QPlainTextEdit *editor;
protected: void closeEvent(QCloseEvent *event) override;
private:
    QString file;
    bool save(bool as = false);
    bool canDiscard();
};
class Canvas : public QWidget {
    Q_OBJECT
public:
    QImage image;
    QColor color=QColor("#182335");
    int width=5;
    QString tool="pincel", text="Texto";
    bool modified=false;
    explicit Canvas(QWidget *parent=nullptr);
    void reset(QSize size);
    bool load(const QString &path);
    bool save(const QString &path);
    void undo(); void redo(); void checkpoint();
protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
private:
    QPoint start,last,position; bool drawing=false;
    QList<QImage> history,future;
    void drawShape(QPainter &p, QPoint end);
};
class PaintWindow : public QMainWindow {
    Q_OBJECT
public: explicit PaintWindow(const QString &path={}); Canvas *canvas;
protected: void closeEvent(QCloseEvent*) override;
private: bool canDiscard(); bool save();
};
QWidget *filesWindow(const QString &path);
QWidget *terminalWindow(const QStringList &command={});
QWidget *mediaWindow(const QString &path);
QWidget *captureWindow();
QWidget *settingsWindow(bool welcome=false);
