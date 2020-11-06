/*********************************************************************
 * @file   PathFinder.cpp
 * @brief  This source file contains the definitions for PathFinder.h
 * 
 * @author Abinav
 * @date   October 2020
 *********************************************************************/

#include "PathFinder.h"
#include <chrono>



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

std::tuple<double, int, int> distanceBetweenProductsTaxicab(Product& product1, Product& product2) { // returns tuple (distance, left/right, up/down), where -1 = left or down
    double deltaX = product1.getXPosition() - product2.getXPosition();
    double deltaY = product1.getYPosition() - product2.getYPosition();
    double distance = abs(deltaX) + abs(deltaY);

    std::tuple<double, int, int> taxiDist;

    int leftright = 1, updown = 1;

    if (deltaX < 0) {
        leftright = -1;
    }

    if (deltaY < 0) {
        updown = -1;
    }

    taxiDist = std::make_tuple(distance,leftright,updown);

    return taxiDist;
}

/**
 * @brief	Calculates the maximum distance to be traveled towards right
 *
 * @param	shelfStart		Indicates the starting shelf number
 * @param	shelfEnd		Indicates the ending shelf number
 *
 * @return  Maxmimum X coordinate to be traveled aling the aisle
 */

int PathFinder::findMaxEnd(int shelfStart, int shelfEnd) {
    WarehouseMap* wMap = wMap->getInstance();
    json shelves = wMap->getShelves();
    int max = 0;
    for(int i = shelfStart; i<=shelfEnd; i+=2) {
        std::string aisleCount = std::to_string(i);
        int endCoord = static_cast<int>(shelves[aisleCount]["end"]) + 1;
        max = max > std::ceil(endCoord) ? max : std::ceil(endCoord);
    }
    return max;
}

/**
 * @brief	Calculates the maximum distance to be traveled towards left
 *
 * @param	shelfStart		Indicates the starting shelf number
 * @param	shelfEnd		Indicates the ending shelf number
 *
 * @return  Minimum X coordinate to be traveled aling the aisle
 */

int PathFinder::findMinBegin(int shelfStart, int shelfEnd) {
    WarehouseMap* wMap = wMap->getInstance();
    json shelves = wMap->getShelves();
    int min = INT_MAX;
    for(int i = shelfStart; i<=shelfEnd; i+=2) {
        std::string aisleCount = std::to_string(i);
        int beginCoord = static_cast<int>(shelves[aisleCount]["begin"]) - 1;
        min = min < std::floor(beginCoord) ? min : std::floor(beginCoord);
    }
    return min;
}

/**
 * @brief	Navigates along the warehouse aisles in an S shape
 *
 * @param	productList		List of products to be picked up
 * @param	startLocation   Start location deaulted to (0,1)
 * @param   endLocation     End location defaulted to (0,1)
 *
 * @return  Ordered set of points to be visited
 */

