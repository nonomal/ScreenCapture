﻿#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>

#include "../App/App.h"
#include "../Win/WinBox.h"
#include "ToolMain.h"
#include "ToolSub.h"

namespace
{
std::vector<ToolBtn> btns;
std::vector<unsigned> spliterIndexs;
}

ToolMain::ToolMain(QWidget* parent) : QWidget(parent)
{
    setFixedSize(btns.size()*btnW + 8, 32);
    setCursor(Qt::PointingHandCursor);
    setMouseTracking(true);
    setVisible(false);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint| Qt::Tool);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_Hover);
}
ToolMain::~ToolMain()
{
}

void ToolMain::InitData(const QJsonArray& arr, const QString& lang)
{
    int index{ 0 };
    for (const QJsonValue& val : arr)
    {
        ToolBtn btn;
        auto name = val["name"].toString();
        if (name == "splitter")
        {
            spliterIndexs.push_back(index);
            continue;
        }
        else if (name == "rect")
        {
            btn.state = State::rect;
        }
        else if (name == "ellipse")
        {
            btn.state = State::ellipse;
        }
        else if (name == "arrow")
        {
            btn.state = State::arrow;
        }
        else if (name == "number")
        {
            btn.state = State::number;
        }
        else if (name == "line")
        {
            btn.state = State::line;
        }
        else if (name == "text")
        {
            btn.state = State::text;
        }
        else if (name == "mosaic")
        {
            btn.state = State::mosaic;
        }
        else if (name == "eraser")
        {
            btn.state = State::eraser;
        }
        else
        {
            btn.name = name;
        }
        btn.tipText = val[lang].toString("");
        btn.selected = val["selectDefault"].toBool(false);
        {
            bool ok;
            uint codePoint = val["icon"].toString().toUInt(&ok, 16);
            if (ok)
            {
                btn.icon = QChar(codePoint);
            }
            else
            {
                qWarning() << "转换失败";
            }
        }
        btns.push_back(btn);
        index += 1;
    }
}

void ToolMain::paintEvent(QPaintEvent* event)
{
    auto font = App::getIconFont();
    font->setPixelSize(15);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setFont(*font);
    painter.setPen(Qt::GlobalColor::white);
    painter.setBrush(Qt::GlobalColor::white);
    painter.drawRect(rect());
    for (int i = 0; i < btns.size(); i++)
    {
        QRect rect(4 + i * btnW, 0, btnW, height());
        if (i == selectIndex)
        {
            rect.adjust(2, 4, -2, -4);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(228, 238, 255));
            painter.drawRoundedRect(rect, 6, 6);
            painter.setPen(QColor(9, 88, 217));
        }
        else if (i == hoverIndex)
        {
            rect.adjust(2, 4, -2, -4);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(238, 238, 238));
            painter.drawRoundedRect(rect, 6, 6);
            painter.setPen(QColor(33, 33, 33));
        }
        else
        {
            painter.setPen(QColor(33, 33, 33));
        }
        painter.drawText(rect, Qt::AlignCenter, btns[i].icon);
    }
    painter.setPen(QColor(190, 190, 190));
    for (int i = 0; i < spliterIndexs.size(); i++)
    {
        painter.drawLine(4 + spliterIndexs[i] * btnW + 0.5, 9, 4 + spliterIndexs[i] * btnW + 0.5, height() - 9);
    }
}

void ToolMain::mousePressEvent(QMouseEvent* event)
{
    if (hoverIndex == -1) return;
    if (hoverIndex == selectIndex)
    {
        selectIndex = -1;
        win->state = State::tool;
    }
    else
    {
        auto& btn = btns[hoverIndex];
        if (btn.name.isEmpty())
        {
            win->state = btn.state;
            selectIndex = hoverIndex;
            win->showToolSub();
        }
        else if (btn.name == "close")
        {
            win->close();
            return;
        }
    }
    update();
    return;
}

void ToolMain::mouseMoveEvent(QMouseEvent* event)
{
    auto x = event->pos().x() - 4;
    auto index{ x / (int)btnW };
    if (index >= btns.size())index = -1;
    if (index != hoverIndex)
    {
        hoverIndex = index;
        repaint();
        if (hoverIndex > -1)
        {
            auto pos = event->globalPosition();
            //QToolTip::showText(QPoint(pos.x(), pos.y()), btns[hoverIndex].tipText, this);
        }
    }
}

void ToolMain::leaveEvent(QEvent* event)
{
    if (hoverIndex != -1)
    {
        hoverIndex = -1;
        update();
    }
    QWidget::leaveEvent(event);
}
