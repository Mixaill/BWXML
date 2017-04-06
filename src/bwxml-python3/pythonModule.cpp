#include <Python.h>

#include "BWReader.h"
#include "BWWriter.h"

static PyObject *
bwxml_command_pack_file(PyObject *self, PyObject *args)
{
	const char *unpacked_source, *packed_destination;
	int sts;

	if (!PyArg_ParseTuple(args, "ss", &unpacked_source, &packed_destination))
	{
		return NULL;
	}

	try
	{
		BWPack::BWXMLWriter(unpacked_source).saveTo(packed_destination);
	}
	catch (const std::exception& e)
	{
		PyErr_SetString(PyExc_RuntimeError, e.what());
		return NULL;
	}

	Py_RETURN_TRUE;
}

static PyObject *
bwxml_command_unpack_file(PyObject *self, PyObject *args)
{
	const char *packed_source, *unpacked_destination;
	int sts;

	if (!PyArg_ParseTuple(args, "ss", &packed_source, &unpacked_destination))
	{
		return NULL;
	}

	try
	{
		BWPack::BWXMLReader(packed_source).saveTo(unpacked_destination);
	}
	catch (const std::exception& e)
	{
		PyErr_SetString(PyExc_RuntimeError, e.what());
		return NULL;
	}

	Py_RETURN_TRUE;
}

static PyMethodDef bwxml_methods[] = {
	{ "pack_file",  bwxml_command_pack_file, METH_VARARGS, "pack_file(unpacked_source, packed_destination)" },
	{ "unpack_file",  bwxml_command_unpack_file, METH_VARARGS, "unpack_file(packed_source, unpacked_destination)"},
	{NULL, NULL, 0, NULL}
};

const char bwxml_doc[] = "A BWXML library for python";

static struct PyModuleDef bwxml_module = {
   PyModuleDef_HEAD_INIT,
   "bwxml",
   bwxml_doc,
   -1,      
   bwxml_methods
};

PyMODINIT_FUNC
PyInit_bwxml(void)
{
    return PyModule_Create(&bwxml_module);
}