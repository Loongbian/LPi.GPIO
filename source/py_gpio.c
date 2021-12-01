#include <Python.h>

// Function 1: A simple 'hello world' function
static PyObject* helloworld(PyObject* self, PyObject* args)
{
    printf("Hello World\n");
    return Py_None;
}

// Our Module's Function Definition struct
// We require this `NULL` to signal the end of our method
// definition
static PyMethodDef lpi_gpio_methods[] = {
    { "helloworld", helloworld, METH_NOARGS, "Prints Hello World" },
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
    return PyModule_Create(&lpigpiomodule);
}
