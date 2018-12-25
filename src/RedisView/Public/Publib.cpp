#include "Publib.h"


void PubLib::getList(const QString & str, QList<QByteArray> & strList) {

    char c = '\0', nextC = '\0';
    bool bCheck = false;
    QByteArray strCmd = str.trimmed().toLocal8Bit();
    QByteArray strBuffer;

    for(int i = 0; i < strCmd.length(); ++i) {
        c = strCmd.at(i);
        if (bCheck) {
            nextC = i < strCmd.length() - 1 ? strCmd.at(i + 1) : ' ';
            if (c == '\\' && nextC == '"') { // 略过转义\"中的'\'
                i++;
                continue;
            } else if (c == '"') { // 遇到了第二个"
                bCheck = false;
            }
            strBuffer += c;
        } else {
            if (!isspace(c)) {
                if (c == '\\' && nextC == '"') { // 略过转义\"中的'\'
                    i++;
                    continue;
                } else if (c == '"') { // 遇到第一个"
                    bCheck = true;
                }
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
