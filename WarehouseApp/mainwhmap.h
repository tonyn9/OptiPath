/*******************************************************************
 * @file   mainwhmap.h
 * @brief  This header includes the mainwhmap class.
 * @date   October 2020
 *********************************************************************/
#ifndef mainwhmap_H
#define mainwhmap_H

#include <QMainWindow>
#include <QPainter>
#include <QVector>
#include <QPointF>
#include <QtWidgets>
#include "secondproductwindow.h"
#include "WarehouseMap.h"

QT_BEGIN_NAMESPACE
namespace Ui { class mainwhmap; }
QT_END_NAMESPACE

class mainwhmap : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainwhmap(QWidget *parent = 0);
    ~mainwhmap();

    void loadAllPoints(QVector <QPointF> ptsList);
    void loadProductPoints(QVector <QPointF> ptsList);
    void loadRoutePrinter(QVector <QPointF> route);
    void loadInstructions(QVector <std::string> instrList);

    virtual void paintEvent(QPaintEvent *event);

signals:
    void fromOtherMenu();
/*
public slots:
    void onOtherSignal(); */

private slots:
    void handleButton();

private:
    Ui::mainwhmap *ui;
    QVector <QPointF> allPoints;
    QVector <QPointF> productPoints;
    QVector <QPointF> routePoints;
    QVector <std::string> directions;
    QPushButton *p_button;
    QLineEdit *txtID;
    QLabel *txtLbl;
    secondProductWindow *secWindow;
    QVector <QPointF> singleRoute;
    QVBoxLayout *instrList;
    QScrollArea *instrArea;
    QWidget *dockArea;
};
#endif // mainwhmap_H
