#include "Python.h"
#include "constants.h"
#include "common.h"

void define_constants(PyObject *module)
{

   unknown = Py_BuildValue("i", MODE_UNKNOWN);
   PyModule_AddObject(module, "UNKNOWN", unknown);

   board = Py_BuildValue("i", BOARD);
   PyModule_AddObject(module, "BOARD", board);

   bcm = Py_BuildValue("i", BCM);
   PyModule_AddObject(module, "BCM", bcm);
}
