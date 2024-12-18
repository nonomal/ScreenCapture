﻿#pragma once
#include <QWidget>

#include "ToolBtn.h"

class StrokeCtrl;
class ColorCtrl;
class WinBox;
class ToolSub : public QWidget
{
	Q_OBJECT

public:
	ToolSub(QWidget *parent = nullptr);
	~ToolSub();
	static void InitData(const QJsonObject& obj, const QString& lang);
	bool getSelectState(const QString& btnName);
    QColor getColor();
    int getStrokeWidth();
public:
	WinBox* win;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void showEvent(QShowEvent* event) override;
private:
	static std::vector<ToolBtn> makeBtns(const QJsonArray& arr, const QString& lang);
private:
	qreal btnW{ 32 };
	StrokeCtrl* strokeCtrl;
	ColorCtrl* colorCtrl;
	int hoverIndex{ -1 };
	int triangleX{ 0 };
};
