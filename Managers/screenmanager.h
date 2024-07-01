#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <QObject>
#include <QSize>

#define DEFAULT_SCREEN_WIDTH 568
#define DEFAULT_SCREEN_HEIGHT 320
#define DEFAULT_SPACER 8
#define DEFAULT_WEIGHT_VALUE_FONT_SIZE 70
#define DEFAULT_WEIGHT_TITLE_FONT_SIZE 10
#define DEFAULT_NORMAL_FONT_SIZE 13
#define DEFAULT_LARGE_FONT_SIZE 18
#define DEFAULT_BUTTON_SIZE 40
#define DEFAULT_FLAG_SIZE 20
#define DEFAULT_EDIT_WIDTH 200
#define SHOWCASE_ROW_IMAGES 5

class ScreenManager : public QObject
{
    Q_OBJECT

public:
    ScreenManager(const QSize&, QObject* parent = nullptr);

    // Экранные размеры:
    Q_INVOKABLE int screenWidth() const { return screenSize.width(); }
    Q_INVOKABLE int screenHeight() const { return screenSize.height(); }
    Q_INVOKABLE int popupWidth() const { return screenSize.width() * 3 / 4; }
    Q_INVOKABLE int popupHeight() const { return qMin(screenSize.height() * 3 / 4, screenSize.width() / 2); }
    Q_INVOKABLE int weightTitleFontSize() const { return (int)(DEFAULT_WEIGHT_TITLE_FONT_SIZE * screenScale); }
    Q_INVOKABLE int weightValueFontSize() const { return (int)(DEFAULT_WEIGHT_VALUE_FONT_SIZE * screenScale); }
    Q_INVOKABLE int largeFontSize() const { return (int)(DEFAULT_LARGE_FONT_SIZE * screenScale); }
    Q_INVOKABLE int normalFontSize() const { return (int)(DEFAULT_NORMAL_FONT_SIZE * screenScale); }
    Q_INVOKABLE int buttonSize() const { return (int)(DEFAULT_BUTTON_SIZE * screenScale); }
    Q_INVOKABLE int editWidth() const { return (int)(DEFAULT_EDIT_WIDTH * screenScale); }
    Q_INVOKABLE int scrollBarWidth() const { return 0; /* (int)(DEFAULT_SPACER * screenScale) / 2; */ }
    Q_INVOKABLE int spacer() const { return (int)(DEFAULT_SPACER * screenScale); }
    Q_INVOKABLE int flagSize() const { return (int)(DEFAULT_FLAG_SIZE * screenScale); }
    Q_INVOKABLE int showcaseRowImages() const { return SHOWCASE_ROW_IMAGES; }
    Q_INVOKABLE int flickVelocity() const { return 300; }
    Q_INVOKABLE int keyboardHeight() const { return buttonSize() * 3; }

private:
    QSize screenSize;
    double screenScale = 0;

};

#endif // SCREENMANAGER_H
