/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once



#include <utki/Exc.hpp>



namespace stob{



/**
 * @brief Basic exception class.
 */
class Exc : public utki::Exc{
public:
	/**
	 * @brief Exception object constructor.
     * @param message - human readable message describing the error.
     */
	Exc(const std::string& message) :
			utki::Exc(message)
	{}
};



}//~namespace
