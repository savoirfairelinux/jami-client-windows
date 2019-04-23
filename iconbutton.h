#pragma once
#include <QAbstractButton>
#include <QPainter>

class IconButton : public QAbstractButton {
    Q_OBJECT;

    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)

public:
    IconButton(QWidget* parent = nullptr)
        : QAbstractButton(parent) {
    }

    QPixmap pixmap() {
        return pixmap_;
    }

    void setPixmap(const QPixmap& pm) {
        pixmap_ = pm;
        update();
    }

    void enterEvent(QEvent* e) override {
        hovered_ = true;
        QAbstractButton::enterEvent(e);
    };

    void leaveEvent(QEvent* e) override {
        hovered_ = false;
        QAbstractButton::leaveEvent(e);
    };

    QSize sizeHint() const { return pixmap_.size(); }
protected:
    void paintEvent(QPaintEvent* e) {
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
        auto _y = (this->height() - pixmap_.height()) * 0.5;
        p.drawPixmap(24, _y, pixmap_);
    }

private:
    QPixmap pixmap_;

    bool hovered_;
};