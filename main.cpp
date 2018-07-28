//#include<Python.h>
////#include<boost/python.hpp>
//int main() {
//	Py_Initialize();
//	PyRun_SimpleString("print(12)");

//
//}
#include<iostream>
#include<string>
#include<vector>
#include <boost/foreach.hpp>
#include <boost/python.hpp>
#include "to_python_converter.h"
#include "from_python_converter.h"
#include "handle_error.h"
#include "container_converters.h"
#include "access.h"
#include "exec_script.h"
#include "interpretator_info.h"
#include <thread>
#include <fstream>


namespace py = boost::python;
//using namespace py;

template<typename T, typename U>
py::ConvertableTypes py::From_python_converter<T, U>::type;

void error_handling_test() {
	std::cout << "error_handling_test started" << std::endl;

	// 1. std::string parse_python_exception()

	//before
	try {
		py::object main_module = py::import("fake_module");
	}
	catch (py::error_already_set const &) {
		//user must call various Python/C API functions to get information about the exception
		PyErr_Print();
		//....
		//....
	}

	//now
	try {
		py::object main_module = py::import("fake_module");
	}
	catch (py::error_already_set const &) {
		//call only parse_python_exception() function to get information about the exception
		std::string perror_str = py::parse_python_exception();
		std::cout << "Error in Python: " << perror_str << std::endl;
	}

	// 2. template <class return_type>
	//    return_type call_python_func(boost::function<return_type()> to_call, const std::string &error_pre);

	try {
		py::call_python_function<py::object>(std::bind(&py::import, "fake_module"), "failed to import fake_module: ");
	}
	catch (std::runtime_error& e) {
		std::cout << "runtome error " << e.what() << std::endl;
	}

	std::cout << "error_handling_test ended\n" << std::endl;
}

//some user defined type
class SomeClass {
public:
	SomeClass(long i)
		: value(i) {}
	long get_value() const { return value; }
	void set_value(const long v) { value = v; }
private:
	long value;
};

long some_class_to_int(const SomeClass& obj)
{
	return obj.get_value();
}

py::object to_python_converter_test()
{
	std::cout << "to_python_converter_test started" << std::endl;

	//user want to convert the user defined class's object into py::object

	//before
	try {
		//to convert user defined c++ type to py::object users must define a struct(class) with a convert function for each newly defined c++ type
		struct SomeClass_to_py_int {
			static PyObject* convert(const SomeClass & obj)
			{
				return boost::python::incref(boost::python::object(obj.get_value()).ptr());
			}
		};
		py::to_python_converter<SomeClass, SomeClass_to_py_int>();
		SomeClass some_class_obj(0);
		py::object obj(some_class_obj);
	}
	catch (py::error_already_set const &) {
		//call only parse_python_exception() function to get information about the exception
		std::string perror_str = py::parse_python_exception();
		std::cout << "Error in Python: " << perror_str << std::endl;
	}

	//now
	try {
		// register the to-python converter
		py::to_python_converter<SomeClass, py::To_python_object<SomeClass, long, some_class_to_int>>();
	}
	catch (py::error_already_set const &) {
		//call only parse_python_exception() function to get information about the exception
		std::string perror_str = py::parse_python_exception();
		std::cout << "Error in Python: " << perror_str << std::endl;
	}

	/////////////////////////
	SomeClass some_class_obj(10);
	py::object obj(some_class_obj);

	std::cout << "to_python_converter_test ended\n" << std::endl;

	return obj;
}

void from_python_converter_test(py::object obj)
{
	std::cout << "from_python_converter_test started" << std::endl;
	//user want to do the opposite thing of previous example: convert py::object to c++ object


	//before
	try {
		//to convert py::object to C++ object users must define a struct(class) with a convert function for each newly defined c++ type
		struct SomeClass_from_python_int
		{
			SomeClass_from_python_int()
			{
				boost::python::converter::registry::push_back(
					&convertible,
					&construct,
					boost::python::type_id<SomeClass>());
			}

			// Determine if obj_ptr can be converted in a SomeClass
			static void* convertible(PyObject* obj_ptr)
			{
				if (!PyLong_Check(obj_ptr)) return 0;
				return obj_ptr;
			}

			// Convert obj_ptr into a SomeClass
			static void construct(
				PyObject* obj_ptr,
				boost::python::converter::rvalue_from_python_stage1_data* data)
			{
				long value = PyLong_AsLong(obj_ptr);

				assert(value);

				void* storage = (
					(boost::python::converter::rvalue_from_python_storage<SomeClass>*)
					data)->storage.bytes;

				// in-place construct the new SomeClass
				new (storage)SomeClass(value);
				data->convertible = storage;
			}
		};

		// register the from-python converter
		SomeClass_from_python_int();
	}
	catch (py::error_already_set const &) {
		//call only parse_python_exception() function to get information about the exception
		std::string perror_str = py::parse_python_exception();
		std::cout << "Error in Python: " << perror_str << std::endl;
	}



	//now
	try {
		py::From_python_converter<SomeClass, long>::register_converter(py::ConvertableTypes::LONG);
	}
	catch (py::error_already_set const &) {
		//call only parse_python_exception() function to get information about the exception
		std::string perror_str = py::parse_python_exception();
		std::cout << "Error in Python: " << perror_str << std::endl;
	}

	// extruct SomeClass from obj
	SomeClass c_object = py::extract<SomeClass>(obj);
	std::cout << "checking if the convertion done correctly: obj.value == " << c_object.get_value() << std::endl;

	std::cout << "from_python_converter_test ended\n" << std::endl;
}

