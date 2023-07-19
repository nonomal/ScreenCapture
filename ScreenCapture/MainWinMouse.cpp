#include "MainWin.h"

void MainWin::leftBtnDown(const POINT& pos)
{
    isLeftBtnDown = true;
    mouseDownPos = pos;
    if (state != State::start) {
        if (mouseEnterMainToolIndex != -1 && mouseEnterMainToolIndex < 9) {
            selectedToolIndex = mouseEnterMainToolIndex;
            InvalidateRect(hwnd, nullptr, false);
            state = (State)(selectedToolIndex+2);
            return;
        }
        if (mouseEnterSubToolIndex != -1) {
            subToolBtnClick();
            return;
        }
        if (state == State::maskReady) {
            dragStartCutBoxStartPos = BLPoint(cutBox.x0, cutBox.y0);
            dragStartCutBoxEndPos = BLPoint(cutBox.x1, cutBox.y1);
            if (mouseInMaskBoxIndex < 8) {
                setCutBox(pos);
            }            
            return;
        }
    }
}
void MainWin::rightBtnDown(const POINT& pos)
{
    //canvasImage->writeToFile("123.png");
    CloseWindow(hwnd);
	PostQuitMessage(0);
}
void MainWin::mouseMove(const POINT& pos)
{
    if (state != State::start) {
        checkMouseEnterToolBox(pos);
        if (mouseEnterMainToolIndex != -1 || mouseEnterSubToolIndex != -1) return;
    }    
    if (isLeftBtnDown) {
        switch (state)
        {
            case State::start:
            {
                BLPoint startPos(pos.x, pos.y);
                BLPoint endPos(mouseDownPos.x, mouseDownPos.y);
                setCutBox(startPos, endPos);
                break;
            }
            case State::maskReady:
            {
                setCutBox(pos);
                break;
            }
            case State::rect:
                drawRect(pos);
                break;
            case State::arrow:
                break;
            case State::pen:
            {
                drawPen(pos);
                break;
            }
            case State::mosaic:
                break;
            case State::text:
                break;
            case State::eraser:
            {
                drawEraser(pos);
                break;
            }
            case State::number:
                break;
            case State::lastPathDrag:
                break;
            default:
                break;
        }
    }
    else if (state != State::start) {
        if (state == State::maskReady) {
            checkMouseEnterMaskBox(pos);
        }
        else
        {
            setCursor(IDC_CROSS);
        }
    }

}
void MainWin::leftBtnUp(const POINT& pos)
{
    if (!isLeftBtnDown) return;
    isLeftBtnDown = false;
    if (state == State::start) {
        state = State::maskReady;
        InvalidateRect(hwnd, nullptr, false);
    }
    else if (state == State::eraser) {
        
    }
}