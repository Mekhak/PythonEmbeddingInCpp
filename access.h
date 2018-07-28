#pragma once

#include <boost/python.hpp>

//access python global and member variables easily
namespace boost
{
	namespace python
	{
		template <typename return_type>
		return_type get_global_variable(py::object module, const char* attribute_name) {
			py::object obj = module.attr(attribute_name);
			return py::extract<return_type>(obj);
		}

		template <typename return_type>
		return_type get_member_variable(py::object module, const char* class_name, const char* attribute_name) {
			py::object c = module.attr(class_name);
			py::object obj = c.attr(attribute_name);
			return py::extract<return_type>(obj);
		}
	}
}


