/*********************************************************************
 * @file   PathFinder.cpp
 * @brief  This source file contains the definitions for PathFinder.h
 * 
 * @author Abinav
 * @date   October 2020
 *********************************************************************/

#include "PathFinder.h"


/**
 * @brief	Calculates distance between two products on the warehouse. 
            Takes two Products as arguments.
 * 
 * @param	product1		product1 is a product that will indicate the 
 *					        starting product
 * @param	product2		product1 is a product that will indicate the 
                            ending product
 * @return  Euclidean distance between products
 */
double PathFinder::distanceBetweenProductsEuclidean(Product& product1, Product& product2) {
    double distance = sqrt(pow(product1.getXPosition() - product2.getXPosition(),2) +
                        pow(product1.getYPosition() - product2.getYPosition(),2));
    //std::cout<<"Distance between "<<product1.getProductID()<<" and "<<product2.getProductID()<<" = "<<distance<<std::endl;
    return distance;
}

std::deque<std::tuple<float,float>> PathFinder::STraversal(
        std::deque<Product>& productList,
        Product& startLocation,
        Product& endLocation
        ) {
        int traversalOrder = 1; //1 -> left to right, 0-> right to left
        std::deque<std::tuple<float,float>> points;
        WarehouseMap* wMap = wMap->getInstance();
        currentPosition = startLocation.getPositionTuple();
        points.push_back(currentPosition);
        json shelves = wMap->getShelves();
        for(auto& shelf : shelves.items()) {
            for(auto& product : productList) {
                if(product.getYPosition() == std::stoi(shelf.key()))
                    aislesToBeVisited.push_back(std::stoi(shelf.key()) + 1); //aisle "1" can access shelf "0"
            }
        }

        std::cout<<"Aisles to be visited:"<<std::endl;
        std::sort(aislesToBeVisited.begin(),aislesToBeVisited.end());

        aislesToBeVisited.erase(std::unique(aislesToBeVisited.begin(), aislesToBeVisited.end()), aislesToBeVisited.end());
        for(auto& element : aislesToBeVisited) {
            std::cout<<element<<std::endl;
        }

        for(std::vector<int> :: iterator it = aislesToBeVisited.begin();it!=aislesToBeVisited.end()-1;++it) {
            int yCoord = *it;
            std::tuple<float,float> point;
            if(yCoord < 21) {
                if(traversalOrder == 1) {
                    points.push_back(std::make_tuple(shelves[std::to_string(yCoord-1)]["begin"],yCoord));
                    points.push_back(std::make_tuple(shelves[std::to_string(*(it+1)-1)]["end"],yCoord));
                    points.push_back(std::make_tuple(shelves[std::to_string(*(it+1)+1)]["end"],*(it+1)));
                    traversalOrder = 0;
                }
                else {
                    points.push_back(std::make_tuple(shelves[std::to_string(yCoord-1)]["end"],yCoord));
                    points.push_back(std::make_tuple(shelves[std::to_string(*(it+1)-1)]["begin"],yCoord));
                    points.push_back(std::make_tuple(shelves[std::to_string(*(it+1)+1)]["begin"],*(it+1)));
                    traversalOrder = 1;
                }
            }
        }
        points.push_back(std::make_tuple(20,*(aislesToBeVisited.end()-1)));
        points.push_back(endLocation.getPositionTuple());

        return points;
}

/**
 * @brief	Finds the most optimal path the user must take
 * 
 * @param   graph           Adjacency matrix of the products
 * @param   productList     List of products to be picked up
 * @param	startLocation   Take in the start location from the user
 * @param	endLocation		Takes in the end location from the user
 *
 * @return  Ordered list of products in the path
 */
std::deque<Product> PathFinder::calculatePath(
    std::unordered_map<Product*, std::deque<Product>> graph,
    std::deque<Product>& productList,
    Product& startLocation,
    Product& endLocation
    ) {
        long numItr = 0;
        std::vector<Product> vertices;
        for(auto& entry : productList) {
            if(entry.getProductID() != startLocation.getProductID())
                vertices.push_back(entry);
        }

        //Sort the vector to make sure no permutation is missed
        std::sort(vertices.begin(),vertices.end(),[](Product& a, Product& b) -> bool
        {
            return a.getProductID()< b.getProductID();
        });

        pathLength = (double)INT_MAX;

        //Go through all permutations and find the lowest path length
        do {
            double currentPathLength = 0;
            Product k = startLocation;
            std::deque<Product> currPath;
            currPath.push_back(startLocation);
            for(auto entry : vertices) {
                Product next = entry;
                currentPathLength += distanceBetweenProductsEuclidean(k,next);
                k = next; 
                currPath.push_back(next);
            }
            currentPathLength += distanceBetweenProductsEuclidean(k,endLocation);
            currPath.push_back(endLocation);
            if(currentPathLength < pathLength){
                path = currPath;
                pathLength = currentPathLength;
            }
            numItr++;
        } while(next_permutation(vertices.begin(),vertices.end(), [](Product& a, Product& b) -> bool 
        {
            return a.getProductID()< b.getProductID();
        }));
        //Print out number of paths evaluated
        std::cout<<"Number of paths evaluated = "<<numItr<<std::endl;
    return path;  
}

