#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <QObject>
#include <QSize>
#include "constants.h"

class ScreenManager : public QObject
{
    Q_OBJECT

public:
    ScreenManager(const QSize&, QObject* parent = nullptr);

    // Экранные размеры:
    Q_INVOKABLE int screenWidth() const { return screenSize.width(); }
    Q_INVOKABLE int screenHeight() const { return screenSize.height(); }
    Q_INVOKABLE int weightTitleFontSize() const { return (int)(DEFAULT_WEIGHT_TITLE_FONT_SIZE * screenScale); }
    Q_INVOKABLE int weightValueFontSize() const { return (int)(DEFAULT_WEIGHT_VALUE_FONT_SIZE * screenScale); }
    Q_INVOKABLE int largeFontSize() const { return (int)(DEFAULT_LARGE_FONT_SIZE * screenScale); }
    Q_INVOKABLE int normalFontSize() const { return (int)(DEFAULT_NORMAL_FONT_SIZE * screenScale); }
    Q_INVOKABLE int buttonSize() const { return (int)(DEFAULT_BUTTON_SIZE * screenScale); }
    Q_INVOKABLE int editWidth() const { return (int)(DEFAULT_EDIT_WIDTH * screenScale); }
    Q_INVOKABLE int scrollBarWidth() const { return (int)(DEFAULT_SPACER * screenScale) / 2; }
    Q_INVOKABLE int spacer() const { return (int)(DEFAULT_SPACER * screenScale); }

private:
    QSize screenSize;
    double screenScale = 0;

};

#endif // SCREENMANAGER_H