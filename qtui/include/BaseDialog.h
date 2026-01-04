#pragma once

#include <QDialog>
#include <QAbstractSpinBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QEvent>
#include <QTimer>
#include <QShowEvent>
#include <QMouseEvent>

#include <algorithm>

/*
 * Helper: compute numeric selection length
 *
 * Use std::max<int>(...) to avoid MSVC overload/macro ambiguities on Windows.
 */
inline int numericTextLength(QAbstractSpinBox* spin, QLineEdit* le)
{
    if (!spin || !le)
        return 0;

    if (auto* sb = qobject_cast<QSpinBox*>(spin))
        return std::max<int>(0, le->text().length() - sb->suffix().length());

    if (auto* dsb = qobject_cast<QDoubleSpinBox*>(spin))
        return std::max<int>(0, le->text().length() - dsb->suffix().length());

    return le->text().length();
}

/*
 * Helper: enforce cursor + selection inside numeric region
 */
inline void enforceNumericRegion(QAbstractSpinBox* spin)
{
    if (!spin || !spin->isEnabled() || spin->isReadOnly())
        return;

    QLineEdit* le = spin->findChild<QLineEdit*>();
    if (!le)
        return;

    const int numericLen = numericTextLength(spin, le);
    if (numericLen <= 0)
        return;

    // If no selection or selection spills into suffix, clamp it
    const int selStart = le->selectionStart();
    const int selLen = le->selectedText().length();

    if (selStart < 0 || selStart + selLen > numericLen)
    {
        le->setSelection(0, numericLen);
    }

    // Clamp cursor position
    if (le->cursorPosition() > numericLen)
    {
        le->setCursorPosition(numericLen);
    }
}

/*
 * BaseDialog
 *
 * Ensures QSpinBox / QDoubleSpinBox suffixes are never selectable
 * and the cursor can never enter the suffix region.
 */
class BaseDialog : public QDialog
{
public:
    explicit BaseDialog(QWidget* parent = nullptr)
        : QDialog(parent)
    {
    }

protected:
    void showEvent(QShowEvent* ev) override
    {
        QDialog::showEvent(ev);

        // Install filter on spinboxes AND their internal line edits
        const auto spinboxes = findChildren<QAbstractSpinBox*>();
        for (QAbstractSpinBox* spin : spinboxes)
        {
            spin->installEventFilter(this);

            if (QLineEdit* le = spin->findChild<QLineEdit*>())
                le->installEventFilter(this);
        }
    }

    bool eventFilter(QObject* obj, QEvent* event) override
    {
        // SpinBox focus / mouse events
        if (auto* spin = qobject_cast<QAbstractSpinBox*>(obj))
        {
            if (!isAncestorOf(spin))
                return QDialog::eventFilter(obj, event);

            if (event->type() == QEvent::FocusIn ||
                event->type() == QEvent::MouseButtonPress ||
                event->type() == QEvent::MouseButtonRelease)
            {
                QTimer::singleShot(0, this, [spin]() {
                    enforceNumericRegion(spin);
                    });
            }
        }

        // LineEdit mouse release (this is where Qt likes to move the cursor)
        if (auto* le = qobject_cast<QLineEdit*>(obj))
        {
            if (auto* spin = qobject_cast<QAbstractSpinBox*>(le->parentWidget()))
            {
                if (!isAncestorOf(spin))
                    return QDialog::eventFilter(obj, event);

                if (event->type() == QEvent::MouseButtonRelease ||
                    event->type() == QEvent::MouseButtonPress)
                {
                    QTimer::singleShot(0, this, [spin]() {
                        enforceNumericRegion(spin);
                        });
                }
            }
        }

        return QDialog::eventFilter(obj, event);
    }
};
