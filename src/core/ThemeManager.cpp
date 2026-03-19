#include "ThemeManager.h"

#include <QApplication>
#include <QSettings>
#include <QStyleFactory>

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------

ThemeManager& ThemeManager::sInstance()
{
    static ThemeManager sThemeManager;
    return sThemeManager;
}

ThemeManager::ThemeManager()
{
    mRegisterTheme(QStringLiteral("Light"),          sBuildLightPalette());
    mRegisterTheme(QStringLiteral("Dark"),           sBuildDarkPalette());
    mRegisterTheme(QStringLiteral("Solarized Dark"), sBuildSolarizedDarkPalette());

    dCurrentId = QStringLiteral("Light");
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void ThemeManager::mApplyTheme(const QString& xId)
{
    if (!dThemes.contains(xId)) {
        return;
    }

    dCurrentId = xId;

    if (qApp != nullptr) {
        qApp->setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
        qApp->setPalette(dThemes.value(xId));
        qApp->setStyleSheet(sBuildStyleSheet(xId));
    }

    QSettings lSettings;
    lSettings.setValue(QStringLiteral("theme/current"), xId);
}

void ThemeManager::mRestoreTheme()
{
    QSettings lSettings;
    const QString lSaved = lSettings.value(
        QStringLiteral("theme/current"),
        QStringLiteral("Light")).toString();

    mApplyTheme(dThemes.contains(lSaved) ? lSaved : QStringLiteral("Light"));
}

QStringList ThemeManager::mThemeIds() const
{
    return dThemes.keys();
}

QString ThemeManager::mCurrentThemeId() const
{
    return dCurrentId;
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

void ThemeManager::mRegisterTheme(const QString& xId, const QPalette& xPalette)
{
    dThemes.insert(xId, xPalette);
}

QString ThemeManager::sBuildStyleSheet(const QString& xId)
{
    if (xId == QStringLiteral("Dark")) {
        return QStringLiteral(
            "QTabWidget::pane { border: 1px solid #555555; background: #2D2D2D; }"
            "QTabBar::tab { background: #3D3D3D; color: #F0F0F0; padding: 6px 12px; border: 1px solid #555555; }"
            "QTabBar::tab:selected { background: #1E1E1E; color: #FFFFFF; }"
            "QTabBar::tab:!selected { margin-top: 2px; }"
        );
    }

    if (xId == QStringLiteral("Solarized Dark")) {
        return QStringLiteral(
            "QTabWidget::pane { border: 1px solid #586e75; background: #073642; }"
            "QTabBar::tab { background: #073642; color: #93a1a1; padding: 6px 12px; border: 1px solid #586e75; }"
            "QTabBar::tab:selected { background: #002b36; color: #fdf6e3; }"
            "QTabBar::tab:!selected { margin-top: 2px; }"
        );
    }

    return QString();
}

// ---------------------------------------------------------------------------
// Light palette  (clean, neutral, close to fusion light)
// ---------------------------------------------------------------------------
QPalette ThemeManager::sBuildLightPalette()
{
    QPalette lPal;

    lPal.setColor(QPalette::Window,          QColor(0xF0F0F0));
    lPal.setColor(QPalette::WindowText,      QColor(0x1A1A1A));
    lPal.setColor(QPalette::Base,            QColor(0xFFFFFF));
    lPal.setColor(QPalette::AlternateBase,   QColor(0xF5F5F5));
    lPal.setColor(QPalette::Text,            QColor(0x1A1A1A));
    lPal.setColor(QPalette::BrightText,      QColor(0xFF0000));
    lPal.setColor(QPalette::Button,          QColor(0xE0E0E0));
    lPal.setColor(QPalette::ButtonText,      QColor(0x1A1A1A));
    lPal.setColor(QPalette::Highlight,       QColor(0x0078D7));
    lPal.setColor(QPalette::HighlightedText, QColor(0xFFFFFF));
    lPal.setColor(QPalette::ToolTipBase,     QColor(0xFFFFC0));
    lPal.setColor(QPalette::ToolTipText,     QColor(0x1A1A1A));
    lPal.setColor(QPalette::Link,            QColor(0x0066CC));
    lPal.setColor(QPalette::LinkVisited,     QColor(0x551A8B));
    lPal.setColor(QPalette::Mid,             QColor(0xB0B0B0));
    lPal.setColor(QPalette::Midlight,        QColor(0xD8D8D8));
    lPal.setColor(QPalette::Dark,            QColor(0x707070));
    lPal.setColor(QPalette::Shadow,          QColor(0x505050));

    // Disabled state keeps colour but dims it
    lPal.setColor(QPalette::Disabled, QPalette::WindowText,  QColor(0xA0A0A0));
    lPal.setColor(QPalette::Disabled, QPalette::Text,        QColor(0xA0A0A0));
    lPal.setColor(QPalette::Disabled, QPalette::ButtonText,  QColor(0xA0A0A0));

    return lPal;
}

// ---------------------------------------------------------------------------
// Dark palette  (dark charcoal, blue highlight)
// ---------------------------------------------------------------------------
QPalette ThemeManager::sBuildDarkPalette()
{
    QPalette lPal;

    lPal.setColor(QPalette::Window,          QColor(0x2D2D2D));
    lPal.setColor(QPalette::WindowText,      QColor(0xF0F0F0));
    lPal.setColor(QPalette::Base,            QColor(0x1E1E1E));
    lPal.setColor(QPalette::AlternateBase,   QColor(0x252525));
    lPal.setColor(QPalette::Text,            QColor(0xF0F0F0));
    lPal.setColor(QPalette::BrightText,      QColor(0xFF5555));
    lPal.setColor(QPalette::Button,          QColor(0x3D3D3D));
    lPal.setColor(QPalette::ButtonText,      QColor(0xF0F0F0));
    lPal.setColor(QPalette::Highlight,       QColor(0x0078D7));
    lPal.setColor(QPalette::HighlightedText, QColor(0xFFFFFF));
    lPal.setColor(QPalette::ToolTipBase,     QColor(0x3D3D3D));
    lPal.setColor(QPalette::ToolTipText,     QColor(0xF0F0F0));
    lPal.setColor(QPalette::Link,            QColor(0x6EA6D8));
    lPal.setColor(QPalette::LinkVisited,     QColor(0xBB86FC));
    lPal.setColor(QPalette::Mid,             QColor(0x555555));
    lPal.setColor(QPalette::Midlight,        QColor(0x404040));
    lPal.setColor(QPalette::Dark,            QColor(0x252525));
    lPal.setColor(QPalette::Shadow,          QColor(0x1A1A1A));

    lPal.setColor(QPalette::Disabled, QPalette::WindowText,  QColor(0x707070));
    lPal.setColor(QPalette::Disabled, QPalette::Text,        QColor(0x707070));
    lPal.setColor(QPalette::Disabled, QPalette::ButtonText,  QColor(0x707070));

    return lPal;
}

// ---------------------------------------------------------------------------
// Solarized Dark palette  (Ethan Schoonover's Solarized colour scheme)
// base03:#002b36  base02:#073642  base01:#586e75  base0:#839496
// base1:#93a1a1   base3:#fdf6e3   blue:#268bd2    cyan:#2aa198
// ---------------------------------------------------------------------------
QPalette ThemeManager::sBuildSolarizedDarkPalette()
{
    // Named solarized colours
    const QColor lBase03  (0x00, 0x2B, 0x36);
    const QColor lBase02  (0x07, 0x36, 0x42);
    const QColor lBase01  (0x58, 0x6E, 0x75);
    const QColor lBase0   (0x83, 0x94, 0x96);
    const QColor lBase1   (0x93, 0xA1, 0xA1);
    const QColor lBase3   (0xFD, 0xF6, 0xE3);
    const QColor lBlue    (0x26, 0x8B, 0xD2);
    const QColor lRed     (0xDC, 0x32, 0x2F);
    const QColor lViolet  (0x6C, 0x71, 0xC4);
    const QColor lCyan    (0x2A, 0xA1, 0x98);

    QPalette lPal;

    lPal.setColor(QPalette::Window,          lBase02);
    lPal.setColor(QPalette::WindowText,      lBase0);
    lPal.setColor(QPalette::Base,            lBase03);
    lPal.setColor(QPalette::AlternateBase,   lBase02);
    lPal.setColor(QPalette::Text,            lBase0);
    lPal.setColor(QPalette::BrightText,      lBase1);
    lPal.setColor(QPalette::Button,          lBase02);
    lPal.setColor(QPalette::ButtonText,      lBase0);
    lPal.setColor(QPalette::Highlight,       lBlue);
    lPal.setColor(QPalette::HighlightedText, lBase3);
    lPal.setColor(QPalette::ToolTipBase,     lBase02);
    lPal.setColor(QPalette::ToolTipText,     lBase1);
    lPal.setColor(QPalette::Link,            lBlue);
    lPal.setColor(QPalette::LinkVisited,     lViolet);
    lPal.setColor(QPalette::Mid,             lBase01);
    lPal.setColor(QPalette::Midlight,        lBase02.lighter(120));
    lPal.setColor(QPalette::Dark,            lBase03.darker(120));
    lPal.setColor(QPalette::Shadow,          lBase03.darker(150));

    lPal.setColor(QPalette::Disabled, QPalette::WindowText,  lBase01);
    lPal.setColor(QPalette::Disabled, QPalette::Text,        lBase01);
    lPal.setColor(QPalette::Disabled, QPalette::ButtonText,  lBase01);

    return lPal;
}
