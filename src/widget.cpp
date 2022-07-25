#include "widget.h"
#include "ui_widget.h"

#include <QPainter>
#include "shader.h"


extern vec2 offset[4];



Widget::Widget(int _width, int _height, int _SampleN, QWidget *parent) :  QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    width = _width;
    height = _height;
    SampleN = _SampleN;

    color_buffer.resize(SampleN * width * height + SampleN * width);


    setFixedSize(width, height);
}


void Widget::paintEvent(QPaintEvent*){
    QPainter painter(this);
    QPen pen;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            vec3 color(0, 0, 0);
            vec2 center(x + 0.5, y + 0.5);
            for (int i = 0; i < SampleN; i++) {
                vec2 p = center + offset[i];
                int idx = SampleN * (p.x + p.y * width) + i;
                color = color +  color_buffer[idx];

            }
            color = color / SampleN;

            ACES(color);

            //Qt
            QColor Pencolor(color[0], color[1], color[2]);
            pen.setColor(Pencolor);
            painter.setPen(pen);
            painter.drawPoint(center.x, height - center.y);
        }
    }


}

Widget::~Widget()
{
    delete ui;
}

