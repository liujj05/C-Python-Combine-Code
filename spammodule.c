#include <Python.h>
// 该程序参考自Python官方教程

// Error Process
static PyObject *SpamError;

static PyObject *
spam_system(PyObject *self, PyObject *args)
{
	const char *command;
	int sts;
	if (!PyArg_ParseTuple(args, "s", &command))
	{
		return NULL;
	}
	sts = system(command);
	/* 这是教程第一版没有错误处理的
	return Py_BuildValue("i", sts);
	*/

	// 这是教程第二版添加错误处理的
	if (sts < 0)
	{
		PyErr_SetString(SpamError, "System command failed");
		return NULL;
	}
	return PyLong_FromLong(sts);
}


// 方法表
static PyMethodDef SpamMethods[] = {
	{ "system", spam_system, METH_VARARGS, "Execute a shell command." },
{ NULL, NULL, 0, NULL }
};


PyMODINIT_FUNC
initspam(void)
{
	PyObject *m;
	m = Py_InitModule("spam", SpamMethods);
	if (m == NULL)
	{
		return;
	}
	SpamError = PyErr_NewException("spam.error", NULL, NULL);
	Py_INCREF(SpamError);
	PyModule_AddObject(m, "error", SpamError);
}


main(int argc, char *argv[])
{
	Py_SetProgramName(argv[0]);

	Py_Initialize();

	initspam();
}
