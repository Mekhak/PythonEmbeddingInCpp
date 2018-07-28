#pragma once

#include <boost/python.hpp>

namespace boost
{
	namespace python
	{
		//print information about interpretator
		void print_interpreter_info()
		{
			printf("Prefix: %s\nExec Prefix: %s\nPython Path: %s\n",
				Py_GetPrefix(),
				Py_GetExecPrefix(),
				Py_GetProgramFullPath());
			printf("Module Path: %s\n",
				Py_GetPath());
			printf("Version: %s\nPlatform: %s\nCopyright: %s\n",
				Py_GetVersion(),
				Py_GetPlatform(),
				Py_GetCopyright());
			printf("Compiler String: %s\nBuild Info: %s\n",
				Py_GetCompiler(),
				Py_GetBuildInfo());
		}
	}
}
