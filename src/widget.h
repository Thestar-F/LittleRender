#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "geometry.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(int _width, int _height, int _SampleN, QWidget *parent = nullptr);
    ~Widget();


    int width;
    int height;
    int SampleN;

    std::vector<vec3> color_buffer;

    void paintEvent(QPaintEvent*);

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
