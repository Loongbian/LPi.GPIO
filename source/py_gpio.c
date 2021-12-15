#include "Python.h"
#include "c_gpio.h"
#include "event_gpio.h"
#include "cpuinfo.h"
#include "constants.h"
#include "common.h"


static PyObject *lpi_revision; // deprecated
static PyObject *board_info;
static int gpio_warnings = 1;


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
    //printf("Hello World\n");
    return Py_None;
}

// python function cleanup(channel=None)
static PyObject *py_cleanup(PyObject *self, PyObject *args, PyObject *kwargs)
{
   unsigned int gpio;

   int i;
   int chancount = -666;
   int found = 0;
   int channel = -666;

   PyObject *chanlist = NULL;
   PyObject *chantuple = NULL;
   PyObject *tempobj;
   
   static char *kwlist[] = {"channel", NULL};

   void cleanup_one(void)
   {

     // set everything back to input
      if (gpio_direction[gpio] != -1) {
         //setup_gpio(gpio, INPUT, PUD_OFF);
         setup_gpio(gpio, INPUT);
         gpio_direction[gpio] = -1;
         found = 1;
      }


   }

   //printf("cleanup()\n");
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

   if (module_setup && !setup_error) {
      if (channel == -666 && chancount == -666) {   // channel not set - cleanup everything
	  // set everything back to input
         for (i=0; i<74; i++) {
            if (gpio_direction[i] != -1) {
               //setup_gpio(i, INPUT, PUD_OFF);
               setup_gpio(i, INPUT);
               gpio_direction[i] = -1;
               found = 1;
            }
         }
         gpio_mode = MODE_UNKNOWN;
      } else if (channel != -666) {    // channel was an int indicating single channel
         if (get_gpio_number(channel, &gpio))
            return NULL;
         cleanup_one();
      } else {  // channel was a list/tuple
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

            if (PyLong_Check(tempobj)) {
               channel = (int)PyLong_AsLong(tempobj);
               if (PyErr_Occurred())
                  return NULL;
            } else {
               PyErr_SetString(PyExc_ValueError, "Channel must be an integer");
               return NULL;
            }

            if (get_gpio_number(channel, &gpio))
               return NULL;
            cleanup_one();
         }
      }
   }

   // check if any channels set up - if not warn about misuse of GPIO.cleanup()
   if (!found && gpio_warnings) {
      PyErr_WarnEx(NULL, "No channels have been set up yet - nothing to clean up!  Try cleaning up at the end of your program instead!", 1);
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
	   
      if (get_gpio_number(channel, &gpio))  // common.h
         return 0;

      printf("channel is %d, gpio is %d", channel, gpio);
     
      if (direction == OUTPUT && (initial == LOW || initial == HIGH)) {
	 setup_gpio(gpio, direction);
         output_gpio(gpio, initial);
	 return 1;
      }

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

   // check module has been imported cleanly
   if (setup_error)
   {
      PyErr_SetString(PyExc_RuntimeError, "Module not imported correctly!");
      return NULL;
   }


   if (mmap_gpio_mem()) {
      printf("Failed mmap:gpio->mem");
      return NULL;
   }

   if (direction != INPUT && direction != OUTPUT) {
      PyErr_SetString(PyExc_ValueError, "An invalid direction was passed to setup()");

      //printf("direction: is %d\n",direction);
      return 0;
   }

   if (direction == INPUT && initial != -1) {
      PyErr_SetString(PyExc_ValueError, "initial parameter is not valid for inputs");
      return 0;
   }


  // printf("direction: is %d\n",direction);
   
   if (chanlist) {
       chancount = PyList_Size(chanlist);
   } else if (chantuple) {
       chancount = PyTuple_Size(chantuple);
   } else {
       if (!setup_one())
          return NULL;
       Py_RETURN_NONE;
   }

   
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

      if (PyLong_Check(tempobj)) {
         channel = (int)PyLong_AsLong(tempobj);
         if (PyErr_Occurred())
             return NULL;
      } else {
          PyErr_SetString(PyExc_ValueError, "Channel must be an integer");
          return NULL;
      }


      if (!setup_one())
         return NULL;

   }
   
   Py_RETURN_NONE;
}

