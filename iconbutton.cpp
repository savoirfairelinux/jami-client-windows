#include "iconbutton.h"

IconButton::IconButton(QWidget* parent)
    : QAbstractButton(parent),
    hovered_(false)
{}

void IconButton::enterEvent(QEvent* e)
{
    hovered_ = true;
    QAbstractButton::enterEvent(e);
};

void IconButton::leaveEvent(QEvent* e)
{
    hovered_ = false;
    QAbstractButton::leaveEvent(e);
};

QSize IconButton::sizeHint() const
{
    return pixmap_.size();
}

void IconButton::paintEvent(QPaintEvent* e) {
    Q_UNUSED(e);
    QPainter p(this);
    QRect rect = this->rect();
    p.setPen(Qt::NoPen);
    if (isChecked()) {
        p.setBrush(QColor(240, 240, 240, 240));
        p.drawRect(rect);
    } else if (hovered_) {
        p.setBrush(QColor(245, 245, 245, 245));
        p.drawRect(rect);
    }
    auto _y = (rect.height() - pixmap_.height()) * 0.5;
    p.drawPixmap(24, _y, pixmap_);
    p.setPen(Qt::black);
    auto textHeight = QFontMetrics(font()).size(Qt::TextSingleLine, text_).height();
    auto _y2 = rect.height() - textHeight;
    p.drawText(QPoint(24 + pixmap_.width() + 24, _y2), text_);
}