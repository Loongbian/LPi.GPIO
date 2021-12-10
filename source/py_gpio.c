#include "Python.h"
#include "c_gpio.h"
#include "common.h"
#include "constants.h"


static int mmap_gpio_mem(void)
{
   int result;

   if (module_setup)    //common.h 
      return 0;

   printf("mmap_gpio_mem()->setup()[c_gpio.c]\n");
   result = setup();    //c_gpio.c
   if (result == SETUP_DEVMEM_FAIL)
   {
      PyErr_SetString(PyExc_RuntimeError, "No access to /dev/mem.  Try running as root!");
      return 1;
   } else { // result == SETUP_OK
      module_setup = 1;
      return 0;
   }


}

// Function 1: A simple 'hello world' function
static PyObject* helloworld(PyObject* self, PyObject* args)
{
    printf("Hello World\n");
    return Py_None;
}

// python function cleanup(channel=None)
static PyObject *py_cleanup(PyObject *self, PyObject *args, PyObject *kwargs)
{
   unsigned int gpio;

   int chancount = -666;
   int channel = -666;

   PyObject *chanlist = NULL;
   PyObject *chantuple = NULL;
   PyObject *tempobj;
   
   static char *kwlist[] = {"channel", NULL};

   void cleanup_one(void)
   {


   }


   printf("cleanup()\n");
   if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", kwlist, &chanlist))
      return NULL;

   if (chanlist == NULL) {  // channel kwarg not set
      // do nothing
   } else if (PyLong_Check(chanlist)) {
      channel = (int)PyLong_AsLong(chanlist);
      if (PyErr_Occurred())
         return NULL;
      chanlist = NULL;
   } else if (PyList_Check(chanlist)) {
      chancount = PyList_Size(chanlist);
   } else if (PyTuple_Check(chanlist)) {
      chantuple = chanlist;
      chanlist = NULL;
   } else {
      // raise exception
      PyErr_SetString(PyExc_ValueError, "Channel must be an integer or list/tuple of integers");
      return NULL;
   }


   Py_RETURN_NONE;
}


// python function setup(channel(s), direction, pull_up_down=PUD_OFF, initial=None)
static PyObject *py_setup_channel(PyObject *self, PyObject *args, PyObject *kwargs)
{
   unsigned int gpio;
   int channel = -1;
   int direction;

   int i, chancount;
   PyObject *chanlist = NULL;
   PyObject *chantuple = NULL;
   PyObject *tempobj;
//   int pud = PUD_OFF + PY_PUD_CONST_OFFSET;
   int initial = -1;
   static char *kwlist[] = {"channel", "direction", "initial", NULL};

   int setup_one(void) {
	   
//      if (get_gpio_number(channel, &gpio))
//         return 0;  
      printf("channel->gpio: %d\n",gpio);
      setup_gpio(gpio, direction);
      gpio_direction[gpio] = direction;
      return 1;
   }


   printf("setup()->py_setup_channel()\n");

   if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|ii", kwlist, &chanlist, &direction, &initial))
      return NULL;

   if (PyLong_Check(chanlist)) {
      channel = (int)PyLong_AsLong(chanlist);

      if (PyErr_Occurred())
         return NULL;
      chanlist = NULL;

   } else if (PyList_Check(chanlist)) {
      // do nothing
   } else if (PyTuple_Check(chanlist)) {
      chantuple = chanlist;
      chanlist = NULL;
   } else {
      // raise exception
      PyErr_SetString(PyExc_ValueError, "Channel must be an integer or list/tuple of integers");
      return NULL;
   }

   // printf("setup: chanlist  is OK\n");
   

  //  if (mmap_gpio_mem()) {
  //    printf("Failed mmap:gpio->mem");
  //    return NULL;
  // }

   if (direction != INPUT && direction != OUTPUT) {
      PyErr_SetString(PyExc_ValueError, "An invalid direction was passed to setup()");

      printf("direction: is %d\n",direction);
      return 0;
   }

   printf("direction: is %d\n",direction);
   
   if (chanlist) {
       chancount = PyList_Size(chanlist);
   } else if (chantuple) {
       chancount = PyTuple_Size(chantuple);
   } else {
       if (!setup_one())
          return NULL;
       Py_RETURN_NONE;
   }

   /*
   for (i=0; i<chancount; i++) {
      if (chanlist) {
         if ((tempobj = PyList_GetItem(chanlist, i)) == NULL) {
            return NULL;
         }
      } else { // assume chantuple
         if ((tempobj = PyTuple_GetItem(chantuple, i)) == NULL) {
            return NULL;
         }
      }


      if (!setup_one())
         return NULL;

   }
   */

   if (!setup_one())
	 return NULL;

   Py_RETURN_NONE;
}

// python function output(channel(s), value(s))
static PyObject *py_output_gpio(PyObject *self, PyObject *args)
{
   unsigned int gpio;
   int channel = -1;
   int value = -1;
   PyObject *chanlist = NULL;
   PyObject *valuelist = NULL;


   int output(void) {
//      if (get_gpio_number(channel, &gpio))
//          return 0;

      printf("gpio:%d\n", gpio);
      output_gpio(gpio, value);
      return 1;


   }

   printf("output()\n");
   if (!PyArg_ParseTuple(args, "OO", &chanlist, &valuelist))
      return NULL;

   if (PyLong_Check(chanlist)) 
      channel = (int)PyLong_AsLong(chanlist);

   if (PyLong_Check(valuelist)) 
      value = (int)PyLong_AsLong(valuelist);

   printf("channel:%d value:%d", channel, value);
   if (!output())
      return NULL;

   Py_RETURN_NONE;
}

// python function setmode(mode)
static PyObject *py_setmode(PyObject *self, PyObject *args)
{
   int new_mode;
  
   printf("setmode()->py_setmod()\n"); 
   if (!PyArg_ParseTuple(args, "i", &new_mode)) 
      return NULL;


   gpio_mode = new_mode;     // common.h
   Py_RETURN_NONE;
}

// Our Module's Function Definition struct
// We require this `NULL` to signal the end of our method
// definition
static PyMethodDef lpi_gpio_methods[] = {
    { "helloworld", helloworld, METH_NOARGS, "Prints Hello World" },
    {"setup", (PyCFunction)py_setup_channel, METH_VARARGS | METH_KEYWORDS, "Set up a GPIO channel or list of channels with a direction and (optional) pull/up down control\nchannel        - either board pin number or BCM number depending on which mode is set.\ndirection      - IN or OUT\n[pull_up_down] - PUD_OFF (default), PUD_UP or PUD_DOWN\n[initial]      - Initial value for an output channel"},
    {"cleanup", (PyCFunction)py_cleanup, METH_VARARGS | METH_KEYWORDS, "Clean up by resetting all GPIO channels that have been used by this program to INPUT with no pullup/pulldown and no event detection\n[channel] - individual channel or list/tuple of channels to clean up.  Default - clean every channel that has been used."},
    {"output", py_output_gpio, METH_VARARGS, "Output to a GPIO channel or list of channels\nchannel - either board pin number or BCM number depending on which mode is set.\nvalue   - 0/1 or False/True or LOW/HIGH"},
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
   
   pin_to_gpio = &pin_to_gpio_rev;


   return module;


//    return PyModule_Create(&lpigpiomodule);
}