// python function output(channel(s), value(s))
static PyObject *py_output_gpio(PyObject *self, PyObject *args)
{
   unsigned int gpio;
   int channel = -1;
   int value = -1;
   int i;

   PyObject *chanlist = NULL;
   PyObject *valuelist = NULL;
   PyObject *chantuple = NULL;
   PyObject *valuetuple = NULL;
   PyObject *tempobj = NULL;
   int chancount = -1;
   int valuecount = -1;

   int output(void) {
      if (get_gpio_number(channel, &gpio))
          return 0;

      if (gpio_direction[gpio] != OUTPUT)
      {
         PyErr_SetString(PyExc_RuntimeError, "The GPIO channel has not been set up as an OUTPUT");
         return 0;
      }

      if (check_gpio_priv())
         return 0;

      printf("gpio:%d\n", gpio);
      output_gpio(gpio, value);
      return 1;


   }

   printf("output()\n");
   if (!PyArg_ParseTuple(args, "OO", &chanlist, &valuelist))
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
       PyErr_SetString(PyExc_ValueError, "Channel must be an integer or list/tuple of integers");
       return NULL;
   }

   if (PyLong_Check(valuelist)) {
       value = (int)PyLong_AsLong(valuelist);
      if (PyErr_Occurred())
         return NULL;
      valuelist = NULL;
   } else if (PyList_Check(valuelist)) {
      // do nothing
   } else if (PyTuple_Check(valuelist)) {
      valuetuple = valuelist;
      valuelist = NULL;
   } else {
       PyErr_SetString(PyExc_ValueError, "Value must be an integer/boolean or a list/tuple of integers/booleans");
       return NULL;
   }

   if (chanlist)
       chancount = PyList_Size(chanlist);
   if (chantuple)
       chancount = PyTuple_Size(chantuple);
   if (valuelist)
       valuecount = PyList_Size(valuelist);
   if (valuetuple)
       valuecount = PyTuple_Size(valuetuple);
   if ((chancount != -1 && chancount != valuecount && valuecount != -1) || (chancount == -1 && valuecount != -1)) {
       PyErr_SetString(PyExc_RuntimeError, "Number of channels != number of values");
       return NULL;
   }

   if (chancount == -1) {
      if (!output())
         return NULL;
      Py_RETURN_NONE;
   }

   for (i=0; i<chancount; i++) {
      // get channel number
      if (chanlist) {
         if ((tempobj = PyList_GetItem(chanlist, i)) == NULL) {
            return NULL;
         }
      } else { // assume chantuple
         if ((tempobj = PyTuple_GetItem(chantuple, i)) == NULL) {
            return NULL;
         }
      }

      if (PyLong_Check(tempobj)) {
         channel = (int)PyLong_AsLong(tempobj);
         if (PyErr_Occurred())
             return NULL;
      } else {
          PyErr_SetString(PyExc_ValueError, "Channel must be an integer");
          return NULL;
      }

      // get value
      if (valuecount > 0) {
          if (valuelist) {
             if ((tempobj = PyList_GetItem(valuelist, i)) == NULL) {
                return NULL;
             }
          } else { // assume valuetuple
             if ((tempobj = PyTuple_GetItem(valuetuple, i)) == NULL) {
                return NULL;
             }
          }
          if (PyLong_Check(tempobj)) {
             value = (int)PyLong_AsLong(tempobj);
             if (PyErr_Occurred())
                 return NULL;
          } else {
              PyErr_SetString(PyExc_ValueError, "Value must be an integer or boolean");
              return NULL;
          }
      }
      if (!output())
         return NULL;
   }

   Py_RETURN_NONE;

}

// python function value = input(channel)
static PyObject *py_input_gpio(PyObject *self, PyObject *args)
{
   unsigned int gpio;
   int channel;
   PyObject *value;

   if (!PyArg_ParseTuple(args, "i", &channel))
      return NULL;

   if (get_gpio_number(channel, &gpio))
       return NULL;


   // check channel is set up as an input or output
   if (gpio_direction[gpio] != INPUT && gpio_direction[gpio] != OUTPUT)
   {
      PyErr_SetString(PyExc_RuntimeError, "You must setup() the GPIO channel first");
      return NULL;
   }

   if (check_gpio_priv())
      return NULL;


}


// python function setmode(mode)
static PyObject *py_setmode(PyObject *self, PyObject *args)
{
   int new_mode;
  
   printf("setmode()->py_setmod()\n"); 
   if (!PyArg_ParseTuple(args, "i", &new_mode)) 
      return NULL;

   // in common.h
   if (gpio_mode != MODE_UNKNOWN && new_mode != gpio_mode)
   {
      PyErr_SetString(PyExc_ValueError, "A different mode has already been set!");
      return NULL;
   }

   if (setup_error)
   {
      PyErr_SetString(PyExc_RuntimeError, "Module not imported correctly!");
      return NULL;
   }

   if (new_mode != BOARD && new_mode != LS2K)
   {
      PyErr_SetString(PyExc_ValueError, "An invalid mode was passed to setmode()");
      return NULL;
   }

   gpio_mode = new_mode;     // common.h
   Py_RETURN_NONE;
}

// python function getmode()
static PyObject *py_getmode(PyObject *self, PyObject *args)
{
	PyObject *value;
	if (setup_error)
	{
		 PyErr_SetString(PyExc_RuntimeError, "Module not imported correctly!");
		 return NULL;
	}

	if (gpio_mode == MODE_UNKNOWN)
		Py_RETURN_NONE;
	value = Py_BuildValue("i", gpio_mode);
	return value;

}

// python function add_event_callback(gpio, callback)
static PyObject *py_add_event_callback(PyObject *self, PyObject *args, PyObject *kwargs)
{
	
   	Py_RETURN_NONE;
}

// python function add_event_detect(gpio, edge, callback=None, bouncetime=None)
static PyObject *py_add_event_detect(PyObject *self, PyObject *args, PyObject *kwargs)
{


	Py_RETURN_NONE;
}

