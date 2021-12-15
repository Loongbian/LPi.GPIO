#define PY_EVENT_CONST_OFFSET 30

PyObject *high;
PyObject *low;
PyObject *input;
PyObject *output;

PyObject *unknown;
PyObject *board;
//PyObject *bcm;
PyObject *ls2k;

PyObject *rising_edge;
PyObject *falling_edge;
PyObject *both_edge;



void define_constants(PyObject *module);
