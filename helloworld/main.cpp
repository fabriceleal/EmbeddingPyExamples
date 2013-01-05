#include <Python.h>
#include <iostream>
#include <string>

#include <Python.h>
#include "../common/common.h"

int main(int argc, char** argv)
{
  // Helper functions
  // As soon as this gets out of scope, python will be finalized
  PythonContext ctx;

  // Add local dir to PYTHON_PATH, so we can import local files
  ctx.add_to_path(".");

  // import module (expects a plugins.py file; 
  // likely to work with global modules)
  PyObject* module = ctx.import("plugins");

  // get stuff from module
  PyObject* filter = ctx.get(module, "filterFunc"); 
  
  std::clog << "Type lines of text: " << std::endl;
  std::string input;
  while(true) {
    // Reads from stdin
    std::getline(std::cin, input);
    if(!std::cin.good()) {
      break;
    }
    
    // Prepare to call function
    // * Make args
    PyObject* args = Py_BuildValue("(s)", input.c_str());
    // Call function with args
    PyObject* result = PyObject_CallObject(filter, args);
    // Convert result (PyObject*) to std::string
    std::string res = PyString_AsString(result);

    std::cout << res << std::endl;

    ctx.free(result);
    ctx.free(args);
  }

  return 0;
}
