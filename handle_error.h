#pragma once

#include <iostream>
#include <boost/python.hpp>
#include <string>

namespace py = boost::python;

//*** added some functions which help users to handle boost::python exceptions ***\\

namespace boost {
namespace python {

	// 1. std::string parse_python_exception();
	//    provides a way for getting information about the reason of the throwed exception without dealing with Python/C API

	std::string parse_python_exception();


	// 2. template <class return_type>
	//    return_type call_python_func(boost::function<return_type()> to_call, const std::string &error_pre);
	//	  Wraps boost::python function call to handle boost::python exceptions and throw std::exception instead to make users 
	//    not to deal with Python/C and boost::python exceptions

	template <class return_type>
	return_type call_python_function(std::function<return_type()> to_call, const std::string &error_pre){
		// Prefix the error string
		std::string error_str(error_pre);
		try {
			return to_call();
		}
		catch (boost::python::error_already_set const &){
			// Handle the exception and repackage as a C++ exception
			error_str = error_str + parse_python_exception();
			throw std::runtime_error(error_str);
		}
	}

} // end of namespace python
} // end of namespace boost