// python function remove_event_detect(gpio)
static PyObject *py_remove_event_detect(PyObject *self, PyObject *args)
{
  	 unsigned int gpio;
	 Py_RETURN_NONE;
}

// python function value = event_detected(channel)
static PyObject *py_event_detected(PyObject *self, PyObject *args)
{

}

// python function channel = wait_for_edge(channel, edge, bouncetime=None, timeout=None)
static PyObject *py_wait_for_edge(PyObject *self, PyObject *args, PyObject *kwargs)
{

}


// Our Module's Function Definition struct
// We require this `NULL` to signal the end of our method
// definition
static PyMethodDef lpi_gpio_methods[] = {
    { "helloworld", helloworld, METH_NOARGS, "Prints Hello World" },

    // setup->py_setup_channel
    {"setup", (PyCFunction)py_setup_channel, METH_VARARGS | METH_KEYWORDS, "Set up a GPIO channel or list of channels with a direction and (optional) pull/up down control\nchannel        - either board pin number or LS2K number depending on which mode is set.\ndirection      - IN or OUT\n[pull_up_down] - PUD_OFF (default), PUD_UP or PUD_DOWN\n[initial]      - Initial value for an output channel"},

    // cleanup->py_cleanup
    {"cleanup", (PyCFunction)py_cleanup, METH_VARARGS | METH_KEYWORDS, "Clean up by resetting all GPIO channels that have been used by this program to INPUT with no pullup/pulldown and no event detection\n[channel] - individual channel or list/tuple of channels to clean up.  Default - clean every channel that has been used."},

    // output->py_output_gpio
    {"output", py_output_gpio, METH_VARARGS, "Output to a GPIO channel or list of channels\nchannel - either board pin number or LS2K number depending on which mode is set.\nvalue   - 0/1 or False/True or LOW/HIGH"},

    // input->py_input_gpio
    {"input", py_input_gpio, METH_VARARGS, "Input from a GPIO channel.  Returns HIGH=1=True or LOW=0=False\nchannel - either board pin number or LS2K number depending on which mode is set."},

    // setmode->py_setmode
    {"setmode", py_setmode, METH_VARARGS, "Set up numbering mode to use for channels.\nBOARD - Use Loongson Pi board numbers\nLS2K   - Use Broadcom GPIO 00..nn numbers"},

    // getmode->py_getmode
    {"getmode", py_getmode, METH_VARARGS, "Get numbering mode used for channel numbers.\nReturns BOARD, LS2K or None"},

    // add_event_detect->py_add_event_detect
    {"add_event_detect", (PyCFunction)py_add_event_detect, METH_VARARGS | METH_KEYWORDS, "Enable edge detection events for a particular GPIO channel.\nchannel      - either board pin number or BCM number depending on which mode is set.\nedge         - RISING, FALLING or BOTH\n[callback]   - A callback function for the event (optional)\n[bouncetime] - Switch bounce timeout in ms for callback"},

    // remove_event_detect
    {"remove_event_detect", py_remove_event_detect, METH_VARARGS, "Remove edge detection for a particular GPIO channel\nchannel - either board pin number or LS2K number depending on which mode is set."},

    // event_detected->py_event_detected
    {"event_detected", py_event_detected, METH_VARARGS, "Returns True if an edge has occurred on a given GPIO.  You need to enable edge detection using add_event_detect() first.\nchannel - either board pin number or LS2K number depending on which mode is set."},
 
    // add_event_callback->py_add_event_callback
    {"add_event_callback", (PyCFunction)py_add_event_callback, METH_VARARGS | METH_KEYWORDS, "Add a callback for an event already defined using add_event_detect()\nchannel      - either board pin number or BCM number depending on which mode is set.\ncallback     - a callback function"},

   // wait_for_edge -> py_wait_for_edge
   {"wait_for_edge", (PyCFunction)py_wait_for_edge, METH_VARARGS | METH_KEYWORDS, "Wait for an edge.  Returns the channel number or None on timeout.\nchannel      - either board pin number or BCM number depending on which mode is set.\nedge         - RISING, FALLING or BOTH\n[bouncetime] - time allowed between calls to allow for switchbounce\n[timeout]    - timeout in ms"},



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

   for (i=0; i<74; i++)
	   gpio_direction[i] = -1;

   // detect board revision and set up accordingly
   if (get_lpi_info(&lpiinfo))
   {
	   PyErr_SetString(PyExc_RuntimeError, "This module can only be run on a Loongson Edu Pi!");
	   setup_error = 1;
	   return NULL;

   }

   pin_to_gpio = &pin_to_gpio_rev;

   // register exit functions - last declared is called first
   if (Py_AtExit(cleanup) != 0)
   {
   	setup_error = 1;
	cleanup();
	return NULL;
   }

 
   if (Py_AtExit(event_cleanup_all) != 0)
   {
   	setup_error = 1; 
	cleanup();
	return NULL;
   }


   return module;


//    return PyModule_Create(&lpigpiomodule);
}
