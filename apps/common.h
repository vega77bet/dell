#pragma once
#include <QtWidgets>
#include <functional>
namespace Dell {
void theme();
QIcon icon(const QString &name);
QAction *action(QToolBar *bar, const QString &text, const QString &iconName, const std::function<void()> &fn, const QKeySequence &shortcut = {});
void launch(const QString &app, const QStringList &args = {});
void external(const QString &program, const QStringList &args = {});
void error(QWidget *parent, const QString &message);
QString openPath(const QString &value);
QPixmap wallpaper(QSize size, const QString &style = {});
}
