This folder is intended for scripts that can be run from the `Project > Example
Scripts` menu. All of this code is imported when the CWCapture program starts.
In other words, this code is run at startup time.

WARNING: Do not use this folder for "loose" Python scripts. For example, keeping
a file with the lines

	from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
	api = CWCoreAPI()

can cause Bad Things (TM). Instead, put this into the Python __main__ check:

	if __name__ == "__main__":
		from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
		api = CWCoreAPI()
