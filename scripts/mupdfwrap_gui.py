#! /usr/bin/env python3

'''
Basic PDF viewer using PyQt and MuPDF.

Example usage:
    PYTHONPATH=build/shared-debug-extract ./scripts/mupdfwrap_gui.py
'''
import sys

import mupdf

import PyQt5
import PyQt5.QtWidgets
import PyQt5.Qt


class MainWindow(PyQt5.QtWidgets.QMainWindow):

    def __init__(self):
        super().__init__()
        self.page_number = None
        self.zoom_multiple = 4
        self.zoom = 0
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

    def keyPressEvent(self, event):
        print(f'event.key()={event.key()}')
        if self.page_number is not None:
            if event.key() == PyQt5.Qt.Qt.Key_PageUp:
                self.goto_page(self.page_number - 1)
            elif event.key() == PyQt5.Qt.Qt.Key_PageDown:
                self.goto_page(self.page_number + 1)
            elif event.key() in (ord('='), ord('+')):
                self.goto_page(self.page_number, self.zoom + 1)
            elif event.key() in (ord('-'), ord('_')):
                self.goto_page(self.page_number, self.zoom - 1)
            elif event.key() == (ord('0')):
                self.goto_page(self.page_number, 0)

    def open_(self):
        path = PyQt5.QtWidgets.QFileDialog.getOpenFileName(self, 'Open', filter='*.pdf')[0]
        if path:
            self.open_path(path)

    def quit(self):
        sys.exit()

    def goto_page(self, page_number, zoom=None):
        if zoom is None:
            zoom = self.zoom
        try:
            page = mupdf.Page(self.document, page_number)
        except Exception as e:
            print(f'Cannot go to page {page_number}: {e}')
            return
        z = 2**(zoom / self.zoom_multiple)
        ctm = mupdf.Matrix(z, 0, 0, z, 0, 0)
        colorspace = mupdf.Colorspace(mupdf.Colorspace.Fixed_RGB)
        alpha = 0
        pixmap = page.new_pixmap_from_page_contents(ctm, colorspace, alpha)
        # Need to preserve <pixmap> after we return because <image> will refer to
        # it.
        self.pixmap = pixmap
        print(f'pixmap.pixmap_width()={pixmap.pixmap_width()} pixmap.pixmap_height()={pixmap.pixmap_height()} pixmap.pixmap_stride()={pixmap.pixmap_stride()}')
        image = PyQt5.QtGui.QImage(
                int(pixmap.pixmap_samples()),
                pixmap.pixmap_width(),
                pixmap.pixmap_height(),
                pixmap.pixmap_stride(),
                PyQt5.QtGui.QImage.Format_RGB888,
                );
        qpixmap = PyQt5.QtGui.QPixmap.fromImage(image)
        self.centralWidget().setPixmap(qpixmap)
        self.page_number = page_number
        self.zoom = zoom

    def open_path(self, path):
        try:
            self.document = mupdf.Document(path)
        except Exception:
            print(f'Failed to open path={path!r}')
            return
        self.goto_page(0)


app = PyQt5.QtWidgets.QApplication([])

main_window = MainWindow()

main_window.open_path('zlib.clean.pdf')

main_window.show()
app.exec_()
