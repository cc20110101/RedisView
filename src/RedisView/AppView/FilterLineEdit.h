#ifndef FILTERLINEEDIT_H
#define FILTERLINEEDIT_H

#include "Public/Define.h"

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
QT_END_NAMESPACE

Q_DECLARE_METATYPE(QRegExp::PatternSyntax)

class FilterLineEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity WRITE setCaseSensitivity)
    Q_PROPERTY(QRegExp::PatternSyntax patternSyntax READ patternSyntax WRITE setPatternSyntax)
public:
    explicit FilterLineEdit(QWidget *parent = 0);

    Qt::CaseSensitivity caseSensitivity() const;
    void setCaseSensitivity(Qt::CaseSensitivity);

    QRegExp::PatternSyntax patternSyntax() const;
    void setPatternSyntax(QRegExp::PatternSyntax);

signals:
    void filterChanged();

private:
    QAction *m_caseSensitivityAction;
    QActionGroup *m_patternGroup;
};

#endif // FILTERLINEEDIT_H
