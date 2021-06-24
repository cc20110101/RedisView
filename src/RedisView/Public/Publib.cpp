/**
* @file      Publib.h
* @brief     公共函数库
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "Public/Publib.h"

int64_t PubLib::_sequenceId = 0;


void PubLib::setIndexNums(RedisCluster *redisClient, int indexNums) {
    if(!redisClient)
        return;
    QList<ClientInfoDialog> vClientInfo;
    ClientInfoDialog clientInfo;
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    int size = settings.beginReadArray("logins");
    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        clientInfo._name = settings.value("name").toString().trimmed();
        clientInfo._encodeAddr = settings.value("addr").toByteArray();
        clientInfo._encodePasswd = settings.value("passwd").toByteArray();
        clientInfo._encode = settings.value("encode","GB18030").toString().trimmed();
        clientInfo._keyPattern = settings.value("keypattern","").toString();
        clientInfo._valuePattern = settings.value("valuepattern","").toString();
        clientInfo._indexNums = settings.value("indexnums","").toInt();
        vClientInfo << clientInfo;
    }
    settings.endArray();
    settings.remove("logins");
    settings.beginWriteArray("logins");
    for(int j =0; j < vClientInfo.size(); ++j) {
        settings.setArrayIndex(j);
        settings.setValue("name", vClientInfo[j]._name);
        settings.setValue("addr", vClientInfo[j]._encodeAddr);
        settings.setValue("passwd", vClientInfo[j]._encodePasswd);
        settings.setValue("encode", vClientInfo[j]._encode);
        if(redisClient->getConnectName() == vClientInfo[j]._name)
            settings.setValue("indexnums", indexNums);
        else
            settings.setValue("indexnums", vClientInfo[j]._indexNums);
        settings.setValue("keypattern", vClientInfo[j]._keyPattern);
        settings.setValue("valuepattern", vClientInfo[j]._valuePattern);
    }
    settings.endArray();
}

int PubLib::getIndexNums(RedisCluster *redisClient) {
    int indexNum = 1;
    if(!redisClient)
        return indexNum;
    ClientInfoDialog clientInfo;
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    int size = settings.beginReadArray("logins");
    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        clientInfo._name = settings.value("name").toString().trimmed();
        if(redisClient->getConnectName() == clientInfo._name) {
            indexNum = settings.value("indexnums","1").toInt();
            break;
        }
    }
    settings.endArray();
    return indexNum == 0 ? 1: indexNum;
}

void PubLib::log(QString info) {
    QMutexLocker locker(&G_PUBLIC_LIB_MUTEX);
    QString logfile = QCoreApplication::applicationDirPath() + "/" + LogName;
    QString message = QString("[%1] : %2\r\n")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
            .arg(info);
    QFile file(logfile);
    if(file.size() >= 1024*1024*10)
        file.resize(1024);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message;
    file.flush();
    file.close();
}

int PubLib::getKeyType(const QByteArray & type) {
    if(type == "hash") {
        return KEY_HASH;
    } else if(type == "zset") {
        return KEY_ZSET;
    } else if(type == "set") {
        return KEY_SET;
    } else if(type == "string") {
        return KEY_STRING;
    } else if(type == "list") {
        return KEY_LIST;
    } else {
        return KEY_NONE;
    }
}

void PubLib::setSequenceId(int64_t sequenceId) {
    QMutexLocker locker(&G_SEQUENCE_MUTEX);
    _sequenceId = sequenceId;
}

int64_t PubLib::getSequenceId() {
    QMutexLocker locker(&G_SEQUENCE_MUTEX);
    ++ _sequenceId;
    return _sequenceId;
}

void PubLib::getList(const QString & str, QList<QString> & strList) {
    QChar c = '\0', lastC='\0', nextC = '\0';
    bool bCheck = false;
    QString strBuffer;

    for(int i = 0; i < str.length(); ++i) {
        c = str.at(i);
        nextC = i < str.length() - 1 ? str.at(i + 1) : ' ';
        if (bCheck) {
            if (c == '\\' && nextC == '"') { // 略过转义\"中的'\'
                lastC = c;
                continue;
            } else if (lastC != '\\' && c == '"') { // 遇到了第二个"
                bCheck = false;
            } else
                strBuffer += c;
        } else {
            if (!c.isSpace()) {
                if (c == '\\' && nextC == '"') { // 略过转义\"中的'\'
                    lastC = c;
                    continue;
                } else if (lastC != '\\' && c == '"') { // 遇到第一个"
                    bCheck = true;
                } else
                    strBuffer += c;
            } else if (!strBuffer.isEmpty()) {
                strList << strBuffer; // 追加一个单词
                strBuffer.clear();
            }
        }
        lastC = c;
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

void PubLib::setConfigB(const QString & key, const bool & value) {
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.setValue(key, value);
}

bool PubLib::getConfigB(const QString & key, const bool &defaultValue) {
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    return settings.value(key, defaultValue).toBool();
}

QString PubLib::getCode() {
    QString md5;
    QByteArray qba = AesEncrypt::CBC256Crypt(getSoftCode());
    qba = QCryptographicHash::hash(qba, QCryptographicHash::Md5);
    md5.append(qba.toHex());
    return md5;
}

QString PubLib::getSoftCode() {
    return QString("RV%1100").arg(getCpuId());
}

void PubLib::saveCode(QString code) {
    QSettings settings;
    settings.setValue("_sys_c", code);
}

bool PubLib::checkCode() {
    QString md5;
    QByteArray qba = AesEncrypt::CBC256Crypt(getSoftCode());
    qba = QCryptographicHash::hash(qba, QCryptographicHash::Md5);
    md5.append(qba.toHex());
    //QSettings settings;
    //settings.setIniCodec("UTF-8");
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    if(md5 == settings.value("_sys_c", "").toString())
        return true;
    else
        return false;
}

QStringList PubLib::getMacAddr() {
    QStringList mac_list;
    QString strMac;
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    for (int i=0; i<ifaces.count(); i++)
    {
        QNetworkInterface iface = ifaces.at(i);
        //过滤掉本地回环地址、没有开启的地址
        if (iface.flags().testFlag(QNetworkInterface::IsUp) && !iface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            //过滤掉虚拟地址
            if (!(iface.humanReadableName().contains("VMware",Qt::CaseInsensitive)))
            {
                strMac = iface.hardwareAddress();
                mac_list.append(strMac);
            }
        }
    }
    return mac_list;
}

QString PubLib::getCpuId() {
    QString cpu_id = "";
    unsigned int dwBuf[4];
    unsigned long long ret;
    _getcpuid(dwBuf, 1);
    ret = dwBuf[3];
    ret = ret << 32;
    cpu_id = QString::number(dwBuf[3], 16).toUpper();
    cpu_id = cpu_id + QString::number(dwBuf[0], 16).toUpper();
    return cpu_id;
}

void PubLib::_getcpuid(unsigned int CPUInfo[4], unsigned int InfoType) {
#if defined(__GNUC__)// GCC
    __cpuid(InfoType, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
#elif defined(_MSC_VER)// MSVC
#if _MSC_VER >= 1400 //VC2005才支持__cpuid
    __cpuid((int*)(void*)CPUInfo, (int)(InfoType));
#else //其他使用getcpuidex
    _getcpuidex(CPUInfo, InfoType, 0);
#endif
#endif
}

void PubLib::_getcpuidex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue) {
#if defined(_MSC_VER) // MSVC
#if defined(_WIN64)	// 64位下不支持内联汇编. 1600: VS2010, 据说VC2008 SP1之后才支持__cpuidex.
    __cpuidex((int*)(void*)CPUInfo, (int)InfoType, (int)ECXValue);
#else
    if (NULL==CPUInfo)	return;
    _asm{
        // load. 读取参数到寄存器.
        mov edi, CPUInfo;
        mov eax, InfoType;
        mov ecx, ECXValue;
        // CPUID
        cpuid;
        // save. 将寄存器保存到CPUInfo
        mov	[edi], eax;
        mov	[edi+4], ebx;
        mov	[edi+8], ecx;
        mov	[edi+12], edx;
    }
#endif
#else
    Q_UNUSED(CPUInfo)
    Q_UNUSED(InfoType)
    Q_UNUSED(ECXValue)
#endif
}
