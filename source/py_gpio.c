#include <Python.h>

// python function cleanup(channel=None)
static PyObject *py_cleanup(PyObject *self, PyObject *args, PyObject *kwargs)
{
	Py_RETURN_NONE;
}

// python function setup(channel(s), direction, pull_up_down=PUD_OFF, initial=None)
static PyObject *py_setup_channel(PyObject *self, PyObject *args, PyObject *kwargs)
{

	Py_RETURN_NONE;

}

// python function setmode(mode)
static PyObject *py_setmode(PyObject *self, PyObject *args)
{

   Py_RETURN_NONE;
}

// Function 1: A simple 'hello world' function
static PyObject* helloworld(PyObject* self, PyObject* args)
{
    printf("Hello World\n");
    return Py_None;
}

// Our Module's Function Definition struct
// We require this `NULL` to signal the end of our method
// definition
static PyMethodDef myMethods[] = {
    { "helloworld", helloworld, METH_NOARGS, "Prints Hello World" },
    {"setup", (PyCFunction)py_setup_channel, METH_VARARGS | METH_KEYWORDS, "Set up a GPIO channel or list of channels with a direction and (optional) pull/up down control\nchannel        - either board pin number or BCM number depending on which mode is set.\ndirection      - IN or OUT\n[pull_up_down] - PUD_OFF (default), PUD_UP or PUD_DOWN\n[initial]      - Initial value for an output channel"},
    {"cleanup", (PyCFunction)py_cleanup, METH_VARARGS | METH_KEYWORDS, "Clean up by resetting all GPIO channels that have been used by this program to INPUT with no pullup/pulldown and no event detection\n[channel] - individual channel or list/tuple of channels to clean up.  Default - clean every channel that has been used."},
    {"setmode", py_setmode, METH_VARARGS, "Set up numbering mode to use for channels.\nBOARD - Use Raspberry Pi board numbers\nBCM   - Use Broadcom GPIO 00..nn numbers"},


    { NULL, NULL, 0, NULL }
};

// Our Module Definition struct
static struct PyModuleDef LPi = {
    PyModuleDef_HEAD_INIT,
    "LPi",
    "Test Module",
    -1,
    myMethods
};

// Initializes our module using our above struct
PyMODINIT_FUNC PyInit_LPi(void)
{
    return PyModule_Create(&LPi);
}
