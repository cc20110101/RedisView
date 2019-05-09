#include "Public/Publib.h"

void PubLib::getList(const QString & str, QList<QString> & strList) {
    QChar c = '\0', nextC = '\0';
    bool bCheck = false;
    QString strBuffer;

    for(int i = 0; i < str.length(); ++i) {
        c = str.at(i);
        if (bCheck) {
            nextC = i < str.length() - 1 ? str.at(i + 1) : ' ';
            if (c == '\\' && nextC == '"') { // 略过转义\"中的'\'
                i++;
                continue;
            } else if (c == '"') { // 遇到了第二个"
                bCheck = false;
            } else
                strBuffer += c;
        } else {
            if (!c.isSpace()) {
                if (c == '\\' && nextC == '"') { // 略过转义\"中的'\'
                    i++;
                    continue;
                } else if (c == '"') { // 遇到第一个"
                    bCheck = true;
                } else
                    strBuffer += c;
            } else if (!strBuffer.isEmpty()) {
                strList << strBuffer; // 追加一个单词
                strBuffer.clear();
            }
        }
    }
    if (!strBuffer.isEmpty()) // 当最后一个字母不是' '也不是'"'时
        strList << strBuffer;
}

void PubLib::setConfig(const QString & key, const QString & value) {
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.setValue(key, value);
}

QString PubLib::getConfig(const QString & key, const QString &defaultValue) {
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    return settings.value(key, defaultValue).toString();
}
