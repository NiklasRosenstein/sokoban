#/usr/bin/python

import base64

image_file = open("image.png", "rb")
encoded_str = base64.b64encode(image_file.read())
print encoded_str
