#pragma once

#include <QObject>
#include <QWidget>
#include <QMouseEvent>

class ShapeText;
class ShapeTextInput;
class ShapeTextContainer  : public QWidget
{
	Q_OBJECT

public:
	ShapeTextContainer(ShapeText* shapeText, QWidget* parent = nullptr);
	~ShapeTextContainer();
public:
	//bool creating{ true };
	bool painting{ false };
	ShapeTextInput* shapeTextInput;
	bool isPress{ false };
	QRect ctrlRect;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
private:
	void adjustSize();
	void setNativeRect();
private:
	ShapeText* shapeText;
	QPointF pressPos;
};
