#include "AppView/FilterLineEdit.h"


FilterLineEdit::FilterLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , m_patternGroup(new QActionGroup(this))
{
    setClearButtonEnabled(true);
    //connect(this, &QLineEdit::textChanged, this, &FilterLineEdit::filterChanged);

    QMenu *menu = new QMenu(this);
    m_caseSensitivityAction = menu->addAction("Case Sensitive");
    m_caseSensitivityAction->setCheckable(true);
    connect(m_caseSensitivityAction, &QAction::toggled, this, &FilterLineEdit::filterChanged);

    menu->addSeparator();
    m_patternGroup->setExclusive(true);
    QAction *patternAction = menu->addAction("Fixed String");
    patternAction->setData(QVariant(int(QRegExp::FixedString)));
    patternAction->setCheckable(true);
    patternAction->setChecked(true);
    m_patternGroup->addAction(patternAction);
    patternAction = menu->addAction("Regular Expression");
    patternAction->setCheckable(true);
    patternAction->setData(QVariant(int(QRegExp::RegExp2)));
    m_patternGroup->addAction(patternAction);
    patternAction = menu->addAction("Wildcard");
    patternAction->setCheckable(true);
    patternAction->setData(QVariant(int(QRegExp::Wildcard)));
    m_patternGroup->addAction(patternAction);
    connect(m_patternGroup, &QActionGroup::triggered, this, &FilterLineEdit::filterChanged);

    const QIcon icon = QIcon(QPixmap(PNG_HISTORY));
    QToolButton *optionsButton = new QToolButton;
#ifndef QT_NO_CURSOR
    optionsButton->setCursor(Qt::ArrowCursor);
#endif
    optionsButton->setFocusPolicy(Qt::NoFocus);
    optionsButton->setStyleSheet("* { border: none; }");
    optionsButton->setIcon(icon);
    optionsButton->setMenu(menu);
    optionsButton->setPopupMode(QToolButton::InstantPopup);

    QWidgetAction *optionsAction = new QWidgetAction(this);
    optionsAction->setDefaultWidget(optionsButton);
    addAction(optionsAction, QLineEdit::LeadingPosition);
}

Qt::CaseSensitivity FilterLineEdit::caseSensitivity() const
{
    return m_caseSensitivityAction->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
}

void FilterLineEdit::setCaseSensitivity(Qt::CaseSensitivity cs)
{
    m_caseSensitivityAction->setChecked(cs == Qt::CaseSensitive);
}

static inline QRegExp::PatternSyntax patternSyntaxFromAction(const QAction *a)
{
    return static_cast<QRegExp::PatternSyntax>(a->data().toInt());
}

QRegExp::PatternSyntax FilterLineEdit::patternSyntax() const
{
    return patternSyntaxFromAction(m_patternGroup->checkedAction());
}

void FilterLineEdit::setPatternSyntax(QRegExp::PatternSyntax s)
{
    foreach (QAction *a, m_patternGroup->actions()) {
        if (patternSyntaxFromAction(a) == s) {
            a->setChecked(true);
            break;
        }
    }
}
