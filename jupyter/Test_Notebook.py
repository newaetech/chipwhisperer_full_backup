#!/usr/bin/env python
# coding: utf-8

# In[9]:


import os
import subprocess
import tempfile
from pathlib import Path
import io

import nbformat
from nbconvert.preprocessors import ExecutePreprocessor
from nbconvert import HTMLExporter
from nbparameterise import extract_parameters, parameter_values, replace_definitions
from nbconvert.filters import ansi2html


def _notebook_run(path, SCOPETYPE='OPENADC', PLATFORM='CWLITEARM', **kwargs):
    """Execute a notebook via nbconvert and collect output.
       :returns (parsed nb object, execution errors)
    """
    
    html_path = Path("html/" + path + "-{}-{}".format(SCOPETYPE,PLATFORM) + ".html")
    real_path = Path(path)
    
    with open(real_path) as nbfile:
        nb = nbformat.read(nbfile, as_version=4)
        orig_parameters = extract_parameters(nb)
        params = parameter_values(orig_parameters, SCOPETYPE=SCOPETYPE, PLATFORM=PLATFORM, **kwargs)
        new_nb = replace_definitions(nb, params, execute=False)

        ep = ExecutePreprocessor(timeout=None, kernel_name='python3', allow_errors=True)

        ep.preprocess(new_nb, {'metadata': {'path': './'}})

        errors = [[i+1,output] for i,cell in enumerate(new_nb.cells) if "outputs" in cell
                        for output in cell["outputs"]\
                                if output.output_type == "error"]
        
        with open(html_path, "w", encoding='utf-8') as html_file:
            html_exporter = HTMLExporter()
            
            body, res = html_exporter.from_notebook_node(new_nb)
            
            body = ansi2html(body)
            
            html_file.write(body)
            
        return nb, errors


def _print_tracebacks(errors):
    if errors == []:
        print("Passed all tests!")
    for error in errors:
        print("Test failed in cell {}: {}: {}".format(error[0], error[1]['ename'], error[1]['evalue']))
        for line in error[1]['traceback']:
            print(line)
            
def _get_outputs(nb):
    return [[i,cell] for i,cell in enumerate(nb.cells) if "outputs" in cell]
    
def _print_stderr(nb):
    outputs = _get_outputs(nb)
    printed_output = [[cell[0], output] for cell in outputs for output in cell[1]['outputs'] if ('name' in output and output['name'] == 'stderr')]
    for out in printed_output:
        print("[{}]:\n{}".format(out[0], out[1]['text']))
        
def _print_stdout(nb):
    outputs = _get_outputs(nb)
    printed_output = [[cell[0], output] for cell in outputs for output in cell[1]['outputs'] if ('name' in output and output['name'] == 'stdout')]
    for out in printed_output:
        print("[{}]:\n{}".format(out[0], out[1]['text']))
        
def test_notebook(path,print_stdout=False, print_stderr=False,**kwargs):
    print("Testing: {}:...".format(path), end="")
    nb, errors = _notebook_run(path, **kwargs)
    if errors == []:
        print("PASSED")
    else:
        print("FAILED:")
        _print_tracebacks(errors)
    if print_stdout:
        _print_stdout(nb)
    if print_stderr:
        _print_stderr(nb)


# In[2]:


import os
import subprocess
import tempfile
from pathlib import Path
import io
from os import listdir
from os.path import isfile, join

import nbformat
from nbconvert.preprocessors import ClearOutputPreprocessor
from nbconvert.exporters import NotebookExporter
def clear_notebook(path):
    real_path = Path(path)
    body = ""
    with open(real_path, "r", encoding="utf-8") as nbfile:
        nb = nbformat.read(nbfile, as_version=4)
        co = ClearOutputPreprocessor()
        exporter = NotebookExporter()
        node, resources = co.preprocess(nb, {'metadata': {'path': './'}})
        body, resources = exporter.from_notebook_node(node, resources)
    with open(real_path, "w", encoding="utf-8") as nbfile:
        nbfile.write(body)
        
