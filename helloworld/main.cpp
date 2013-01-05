#include <Python.h>
#include <iostream>
#include <string>

#define FAIL_IF(cond, message)			\
  if(cond) {					\
    PyErr_Print();				\
    std::cerr << (message) << std::endl;	\
    exit(-1);					\
  }

class PythonContext {
public:
  PythonContext () {
    Py_Initialize();
    FAIL_IF((!Py_IsInitialized()), "Couldn't init python");
  }

  void add_to_path(std::string newpath) {
    PyObject* sysPath = PySys_GetObject((char*) "path");
    FAIL_IF((!sysPath), "Couldn't create sysPath");
    PyObject* curDir = this->str(newpath);
    PyList_Append(sysPath, curDir);
    this->free(curDir);
    // Do NOT free sysPath, otherwise will get seg faults on import
  }

  PyObject* import(std::string modulename) {
    PyObject* _name = this->str(modulename);
    PyObject* ret = PyImport_Import(_name);
    FAIL_IF((!ret), "Couldn't import");
    this->free(_name);
    return ret;
  }

  PyObject* get(PyObject* &module, std::string name) {
    PyObject* ret = PyObject_GetAttrString(module, name.c_str());
    FAIL_IF((!ret), "Couldn't get stuff");
    return ret;
  }

  PyObject* str(std::string s) {
    PyObject* ret = NULL;
    ret = PyString_FromString(s.c_str());
    FAIL_IF((!ret), "Couldn't make string");
    return ret;
  } 

  PyObject* str(char* s) {
    PyObject* ret = NULL; 
    ret = PyString_FromString(s);
    FAIL_IF((!ret), "Couldn't make string");
    return ret;
  }

  void free(PyObject* &obj) {
    Py_DECREF(obj);
  }

  ~PythonContext() {
    Py_Finalize();
  }
};


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
