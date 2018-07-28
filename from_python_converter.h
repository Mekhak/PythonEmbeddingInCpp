#pragma once

#include <boost/python.hpp>
#include <boost/any.hpp>
#include <iostream>

// *** convert boost::python object to user defined c++ object *** \\
//implemented generic struct. Now users only call
//							 From_python_converter<U, T>::register_converter(ConvertableTypes::type);
//instead of creating a converter struct and defining a bunch of functions for each user defined c++ type

namespace boost
{
	namespace python
	{
		enum class ConvertableTypes {
			BOOL = 0,
			FLOAT,
			LONG,
			STRING
		};

		template<typename T, typename U>
		struct From_python_converter
		{
			static void register_converter(const ConvertableTypes t) {
				type = t;
				boost::python::converter::registry::push_back(
					&convertible,
					&construct,
					boost::python::type_id<T>());
			}

			static void* convertible(PyObject* obj_ptr)
			{
				if (!check_type(obj_ptr)) {
					return 0;
				}
				return obj_ptr;
			}

			static void construct(
				PyObject* obj_ptr,
				boost::python::converter::rvalue_from_python_stage1_data* data)
			{
				U value = boost::any_cast<U>(get_value(obj_ptr));
				assert(value);
				void* storage = ((boost::python::converter::rvalue_from_python_storage<T>*)data)->storage.bytes;
				new (storage)T(value);
				data->convertible = storage;
			}

		private:

			static bool check_type(PyObject* obj_ptr) {
				switch (type) {
				case ConvertableTypes::BOOL:
					return PyBool_Check(obj_ptr);
				case ConvertableTypes::FLOAT:
					return PyFloat_Check(obj_ptr);
				case ConvertableTypes::LONG:
					return PyLong_Check(obj_ptr);
				case ConvertableTypes::STRING:
					return PyUnicode_Check(obj_ptr);
				}
				assert(false);
			}

			static boost::any get_value(PyObject* obj_ptr)
			{
				switch (type) {
				case ConvertableTypes::BOOL:
					return PyObject_IsTrue(obj_ptr) == 1;
				case ConvertableTypes::FLOAT:
					return PyFloat_AsDouble(obj_ptr);
				case ConvertableTypes::LONG:
					return PyLong_AsLong(obj_ptr);
				case ConvertableTypes::STRING:
					return std::string(PyBytes_AS_STRING(PyUnicode_AsEncodedString(obj_ptr, "UTF-8", "strict")));
				}
				assert(false);
			}

			static ConvertableTypes type;
		};
	}
}

