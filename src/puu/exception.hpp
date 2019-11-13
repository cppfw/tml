/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once



#include <utki/Exc.hpp>



namespace puu{



/**
 * @brief Basic exception class.
 */
class exception : public utki::Exc{
public:
	/**
	 * @brief Exception object constructor.
     * @param message - human readable message describing the error.
     */
	exception(const std::string& message = std::string()) :
			utki::Exc(message)
	{}
};



}//~namespace
