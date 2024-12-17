#pragma once

#include <QObject>
#include <QMap>
#include <QString>
#include <QColor>
#include <QFont>
#include <QVariant>

namespace Core {

/**
 * @brief 主题管理器
 * 提供以下功能：
 * 1. 主题切换
 * 2. 主题自定义
 * 3. 动态样式更新
 * 4. 主题导入导出
 */
class ThemeManager : public QObject
{
    Q_OBJECT
public:
    static ThemeManager* instance();

    /**
     * @brief 初始化主题管理器
     * @param themesPath 主题文件目录
     */
    bool initialize(const QString& themesPath);

    /**
     * @brief 获取当前主题名称
     */
    QString currentTheme() const;

    /**
     * @brief 设置当前主题
     * @param themeName 主题名称
     */
    bool setTheme(const QString& themeName);

    /**
     * @brief 获取可用主题列表
     */
    QStringList availableThemes() const;

    /**
     * @brief 获取主题颜色
     * @param colorRole 颜色角色
     */
    QColor getColor(const QString& colorRole) const;

    /**
     * @brief 获取主题字体
     * @param fontRole 字体角色
     */
    QFont getFont(const QString& fontRole) const;

    /**
     * @brief 获取主题样式表
     * @param widgetType 控件类型
     */
    QString getStyleSheet(const QString& widgetType) const;

    /**
     * @brief 导出主题
     * @param themeName 主题名称
     * @param filePath 导出文件路径
     */
    bool exportTheme(const QString& themeName, const QString& filePath) const;

    /**
     * @brief 导入主题
     * @param filePath 主题文件路径
     */
    bool importTheme(const QString& filePath);

    /**
     * @brief 创建自定义主题
     * @param themeName 主题名称
     * @param baseTheme 基础主题名称
     */
    bool createCustomTheme(const QString& themeName, const QString& baseTheme);

signals:
    void themeChanged(const QString& themeName);
    void colorChanged(const QString& colorRole, const QColor& color);
    void fontChanged(const QString& fontRole, const QFont& font);
    void styleSheetChanged(const QString& widgetType, const QString& styleSheet);

private:
    ThemeManager();
    ~ThemeManager() override;

    static ThemeManager* instance_;
    QString themesPath_;
    QString currentThemeName_;
    
    struct ThemeData {
        QMap<QString, QColor> colors;
        QMap<QString, QFont> fonts;
        QMap<QString, QString> styleSheets;
    };
    
    QMap<QString, ThemeData> themes_;
    
    bool loadTheme(const QString& themeName);
    bool saveTheme(const QString& themeName) const;
    QString getThemeFilePath(const QString& themeName) const;
};

} // namespace Core
