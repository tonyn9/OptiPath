#include "secondproductwindow.h"
#include "ui_secondproductwindow.h"
#include <QPointF>
#include <QVector>
#include "Inventory.h"
#include <iostream>
#include <QDebug>
#include <QString>
#include <string>
#include "Database.h"

static const int TILE_SIZE = 30;
const int INKSCALE = 5;
const int MAPSCALE = 5;


secondProductWindow::secondProductWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::secondProductWindow)
{
    returnButton = new QPushButton("Close", this);
    returnButton->setGeometry(200,800,50,25);
    connect(returnButton, SIGNAL (clicked()), this, SLOT (handleButton()));
}

secondProductWindow::~secondProductWindow()
{
    delete ui;
}

void secondProductWindow::loadProductPoint(std::string pID) {
    productPoint.setX(0);
    productPoint.setY(0);
    myPID = pID;
    Database *d = d->getInstance();
    std::tuple<float, float> t = d->getProductPosition(myPID);
    float x = std::get<0>(t);
    float y = std::get<1>(t);

    if (x < 0 || y < 0) {
        close();
    }

    productPoint.setX(x * TILE_SIZE/MAPSCALE);
    productPoint.setY(y * TILE_SIZE/MAPSCALE);
    std::cout << "loaded product" << std::endl;
}

void secondProductWindow::loadRoutePrinter(QVector<QPointF> route) {
    routePoints.clear();
    QPointF temp = route.back();
    routePoints = route;
    routePoints.pop_back();
    routePoints.push_back(productPoint);
    routePoints.push_back(temp);
    std::cout << routePoints.size() << std::endl;
    std::cout << "loaded route" << std::endl;
}

void secondProductWindow::loadInstructions() {
    directions.clear();
    for (int i = 0; i < routePoints.size() - 1; i++) {
        int leftOrRight = 1; // 1 = right, -1 = left
        int upOrDown = 1; // 1 = down, -1 = up
        std::string updown = "down";
        std::string leftright = "right";

        float dx = (routePoints[i+1].x() - routePoints[i].x()) * MAPSCALE / TILE_SIZE;
        float dy = (routePoints[i+1].y() - routePoints[i].y()) * MAPSCALE / TILE_SIZE;

        if (dx < 0) {
            leftOrRight = -1;
            leftright = "left";
        }

        if (dy < 0) {
            upOrDown = -1;
            updown = "up";
        }

        std::string temp = "Go " + std::to_string(dx) + " " + leftright + ", then go " + std::to_string(dy) + " " + updown + ".\n";
        directions.push_back(temp);
    }

    std::cout << "loaded instructions" << std::endl;
}

void secondProductWindow::handleButton() {
    close();
}

void secondProductWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.eraseRect(0,0,1500,1000);
    painter.setBrush(Qt::DiagCrossPattern);
    QPen pen;

    int xboundary = TILE_SIZE * 40; // max 40 length
    int yboundary = TILE_SIZE * 22;
    int legendX = xboundary + 100; // x = 1300, location for legend

    //pen.setColor(Qt::green);
    pen.setWidth(5);

    // create and label grid begin. scaling coordinates by 30. Max product coordinate is 37.98 (*30 to scale)
    for (int i = 0; i < xboundary+1; i += TILE_SIZE) {     // make a 40x22 map with 30 pixels per square
        painter.drawLine(i, 0, i, yboundary);
    }

    for (int j = 0; j < yboundary+1; j += TILE_SIZE) {
        painter.drawLine(0, j, xboundary, j);
    }

    for (int i = 0; i < xboundary-1; i += TILE_SIZE*5) { // label the 40x22 map
        painter.drawText(i, yboundary+9, QString::number(i / TILE_SIZE));
    }

    painter.drawText(xboundary - 5, yboundary+9, QString::number(xboundary / TILE_SIZE)); // x = 40

    for (int j = 0; j < yboundary+1; j += TILE_SIZE*5) {
        painter.drawText(xboundary+3, j+4, QString::number(j / TILE_SIZE));
    }
    // create and label grid end

    // write instructions begin
    painter.drawText(xboundary/2 + 40, yboundary + 40, "Instructions");
    painter.drawText(xboundary/2 - 25, yboundary + 45, "--------------------------------------------------");
    for (int i = 0; i < directions.size(); i++) {
        std::string toBeNumbered = std::to_string(i+1) + ". " + directions[i];
        QString temp = QString::fromStdString(toBeNumbered);
        painter.drawText(xboundary/2 - 25, yboundary + 60 + i*20, temp);
    }
    // write instructions end

    // create legend begin
    painter.drawText(legendX + 40, 30, "LEGEND");
    painter.drawText(legendX - 25, 35, "-------------------------------------------");
    //painter.drawText(legendX + 25, 60, "Unselected Product");
    painter.drawText(legendX + 25, 60, "Selected Product");
    painter.drawText(legendX + 25, 90, "Route, numbered from");
    painter.drawText(legendX + 25, 100, "start->1->...->end");
    painter.drawText(legendX + 25, 120, "Start and End points");

    //painter.setPen(QPen(Qt::red, 5/(INKSCALE), Qt::SolidLine, Qt::RoundCap));
    painter.scale(MAPSCALE, MAPSCALE);
    //painter.drawPoint(QPointF((legendX + 15) / MAPSCALE, 60 / MAPSCALE));

    painter.setPen(QPen(Qt::green, 5/(INKSCALE), Qt::SolidLine, Qt::RoundCap));
    painter.drawPoint(QPointF((legendX + 15) / MAPSCALE, 60 / MAPSCALE));

    painter.setPen(QPen(Qt::blue, 5/(INKSCALE), Qt::SolidLine, Qt::RoundCap));
    painter.drawLine((legendX + 5) / MAPSCALE, 90 / MAPSCALE, (legendX + 15) / MAPSCALE, 90 / MAPSCALE);

    painter.setPen(QPen(Qt::cyan, 5/(INKSCALE), Qt::SolidLine, Qt::RoundCap));
    painter.drawPoint(QPointF((legendX + 15) / MAPSCALE, 120 / MAPSCALE));
    // create legend end

    // draw map contents begin
    painter.setPen(QPen(Qt::red, 5/(INKSCALE), Qt::SolidLine, Qt::RoundCap));

    /*for (auto& it : allPoints) { // draw all points
        painter.drawPoint(it);
    }*/

    painter.setPen(QPen(Qt::green, 10/(INKSCALE), Qt::SolidLine, Qt::RoundCap));
    painter.drawPoint(productPoint);

    painter.setPen(QPen(Qt::cyan, 10/(INKSCALE), Qt::SolidLine, Qt::RoundCap));
    painter.drawPoint(routePoints[0].x(), routePoints[0].y());
    painter.drawPoint(routePoints[routePoints.size() - 1].x(), routePoints[routePoints.size() - 1].y());

    painter.setPen(QPen(Qt::cyan, 1/(INKSCALE), Qt::SolidLine, Qt::RoundCap));
    painter.setFont(QFont("times",2));
    painter.drawText(routePoints[0].x()+2, routePoints[0].y()+4, "START");
    painter.drawText(routePoints[routePoints.size() - 1].x()+2, routePoints[routePoints.size() - 1].y()+4, "END");

    painter.setPen(QPen(Qt::blue, 1/(INKSCALE), Qt::SolidLine, Qt::RoundCap));
    for (int i = 1; i < routePoints.size() - 1; i++) {
        painter.drawLine(routePoints[i-1].x(), routePoints[i-1].y(), routePoints[i].x(), routePoints[i].y());
        painter.drawText(routePoints[i].x()+2, routePoints[i].y()+4, QString::number(i));
    }
    painter.drawLine(routePoints[routePoints.size() - 2].x(), routePoints[routePoints.size() - 2].y(), routePoints[routePoints.size() - 1].x(), routePoints[routePoints.size() - 1].y());
    // draw map contents end
}

