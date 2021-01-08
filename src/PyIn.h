namespace cpython {
    
	using std::cerr, std::endl, std::ofstream;

	static void checkPyErr() {
		if (PyErr_Occurred())
			PyErr_Print();
	}

	static void initialize() {
		Py_Initialize();
		PyRun_SimpleString("import sys");
		PyRun_SimpleString("sys.path.append(\".\")");
	}
	
	static void finalize() {
		if (Py_FinalizeEx() < 0) {
			cerr << "error closing python\n";
			exit(120);
		}
	}

	static void pyfile_buildFunction(const char* f) { // build a file with a single function, defined as f
		ofstream stream{ "../Debug/Func.py" };
		stream << "def f(x,y) :\n\treturn " << f << endl;
		stream.close();
	}

	static void pyfile_appendFunction(const char* function_identifier, const char* function_expression) {
		ofstream ofs;
		ofs.open( "../Debug/Func.py", ofstream::out | ofstream::app);
		ofs << "\ndef " << function_identifier << "(x,y) :\n\treturn " << function_expression << std::endl;
		ofs.close();
	}

	static PyObject* pyfile_retrieveCallable(const char* module_name, const char* function_name)
	{
		PyObject* func, * module_string, * module, * module_dict;
		module_string = PyUnicode_FromString(module_name);
		module = PyImport_Import(module_string);
		Py_DecRef(module_string);
		checkPyErr();
		if (module != NULL) {
			module_dict = PyModule_GetDict(module);
			Py_DecRef(module);
			func = PyDict_GetItem(module_dict, PyUnicode_FromString(function_name));
			Py_DecRef(module_dict);
			if (func != NULL && PyCallable_Check(func))
				return func;
			else {
				checkPyErr();
				return NULL;
			}
		}
		else {
			checkPyErr();
			return NULL;
		}
	}	
}