#pragma once

#include <QObject>
#include <qtextedit.h>

class ShapeText;
class ShapeTextInput  : public QTextEdit
{
	Q_OBJECT

public:
	ShapeTextInput(QWidget* parent = nullptr);
	~ShapeTextInput();
	static ShapeTextInput* create(ShapeText* parent);
	void moveTo(const QPoint& pos);
signals:
	void focusOut();
	void focusIn();
public:
	QColor textInputCursorColor;
private:
	void focusOutEvent(QFocusEvent* event) override;
	void focusInEvent(QFocusEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
	void adjustSize();
private:
	bool showTextInputCursor{ true };
};
