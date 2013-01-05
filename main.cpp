#include <Python.h>
#include <iostream>
#include <string>

class PythonContext {
public:
  PythonContext () {
    Py_Initialize();
  }

  void add_to_path(std::string newpath) {
    PyObject* sysPath = PySys_GetObject((char*) "path");
    if(!sysPath) {
      PyErr_Print();
      exit(-1);
    }
    PyObject* curDir = this->str(newpath);
    PyList_Append(sysPath, curDir);
    this->free(curDir);
    // Do NOT free sysPath, otherwise will get seg faults on import
  }

  PyObject* import(std::string modulename) {
    std::clog << "module name to python str ... " << std::endl;
    PyObject* _name = this->str(modulename);
    //std::clog << _name << std::endl;

    std::clog << "actually importing module ... " << std::endl;
    PyObject* ret = PyImport_Import(_name);
    if(!ret) {
      PyErr_Print();
      exit(-1);
    }

    std::clog << "free module name ..." << std::endl;
    this->free(_name);

    std::clog << "return ..." << std::endl;
    return ret;
  }

  PyObject* get(PyObject* &module, std::string name) {
    PyObject* ret = PyObject_GetAttrString(module, name.c_str());
    if(!ret) {
      PyErr_Print();
      exit(-1);
    }
    return ret;
  }

  PyObject* str(std::string s) {
    PyObject* ret = NULL; 
    ret = PyString_FromString(s.c_str());
    if(!ret) {
      PyErr_Print();
      exit(-1);
    }
    return ret;
  } 

  PyObject* str(char* s) {
    PyObject* ret = NULL; 
    ret = PyString_FromString(s);
    if(!ret) {
      PyErr_Print();
      exit(-1);
    }
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
