#pragma once

#include <boost/python.hpp>
#include <string>
#include <tuple>


//created wrapper functoins go get exection time of python code execution

namespace boost
{
	namespace python {

		std::tuple<py::object, double> _exec_file(py::str file_name, py::object globals = py::object(), py::object locals = py::object())
		{
			py::object time_module = py::import("time");
			double start_time = py::extract<double>(time_module.attr("time")());
			py::object res = py::exec_file(file_name, globals, locals);
			double end_time = py::extract<double>(time_module.attr("time")());
			return std::make_tuple(res, end_time - start_time);
		}

		std::tuple<py::object, double> _exec(py::str file_name, py::object globals = py::object(), py::object locals = py::object())
		{
			py::object time_module = py::import("time");
			double start_time = py::extract<double>(time_module.attr("time")());
			py::object res = py::exec_file(file_name, globals, locals);
			double end_time = py::extract<double>(time_module.attr("time")());
			return std::make_tuple(res, end_time - start_time);
		}

		std::tuple<py::object, double> _eval(py::str file_name, py::object globals = py::object(), py::object locals = py::object())
		{
			py::object time_module = py::import("time");
			double start_time = py::extract<double>(time_module.attr("time")());
			py::object res = py::exec_file(file_name, globals, locals);
			double end_time = py::extract<double>(time_module.attr("time")());
			return std::make_tuple(res, end_time - start_time);
	}
}

}