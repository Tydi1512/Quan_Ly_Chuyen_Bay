#ifndef BUTTONGRIDWIDGET_H
#define BUTTONGRIDWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QVector>

class ButtonGridWidget : public QWidget {
    Q_OBJECT

public:
    ButtonGridWidget(int r, int c, QWidget *parent = nullptr) : QWidget(parent)
    {
        // Khởi tạo layout dạng lưới
        QGridLayout *gridLayout = new QGridLayout(this);
        this->setLayout(gridLayout);

        int columns = c; // Số cột mong muốn (có thể thay đổi theo ý bạn)
        int rows = r; // Tính số hàng

        // Tạo và thêm các nút vào lưới
        for (int i = 0; i < c * r; ++i)
        {
            QPushButton *button = new QPushButton(QString::number(i + 1), this);
            button->setFixedSize(30, 30); // Kích thước cố định cho mỗi nút
            button->setStyleSheet("background-color: #b3b3b3; color: #333333;");

            // Tính toán hàng và cột cho mỗi nút
            int row = i / columns;
            int column = i % columns;

            // Thêm nút vào layout
            gridLayout->addWidget(button, row, column);
        }
    }
};

#endif // BUTTONGRIDWIDGET_H
