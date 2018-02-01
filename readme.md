# Python 与 C 的混合编程

## 参考

1. python的[官方文档](https://docs.python.org/2/extending/extending.html#id5)
2. 程序园上面的[简明教程](http://www.voidcn.com/article/p-vrcojger-uo.html)

## C代码在Python中的使用

简而言之，最终需要的效果是，我们自己开发一个包，叫作spam

	import spam
	status = spam.system("dir")

上例是官方教程中的例子。这个spam包中，只有一个函数叫system，作用是把输入的字符串作为CMD命令执行，将返回字符串写入status

### 第一步-写一个spammodule.c的文件

第一行是固定的：

	#include <Python.h>

这个包含语句必须在最前面。
接下来，官方给出了一个函数定义。

	static PyObject *
	spam_system(PyObject *self, PyObject *args)
	{
	    const char *command;
	    int sts;
	    if (!PyArg_ParseTuple(args, "s", &command))
	        return NULL;
	    sts = system(command);
	    return Py_BuildValue("i", sts);
	}

这个函数定义为什么这么写，讲得并不清楚。参考程序园。

我们有一个很牛的函数：
	
	int great_function(int a)
	{
		return a+1;
	}

这个函数在写入编译为Python包的c文件时，要经过一个“封装”：

	static PyObject * _great_function(PyObject *self, PyObject *args)
	{
	    int _a;
	    int res;
	    if (!PyArg_ParseTuple(args, "i", &_a))
	        return NULL;
	    res = great_function(_a);
	    return PyLong_FromLong(res);
	}

	static PyMethodDef GreateModuleMethods[] = {
	    {
	        "great_function",
	        _great_function,
	        METH_VARARGS,
	        ""
	    },
	    {NULL, NULL, 0, NULL}
	};

	PyMODINIT_FUNC initgreat_module(void) {
	    (void) Py_InitModule("great_module", GreateModuleMethods);
	}

这三个部分依次是：
1. 包裹函数 \_great_function。这个函数用来将Python的输入参数转换为C的，并将C的输出参数转化为Python的。
2. 导出表 GreatModuleMethos。他负责告诉Python，这个包（模块）中有哪些函数可供Python调用。导出表的名字可以随便起，每一项当中有4个参数：第一个是提供给Python环境的函数名称；第二个参数是包裹函数名称；第三个参数的含义是参数变长（简单解释一下，这个参数通常用METH_VARARGS，然后包裹函数的输入参数必须用PyArg_ParseTuple来进行解析，再输入到原来最初我们定义的c函数当中）；第四个参数是一些说明性的字符串。注意：导出表总是以{NULL,NULL,0,NULL}结束
3. 导出函数initgreat_module。这个名字不是任意取的，而是你的module名称添加前缀init。导出函数中，会将模块名称与导出表进行连接。

所以，回到原来的那个spammodule.c文件中：对应的三个部分就是：

	// Error Process
	static PyObject *SpamError;

	// 包裹函数
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


	// 方法表（导出表）
	static PyMethodDef SpamMethods[] = {
		{ "system", spam_system, METH_VARARGS, "Execute a shell command." },
	{ NULL, NULL, 0, NULL }
	};

	// 导出函数
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

注意到这里多了一个错误处理的步骤，我们后续再说。
此外，原本的c文件中有个main函数，似乎可以不要！

### 第二步-编写一个安装py文件setup.py
这一步分为两种方法，首先是已经验证通过的也即编写一个setup.py的方法：

	from distutils.core import setup, Extension
	MOD = 'spam'
	setup(name=MOD, ext_modules=[Extension(MOD, sources=['spammodule.c'])])

以上是setup.py中的全部内容，写好之后，在命令提示符CMD中（注意要用管理员模式运行CMD）按照如下步骤进行编译：
1. 利用CD命令进入目标文件夹
2. 输入如下命令
	
	python setup.py build

即可编译完成，得到的.pyd文件就可以以文件名作为模块的名字在python中进行import调用

## 可能出现问题的地方
### CMD编译时报错：找不到vcvarsall.bat文件
由于我使用了VS2017，当前的编译方法找不到这个文件，需要修改相关的.py文件的代码。
修改文件：msvc9compiler.py
由于我通过anaconda安装python，所以有多个地方都有这个文件，需要修改的是：
- C:\ProgramData\Anaconda2\Lib\distutils\msvc9compiler.py
- C:\Users\jiajun\AppData\Local\conda\conda\envs\Python27\Lib\distutils\msvc9compiler.py
这个两个路径之一下面的文件（时间长了有点记不清是哪一个了，我的装在C盘所以是这个，不同的安装位置路径略有不同）

修改函数：

	find_vcvarsall

修改内容：
1. 注释所有内容
2. 添加code来return当前vcvarsall.bat所在的完全路径，我的在：
	
	return r"C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat"

