﻿#pragma once

#include <QWidget>
#include <QPainter>

class BtnCheck;
class Btn;
class ToolBase : public QWidget
{
	Q_OBJECT
public:
	ToolBase(QWidget *parent = nullptr);
	virtual ~ToolBase();
	virtual void btnCheckChange(BtnCheck* btn);
	virtual void btnClick(Btn* btn);
public:
	int selectIndex{ -1 };
protected:
	void initWindow();
	void showEvent(QShowEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
protected:
	int hoverIndex{ -1 };
	qreal btnW{ 32 };
	qreal border{ 0.8 };
private:
};
