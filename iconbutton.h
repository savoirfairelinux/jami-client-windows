#pragma once
#include <QAbstractButton>
#include <QPainter>

class IconButton : public QAbstractButton {
    Q_OBJECT;

    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap);
    Q_PROPERTY(QString text READ text WRITE setText);

public:
    explicit IconButton(QWidget* parent = nullptr);

    QPixmap pixmap() {
        return pixmap_;
    }

    void setPixmap(const QPixmap& pm) {
        pixmap_ = pm;
        update();
    }

    QString text() const {
        return text_;
    }
    void setText(const QString& text) {
        text_ = text;
    }

protected:
    void enterEvent(QEvent* e) override;
    void leaveEvent(QEvent* e) override;
    void paintEvent(QPaintEvent* e);

    QSize sizeHint() const;

private:
    QPixmap pixmap_;
    QString text_;

    bool hovered_;
};