﻿#include "MainWin.h"
#include "PinWin.h"


MainWin::MainWin()
{
    state = State::start;
    initWindowSize();
    shotScreen();
    enumDesktopWindow();    
    InitLayerImg();
    InitWindow();
    Show();
}
MainWin::~MainWin()
{

}

void MainWin::initWindowSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

void MainWin::shotScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    unsigned int stride = w * 4;
    unsigned int dataSize = stride * h;
    auto desktopPixelData = new unsigned char[dataSize];
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };
    GetDIBits(hDC, hBitmap, 0, h, desktopPixelData, &info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);


    auto bottomPixels = new unsigned char[dataSize];
    bottomHbitmap = CreateDIBSection(hScreen, &info, DIB_RGB_COLORS, reinterpret_cast<void**>(&bottomPixels), NULL, NULL);
    ReleaseDC(NULL, hScreen);

    OriginalImage = new BLImage();
    OriginalImage->createFromData(w, h, BL_FORMAT_PRGB32, desktopPixelData, stride, BL_DATA_ACCESS_RW, [](void* impl, void* externalData, void* userData) {
        delete[] externalData;
        });


    BottomImage = new BLImage();
    BottomImage->createFromData(w, h, BL_FORMAT_PRGB32, bottomPixels, stride, BL_DATA_ACCESS_RW, [](void* impl, void* externalData, void* userData) {
        delete[] externalData; //todo
    });
}

void MainWin::SaveFile(const std::string& filePath) {
    auto w = cutBox.x1 - cutBox.x0;
    auto h = cutBox.y1 - cutBox.y0;
    BLImage imgSave(w, h, BL_FORMAT_PRGB32);
    PaintCtx->begin(imgSave);
    PaintCtx->blitImage(BLPoint(0, 0), *OriginalImage, BLRectI((int)cutBox.x0, (int)cutBox.y0, (int)w, (int)h));
    PaintCtx->blitImage(BLPoint(0, 0), *CanvasImage, BLRectI((int)cutBox.x0, (int)cutBox.y0, (int)w, (int)h));
    PaintCtx->end();
    imgSave.writeToFile(filePath.c_str());
}

void MainWin::BeforePaint() {
    PaintCtx->begin(*BottomImage);
    PaintCtx->blitImage(BLRect(0, 0, w, h), *OriginalImage);
    if (IsMosaicUsePen) {
        PaintCtx->blitImage(BLRect(0, 0, w, h), *MosaicImage);
    }
    else
    {
        PaintCtx->blitImage(BLRect(0, 0, w, h), *CanvasImage);
    }
    if (IsDrawing) {
        PaintCtx->blitImage(BLRect(0, 0, w, h), *PrepareImage);
    }
    drawMaskBoxes();
    if (state != State::start) {
        setToolBoxPos();
        drawToolMain();
    }    
    if (!IsLeftBtnDown && state == State::start) {
        drawPixelInfo();
    }
    PaintCtx->end();
}

void MainWin::PinWindow() {   
    History::LastShapeDrawEnd();
    auto w = cutBox.x1 - cutBox.x0;
    auto h = cutBox.y1 - cutBox.y0;
    auto img = new BLImage(w, h, BL_FORMAT_PRGB32);
    PaintCtx->begin(*img);
    PaintCtx->clearAll();
    PaintCtx->blitImage(BLPoint(0,0), *OriginalImage, BLRectI((int)cutBox.x0, (int)cutBox.y0, (int)w, (int)h));
    PaintCtx->blitImage(BLPoint(0,0), *CanvasImage, BLRectI((int)cutBox.x0, (int)cutBox.y0, (int)w, (int)h));
    PaintCtx->blitImage(BLPoint(0, 0), *PrepareImage, BLRectI((int)cutBox.x0, (int)cutBox.y0, (int)w, (int)h));
    PaintCtx->end();
    History::Clear();
    new PinWin(cutBox.x0, cutBox.y0, img);    
    SendMessage(hwnd, WM_CLOSE, NULL, NULL);
}


void MainWin::setToolBoxPos()
{
    toolBoxMain.x0 = cutBox.x1 - toolBoxWidth;
    auto heightSpan = toolBoxSpan * 3 + toolBoxHeight * 2;
    if (int(h - cutBox.y1) > heightSpan)
    {
        //屏幕底部还有足够的空间
        //两个高度，为屏幕底边也留一点间隙 
        toolBoxMain.y0 = cutBox.y1 + toolBoxSpan;
    }
    else if (int(cutBox.y0) > heightSpan)
    {
        //屏幕顶部还有足够的空间
        if (SelectedToolIndex == -1)
        {
            //尚未确定state，主工具条贴着截图区
            toolBoxMain.y0 = cutBox.y0 - toolBoxSpan - toolBoxHeight;
        }
        else
        {
            //已经确定了state，要为子工具条留出区域
            toolBoxMain.y0 = cutBox.y0 - (double)toolBoxSpan * 2 - (double)toolBoxHeight * 2;
        }
    }
    else
    {
        //顶部底部都没有足够的空间
        if (SelectedToolIndex == -1)
        {
            //尚未确定state，主工具条贴着截图区底部上方
            toolBoxMain.y0 = cutBox.y1 - toolBoxSpan - toolBoxHeight;
        }
        else
        {
            //尚未确定state，主工具条贴着截图区底部上方，并为子工具条留出空间
            toolBoxMain.y0 = cutBox.y1 - (double)toolBoxSpan * 2 - (double)toolBoxHeight * 2;
        }
    }
    if (toolBoxMain.x0 < 0) {
        //如果工具条超出了屏幕左侧，那么让工具条与截图区域左侧对齐
        //todo 如果工具条在右侧屏幕的左边缘，并超出了右侧屏幕的左边缘
        toolBoxMain.x0 = cutBox.x0;
    }
    toolBoxMain.y1 = toolBoxMain.y0 + toolBoxHeight;
    toolBoxMain.x1 = toolBoxMain.x0 + toolBoxWidth;

}