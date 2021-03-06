/*******************************************************************
 * @file   mainwhmap.h
 * @brief  This header includes the mainwhmap class.
 * @date   October 2020
 *********************************************************************/
#ifndef mainwhmap_H
#define mainwhmap_H

#include <deque>
#include <QMainWindow>
#include <QPainter>
#include <QVector>
#include <QPointF>
#include <QtWidgets>
#include "Order.h"
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
    void loadUnconvertedPoints(QVector <QPointF> ptsList);
    void loadProductPoints(QVector <QPointF> ptsList);
    void loadUnconvertedProductPoints(QVector<QPointF> ptsList);
    void loadRoutePrinter(QVector <QPointF> route);
    void loadUnconvertedRoutePrinter(QVector <QPointF> route);
    void loadInstructions(QVector <std::string> instrList);
    void loadOrderStatus(Order::Status stat);
    void loadUnconvertedPointsPF(std::deque<std::tuple<float, float, std::string>> p);

    virtual void paintEvent(QPaintEvent *event);

signals:
    void fromOtherMenu();
    void COB();
/*
public slots:
    void onOtherSignal(); */

private slots:
    void handleButton();
    void handleCompleteOrderButton();

private:
    Ui::mainwhmap *ui;
    QVector <QPointF> allPoints;
    QVector <QPointF> productPoints;
    QVector <QPointF> routePoints;
    QVector <std::string> directions;
    QPushButton *p_button;
    QPushButton* completeOrder_button;
    QLineEdit *txtID;
    QLabel *txtLbl;
    secondProductWindow *secWindow;
    QVector <QPointF> singleRoute;
    QVBoxLayout *instrList;
    QScrollArea *instrArea;
    QWidget *dockArea;
    int width;
    int height;
    bool flag;
    Order::Status status;
    std::deque<std::tuple<float, float, std::string>> pointsPF;

    void createGrid(QPainter* painter);
    void drawContents(QPainter* painter);
    void drawInstructions(QPainter* painter);
    void drawOrderStatus(QPainter* painter);
    void drawLegend(QPainter* painter);
    void drawShelves(QPainter* painter);

    void drawRedProducts(QPainter* painter);
    void drawGreenProducts(QPainter* painter);
    void drawPFRoute(QPainter* painter);

    
};
#endif // mainwhmap_H
