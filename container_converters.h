#pragma once

#include <boost/python.hpp>
#include <iostream>
#include <vector>
#include <list>
#include <map>

namespace boost {
	namespace python {
		//py::list form std::vector
		template <typename T>
		boost::python::list std_vector_to_python_list(const std::vector<T>& v) {
			boost::python::list list;
			for (const auto & item : v) {
				list.append(item);
			}
			return list;
		}

		//py::tuple from std::vector
		template <typename T>
		boost::python::tuple std_vector_to_python_tuple(const std::vector<T>& v) {
			boost::python::list list;
			for (const auto & item : v) {
				list.append(item);
			}
			return boost::python::tuple(list);
		}

		//py::dict from std::map
		template <typename K, typename V>
		boost::python::dict std_map_to_python_dict(std::map<K, V>& map) {
			boost::python::dict dictionary;
			for (const auto& item : map) {
				dictionary[item.first] = item.second;
			}
			return dictionary;
		}

		//------------------------------------\\
			//std::vector from py:: list, tuple, set, frozenset ...
		template<typename T>
		std::vector<T> to_std_vector(const boost::python::object& iterable)
		{
			return std::vector<T>(py::stl_input_iterator<T>(iterable), py::stl_input_iterator<T>());
		}

		//std::map from py::dict
		template <class K, class V>
		std::map<K, V> python_dict_to_std_map(const py::dict& dict) {
			std::map<K, V> map;
			const std::vector<K> keys = to_std_vector<K>(dict.keys());
			const std::vector<V> values = to_std_vector<V>(dict.values());
			for (int i = 0; i < keys.size(); ++i) {
				map.insert({ keys[i], values[i] });
			}
			return map;
		}
	} // end of namespace python
} // end of namespace boost