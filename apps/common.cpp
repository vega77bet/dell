#include "common.h"
namespace Dell {
void theme() {
    QCoreApplication::setOrganizationName("DellOS");
    QCoreApplication::setApplicationVersion("0.1.0");
    QApplication::setStyle("Fusion");
    QIcon::setThemeName("Papirus-Dark");
    QSettings prefs("DellOS", "desktop");
    bool light = prefs.value("light", false).toBool();
    QPalette p;
    p.setColor(QPalette::Window, light ? QColor("#f3f5f8") : QColor("#151d2b"));
    p.setColor(QPalette::WindowText, light ? QColor("#182335") : QColor("#edf2fa"));
    p.setColor(QPalette::Base, light ? QColor("#ffffff") : QColor("#101724"));
    p.setColor(QPalette::AlternateBase, light ? QColor("#edf1f6") : QColor("#1d293b"));
    p.setColor(QPalette::Text, p.color(QPalette::WindowText));
    p.setColor(QPalette::Button, light ? QColor("#e3e9f1") : QColor("#243249"));
    p.setColor(QPalette::ButtonText, p.color(QPalette::Text));
    p.setColor(QPalette::Highlight, QColor("#e6b85c"));
    p.setColor(QPalette::HighlightedText, QColor("#172032"));
    p.setColor(QPalette::Disabled, QPalette::Text, QColor("#7d8a9f"));
    p.setColor(QPalette::ToolTipBase, QColor("#e6b85c"));
    p.setColor(QPalette::ToolTipText, QColor("#101724"));
    qApp->setPalette(p);
    qApp->setFont(QFont("Noto Sans", 10));
    qApp->setStyleSheet("QToolBar{border:0;spacing:6px;padding:8px} QToolButton{padding:7px;border-radius:7px} QToolButton:hover{background:#526079} QPushButton{padding:7px 12px;border-radius:6px} QLineEdit,QComboBox,QSpinBox{padding:6px} QStatusBar{padding:3px} QTabBar::tab{padding:9px 16px}");
}
QIcon icon(const QString &name) { return QIcon::fromTheme(name, QIcon::fromTheme("application-x-executable")); }
QAction *action(QToolBar *bar, const QString &text, const QString &name, const std::function<void()> &fn, const QKeySequence &key) {
    auto *a = bar->addAction(icon(name), text); a->setShortcut(key);
    QObject::connect(a, &QAction::triggered, bar, fn); return a;
}
void launch(const QString &app, const QStringList &args) {
    QStringList all{app}; all.append(args); external("dell-app", all);
}
void external(const QString &program, const QStringList &args) {
    if (!QProcess::startDetached(program, args)) error(nullptr, "Não foi possível iniciar: " + program);
}
void error(QWidget *parent, const QString &msg) { QMessageBox::warning(parent, "Dell OS", msg); }
QString openPath(const QString &v) { return v.startsWith("file:") ? QUrl(v).toLocalFile() : v; }
QPixmap wallpaper(QSize size, const QString &requested) {
    QSettings s("DellOS", "desktop"); QString style = requested.isEmpty() ? s.value("wallpaper", "aurora").toString() : requested;
    QPixmap pm(size); QPainter p(&pm); p.setRenderHint(QPainter::Antialiasing);
    QLinearGradient g(0,0,size.width(),size.height());
    g.setColorAt(0, QColor(style == "areia" ? "#493e38" : "#0b1526"));
    g.setColorAt(1, QColor(style == "oceano" ? "#134651" : "#283c58"));
    p.fillRect(pm.rect(),g);
    p.setPen(Qt::NoPen);
    for(int i=0;i<5;++i) {
        QPainterPath wave; double h=size.height(), w=size.width();
        wave.moveTo(-w*.1,h*(.56+i*.07));
        wave.cubicTo(w*.3,h*(1.0-i*.04),w*.55,h*(.05+i*.11),w*1.1,h*(.4+i*.09));
        wave.lineTo(w*1.1,h*1.1);wave.lineTo(-w*.1,h*1.1);wave.closeSubpath();
        p.setBrush(QColor(style == "areia" ? 219 : 55,style == "areia" ? 173 : 111,style == "areia" ? 107 : 149,30+i*9));p.drawPath(wave);
    }
    return pm;
}
}
