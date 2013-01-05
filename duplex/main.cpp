#include <Python.h>
#include <iostream>
#include <string>
#include "../common/common.h"

std::string TransformHelper(const char* str)
{
  char* cpy = (char*)malloc(strlen(str) + 1);
  int i = 0;

  memset(cpy, strlen(cpy) + 1, '\0');
  
  for(; str[i] ; ++i) {
    switch(str[i]) {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
    case 'A':
    case 'E':
    case 'I':
    case 'O':
    case 'U':
      cpy[i] = '_';
      break;
    default:
      cpy[i] = str[i];
    }
  }
  return std::string(cpy);
}

static PyObject* WrapTransformHelper(PyObject *self, PyObject *arg)
{
  const char* str = PyString_AsString(arg);
  std::string result = TransformHelper(str);
  return PyString_FromString(result.c_str());
}

static PyMethodDef ProgModule[] = {
  {"TransformHelper", WrapTransformHelper, METH_O, "Transforms a string."},
  {NULL, NULL, 0, NULL}
};

int main(int argc, char** argv)
{
  // Helper functions
  // As soon as this gets out of scope, python will be finalized
  PythonContext ctx;

  // Add local dir to PYTHON_PATH, so we can import local files
  ctx.add_to_path(".");

  Py_InitModule("prog", ProgModule);

  // import module (expects a plugins.py file; 
  // likely to work with global modules)
  PyObject* module = ctx.import("plugins");

  PyObject* method = ctx.get(module, "main");

  PyObject* args = Py_BuildValue("(s)", "something");
  FAIL_IF((!args), "Couldn't make args");

  PyObject* res = PyEval_CallObject(method, args);
  FAIL_IF((!res), "Couldn't exec method"); 

  return 0;
}
