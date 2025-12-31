// ShrinkingStackedWidget.h
#pragma once
#include <QStackedWidget>

class ShrinkingStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    using QStackedWidget::QStackedWidget;

    QSize sizeHint() const override
    {
        if (currentWidget())
            return currentWidget()->sizeHint();
        return QStackedWidget::sizeHint();
    }

    QSize minimumSizeHint() const override
    {
        if (currentWidget())
            return currentWidget()->minimumSizeHint();
        return QStackedWidget::minimumSizeHint();
    }
};
