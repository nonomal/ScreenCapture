#pragma once
#include "WinBase.h"


class CutMask;
class WinLongTip;
class WinLong : public WinBase
{
	Q_OBJECT

public:
	WinLong(QWidget *parent = nullptr);
	~WinLong();
	CutMask* cutMask;
	QImage imgBg;
	WinLongTip* winLongTip;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
	QImage getTargetImg() override;
private:
	void initWindow();
	void joinImg(const QImage& img1, const QImage& img2);
private:
	QImage resultImg;
};
