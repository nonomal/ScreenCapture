﻿#include <qpainter.h>

#include "ShapeRectBase.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBox.h"
#include "../Win/WinCanvas.h"

ShapeRectBase::ShapeRectBase(QObject* parent) : ShapeBase(parent)
{
}

ShapeRectBase::~ShapeRectBase()
{
}

void ShapeRectBase::resetDragger()
{
    if (draggers.empty()) {
        for (int i = 0; i < 8; i++)
        {
            draggers.push_back(QRect());
        }
    }
    auto x{ shape.x() },y{ shape.y() },w{ shape.width() },h{ shape.height() };
    draggers[0].setRect(x - draggerSize / 2, y - draggerSize / 2, draggerSize, draggerSize);
    draggers[1].setRect(x + w / 2 - draggerSize / 2, y - draggerSize / 2, draggerSize, draggerSize);
    draggers[2].setRect(x + w - draggerSize / 2, y - draggerSize / 2, draggerSize, draggerSize);
    draggers[3].setRect(x + w - draggerSize / 2, y + h / 2 - draggerSize / 2, draggerSize, draggerSize);
    draggers[4].setRect(x + w - draggerSize / 2, y + h - draggerSize / 2, draggerSize, draggerSize);
    draggers[5].setRect(x + w / 2 - draggerSize / 2, y + h - draggerSize / 2, draggerSize, draggerSize);
    draggers[6].setRect(x - draggerSize / 2, y + h - draggerSize / 2, draggerSize, draggerSize);
    draggers[7].setRect(x - draggerSize / 2, y + h / 2 - draggerSize / 2, draggerSize, draggerSize);
}
void ShapeRectBase::paintDragger(QPainter* painter)
{
    if (draggers.empty()) return;
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter->setBrush(Qt::NoBrush);
    for (int i = 0; i < draggers.size(); i++)
    {
        painter->drawRect(draggers[i]);
    }
    if (isEraser) {        
        pen.setStyle(Qt::CustomDashLine);
        pen.setDashPattern({3,3});
        painter->setPen(pen);
        painter->drawRect(shape);
    }
}
void ShapeRectBase::mouseMove(QMouseEvent* event)
{
    if (state != ShapeState::ready) return;
    auto pos = event->pos();
    hoverDraggerIndex = -1;
    if (draggers[0].contains(pos)) {
        hoverDraggerIndex = 0;
        auto win = (WinBox*)parent();
        win->updateCursor(Qt::SizeFDiagCursor);
    }
    else if (draggers[1].contains(pos)) {
        hoverDraggerIndex = 1;
        auto win = (WinBox*)parent();
        win->updateCursor(Qt::SizeVerCursor);
    }
    else if (draggers[2].contains(pos)) {
        hoverDraggerIndex = 2;
        auto win = (WinBox*)parent();
        win->updateCursor(Qt::SizeBDiagCursor);
    }
    else if (draggers[3].contains(pos)) {
        hoverDraggerIndex = 3;
        auto win = (WinBox*)parent();
        win->updateCursor(Qt::SizeHorCursor);
    }
    else if (draggers[4].contains(pos)) {
        hoverDraggerIndex = 4;
        auto win = (WinBox*)parent();
        win->updateCursor(Qt::SizeFDiagCursor);
    }
    else if (draggers[5].contains(pos)) {
        hoverDraggerIndex = 5;
        auto win = (WinBox*)parent();
        win->updateCursor(Qt::SizeVerCursor);
    }
    else if (draggers[6].contains(pos)) {
        hoverDraggerIndex = 6;
        auto win = (WinBox*)parent();
        win->updateCursor(Qt::SizeBDiagCursor);
    }
    else if (draggers[7].contains(pos)) {
        hoverDraggerIndex = 7;
        auto win = (WinBox*)parent();
        win->updateCursor(Qt::SizeHorCursor);
    }
    if (hoverDraggerIndex == -1) {
        mouseOnShape(event);
    }
    if (hoverDraggerIndex > -1) {
        auto win = (WinBox*)parent();
        win->refreshCanvas(this);
        event->accept();
    }
}
void ShapeRectBase::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        shape.setTopLeft(event->pos());
        shape.setBottomRight(event->pos());
        hoverDraggerIndex = 4;
    }
    if (hoverDraggerIndex >= 0) {
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        topLeft = shape.topLeft();
        rightBottom = shape.bottomRight();
        pressPos = event->pos();
        event->accept();
        auto win = (WinBox*)parent();
        win->refreshBoard();
        win->refreshCanvas(isEraser?nullptr:this, true);
    }
}
void ShapeRectBase::mouseRelease(QMouseEvent* event)
{
    if (state >= ShapeState::sizing0) {
        resetDragger();
        state = ShapeState::ready;
        auto win = (WinBox*)parent();
        win->refreshBoard();
        win->refreshCanvas(this,true);
        event->accept();
    }
}
void ShapeRectBase::mouseOnShape(QMouseEvent* event)
{
    auto pos = event->pos();
    if (isFill) {
        if (shape.contains(pos)) {
            hoverDraggerIndex = 8;
            auto win = (WinBox*)parent();
            win->updateCursor(Qt::SizeAllCursor);
        }
    }
    else {
        auto half{ strokeWidth / 2 };
        QRectF outerRect = shape.adjusted(-half, -half, half, half);
        QRectF innerRect = shape.adjusted(half, half, -half, -half);
        if (outerRect.contains(pos) && !innerRect.contains(pos)) {
            hoverDraggerIndex = 8;
            auto win = (WinBox*)parent();
            win->updateCursor(Qt::SizeAllCursor);
        }
    }
}
void ShapeRectBase::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    if (state == ShapeState::sizing0) {
        auto pos = event->pos();
        shape.setCoords(pos.x(), pos.y(), rightBottom.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing1) {
        auto pos = event->pos();
        shape.setCoords(topLeft.x(), pos.y(), rightBottom.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing2) {
        auto pos = event->pos();
        shape.setCoords(topLeft.x(), pos.y(), pos.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing3) {
        auto pos = event->pos();
        shape.setCoords(topLeft.x(), topLeft.y(), pos.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing4) {
        auto pos = event->pos();
        shape.setCoords(topLeft.x(), topLeft.y(),pos.x(), pos.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing5) {
        auto pos = event->pos();
        shape.setCoords(topLeft.x(), topLeft.y(), rightBottom.x(), pos.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing6) {
        auto pos = event->pos();
        shape.setCoords(pos.x(), topLeft.y(), rightBottom.x(), pos.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::sizing7) {
        auto pos = event->pos();
        shape.setCoords(pos.x(), topLeft.y(), rightBottom.x(), rightBottom.y());
        shape = shape.normalized();
    }
    else if (state == ShapeState::moving) {
        auto pos = event->pos();
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
    auto win = (WinBox*)parent();
    if (isEraser) {
        win->refreshBoard();
    }
    else {
        win->refreshCanvas(this, true);
    }
    event->accept();
}