QVector<QPointF> PathFinder::STraversal(
        std::deque<Product>& productList,
        Product& startLocation,
        Product& endLocation
        ) {

        std::cout << "Pathfinder begin" << std::endl;
        int traversalOrder = 1; //1 -> left to right, 0-> right to left
        QVector<QPointF> pointsToDisplay;
        std::unordered_map<int, std::vector<Product>> aisleProductMap;
        auto start = std::chrono::high_resolution_clock::now();

        WarehouseMap* wMap = wMap->getInstance();
        currentPosition = startLocation.getPositionTuple();

        std::cout << "starting current: " << std::get<0>(currentPosition) << " " << std::get<1>(currentPosition) << std::endl;

        points.push_back(std::make_tuple(std::get<0>(currentPosition),std::get<1>(currentPosition),"-1"));
        json shelves = wMap->getShelves();

        //Add aisles to be visited
        for(auto& shelf : shelves.items()) {
            for(auto& product : productList) {
                if(product.getYPosition() == std::stoi(shelf.key())) {
                    aislesToBeVisited.push_back(std::stoi(shelf.key()) + 1); //aisle "1" can access shelf "0"
                    aisleProductMap[product.getYPosition()].push_back(product); // Add product to aisle key
                }
            }
        }

        std::cout<<"Aisles to be visited:"<<std::endl;
        std::sort(aislesToBeVisited.begin(),aislesToBeVisited.end());

        aislesToBeVisited.erase(std::unique(aislesToBeVisited.begin(), aislesToBeVisited.end()), aislesToBeVisited.end());
        for(auto& element : aislesToBeVisited) {
            std::cout<<element<<std::endl;
        }
        //aislesToBeVisited is an ordered vector of aisles. (1,3,5,7,9.....)
        for(std::vector<int> :: iterator it = aislesToBeVisited.begin();it!=aislesToBeVisited.end()-1;++it) {
            int yCoord = *it;
            std::tuple<float,float> point;
            /*
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
            */

            if(yCoord < 21) { //Make sure y coordinate does not exceed 19
                if(traversalOrder == 1) { // left to right

                    //Find the right most coordinate
                    int nextEnd = findMaxEnd(yCoord+1,*(it+1)-1);

                    //Add the Left most coordinate in the aisle
                    points.push_back(std::make_tuple(shelves[std::to_string(yCoord-1)]["begin"],yCoord,"-1"));

                    //Sort based on earliest product encountered in traversal
                    std::sort(aisleProductMap[yCoord-1].begin(),aisleProductMap[yCoord-1].end(),[](Product a, Product b) {
                        return a.getXPosition() < b.getXPosition();
                    });

                    //Add the product locations in the traversal
                    for(auto& product : aisleProductMap[yCoord-1]) {
                        points.push_back(std::make_tuple(product.getXPosition(),product.getYPosition(),product.getProductID()));
                    }

                    //Push the rightmost coordinate in the aisle
                    points.push_back(std::make_tuple(nextEnd,yCoord,"-1"));

                    //Travel to the next aisle
                    points.push_back(std::make_tuple(nextEnd,*(it+1),"-1"));
                    traversalOrder = 0;
                }
                else {//right to left

                    //Find the left most coordinate
                    int nextBegin = findMinBegin(yCoord+1,*(it+1)-1);

                    //Add the Right most coordinate in the aisle
                    points.push_back(std::make_tuple(shelves[std::to_string(yCoord-1)]["end"],yCoord,"-1"));

                    //Sort based on earliest product encountered in traversal
                    std::sort(aisleProductMap[yCoord-1].begin(),aisleProductMap[yCoord-1].end(),[](Product a, Product b) {
                        return a.getXPosition() > b.getXPosition();
                    });

                    //Add the product locations in the traversal
                    for(auto& product : aisleProductMap[yCoord-1]) {
                        points.push_back(std::make_tuple(product.getXPosition(),product.getYPosition(),product.getProductID()));
                    }

                    //Push the leftmost coordinate in the aisle
                    points.push_back(std::make_tuple(nextBegin,yCoord,"-1"));

                    //Travel to the next aisle
                    points.push_back(std::make_tuple(nextBegin,*(it+1),"-1"));
                    traversalOrder = 1;
                }
            }
        }

       for(auto& product : aisleProductMap[aislesToBeVisited.back()-1]) {
            points.push_back(std::make_tuple(product.getXPosition(),product.getYPosition(),product.getProductID()));
       }
        points.push_back(std::make_tuple(0,*(aislesToBeVisited.end()-1),"-1"));
        points.push_back(std::make_tuple(endLocation.getXPosition(), endLocation.getYPosition(), "-1"));

        for(auto& point : points) {
            pointsToDisplay.push_back(QPointF(std::get<0>(point) * TILE_SIZE/SCALE,std::get<1>(point)  * TILE_SIZE/SCALE));
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout<<"Total time taken for execution of baseline algorithm = "<<duration.count()<<" milliseconds"<<std::endl;
        return pointsToDisplay;
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

QVector <std::string> PathFinder::pathAnnotation() {
    QVector <std::string> instructions;
    std::stringstream xStream, yStream;

    xStream << std::fixed << std::setprecision(2) << std::get<0>(points[0]);
    yStream << std::fixed << std::setprecision(2) << std::get<1>(points[0]);

    std::string instruction = "Start at start location present in (" + xStream.str()
                                    + ',' + yStream.str() + ")";

    xStream.str(""); yStream.str("");

    instructions.append(instruction);
    for(unsigned int i = 1;i < points.size()-1;++i) {
        xStream << std::fixed << std::setprecision(2) << std::get<0>(points[i]);
        yStream << std::fixed << std::setprecision(2) << std::get<1>(points[i]);

        if(std::get<2>(points[i]) != "-1") {
            instruction = "Go to product " + std::get<2>(points[i]) + " at (" +
                xStream.str() +"," + yStream.str() + ")";
        }

        else {
            instruction = "Go to point (" +
                xStream.str() +"," + yStream.str() + ")";
        }
        xStream.str(""); yStream.str("");

        instructions.append(instruction);
    }

    xStream << std::fixed << std::setprecision(2) << std::get<0>(points[points.size() - 1]);
    yStream << std::fixed << std::setprecision(2) << std::get<1>(points[points.size() - 1]);

    instruction = "Drop the products off at end location in (" + xStream.str()
            + ',' + yStream.str() + ")";

    xStream.str(""); yStream.str("");
    instructions.append(instruction);
    return instructions;
}

/**
 * @brief	Stores instructions for user in a vector
 *
 * @param   path         Path output by the path finder
 *
 * @return  return instructions in user-readable format
 */

QVector <std::string> PathFinder::oldAnnotation(std::deque<Product>& path) {
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

/**
 * @brief	Return current position of the user. (Unused currently)
 *
 * @return  Tuple of coordinates
 */

std::tuple<float,float> PathFinder::getCurrentPosition(void) {
    return currentPosition;
}

/**
 * @brief	Set current position of the user. (Unused currently)
 *
 * @params  pos Tuple of coordinates
 */

void PathFinder::setCurrentPosition(std::tuple<float,float>& pos) {
    currentPosition = pos;
}

