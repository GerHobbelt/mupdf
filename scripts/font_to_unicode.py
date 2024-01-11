#!/bin/env python

# pip install freetype-py

import sys
import freetype

def font_to_unicode(path):
	index = 0
	while True:
		face = freetype.Face(path, index)
		name = face.postscript_name.decode('ascii')
		print("creating tounicode from " + name)
		with open("resources/to_unicode/" + name, "w") as out:
			out.write("/CMapName /" + name + "\n")
			out.write("begincidchar\n")
			for uni,gid in face.get_chars():
				out.write(str(gid) + " " + str(uni) + "\n")

		index = index + 1
		if index >= face.num_faces:
			break

for arg in sys.argv[1:]:
	font_to_unicode(arg)
