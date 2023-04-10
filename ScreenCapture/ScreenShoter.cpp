#include "ScreenShoter.h"
#include <QPainter>
#include <QPixmap>
#include <QFile>
#include <QScreen>
#include <QGuiApplication>

static ScreenShoter* instance;

ScreenShoter::~ScreenShoter()
{
    delete desktopImage;
}
void ScreenShoter::Dispose()
{
    delete instance;
}
ScreenShoter::ScreenShoter()
{
    auto screens = QGuiApplication::screens();
    QList<QRectF> rects;
    QList<QPixmap> images;
    for(auto s:screens){
        auto ratio = s->devicePixelRatio();
        auto g = s->geometry();
        auto x = g.x();
        auto y = g.y();
        auto w = g.width()*ratio;
        auto h = g.height()*ratio;
        if(x < absoluteX) absoluteX = x;
        if(y < absoluteY) absoluteY = y;
        QRectF r(x,y,w,h);
        rects.append(std::move(r));
        QPixmap pixmap = s->grabWindow();
        images.append(std::move(pixmap));
    }
    for(auto& r:rects){
        r.moveTopLeft({r.x()-absoluteX,r.y()-absoluteY});
        if(totalHeight < r.bottom()) totalHeight = r.bottom();
        if(totalWidth < r.right()) totalWidth = r.right();
    }
    desktopImage = new QPixmap(totalWidth,totalHeight);
    QPainter painter(desktopImage);
    for (int i = 0; i < rects.count(); ++i) {
        painter.drawImage(rects.at(i),images.at(i).toImage());
    }
//    QFile file("desktopImage.png");
//    file.open(QIODevice::WriteOnly);
//    desktopImage->save(&file, "PNG");
}
void ScreenShoter::Init()
{
    if(!instance){
        instance = new ScreenShoter();
    }

}
ScreenShoter* ScreenShoter::Get()
{
    return instance;
}
