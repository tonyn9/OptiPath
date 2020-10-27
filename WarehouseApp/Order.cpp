/*****************************************************************//**
 * @file   Order.cpp
 * @brief  This source file contains the definitions for Order.h
 * 
 * @author Tony
 * @date   October 2020
 *********************************************************************/

#include "Order.h"

/**
 * @brief	This is a constructor for the Order object. This constructor
 *			requires 2 arguments
 * 
 * @param	oID		oID is the assigned orderID. oID should not be a 
 *					negative number
 * @param	p		p is the assigned priority. p's default is 1. p
 *					should be a value between 1 and 100 inclusive
 */
Order::Order(int oID, int p) {
	orderID = oID;
	if (p < 1) {
		priority = 1;
	}
	else if (p > 100) {
		priority = 100;
	}
	else {
		priority = p;
	}

	productList.clear();
}

/**
 * @brief	This function adds an already constructed Product object
 *			to the productList in an Order object. If the product's 
 *			position is not valid, ignore the order
 * 
 * @param	p		p is a Product object. It is expected for the 
 *					object to have valid position variables.
 */
void Order::addProduct(Product p) {
	if (p.getXPosition() == -1 || p.getYPosition() == -1) {
		return;
	}
	productList.push_back(p);
}

/**
 * @brief	Returns the Order object's productList.
 * 
 * @return	Returns the Order object's productList.
 */
std::list<Product> Order::getProductList(void) {
	return productList;
}

/**
 * @brief	This function changes the priority of an order.
 *			Unlike the constructor, this function accepts values
 *			over 100. However, values less than 1 is not accepted.
 * 
 * @param	p		p is an integer > 1.
 */
void Order::changePriority(int p) {
	if (p < 1) {
		priority = 1;
	}
	else {
		priority = p;
	}
	
}

/**
 * @brief	Get the Order object's priority.
 * 
 * @return	returns an integer indicating the object's priority.
 */
int Order::getPriority(void) {
	return priority;
}