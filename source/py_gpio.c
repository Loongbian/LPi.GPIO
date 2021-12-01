#include "Python.h"
#include "common.h"
#include "constants.h"


// Function 1: A simple 'hello world' function
static PyObject* helloworld(PyObject* self, PyObject* args)
{
    printf("Hello World\n");
    return Py_None;
}

// python function setup(channel(s), direction, pull_up_down=PUD_OFF, initial=None)
static PyObject *py_setup_channel(PyObject *self, PyObject *args, PyObject *kwargs)
{

   Py_RETURN_NONE;
}

// python function setmode(mode)
static PyObject *py_setmode(PyObject *self, PyObject *args)
{
   int new_mode;
   
   if (!PyArg_ParseTuple(args, "i", &new_mode))
      return NULL;


   gpio_mode = new_mode;
   Py_RETURN_NONE;
}

// Our Module's Function Definition struct
// We require this `NULL` to signal the end of our method
// definition
static PyMethodDef lpi_gpio_methods[] = {
    { "helloworld", helloworld, METH_NOARGS, "Prints Hello World" },
    {"setup", (PyCFunction)py_setup_channel, METH_VARARGS | METH_KEYWORDS, "Set up a GPIO channel or list of channels with a direction and (optional) pull/up down control\nchannel        - either board pin number or BCM number depending on which mode is set.\ndirection      - IN or OUT\n[pull_up_down] - PUD_OFF (default), PUD_UP or PUD_DOWN\n[initial]      - Initial value for an output channel"},
    {"setmode", py_setmode, METH_VARARGS, "Set up numbering mode to use for channels.\nBOARD - Use Loongson Pi board numbers\nBCM   - Use Broadcom GPIO 00..nn numbers"},
    { NULL, NULL, 0, NULL }
};

// Our Module Definition struct
static struct PyModuleDef lpigpiomodule = {
    PyModuleDef_HEAD_INIT,
    "LPi.GPIO",      // name of module
    "Test Module",
    -1,
    lpi_gpio_methods
};

// Initializes our module using our above struct
PyMODINIT_FUNC PyInit_GPIO(void)
{
   int i;
   PyObject *module = NULL;

   if ((module = PyModule_Create(&lpigpiomodule)) == NULL)
      return NULL;

   define_constants(module);

   return module;



//    return PyModule_Create(&lpigpiomodule);
}
