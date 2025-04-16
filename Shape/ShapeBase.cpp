#include "ShapeBase.h"
#include "../App/App.h"
#include "../Win/WinBox.h"
#include "../Win/WinCanvas.h"

ShapeBase::ShapeBase(QObject* parent):QObject(parent)
{
}

ShapeBase::~ShapeBase()
{
}

void ShapeBase::showDragger()
{
	auto win = (WinBox*)parent();
	if (state == ShapeState::ready && this == win->winCanvas->curShape) {
		//避免鼠标在ready的shape上移动的时候不断的重绘
		return;
	}
	if (state != ShapeState::ready) {
		state = ShapeState::ready;
	}
	win->winCanvas->curShape = this;
	win->winCanvas->paintDragger();
}

void ShapeBase::paintOnBoard()
{
	if (state != ShapeState::ready) {
		state = ShapeState::ready;
	}
}

void ShapeBase::painting()
{
	auto win = (WinBox*)parent();
	win->winCanvas->paintShape();
}

void ShapeBase::prepareDraggers(const int& size)
{
	if (draggers.empty()) {
		for (int i = 0; i < size; i++)
		{
			draggers.push_back(QRect());
		}
	}
}

void ShapeBase::paintingStart()
{
	//ready状态的shape，鼠标按下
	auto win = (WinBox*)parent();
	win->winCanvas->initImg();
	win->winCanvas->curShape = this;
	win->winCanvas->paintShape();
}

void ShapeBase::paintingPrepare()
{
	//temp状态的shape,鼠标按下
	auto win = (WinBox*)parent();
	win->winCanvas->initImg();
	win->winCanvas->curShape = this;
}
