#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QImage>

#include "../App/State.h"

class ToolMain;
class ToolSub;
class PixelInfo;
class WinBoard;
class WinCanvas;
class ShapeBase;
class WinBase  : public QWidget
{
	Q_OBJECT
public:
	WinBase(QWidget* parent = nullptr);
	virtual ~WinBase();
	virtual void showToolMain() {};
	virtual void showToolSub() {};
	virtual void closeWin() = 0;
	ShapeBase* addShape();
	void updateCursor(Qt::CursorShape cur);
	void refreshBoard();
	void refreshCanvas(ShapeBase* shape, bool force=false);
public:
	State state{ State::start };
	std::vector<ShapeBase*> shapes;
	int x, y, w, h;
	ToolMain* toolMain;
	ToolSub* toolSub;
	PixelInfo* pixelInfo;
	WinCanvas* winCanvas;
	WinBoard* winBoard;
	QImage img;
	qreal dpr{ 1.0 };
protected:
	void mousePressOnShape(QMouseEvent* event);
	void mouseMoveOnShape(QMouseEvent* event);
	void mouseDragOnShape(QMouseEvent* event);
	void mouseReleaseOnShape(QMouseEvent* event);
	void initWindow();
	void showEvent(QShowEvent* event) override;
private:
};
