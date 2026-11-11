#include "fileops.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
namespace FileOps {
static bool fail(QString *e, const QString &s) { if(e)*e=s;return false; }
bool copy(const QString &source, const QString &target, QString *e) {
    QFileInfo src(source), dst(target);
    if(!src.exists() && !src.isSymLink()) return fail(e,"Origem inexistente: "+source);
    if(dst.exists() || dst.isSymLink()) return fail(e,"O destino já existe: "+target);
    const QString canonical=src.canonicalFilePath();
    const QString parent=QFileInfo(target).dir().canonicalPath();
    if(src.isDir() && !src.isSymLink() && (parent==canonical || parent.startsWith(canonical+"/")))
        return fail(e,"Não é possível copiar uma pasta para dentro dela mesma.");
    if(src.isSymLink()) return QFile::link(src.symLinkTarget(),target) || fail(e,"Falha ao copiar link.");
    if(src.isDir()) {
        if(!QDir().mkdir(target)) return fail(e,"Falha ao criar pasta: "+target);
        for(const auto &item:QDir(source).entryInfoList(QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Hidden|QDir::System))
            if(!copy(item.absoluteFilePath(),target+"/"+item.fileName(),e)) return false;
        return true;
    }
    QFile file(source); if(!file.copy(target))return fail(e,file.errorString());return true;
}
bool move(const QString &source, const QString &target, QString *e) {
    if(QFileInfo::exists(target) || QFileInfo(target).isSymLink()) return fail(e,"O destino já existe.");
    // Atomic same-filesystem rename; cross-filesystem moves keep the original on any copy failure.
    if(QDir().rename(source,target))return true;
    if(!copy(source,target,e))return false;
    bool ok=QFileInfo(source).isDir()&&!QFileInfo(source).isSymLink()?QDir(source).removeRecursively():QFile::remove(source);
    return ok || fail(e,"Copiado, mas não foi possível remover a origem.");
}
}
