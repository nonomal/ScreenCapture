#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFontDatabase>

#include "App.h"
#include "Tray.h"
#include "../QHotKey/qhotkey.h"
#include "../Win/WinFull.h"

#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../Tool/ColorCtrl.h"

namespace {
    std::unique_ptr<App> app;
    std::unique_ptr<QFont> iconFont;
    std::unique_ptr<QHotkey> hotkey;
    std::unique_ptr<Tray> tray;
}
void App::init()
{
    initConfig();
    start();
}
App* App::get()
{
    return app.get();
}
QFont* App::getIconFont()
{
    return iconFont.get();
}
void App::dispose()
{
    tray.reset();
    hotkey.reset();
    iconFont.reset();
    app.reset();
}
void App::start()
{
    WinFull::init();
}
void App::initConfig()
{
    QFile file("./config/config.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件:" << file.errorString();
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();
    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    if (!document.isObject()) {
        qWarning() << "JSON不是一个对象";
        return;
    }
    QJsonObject jsonObject = document.object();
    auto lang = jsonObject["defaultLang"].toString();
    initHotKey(jsonObject);
    initFont(jsonObject);
    initTool(jsonObject, lang);  
    initTray(jsonObject, lang);
}
void App::initFont(const QJsonObject& obj)
{
    if (obj["icon"].isObject()) {
        auto icon = obj["icon"].toObject();
        QString fontPath = icon["fontPath"].toString();
        auto result = QFontDatabase::addApplicationFont(fontPath);
        if (result != 0) {
            qWarning() << "没有找到字体图标文件";
        }
        auto fontName = icon["fontName"].toString();
        iconFont = std::make_unique<QFont>(fontName);
        iconFont->setStyleStrategy(QFont::PreferAntialias);
        iconFont->setHintingPreference(QFont::PreferNoHinting);
    }
}
void App::initHotKey(const QJsonObject& obj)
{
    if (obj["hotKey"].isString()) {
        auto keyComb = obj["hotKey"].toString();
        hotkey = std::make_unique<QHotkey>(QKeySequence(keyComb), true);
        QObject::connect(hotkey.get(), &QHotkey::activated, &App::start);
    }
}
void App::initTool(const QJsonObject& obj, const QString& lang)
{
    if (obj["toolMain"].isArray()) {
        ToolMain::InitData(obj["toolMain"].toArray(), lang);
    }
    if (obj["toolSub"].isObject()) {
        ToolSub::InitData(obj["toolSub"].toObject(), lang);
    }
    if (obj["colorCtrl"].isObject()) {
        ColorCtrl::InitData(obj["colorCtrl"].toObject(), lang);
    }
}
void App::initTray(const QJsonObject& obj, const QString& lang)
{
    if (obj["tray"].isObject()) {
        tray = std::make_unique<Tray>(obj["tray"].toObject(), lang);
    }
}
