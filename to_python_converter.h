#pragma once 

#include <iostream>
#include <boost/python.hpp>


namespace boost
{
	namespace python
	{
		// *** convert uset defined c++ type toboost::python object */
		//implemented generic struct. Now users only call
		//							 To_python_object<from_type, to_type, converter_func>()
		//instead of creating a converter struct and defining a converter function for each user defined c++ type

		template<typename from_type, typename to_type, to_type(*converter_func)(const from_type&)>
		struct To_python_object
		{
			static PyObject* convert(const from_type & obj)
			{
				return boost::python::incref(boost::python::object(converter_func(obj)).ptr());
			}
		};
	}
}

