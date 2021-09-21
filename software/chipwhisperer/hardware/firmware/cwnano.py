# This file was auto-generated. Do not manually edit or save. What are you doing looking at it? Close it now!
# Generated on 2021-09-21 17:54:28.558207
#
import binascii
import io

fwver = [0, 50]
def getsome(item, filelike=True):
    data = _contents[item].encode('latin-1')
    data = binascii.a2b_base64(data)
    if filelike:
        data = io.BytesIO(data)
    return data

_contents = {
