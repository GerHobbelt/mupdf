#! /usr/bin/env python3

'''
Example usage:
    PYTHONPATH=build/shared-debug-extract ./scripts/mupdfwrap_gui.py
'''

import mupdf

if 0:
    import tkinter

    class App(tkinter.Frame):
        def __init__(self, master=None):
            super().__init__(master)
            self.pack()

    app = App()

    app.master.title("MuPDF tkinter app")
    app.mainloop()

import PyQt5
import PyQt5.QtWidgets


print(f'{PyQt5.__file__}')

g_pixmaps = []

def open_pdf(path):
    document = mupdf.Document(path)
    page = mupdf.Page(document, 0)
    ctm = mupdf.Matrix(1, 0, 0, 1, 0, 0)
    colorspace = mupdf.Colorspace(mupdf.Colorspace.Fixed_RGB)
    alpha = 0
    pixmap = page.new_pixmap_from_page_contents(ctm, colorspace, alpha)
    # Need to preserve <pixmap> after we return because <image> will refer to
    # it.
    g_pixmaps.append(pixmap)
    image = PyQt5.QtGui.QImage(
            int(pixmap.pixmap_samples()),
            pixmap.pixmap_width(),
            pixmap.pixmap_height(),
            PyQt5.QtGui.QImage.Format_RGB888,
            );
    return image

#open_pdf('zlib.clean.pdf')

class MainWindow(PyQt5.QtWidgets.QMainWindow):
    pass

print(dir(PyQt5))
app = PyQt5.QtWidgets.QApplication([])
label = PyQt5.QtWidgets.QLabel('Hello world')
label.show()

g_document = [None]

window = MainWindow()
menu = window.menuBar().addMenu("&File")
open_ = PyQt5.QtWidgets.QAction("&Open")

def open_path(path):
    image = open_pdf(path)
    qpixmap = PyQt5.QtGui.QPixmap.fromImage(image)
    print(f'calling label.setImage')
    label.setPixmap(qpixmap)

def open_fn():
    path = PyQt5.QtWidgets.QFileDialog.getOpenFileName(window, "Open", filter="*.pdf")[0]
    open_path(path)

open_.triggered.connect(open_fn)
menu.addAction(open_)

open_path('zlib.clean.pdf')

window.show()
app.exec_()
