import os

root = r'./_build/html/'

for fn in os.listdir(root):
    fn = root + fn
    if os.path.isfile(fn):
         if fn.endswith('html'):
             data = open(fn, 'r').read()

             data = data.replace('<link rel="stylesheet" href="_static/pygments.css" type="text/css" />', '<link rel="stylesheet" href="_static/pygments.css" type="text/css" />\n\n'+
                          '<script type="text/javascript">\n    window.onload = function(){\n    document.getElementById(\'close\').onclick = function(){\n        this.parentNode.parentNode\n        .removeChild(this.parentNode);\n        return false;\n    };\n    };\n    </script>\n')

             data = data.replace('<body role="document">', '<body role="document">\n\n\n' + '<div id="fixedBox"><span id=\'close\'>x</span><p>This document is currently OUTDATED and will not be updated. ChipWhisperer documention is moved to the wiki page at <a href="https://wiki.newae.com">wiki.newae.com</a>.</p><p>Some material (including API documentation) may not yet be on wiki.</p></div>')
             
             open(fn, 'w').write(data)