/**
 * @brief	To be used if only one product is to be picked
 * 
 * @param   product         Product to be picked up
 * @param	startLocation   Take in the start location from the user
 * @param	endLocation		Takes in the end location from the user
 *
 * @return  Ordered list of products in the path
 */

std::deque<Product> PathFinder::singleProductPath(
    Product& product, 
    Product& startLocation,
    Product& endLocation
    ) {
    //startLocation = std::make_tuple(0,0);
    path.push_back(startLocation);
    path.push_back(product);
    path.push_back(endLocation);
    return path;
}

/**
 * @brief	Display the path in textual human readable format along
 *          with total path length
 * 
 * @param   product         Product to be picked up
 * @param	startLocation   Take in the start location from the user
 * @param	endLocation		Takes in the end location from the user
 *
 * @return  Returns vector of points on the map
 */
QVector <QPointF> PathFinder::displayPath(void) { // was void return type
    QVector <QPointF> routeprinter;

    for(auto& vertex : path) {
        std::cout<<vertex.getProductID()<<"->";
        routeprinter.append(QPointF(vertex.getXPosition() * TILE_SIZE/SCALE, vertex.getYPosition() * TILE_SIZE/SCALE));
    }
    std::cout<<std::endl;
    std::cout<<"Minimum path length = "<< pathLength<<std::endl;

    return routeprinter;
}

/**
 * @brief	Returns path for the user
 */

std::deque<Product> PathFinder::getPath(void) {
    return path;
}

/**
 * @brief	Stores instructions for user in a vector
 *
 * @param   path         Path output by the path finder
 *
 * @return  return instructions in user-readable format
 */

QVector <std::string> PathFinder::pathAnnotation(std::deque<Product>& path) {
    QVector <std::string> instructions;
    std::stringstream xStream, yStream;

    xStream << std::fixed << std::setprecision(2) << path[0].getXPosition();
    yStream << std::fixed << std::setprecision(2) << path[0].getYPosition();

    std::string instruction = "Start at start location present in (" + xStream.str()
                                    + ',' + yStream.str() + ")";

    xStream.str(""); yStream.str("");

    instructions.append(instruction);
    for(unsigned int i = 1;i < path.size()-1;++i) {
        xStream << std::fixed << std::setprecision(2) << path[i].getXPosition();
        yStream << std::fixed << std::setprecision(2) << path[i].getYPosition();

        instruction = "Go to product " + path[i].getProductID() + " at (" +
                xStream.str() +"," + yStream.str() + ")";

        xStream.str(""); yStream.str("");

        instructions.append(instruction);
    }

    xStream << std::fixed << std::setprecision(2) << path[path.size()-1].getXPosition();
    yStream << std::fixed << std::setprecision(2) << path[path.size()-1].getYPosition();

    instruction = "Drop the products off at end location in (" + xStream.str()
            + ',' + yStream.str() + ")";

    xStream.str(""); yStream.str("");

    instructions.append(instruction);
    return instructions;
}

std::tuple<float,float> PathFinder::getCurrentPosition(void) {
    return currentPosition;
}

void PathFinder::setCurrentPosition(std::tuple<float,float>& pos) {
    currentPosition = pos;
}

int PathFinder::findMaxEnd(int aisle, int shelf) {
    WarehouseMap* wMap = wMap->getInstance();
    json shelves = wMap->getShelves();
    int max = 0;
    for(int i = aisle; i<=shelf; ++i) {
        std::string aisle = std::to_string(i);
        int endCoord = shelves[aisle]["end"];
        max = max > endCoord ? max : endCoord;
    }
    return max;
}

int PathFinder::findMinBegin(int aisle, int shelf) {
    WarehouseMap* wMap = wMap->getInstance();
    json shelves = wMap->getShelves();
    int min = INT_MAX;
    for(int i = aisle; i<=shelf; ++i) {
        std::string aisle = std::to_string(i);
        int endCoord = shelves[aisle]["begin"];
        min = min < endCoord ? min : endCoord;
    }
    return min;
}
