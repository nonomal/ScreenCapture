#include "WindowBase.h"


void WindowBase::initLayerImg() {
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
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

    PaintCtx = new BLContext();
    DesktopImage = new BLImage();
    DesktopImage->createFromData(w, h, BL_FORMAT_PRGB32, desktopPixelData, stride, BL_DATA_ACCESS_RW, [](void* impl, void* externalData, void* userData) {
        delete[] externalData;
        });
    CanvasImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    PrepareImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    PaintCtx->begin(*PrepareImage);
    PaintCtx->clearAll();
    PaintCtx->end();
    PaintCtx->begin(*CanvasImage);
    PaintCtx->clearAll();
    PaintCtx->end();


    bottomImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    BLImageData imgData;
    bottomImage->getData(&imgData);
    pixelData = (unsigned char*)imgData.pixelData;
}

bool WindowBase::OnPaint()
{
    PaintCtx->begin(*bottomImage);
    PaintCtx->blitImage(BLRect(0, 0, w, h), *DesktopImage);
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
    DrawMaskBox();
    drawToolMain();
    if (!IsLeftBtnDown && state == State::start) {
        drawPixelInfo();
    }
    PaintCtx->end();
    return true;
}

bool WindowBase::paint()
{
    auto result = OnPaint();
    if (!render) {
        D2D1_SIZE_U size = D2D1::SizeU(w, h);
        factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, size), &render);
        render->SetTransform(D2D1::Matrix3x2F::Identity());
        D2D1_BITMAP_PROPERTIES bmpPorp;
        bmpPorp.dpiX = 0;
        bmpPorp.dpiY = 0;
        bmpPorp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        bmpPorp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
        render->CreateBitmap(size, NULL, NULL, bmpPorp, &d2DImage);
    }
    static auto rect = D2D1::RectU(0, 0, w, h);
    static unsigned int stride = w * 4;
    static unsigned int tempW = w / scaleFactor;
    static unsigned int tempH = h / scaleFactor;
    static auto rect2 = D2D1::RectF(0, 0, tempW, tempH);
    d2DImage->CopyFromMemory(&rect, pixelData, stride);
    render->BeginDraw();
    render->DrawBitmap(d2DImage, rect2);
    auto drawResult = render->EndDraw();
    if (D2DERR_RECREATE_TARGET == drawResult)
    {
        d2DImage->Release();
        render->Release();
        render = nullptr;
    }
    ValidateRect(hwnd, NULL);
    return result;
}