#include "ThemeManager.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>

namespace Core {

ThemeManager* ThemeManager::instance_ = nullptr;

ThemeManager* ThemeManager::instance()
{
    if (!instance_) {
        instance_ = new ThemeManager();
    }
    return instance_;
}

ThemeManager::ThemeManager()
    : QObject(nullptr)
{
}

ThemeManager::~ThemeManager()
{
    // 保存当前主题
    if (!currentThemeName_.isEmpty()) {
        saveTheme(currentThemeName_);
    }
}

bool ThemeManager::initialize(const QString& themesPath)
{
    themesPath_ = themesPath;
    QDir dir(themesPath_);
    
    // 确保主题目录存在
    if (!dir.exists() && !dir.mkpath(".")) {
        return false;
    }

    // 加载所有主题
    QStringList filters;
    filters << "*.json";
    QStringList themeFiles = dir.entryList(filters, QDir::Files);
    
    for (const QString& file : themeFiles) {
        QString themeName = QFileInfo(file).baseName();
        loadTheme(themeName);
    }

    // 如果没有主题，创建默认主题
    if (themes_.isEmpty()) {
        ThemeData defaultTheme;
        // 设置一些默认值
        defaultTheme.colors["primary"] = QColor("#007ACC");
        defaultTheme.colors["background"] = QColor("#FFFFFF");
        defaultTheme.colors["text"] = QColor("#000000");
        
        defaultTheme.fonts["default"] = QApplication::font();
        defaultTheme.fonts["title"] = QFont("Arial", 12, QFont::Bold);
        
        defaultTheme.styleSheets["QPushButton"] = "QPushButton { background-color: #007ACC; color: white; border: none; padding: 5px; }";
        
        themes_["default"] = defaultTheme;
        saveTheme("default");
    }

    // 设置默认主题
    return setTheme(themes_.contains("default") ? "default" : themes_.keys().first());
}

QString ThemeManager::currentTheme() const
{
    return currentThemeName_;
}

bool ThemeManager::setTheme(const QString& themeName)
{
    if (!themes_.contains(themeName)) {
        return false;
    }

    currentThemeName_ = themeName;
    emit themeChanged(themeName);

    // 发送所有主题相关的信号
    const ThemeData& theme = themes_[themeName];
    for (auto it = theme.colors.begin(); it != theme.colors.end(); ++it) {
        emit colorChanged(it.key(), it.value());
    }
    for (auto it = theme.fonts.begin(); it != theme.fonts.end(); ++it) {
        emit fontChanged(it.key(), it.value());
    }
    for (auto it = theme.styleSheets.begin(); it != theme.styleSheets.end(); ++it) {
        emit styleSheetChanged(it.key(), it.value());
    }

    return true;
}

QStringList ThemeManager::availableThemes() const
{
    return themes_.keys();
}

QColor ThemeManager::getColor(const QString& colorRole) const
{
    if (themes_.contains(currentThemeName_)) {
        return themes_[currentThemeName_].colors.value(colorRole);
    }
    return QColor();
}

QFont ThemeManager::getFont(const QString& fontRole) const
{
    if (themes_.contains(currentThemeName_)) {
        return themes_[currentThemeName_].fonts.value(fontRole);
    }
    return QFont();
}

QString ThemeManager::getStyleSheet(const QString& widgetType) const
{
    if (themes_.contains(currentThemeName_)) {
        return themes_[currentThemeName_].styleSheets.value(widgetType);
    }
    return QString();
}

bool ThemeManager::exportTheme(const QString& themeName, const QString& filePath) const
{
    if (!themes_.contains(themeName)) {
        return false;
    }

    QJsonObject themeObj;
    const ThemeData& theme = themes_[themeName];

    // 导出颜色
    QJsonObject colors;
    for (auto it = theme.colors.begin(); it != theme.colors.end(); ++it) {
        colors[it.key()] = it.value().name();
    }
    themeObj["colors"] = colors;

    // 导出字体
    QJsonObject fonts;
    for (auto it = theme.fonts.begin(); it != theme.fonts.end(); ++it) {
        QJsonObject font;
        font["family"] = it.value().family();
        font["size"] = it.value().pointSize();
        font["weight"] = it.value().weight();
        font["italic"] = it.value().italic();
        fonts[it.key()] = font;
    }
    themeObj["fonts"] = fonts;

    // 导出样式表
    QJsonObject styleSheets;
    for (auto it = theme.styleSheets.begin(); it != theme.styleSheets.end(); ++it) {
        styleSheets[it.key()] = it.value();
    }
    themeObj["styleSheets"] = styleSheets;

    QJsonDocument doc(themeObj);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

bool ThemeManager::importTheme(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        return false;
    }

    QString themeName = QFileInfo(filePath).baseName();
    ThemeData theme;

    QJsonObject themeObj = doc.object();
    
    // 导入颜色
    QJsonObject colors = themeObj["colors"].toObject();
    for (auto it = colors.begin(); it != colors.end(); ++it) {
        theme.colors[it.key()] = QColor(it.value().toString());
    }

    // 导入字体
    QJsonObject fonts = themeObj["fonts"].toObject();
    for (auto it = fonts.begin(); it != fonts.end(); ++it) {
        QJsonObject fontObj = it.value().toObject();
        QFont font(fontObj["family"].toString(),
                  fontObj["size"].toInt(),
                  fontObj["weight"].toInt(),
                  fontObj["italic"].toBool());
        theme.fonts[it.key()] = font;
    }

    // 导入样式表
    QJsonObject styleSheets = themeObj["styleSheets"].toObject();
    for (auto it = styleSheets.begin(); it != styleSheets.end(); ++it) {
        theme.styleSheets[it.key()] = it.value().toString();
    }

    themes_[themeName] = theme;
    return saveTheme(themeName);
}

bool ThemeManager::createCustomTheme(const QString& themeName, const QString& baseTheme)
{
    if (themes_.contains(themeName) || !themes_.contains(baseTheme)) {
        return false;
    }

    themes_[themeName] = themes_[baseTheme];
    return saveTheme(themeName);
}

bool ThemeManager::loadTheme(const QString& themeName)
{
    QString filePath = getThemeFilePath(themeName);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        return false;
    }

    ThemeData theme;
    QJsonObject themeObj = doc.object();

    // 加载颜色
    QJsonObject colors = themeObj["colors"].toObject();
    for (auto it = colors.begin(); it != colors.end(); ++it) {
        theme.colors[it.key()] = QColor(it.value().toString());
    }

    // 加载字体
    QJsonObject fonts = themeObj["fonts"].toObject();
    for (auto it = fonts.begin(); it != fonts.end(); ++it) {
        QJsonObject fontObj = it.value().toObject();
        QFont font(fontObj["family"].toString(),
                  fontObj["size"].toInt(),
                  fontObj["weight"].toInt(),
                  fontObj["italic"].toBool());
        theme.fonts[it.key()] = font;
    }

    // 加载样式表
    QJsonObject styleSheets = themeObj["styleSheets"].toObject();
    for (auto it = styleSheets.begin(); it != styleSheets.end(); ++it) {
        theme.styleSheets[it.key()] = it.value().toString();
    }

    themes_[themeName] = theme;
    return true;
}

bool ThemeManager::saveTheme(const QString& themeName) const
{
    if (!themes_.contains(themeName)) {
        return false;
    }

    return exportTheme(themeName, getThemeFilePath(themeName));
}

QString ThemeManager::getThemeFilePath(const QString& themeName) const
{
    return QDir(themesPath_).filePath(themeName + ".json");
}

} // namespace Core
