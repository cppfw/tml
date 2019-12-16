/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once



#include <utki/exception.hpp>



namespace puu{



/**
 * @brief Basic exception class.
 */
class exception : public utki::exception{
public:
	/**
	 * @brief Exception object constructor.
     * @param message - human readable message describing the error.
     */
	exception(const std::string& message = std::string()) :
			utki::exception(message)
	{}
};



}//~namespace
