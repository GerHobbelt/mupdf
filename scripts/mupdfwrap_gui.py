#! /usr/bin/env python3

'''
Example usage:
    PYTHONPATH=build/shared-debug-extract ./scripts/mupdfwrap_gui.py
'''
import sys

import mupdf

import PyQt5
import PyQt5.QtWidgets


class MainWindow(PyQt5.QtWidgets.QMainWindow):

    def __init__(self):
        super().__init__()
        self.pixmap = None
        self.setCentralWidget(PyQt5.QtWidgets.QLabel(''))

        # Need to preserve menu_file_open, otherwise it doesn't appear in the
        # menu.
        #
        self.menu_file_open = PyQt5.QtWidgets.QAction('&Open')
        self.menu_file_open.triggered.connect(self.open_)

        self.menu_file_quit = PyQt5.QtWidgets.QAction('&Quit')
        self.menu_file_quit.triggered.connect(self.quit)

        menu_file = self.menuBar().addMenu('&File')
        menu_file.addAction(self.menu_file_open)
        menu_file.addAction(self.menu_file_quit)

    def open_(self):
        path = PyQt5.QtWidgets.QFileDialog.getOpenFileName(self, 'Open', filter='*.pdf')[0]
        if path:
            self.open_path(path)

    def quit(self):
        sys.exit()

    def open_path(self, path):
        try:
            document = mupdf.Document(path)
        except Exception:
            print(f'Failed to open path={path!r}')
            return
        page = mupdf.Page(document, 0)
        ctm = mupdf.Matrix(1, 0, 0, 1, 0, 0)
        colorspace = mupdf.Colorspace(mupdf.Colorspace.Fixed_RGB)
        alpha = 0
        pixmap = page.new_pixmap_from_page_contents(ctm, colorspace, alpha)
        # Need to preserve <pixmap> after we return because <image> will refer to
        # it.
        self.pixmap = pixmap
        print(f'pixmap.pixmap_width()={pixmap.pixmap_width()} pixmap.pixmap_height()={pixmap.pixmap_height()}')
        image = PyQt5.QtGui.QImage(
                int(pixmap.pixmap_samples()),
                pixmap.pixmap_width(),
                pixmap.pixmap_height(),
                PyQt5.QtGui.QImage.Format_RGB888,
                );
        qpixmap = PyQt5.QtGui.QPixmap.fromImage(image)
        self.centralWidget().setPixmap(qpixmap)

app = PyQt5.QtWidgets.QApplication([])

main_window = MainWindow()

main_window.open_path('zlib.clean.pdf')

main_window.show()
app.exec_()
