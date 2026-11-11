#pragma once
#include <QString>
namespace FileOps {
bool copy(const QString &source, const QString &target, QString *error);
bool move(const QString &source, const QString &target, QString *error);
}
