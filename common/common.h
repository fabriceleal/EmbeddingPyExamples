#include <Python.h>

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
