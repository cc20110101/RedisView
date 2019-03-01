/**
* @file      RedisRespParser.cpp
* @brief     REDIS客户端RESP解析类
* @author    wangcc3
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/

#include "RedisLib/RedisRespParser.h"

RedisRespParser::RedisRespParser() {
    _strList.clear();
    _strCmd.clear();
    _strBuffer.clear();
}

bool RedisRespParser::packRespCmd(const QList<QString> &vInCmd, QByteArray &sOutRespCmd) {
    if(vInCmd.isEmpty())
        return false;

    sOutRespCmd.clear();
    sOutRespCmd.append("*");
    sOutRespCmd.append(QByteArray::number(vInCmd.size()));
    sOutRespCmd.append("\r\n");
    for (int i = 0; i < vInCmd.size(); ++i)
    {
        sOutRespCmd.append("$");
        sOutRespCmd.append(QByteArray::number(vInCmd[i].toLocal8Bit().length()));
        sOutRespCmd.append("\r\n");
        sOutRespCmd.append(vInCmd[i].toLocal8Bit());
        sOutRespCmd.append("\r\n");
    }
    return true;
}

bool RedisRespParser::packRespCmd(const QString &sInCmd, QByteArray &sOutRespCmd)
{
    if(sInCmd.isEmpty())
        return false;

    _strCmd = sInCmd.trimmed().toLocal8Bit();

    char c = '\0', nextC = '\0';
    bool bCheck = false;
    for (int i = 0; i < _strCmd.length(); ++i)
    {
        c = _strCmd.at(i);
        if (bCheck)
        {
            nextC = i < _strCmd.length() - 1 ? _strCmd.at(i + 1) : ' ';
            if (c == '\\' && nextC == '"') // 略过转义\"中的'\'
            {
                i++;
                continue;
            } else if (c == '"') { // 遇到了第二个"
                bCheck = false;
            }
            _strBuffer += c;
        } else {
            if (!isspace(c))
            {
                if (c == '\\' && nextC == '"') // 略过转义\"中的'\'
                {
                    i++;
                    continue;
                } else if (c == '"') { // 遇到第一个"
                    bCheck = true;
                }

                _strBuffer += c;
            } else if (!_strBuffer.isEmpty()) {
                _strList << _strBuffer; // 追加一个单词
                _strBuffer.clear();
            }
        }
    }

    if (!_strBuffer.isEmpty()) // 当最后一个字母不是' '也不是'"'时
    {
        _strList << _strBuffer;
    }

    sOutRespCmd.clear();
    sOutRespCmd.append("*");
    sOutRespCmd.append(QByteArray::number(_strList.length()));
    sOutRespCmd.append("\r\n");
    for (int j = 0; j < _strList.length(); ++j)
    {
        sOutRespCmd.append("$");
        sOutRespCmd.append(QByteArray::number(_strList.at(j).length()));
        sOutRespCmd.append("\r\n");
        sOutRespCmd.append(_strList.at(j));
        sOutRespCmd.append("\r\n");
    }

    _strList.clear();
    _strCmd.clear();
    _strBuffer.clear();

    return true;
}

bool RedisRespParser::packRespCmd(const QByteArray &sInCmd, QByteArray &sOutRespCmd)
{
    if(sInCmd.isEmpty())
        return false;

    _strCmd = sInCmd;

    char c = '\0', nextC = '\0';
    bool bCheck = false;
    for (int i = 0; i < _strCmd.length(); ++i)
    {
        c = _strCmd.at(i);
        if (bCheck)
        {
            nextC = i < _strCmd.length() - 1 ? _strCmd.at(i + 1) : ' ';
            if (c == '\\' && nextC == '"') // 略过转义\"中的'\'
            {
                i++;
                continue;
            } else if (c == '"') { // 遇到了第二个"
                bCheck = false;
            }
            _strBuffer += c;
        } else {
            if (!isspace(c))
            {
                if (c == '\\' && nextC == '"') // 略过转义\"中的'\'
                {
                    i++;
                    continue;
                } else if (c == '"') { // 遇到第一个"
                    bCheck = true;
                }

                _strBuffer += c;
            } else if (!_strBuffer.isEmpty()) {
                _strList << _strBuffer; // 追加一个单词
                _strBuffer.clear();
            }
        }
    }

    if (!_strBuffer.isEmpty()) // 当最后一个字母不是' '也不是'"'时
    {
        _strList << _strBuffer;
    }

    sOutRespCmd.clear();
    sOutRespCmd.append("*");
    sOutRespCmd.append(QByteArray::number(_strList.length()));
    sOutRespCmd.append("\r\n");
    for (int j = 0; j < _strList.length(); ++j)
    {
        sOutRespCmd.append("$");
        sOutRespCmd.append(QByteArray::number(_strList.at(j).length()));
        sOutRespCmd.append("\r\n");
        sOutRespCmd.append(_strList.at(j));
        sOutRespCmd.append("\r\n");
    }

    _strList.clear();
    _strCmd.clear();
    _strBuffer.clear();

    return true;
}

bool RedisRespParser::parseRespInteger(const QByteArray &data, qlonglong &llResult) {
    if(data.isEmpty())
        return false;

    QString value;
    if (data.at(0) == ':') {
        value = data.mid(1);
        value.chop(2); // 删除尾部\r\n
        llResult = value.toLongLong();
    } else
        return false;

    return true;
}

bool RedisRespParser::parseRespDouble(const QByteArray &data, double &dResult) {
    if(data.isEmpty())
        return false;

    QString value;
    if (data.at(0) == ':') {
        value = data.mid(1);
        value.chop(2); // 删除尾部\r\n
        dResult = value.toDouble();
    } else
        return false;

    return true;
}

bool RedisRespParser::parseRespString(const QByteArray &data, QByteArray &sResult) {
    if(data.isEmpty())
        return false;

    if (data.at(0) == '+') {
        sResult = data.mid(1);
        sResult.chop(2); // 删除尾部\r\n
    } else
        return false;

    return true;
}

bool RedisRespParser::parseRespBulkString(const QByteArray &data, QString &sResult, int &iResult) {
    if(data.isEmpty())
        return false;

    if (data.at(0) == '$')
    {
        int index = data.indexOf("\r\n");
        iResult = data.mid(1, index - 1).toInt();
        if (iResult > 0) {
            sResult = data.mid(index + 2, iResult);
        } else if(iResult == 0) {
            sResult = "";
        } else if(iResult == -1) {
            sResult = "nil";
        }
    } else
        return false;

    return true;
}

bool RedisRespParser::parseRespBulkString(const QByteArray &data, QByteArray &sResult, int &iResult) {
    if(data.isEmpty())
        return false;

    if (data.at(0) == '$')
    {
        int index = data.indexOf("\r\n");
        iResult = data.mid(1, index - 1).toInt();
        if (iResult > 0) {
            sResult = data.mid(index + 2, iResult);
        } else if(iResult == 0) {
            sResult = "";
        } else if(iResult == -1) {
            sResult = "nil";
        }
    } else
        return false;

    return true;
}

bool RedisRespParser::parseRespError(const QByteArray &data, QByteArray &sResult) {
    if(data.isEmpty())
        return false;

    if (data.at(0) == '-') {
        sResult = data.mid(1);
        sResult.chop(2); // 删除尾部\r\n
    } else
        return false;

    return true;
}

bool RedisRespParser::parseRespArray(const QByteArray &data, QList<RespType> &vResult, int &iResult) {
    if(data.isEmpty())
        return false;

    if (data.at(0) == '*') {
        RespType respType;
        int ilen = -1;
        int index = data.indexOf("\r\n");
        iResult = data.mid(1, index - 1).toInt(); // 列表中元素个数
        int pos = index + 2; // 第一个元素索引

        for (int i = 0; i < iResult; i++)
        {
            respType.init();
            if(data[pos] == '$') {
                index = data.indexOf("\r\n", pos);
                ilen = data.mid(pos + 1, index - pos - 1).toInt();
                respType._formatType = '$';
                respType._formatLength = ilen;
                if (ilen == -1) {
                    respType._stringValue = "nil";
                    vResult << respType;
                    pos = index + 2; // 下一个元素索引
                } else if (ilen == 0) {
                    respType._stringValue = "";
                    vResult << respType;
                    pos = index + 4; // 下一个元素索引
                } else {
                    respType._stringValue = data.mid(index + 2, ilen);
                    vResult << respType; // 提取并追加元素
                    pos = index + 2 + ilen + 2; //下一个元素索引
                }
            } else if(data[pos] == ':') {
                index = data.indexOf("\r\n", pos);
                respType._formatType = ':';
                respType._integerValue = data.mid(pos + 1, index - pos - 1).toLongLong();
                vResult << respType;
                pos = index + 2;
            } else if(data[pos] == '+') {
                index = data.indexOf("\r\n", pos);
                respType._formatType = '+';
                respType._stringValue = data.mid(pos + 1, index - pos - 1);
                vResult << respType;
                pos = index + 2;
            } else if(data[pos] == '-') {
                index = data.indexOf("\r\n", pos);
                respType._formatType = '-';
                respType._stringValue = data.mid(pos + 1, index - pos - 1);
                vResult << respType;
                pos = index + 2;
            } else if(data[pos] == '*') {
                respType._formatType = '*';
                parseRespArray(data.mid(pos),respType._arrayValue,respType._arrayLength);
                vResult << respType;
                int iArrayLength = 0;
                parseRespArrayLength(data.mid(pos),iArrayLength);
                pos = pos + iArrayLength;
            }
        }
    } else
        return false;

    return true;
}

bool RedisRespParser::parseRespArrayLength(const QByteArray &data, int &pos) {

    if(data.isEmpty())
        return false;

    if (data.at(0) == '*') {
        int ilen = -1;
        int index = data.indexOf("\r\n");
        int iResult = data.mid(1, index - 1).toInt(); // 列表中元素个数
        pos = index + 2; // 第一个元素索引

        for (int i = 0; i < iResult; i++)
        {
            if(data[pos] == '$') {
                index = data.indexOf("\r\n", pos);
                ilen = data.mid(pos + 1, index - pos - 1).toInt();
                if (ilen == -1) {
                    pos = index + 2; // 下一个元素索引
                } else if (ilen == 0) {
                    pos = index + 4; // 下一个元素索引
                } else {
                    pos = index + 2 + ilen + 2; // 下一个元素索引
                }
            } else if(data[pos] == ':' ||
                      data[pos] == '+' ||
                      data[pos] == '-') {
                index = data.indexOf("\r\n", pos);
                pos = index + 2;
            } else if(data[pos] == '*') {
                int iArrayLength = 0;
                parseRespArrayLength(data.mid(pos),iArrayLength);
                pos = pos + iArrayLength;
            }
        }
    } else
        return false;

    return true;
}

bool RedisRespParser::parseRespSingelArray(const QByteArray &data, QList<QByteArray> &vResult, int &iResult) {
    if(data.isEmpty())
        return false;

    if (data.at(0) == '*') {
        int ilen = -1;
        int index = data.indexOf("\r\n");
        iResult = data.mid(1, index - 1).toInt(); // 列表中元素个数
        int pos = index + 2; // 第一个元素索引

        for (int i = 0; i < iResult; i++)
        {
            if(data[pos] == '$') {
                index = data.indexOf("\r\n", pos);
                ilen = data.mid(pos + 1, index - pos - 1).toInt();
                if (ilen == -1) {
                    vResult << "nil";
                    pos = index + 2; // 下一个元素索引
                } else if (ilen == 0) {
                    vResult << "";
                    pos = index + 4; // 下一个元素索引
                } else {
                    vResult << data.mid(index + 2, ilen); // 提取并追加元素
                    pos = index + 2 + ilen + 2; // 下一个元素索引
                }
            } else if(data[pos] == ':' ||
                      data[pos] == '+' ||
                      data[pos] == '-') {
                index = data.indexOf("\r\n", pos);
                vResult << data.mid(pos + 1, index - pos - 1);
                pos = index + 2;
            }
        }
    } else
        return false;

    return true;
}

bool RedisRespParser::parseResp(const QByteArray &data, RespType &rResult) {
    if(data.isEmpty())
        return false;

    if (data.at(0) == '+') { // 简单字符串
        rResult._formatType = '+';
        if(!parseRespString(data, rResult._stringValue)) {
            return false;
        }
    } else if (data.at(0) == '-') { // 错误
        rResult._formatType = '-';
        if(!parseRespError(data, rResult._stringValue)) {
            return false;
        }
    } else if (data.at(0) == ':') {  // 整数
        rResult._formatType = ':';
        if(!parseRespInteger(data, rResult._integerValue)) {
            return false;
        }
    } else if (data.at(0) == '$') {  // 大字符串
        rResult._formatType = '$';
        if(!parseRespBulkString(data, rResult._stringValue, rResult._formatLength)) {
            return false;
        }
    } else if (data.at(0) == '*') {  // 数组
        rResult._formatType = '*';
        if(!parseRespArray(data, rResult._arrayValue, rResult._arrayLength)) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

void RedisRespParser::formatToText(const RespType &inResp, QByteArray &outResp, int spaceNum) {
    outResp.append(4 * spaceNum, ' ');
    if(inResp._formatType == ':') {
        outResp.append(QByteArray::number(inResp._integerValue));
        outResp.append("\r\n");
    } else if(inResp._formatType == '+' ||
              inResp._formatType == '-') {
        outResp.append(inResp._stringValue);
        outResp.append("\r\n");
    } else if(inResp._formatType == '$') {
        if(inResp._formatLength == 0) {
            outResp.append("0\r\n");
        } else if (inResp._formatLength == -1) {
            outResp.append("-1\r\n");
        } else {
            outResp.append(inResp._stringValue);
            outResp.append("\r\n");
        }
    } else if(inResp._formatType == '*') {
        ++spaceNum;
        outResp.append(QByteArray::number(inResp._arrayLength));
        outResp.append(":\r\n");
        for(int i = 0 ; i < inResp._arrayLength; ++i) {
            formatToText(inResp._arrayValue[i], outResp, spaceNum);
        }
    }
}

void RedisRespParser::formatToResp(const RespType &inResp, QByteArray &outResp, int spaceNum) {

    outResp.append(4 * spaceNum, ' ');
    outResp.append(inResp._formatType);
    if(inResp._formatType == ':') {
        outResp.append(QByteArray::number(inResp._integerValue));
        outResp.append("\\r\\n\r\n");
    } else if(inResp._formatType == '+' ||
              inResp._formatType == '-') {
        outResp.append(inResp._stringValue);
        outResp.append("\\r\\n\r\n");
    } else if(inResp._formatType == '$') {
        if(inResp._formatLength == 0) {
            outResp.append("0\\r\\n\\r\\n\r\n");
        } else if (inResp._formatLength == -1) {
            outResp.append("-1\\r\\n\r\n");
        } else {
            outResp.append(QByteArray::number(inResp._formatLength));
            outResp.append("\\r\\n");
            outResp.append(inResp._stringValue);
            outResp.append("\\r\\n\r\n");
        }
    } else if(inResp._formatType == '*') {
        ++spaceNum;
        outResp.append(QByteArray::number(inResp._arrayLength));
        outResp.append("\\r\\n\r\n");
        for(int i = 0 ; i < inResp._arrayLength; ++i) {
            formatToResp(inResp._arrayValue[i], outResp, spaceNum);
        }
    }
}

void RedisRespParser::formatToJson(const RespType &inResp, QJsonArray  &outJson) {
    if(inResp._formatType == ':') {
        outJson.append(inResp._integerValue);
    } else if(inResp._formatType == '+' ||
              inResp._formatType == '-') {
        outJson.append(QTextCodec::codecForLocale()->toUnicode(inResp._stringValue));
    } else if(inResp._formatType == '$') {
        if(inResp._formatLength == 0) {
            outJson.append("");
        } else if (inResp._formatLength == -1) {
            outJson.append("nil");
        } else {
            outJson.append(QTextCodec::codecForLocale()->toUnicode(inResp._stringValue));
        }
    } else if(inResp._formatType == '*') {
        QJsonArray jsonArray;
        for(int i = 0 ; i < inResp._arrayLength; ++i) {
            formatToJson(inResp._arrayValue[i], jsonArray);
        }
        outJson.append(jsonArray);
    }
}

bool RedisRespParser::isValidResp(QByteArray &msg, int &ipos) {

    if(msg.isEmpty()) {
        _sErrorInfo = "-msg is null\r\n";
        return false;
    }

    if (msg.length() < ipos) {
        _sErrorInfo = "-missing data\r\n";
        return false;
    }

    bool bRet = true;
    int index = -1;
    int iLength =  0;

    if (msg.at(ipos) == '+' ||
            msg.at(ipos) == '-' ||
            msg.at(ipos) == ':') {
        do {
            index = msg.indexOf("\r\n",ipos);
            if(index == -1) {
                _sErrorInfo = "-missing data\r\n";
                bRet = false;
                break;
            }
        } while(index == -1);
        if(bRet)
            ipos = index + 2;
    } else if (msg.at(ipos) == '$') {
        bool moreRN = true;
        int indexNext = 0;
        int strlength = 0;
        do {
            index = msg.indexOf("\r\n", ipos + indexNext);
            if(index == -1) {
                _sErrorInfo = "-missing data\r\n";
                bRet = false;
                break;
            } else {
                if(moreRN) {
                    strlength = msg.mid(ipos + 1, index - ipos -1).toInt();
                    if(strlength == 0) {
                        ipos = index;
                        indexNext = 2;
                        index = msg.indexOf("\r\n", ipos + indexNext);
                    } else if(strlength  > 0) {
                        ipos = index + 2 + strlength + 2;
                        if(msg.length() < ipos) {
                            _sErrorInfo = "-missing data\r\n";
                            bRet = false;
                            break;
                        } else {
                            index = msg.indexOf("\r\n", index + 2 + strlength);
                        }
                    }
                    moreRN = false;
                }
            }
        } while(index == -1);
        if(bRet) {
            ipos = index + 2;
        }
    } else if (msg.at(ipos) == '*') {
        do {
            index = msg.indexOf("\r\n",ipos);
            if(index == -1) {
                _sErrorInfo = "-missing data\r\n";
                bRet = false;
                break;
            }
        } while(index == -1);
        if(bRet) {
            iLength = msg.mid(ipos +1, index - ipos -1).toInt();
            ipos = index + 2;
            for(int i =0; i < iLength; ++i) {
                while(msg.length() <= ipos) {
                    _sErrorInfo = "-missing data\r\n";
                    bRet = false;
                    break;
                }
                if(!bRet)
                    break;
                if(!isValidResp(msg, ipos)) {
                    bRet = false;
                    break;
                }
            }
        }
    }

    return bRet;
}

bool RedisRespParser::getTcpResp(QByteArray &msg, int &ipos) {
    _sErrorInfo = "-parse succeed\r\n";
    if(!isValidResp(msg,ipos)) {
        return false;
    }
    return true;
}

QString RedisRespParser::getParseInfo() const
{
    return _sErrorInfo;
}
