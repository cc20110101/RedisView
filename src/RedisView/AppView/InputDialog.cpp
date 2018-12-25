#include "InputDialog.h"
#include "ui_inputdialog.h"

InputDialog::InputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputDialog)
{
    ui->setupUi(this);
    setWindowTitle("插入");
    setFixedSize(this->size());
}

InputDialog::~InputDialog()
{
    delete ui;
}

QString InputDialog::getText() {
    return ui->_textEdit->document()->toPlainText().trimmed();
}

QList<QByteArray> InputDialog::getTextList() {
    return _textList;
}

void InputDialog::setTip(const QString & info) {
    ui->_textEdit->setPlaceholderText(info);
}

void InputDialog::setType(const QByteArray & type) {
    _type = type;
}

void InputDialog::on__pushButtonY_clicked()
{
    _text = getText();
    if(_text.trimmed().isEmpty()) {
        QMessageBox::critical(this, "错误", "数据为空，请重新输入!");
        return;
    }

    if(_type == "hash") {
        _textList.clear();
        PubLib::getList(_text,_textList);
        if(_textList.size() % 2) {
            QMessageBox::critical(this, "错误", "数据错误，哈希字段值个数不匹配");
            return;
        }
    } else if(_type == "set" || _type == "list") {
        _textList.clear();
        PubLib::getList(_text,_textList);
    } else if(_type == "zset") {
        _textList.clear();
        PubLib::getList(_text,_textList);
        if(_textList.size() % 2) {
            QMessageBox::critical(this, "错误", "数据错误，有序集合成员分数个数不匹配");
            return;
        }

        char c;
        for(int i =0 ; i < _textList.size(); ++++i) {
            for(int j = 0; j < _textList[i+1].size(); ++j) {
                c = _textList[i+1].at(j);
                if(c < '0' || c > '9') {
                    QMessageBox::critical(this, "错误", "数据错误，分数不是有效数值");
                    return;
                }
            }
        }
    }

    accept();
}

void InputDialog::clear() {
    _textList.clear();
    ui->_textEdit->clear();
}

void InputDialog::on__pushButtonN_clicked()
{
    reject();
}

void InputDialog::setFlag(int flag)
{
    _flag = flag;
}