def clear_outputs_in_dir(dirpath):
    notebook_files = [f for f in listdir("./") if (isfile(join("./", f)) and f.endswith(".ipynb"))]
    for file in notebook_files:
        clear_notebook(file)


# In[ ]:


clear_outputs_in_dir("./")


# In[3]:


get_ipython().run_cell_magic('bash', '', 'mkdir -p html\ncd ../../hardware/victims/firmware\nrm -rf simpleserial-base-lab*\nrm -rf glitch-simple-lab*\nrm -rf simpleserial-aes-lab*')


# In[4]:


testscope = 'OPENADC'
testplat = 'CWLITEXMEGA'
crypt = 'AVRCRYPTOLIB'


# In[ ]:


testscope = 'CWNANO'
testplat = 'CWNANO'
crypt = 'TINYAES128C'


# In[ ]:


test_notebook('PA_Intro_1-Firmware_Build_Setup.ipynb', SCOPETYPE=testscope, PLATFORM=testplat)


# In[ ]:


test_notebook('PA_Intro_2-Instruction_Differences.ipynb', SCOPETYPE=testscope, PLATFORM=testplat)


# In[ ]:


test_notebook('PA_SPA_1-Timing_Analysis_with_Power_for_Password_Bypass.ipynb', SCOPETYPE=testscope, PLATFORM=testplat)


# In[ ]:


#test_notebook('PA_DPA_1-Hamming_Weight_Measurement.ipynb') #FAILS, COMPILER MOVED SPOT
test_notebook('PA_DPA_2-Large_HW_Swings.ipynb', SCOPETYPE=testscope, PLATFORM=testplat)


# In[ ]:


test_notebook('PA_CPA_1-Using_CW-Analyzer_for_CPA_Attack.ipynb', SCOPETYPE=testscope, PLATFORM=testplat, N=100, CRYPTO_TARGET=crypt)


# In[ ]:


test_notebook('PA_CPA_2-Manual_CPA_Attack.ipynb', SCOPETYPE=testscope, PLATFORM=testplat, N=100, CRYPTO_TARGET=crypt)


# In[ ]:


test_notebook('PA_CPA_3-Resynchronizing_Data_Traces.ipynb', SCOPETYPE=testscope, PLATFORM=testplat, N=150, CRYPTO_TARGET=crypt)


# In[ ]:


#test_notebook('PA_CPA_4-Hardware_Crypto_Attack.ipynb') #FAILS
test_notebook('PA_CPA_5-32bit_AES.ipynb', SCOPETYPE=testscope, PLATFORM=testplat) #MAY FAIL, takes a long time so may want to skip


# In[ ]:


test_notebook('PA_Multi_1-Breaking_AES-256_Bootloader.ipynb', SCOPETYPE=testscope, PLATFORM=testplat)


# In[9]:


assert testscope != "CWNANO", "Clock glitching not available on CWNANO"
test_notebook('Fault_1-Introduction_to_Clock_Glitch_Attacks.ipynb', SCOPETYPE=testscope, PLATFORM=testplat)


# In[8]:


assert testplat != "CWLITEXMEGA" and testplat != "CW303", "Voltage glitching works poorly on platform {}".format(testplat)
test_notebook('Fault_2-Introduction_to_Vcc_Glitch_Attacks.ipynb', SCOPETYPE=testscope, PLATFORM=testplat)


# In[ ]:


test_notebook('Fault_3-Glitch_Buffer_Attacks.ipynb', SCOPETYPE=testscope, PLATFORM=testplat)


# In[6]:


assert testplat != "CWLITEXMEGA" and testplat != "CW303" and testplat != "CWNANO", "RSA fault attack not available on platform {}".format(testplat)
test_notebook('Fault_5-RSA_Fault_Attack.ipynb', SCOPETYPE=testscope, PLATFORM=testplat)

