#ifndef SETTINGSMENU_H
#define SETTINGSMENU_H

#include <QMainWindow>
#include <QPainter>
#include <QVector>
#include <QPointF>
#include <QtWidgets>

QT_BEGIN_NAMESPACE
namespace Ui { class settingsmenu; }
QT_END_NAMESPACE

class settingsmenu : public QMainWindow
{
    Q_OBJECT

public:
    settingsmenu(QWidget *parent = nullptr);
    ~settingsmenu();
    std::tuple<float, float> getSLocation();
    void setSLocation(std::tuple<float, float> startLocation);

    std::tuple<float, float> getELocation();
    void setELocation(std::tuple<float, float> endLocation);

    float getTimer();
    void setTimer(float timer);

    int getAlgoMode();
    void setAlgoMode(int algomode);

    bool getDevMode();
    void setDevMode(bool dev);

    void setOptions();


private slots:
    void handleSLocationButton();
    void handleELocationButton();
    void handleTimerButton();
    void handleReturnButton();
    void handleApplyButton();
    void handleDevMode();

signals:
    void fromOtherMenu();
    void fromSettingsMenu();

private:
    Ui::settingsmenu *ui;
    float myTimer = 60.0;
    std::tuple<float, float> mySLocation;
    std::tuple<float, float> myELocation;

    QPushButton *startLocationButton;
    QPushButton *endLocationButton;
    QPushButton *timerButton;
    QPushButton *applyButton;
    QPushButton *returnButton;

    QLabel *txtSettings;
    QFont *font;
    QLineEdit *txtSLoc;
    QLabel *txtLblSLoc;
    QLineEdit *txtELoc;
    QLabel *txtLblELoc;
    QLineEdit *txtTimer;
    QLabel *txtLblTimer;

    QComboBox *devcbox;
    QCheckBox *devcheck;
    int myAlgoMode = 0; // default: s-traversal
    bool isDev = false; // default: user mode

};
#endif // SETTINGSMENU_H
