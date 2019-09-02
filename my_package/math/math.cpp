#include <Python.h>

/*
 * A template is defined in a separate header. Scikit-build through cmake is
 * aware of this, and changes in the header will trigger a rebuild
 */
#include "math.hpp"

/*
 * This is a regular Python C api function, and works reasonably well with
 * plain setuptools. It has no outside dependencies
 */
static PyObject* add(PyObject*, PyObject* args) noexcept {
    int x, y;
    if (!PyArg_ParseTuple(args, "ii", &x, &y))
        return nullptr;
    return PyLong_FromLong(x + y);
}

/*
 * The divide function in math.hpp uses exception to communicate
 * divide-by-zero, but the Python C api is not aware of that (since it's C).
 * When the inner divide signals an error, catch the exception and translate it
 * into a PyErr_SetString call, which registers the exception with the Python runtime.
 *
 * When a C API function returns a null pointer, the python runtime understands
 * that an exception has been set.
 *
 * This is a separate template function to only write the catch-then-raise
 * once, instead of once per supported type
 */
template < typename T >
static PyObject* do_div(const  char* type, T lhs, T rhs) noexcept (true) {
    try {
        const auto result = divide(lhs, rhs);
        return Py_BuildValue(type, result);
    } catch (zero_div& e) {
        PyErr_SetString(PyExc_ZeroDivisionError, e.what());
        return nullptr;
    }
}

/*
 * The Python-visisble div-function. Use ParseTuple as simple type checker -
 * when it succeeds, just do the work and return the result. If it fails, try
 * next until either a match happens, or all options are exhausted and a type
 * error is raised.
 *
 * It's important to clear the exception between calls to ParseTuple, otherwise
 * the first failure would prevent subsequent attempts at parsing the args.
 */
static PyObject* div(PyObject*, PyObject* args) noexcept {
    int xi, yi;
    if (PyArg_ParseTuple(args, "ii", &xi, &yi))
        return do_div("i", xi, yi);

    PyErr_Clear();
    float xf, yf;
    if (PyArg_ParseTuple(args, "ff", &xf, &yf))
        return do_div("f", xf, yf);

    PyErr_Clear();
    double xd, yd;
    if (PyArg_ParseTuple(args, "dd", &xd, &yd))
        return do_div("d", xd, yd);

    PyErr_SetString(PyExc_TypeError, "div expected int, float, or double");
    return nullptr;
}

/*
 * The function table that will be registered in the module
 */
static PyMethodDef functions[] = {
    { "add", (PyCFunction) add, METH_VARARGS, "Add" },
    { "div", (PyCFunction) div, METH_VARARGS, "Div" },
    { nullptr, },
};

/*
 * Module initialisation data
 */
static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "math", // module name
    "module docstring",
    -1, // all state in global vars
    functions
};

/*
 * The python extension entry point. This registeres all functions and
 * docstring with the python runtime, and is analoguous to the __init__.py file
 */
PyMODINIT_FUNC PyInit_math(void) {
    return PyModule_Create(&module);
}
