﻿#include <QPainter>

#include "ShapeRectBase.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Win/Canvas.h"

ShapeRectBase::ShapeRectBase(QObject* parent) : ShapeBase(parent)
{
}

ShapeRectBase::~ShapeRectBase()
{
}

void ShapeRectBase::resetDragger()
{
    prepareDraggers(8);
    auto x{ shape.x() },y{ shape.y() },w{ shape.width() },h{ shape.height() };
    auto half{ draggerSize / 2 };
    draggers[0].setRect(x - half, y - half, draggerSize, draggerSize);
    draggers[1].setRect(x + w / 2 - half, y - half, draggerSize, draggerSize);
    draggers[2].setRect(x + w - half, y - half, draggerSize, draggerSize);
    draggers[3].setRect(x + w - half, y + h / 2 - half, draggerSize, draggerSize);
    draggers[4].setRect(x + w - half, y + h - half, draggerSize, draggerSize);
    draggers[5].setRect(x + w / 2 - half, y + h - half, draggerSize, draggerSize);
    draggers[6].setRect(x - half, y + h - half, draggerSize, draggerSize);
    draggers[7].setRect(x - half, y + h / 2 - half, draggerSize, draggerSize);
}
void ShapeRectBase::paintDragger(QPainter* painter)
{
    if (state != ShapeState::ready) return;
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    for (int i = 0; i < draggers.size(); i++)
    {
        painter->drawRect(draggers[i]);
    }
}
bool ShapeRectBase::mouseMove(QMouseEvent* event)
{
    if (state != ShapeState::ready) return false;
    auto win = (WinBase*)parent();
    auto pos = event->pos();
    hoverDraggerIndex = -1;
    if (draggers[0].contains(pos)) {
        hoverDraggerIndex = 0;
        win->setCursor(Qt::SizeFDiagCursor);
    }
    else if (draggers[1].contains(pos)) {
        hoverDraggerIndex = 1;
        win->setCursor(Qt::SizeVerCursor);
    }
    else if (draggers[2].contains(pos)) {
        hoverDraggerIndex = 2;
        win->setCursor(Qt::SizeBDiagCursor);
    }
    else if (draggers[3].contains(pos)) {
        hoverDraggerIndex = 3;
        win->setCursor(Qt::SizeHorCursor);
    }
    else if (draggers[4].contains(pos)) {
        hoverDraggerIndex = 4;
        win->setCursor(Qt::SizeFDiagCursor);
    }
    else if (draggers[5].contains(pos)) {
        hoverDraggerIndex = 5;
        win->setCursor(Qt::SizeVerCursor);
    }
    else if (draggers[6].contains(pos)) {
        hoverDraggerIndex = 6;
        win->setCursor(Qt::SizeBDiagCursor);
    }
    else if (draggers[7].contains(pos)) {
        hoverDraggerIndex = 7;
        win->setCursor(Qt::SizeHorCursor);
    }
    if (hoverDraggerIndex == -1) {
        mouseOnShape(event);
    }
    if (hoverDraggerIndex > -1) {
        win->canvas->setHoverShape(this);
        return true;
    }
    else {
        return false;
    }
}
bool ShapeRectBase::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        state = (ShapeState)((int)ShapeState::sizing0 + 4);
        pressPos = event->position();
        topLeft = pressPos;
        rightBottom = pressPos;
        return true;
    }
    if (hoverDraggerIndex >= 0) {
        if (state == ShapeState::ready) {
            auto win = (WinBase*)parent();
            win->canvas->removeShapeFromBoard(this);
        }
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        pressPos = event->position();
        topLeft = shape.topLeft();
        rightBottom = shape.bottomRight();
        return true;
    }
    return false;
}
bool ShapeRectBase::mouseRelease(QMouseEvent* event)
{
    if (shape.isEmpty()) { //鼠标按下，没有拖拽，随即释放
        return false;
    }
    if (state >= ShapeState::sizing0) {
        resetDragger();
        state = ShapeState::ready;
        auto win = (WinBase*)parent();
        win->canvas->setHoverShape(this);
        return true;
    }
    return false;
}
void ShapeRectBase::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    auto pos = event->position();
    if (state == ShapeState::sizing0) {
        shape.setCoords(pos.x(), pos.y(), rightBottom.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing1) {
        shape.setCoords(topLeft.x(), pos.y(), rightBottom.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing2) {
        shape.setCoords(topLeft.x(), pos.y(), pos.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing3) {
        shape.setCoords(topLeft.x(), topLeft.y(), pos.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing4) {
        shape.setCoords(topLeft.x(), topLeft.y(),pos.x(), pos.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing5) {
        shape.setCoords(topLeft.x(), topLeft.y(), rightBottom.x(), pos.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing6) {
        shape.setCoords(pos.x(), topLeft.y(), rightBottom.x(), pos.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing7) {
        shape.setCoords(pos.x(), topLeft.y(), rightBottom.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::moving) {
        auto span = pos - pressPos;
        shape.translate(span);
        pressPos = pos;
    }
    if (event->modifiers() & Qt::ShiftModifier) {
        if (shape.width() > shape.height()) {
            shape.setHeight(shape.width());
        }
        else {
            shape.setWidth(shape.height());
        }
    }
    auto win = (WinBase*)parent();
    win->update();
}
void ShapeRectBase::mouseOnShape(QMouseEvent* event)
{
    auto pos = event->position();
    auto half{ strokeWidth / 2 };
    QRectF outerRect = shape.adjusted(-half, -half, half, half);
    QRectF innerRect = shape.adjusted(half, half, -half, -half);
    if (outerRect.contains(pos) && !innerRect.contains(pos)) {
        hoverDraggerIndex = 8;
        auto win = (WinBase*)parent();
        win->setCursor(Qt::SizeAllCursor);
    }
}