/**
* @file      RedisTransMgr.cpp
* @brief     REDIS客户端传输类
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/

#include "RedisLib/RedisTransMgr.h"

RedisTransMgr::RedisTransMgr(QObject *parent)
    : QObject(parent)
{
    _socket = nullptr;
    _nPort = 0;
    _hostAddress.clear();
    initTransMgr();
}

RedisTransMgr::RedisTransMgr(const QString &hostAddress, quint16 port, QObject *parent)
    : QObject(parent)
{
    _socket = nullptr;
    _nPort = 0;
    _hostAddress.clear();
    setHostAddress(hostAddress);
    setPort(port);
    initTransMgr();
}

RedisTransMgr::~RedisTransMgr()
{
}

void RedisTransMgr::write(const QByteArray &data)
{
    if (!_socket->isValid())
        return;
    _msg.clear();
    // 发送RESP
    _socket->write(data);
}

void RedisTransMgr::write(const QString &str)
{
    if (!_socket->isValid())
        return;
    _msg.clear();
    // 发送RESP
    QTextStream stream(_socket);
    stream << str;
    stream.flush();
}

bool RedisTransMgr::isOpen()
{
    return _socket->isValid();
}

void RedisTransMgr::setHostAddress(const QString &hostAddress)
{
    _hostAddress.setAddress(hostAddress);
    if (_socket->isValid()) // 如果已连接，重连新的地址
        open(_hostAddress, _nPort);
}

void RedisTransMgr::setPort(quint16 port)
{
    _nPort = port;
    if (_socket->isValid())
        open(_hostAddress, _nPort);
}

bool RedisTransMgr::open(int timeOut)
{
    return open(_hostAddress, _nPort, timeOut);
}

bool RedisTransMgr::open(const QString &hostAddress, quint16 port, int timeOut)
{
    return open(QHostAddress(hostAddress), port, timeOut);
}

bool RedisTransMgr::open(const QHostAddress & hostAddress, quint16 port, int timeOut)
{
    _hostAddress = hostAddress;
    _nPort = port;
    _socket->connectToHost(hostAddress, port);
    if(!_socket->waitForConnected(timeOut)) { // 默认超时1.5秒
        emit sigError(_socket->errorString());
        return false;
    }
    return true;
}

void RedisTransMgr::close()
{
    if (_socket->isValid())
        _socket->disconnectFromHost();
}

void RedisTransMgr::initTransMgr()
{
    _socket = new QTcpSocket(this);
    _socket->setProxy(QNetworkProxy::NoProxy);

    connect(_socket, &QTcpSocket::readyRead, [this]()
    {
        _length = 0;
        _msg.append(_socket->readAll());
        if(getTcpResp(_msg,_length)) {
            // 接收RESP协议原始消息完成
            emit sigReply(_msg, _length);
            _msg.clear();
        }
    });

    connect(_socket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError socketError)>(&QTcpSocket::error),
            [this](QAbstractSocket::SocketError socketError)
    {
        QString error(QMetaType::typeName(socketError));
        emit sigError(error);
    });

    connect(_socket, &QTcpSocket::connected, [this]()
    {
        emit sigConnected();
    });

    connect(_socket, &QTcpSocket::disconnected, [this]()
    {
        emit sigDisconnected();
    });
}
