#pragma once

#include <QMap>
#include <QPalette>
#include <QStringList>

class ThemeManager
{
public:
    static ThemeManager& sInstance();

    // Apply a registered theme by id and persist the choice.
    void mApplyTheme(const QString& xId);

    // Read the last persisted theme from QSettings and apply it.
    // Call once after QApplication is constructed.
    void mRestoreTheme();

    QStringList mThemeIds() const;
    QString mCurrentThemeId() const;

private:
    ThemeManager();

    void mRegisterTheme(const QString& xId, const QPalette& xPalette);
    static QString sBuildStyleSheet(const QString& xId);

    static QPalette sBuildLightPalette();
    static QPalette sBuildDarkPalette();
    static QPalette sBuildSolarizedDarkPalette();

    QMap<QString, QPalette> dThemes;
    QString dCurrentId;
};
