#pragma once

#include <QWidget>

class Box;
class Magnifier : public QWidget
{
	Q_OBJECT

public:
	Magnifier(Box* box);
	~Magnifier();
	void mouseMove(QMouseEvent* event);
protected:
	void paintEvent(QPaintEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
private:
private:
	Box* box;
};
