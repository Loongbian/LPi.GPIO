#include "Python.h"
#include "constants.h"
#include "common.h"
#include "c_gpio.h"
#include "event_gpio.h"


void define_constants(PyObject *module)
{
   high = Py_BuildValue("i", HIGH);
   PyModule_AddObject(module, "HIGH", high);

   low = Py_BuildValue("i", LOW);
   PyModule_AddObject(module, "LOW", low);

   output = Py_BuildValue("i", OUTPUT);
   PyModule_AddObject(module, "OUT", output);

   input = Py_BuildValue("i", INPUT);
   PyModule_AddObject(module, "IN", input);

   unknown = Py_BuildValue("i", MODE_UNKNOWN);
   PyModule_AddObject(module, "UNKNOWN", unknown);

   board = Py_BuildValue("i", BOARD);
   PyModule_AddObject(module, "BOARD", board);

//   bcm = Py_BuildValue("i", BCM);
//   PyModule_AddObject(module, "BCM", bcm);
   ls2k = Py_BuildValue("i", LS2K);
   PyModule_AddObject(module, "LS2K", ls2k);

   rising_edge = Py_BuildValue("i", RISING_EDGE + PY_EVENT_CONST_OFFSET);
   PyModule_AddObject(module, "RISING", rising_edge);

   falling_edge = Py_BuildValue("i", FALLING_EDGE + PY_EVENT_CONST_OFFSET);
   PyModule_AddObject(module, "FALLING", falling_edge);

   both_edge = Py_BuildValue("i", BOTH_EDGE + PY_EVENT_CONST_OFFSET);
   PyModule_AddObject(module, "BOTH", both_edge);


}