void container_converters_test()
{
	std::cout << "container_converters_test started" << std::endl;

	std::vector<int> v{ 1, 3, 2, 5, 4 };

	// 1. py::list form std::vector
	//before
	boost::python::list list;
	for (const auto & item : v) {
		list.append(item);
	}

	//now
	py::list list_ = py::std_vector_to_python_list(v);


	// 2. py::tuple from std::vector
	//before
	boost::python::list list1;
	for (const auto & item : v) {
		list1.append(item);
	}
	py::tuple t(list1);

	//now
	py::tuple t_ = py::std_vector_to_python_tuple(v);


	std::map<int, double> m{ { 1, 0.1 }, { 2, 0.2 }, { 3, 0.3 } };

	// 3. py::dict from std::map
	//before
	boost::python::dict d;
	for (const auto& item : m) {
		d[item.first] = item.second;
	}

	//now
	boost::python::dict d_ = py::std_map_to_python_dict(m);



	//------------------------------------\\
		// 4. std::vector from py:: list, tuple ...
	//before 
	v = std::vector<int>(py::stl_input_iterator<int>(t), py::stl_input_iterator<int>());
	v = std::vector<int>(py::stl_input_iterator<int>(list), py::stl_input_iterator<int>());

	//now
	v = py::to_std_vector<int>(t_);
	v = py::to_std_vector<int>(list);

	// 5. std::map from py::dict
	std::map<int, double> m1 = py::python_dict_to_std_map<int, double>(d);

	std::cout << "container_converters_test ended\n" << std::endl;
}

void access_test()
{
	std::cout << "access_test started" << std::endl;

	py::object main_module = py::import("__main__");
	py::object main_namespace = main_module.attr("__dict__");
	py::object ignored = py::exec_file("hello1.txt", main_namespace);
	std::cout << "b == " << py::get_global_variable<int>(main_module, "b") << std::endl;
	std::cout << "A::i == " << py::get_member_variable<int>(main_module, "A", "i") << std::endl;

	std::cout << "access_test ended\n" << std::endl;
}

void exec_script_test(py::object main_namespace)
{
	std::cout << "exec_scrypt_test started" << std::endl;

	py::object result;
	double t;
	std::tie(result, t) = _exec_file("hello1.txt", main_namespace);
	std::cout << "execution time = " << t << std::endl;

	std::cout << "exec_scrypt_test ended\n" << std::endl;
}

void interpretator_info_test()
{
	std::ofstream out("out.txt");
	std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
	std::cout.rdbuf(out.rdbuf());

	std::cout << "interpretator_info_test started" << std::endl;

	py::print_interpreter_info();

	std::cout << "interpretator_info_test ended\n" << std::endl;
}


void tf()
{
	for (int i = 0; i <= 10; ++i)
	{
		std::cout << "tf" << i << "  ";
	}
}
void thread_test() {
	std::thread t(tf);
	for (int i = 0; i <= 100; ++i)
	{
		std::cout << "M" << i << "  ";
	}
	t.join();
}

int main() {
	thread_test();
	try {
		Py_Initialize();
		py::object main_module = py::import("__main__");
		py::object main_namespace = main_module.attr("__dict__");

		error_handling_test();
		py::object obj = to_python_converter_test();
		from_python_converter_test(obj);
		container_converters_test();
		access_test();
		exec_script_test(main_namespace);
		interpretator_info_test();
	}
	catch (py::error_already_set const &) {
		//call only parse_python_exception() function to get information about the exception
		std::string perror_str = py::parse_python_exception();
		std::cout << "Error in Python: " << perror_str << std::endl;
	}
}
