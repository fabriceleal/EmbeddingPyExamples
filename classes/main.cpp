#include <Python.h>
#include <iostream>
#include <string>
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

  PyObject* celsius = ctx.get(module, "celsius");

  PyObject* args = Py_BuildValue("(f)", 100.0f);
  FAIL_IF((!args), "Couldn't make args");

  PyObject* instance = PyEval_CallObject(celsius, args);
  FAIL_IF((!instance), "Couldn't make instance"); 

  PyObject* method = ctx.get(instance, "farenheit");
  FAIL_IF((!instance), "Couldn't find farenheit");

  Py_DECREF(args);
  args = Py_BuildValue("()");
  FAIL_IF((!args), "Couldn't make args (2nd)");

  PyObject* ret = PyEval_CallObject(method, args);
  FAIL_IF((!ret), "Couldn't call method");

  float farenheit;
  PyArg_Parse(ret, "f", &farenheit);
  std::cout << "Result: " << farenheit << std::endl;

  return 0;
}
