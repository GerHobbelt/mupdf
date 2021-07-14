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
        self.central_widget = PyQt5.QtWidgets.QLabel(self)
        self.scroll_area = PyQt5.QtWidgets.QScrollArea()
        self.scroll_area.setWidget(self.central_widget)
        self.scroll_area.setWidgetResizable(True)
        self.setCentralWidget(self.scroll_area)

        # Need to preserve menu_file_open, otherwise it doesn't appear in the
        # menu.
        #
        self.menu_file_open = PyQt5.QtWidgets.QAction('&Open')
        self.menu_file_open.triggered.connect(self.open_)

        self.menu_file_show_html = PyQt5.QtWidgets.QAction('&Show html')
        self.menu_file_show_html.triggered.connect(self.show_html)

        self.menu_file_quit = PyQt5.QtWidgets.QAction('&Quit')
        self.menu_file_quit.triggered.connect(self.quit)

        menu_file = self.menuBar().addMenu('&File')
        menu_file.addAction(self.menu_file_open)
        menu_file.addAction(self.menu_file_show_html)
        menu_file.addAction(self.menu_file_quit)

    def keyPressEvent(self, event):
        #print(f'event.key()={event.key()}')
        if self.page_number is not None:
            if 0:
                pass
            elif event.key() == PyQt5.Qt.Qt.Key_PageUp:
                self.goto_page(page_number=self.page_number - 1)
            elif event.key() == PyQt5.Qt.Qt.Key_PageDown:
                self.goto_page(page_number=self.page_number + 1)
            elif event.key() in (ord('='), ord('+')):
                self.goto_page(zoom=self.zoom + 1)
            elif event.key() in (ord('-'), ord('_')):
                self.goto_page(zoom=self.zoom - 1)
            elif event.key() == (ord('0')):
                self.goto_page(zoom=0)

    def resizeEvent(self, event):
        self.goto_page(self.page_number, self.zoom)
        #print(f'resizeEvent(): oldSize={event.oldSize()} size={event.size()}')
        #print(f'minimumSize()={self.minimumSize()}')

    def show_html(self):
        if 1:
            buffer_ = self.page.document_data_from_page("docx", "html", mupdf.Matrix(1, 0, 0, 1, 0, 0), mupdf.Cookie())
        else:
            buffer_ = mupdf.Buffer(0)
            output = mupdf.Output(buffer_)
            docx_writer = mupdf.DocumentWriter(output, "docx", "html")
            device = docx_writer.begin_page(self.page.bound_page())
            self.page.run_page(device, mupdf.Matrix(1, 0, 0, 1, 0, 0), mupdf.Cookie())
            docx_writer.end_page()
            docx_writer.close_document_writer()

        html_content = buffer_.buffer_extract().decode('utf8')
        #print(f'buffer_.buffer_extract() => {html_content}')
        self.webview = PyQt5.QtWebKitWidgets.QWebView()
        self.webview.setHtml(html_content)
        self.webview.show()

    def open_(self):
        path = PyQt5.QtWidgets.QFileDialog.getOpenFileName(self, 'Open', filter='*.pdf')[0]
        if path:
            self.open_path(path)

    def quit(self):
        sys.exit()

    def goto_page(self, page_number=None, zoom=None):
        if page_number is None:
            page_number = self.page_number
        if zoom is None:
            zoom = self.zoom
        try:
            self.page = mupdf.Page(self.document, page_number)
        except Exception as e:
            print(f'Cannot go to page {page_number}: {e}')
            return
        page_rect = self.page.bound_page()
        z = 2**(zoom / self.zoom_multiple)
        # Using -2 here avoids always-present horizontal scrollbar; not sure
        # why...
        z *= (self.centralWidget().size().width() - 2) / (page_rect.x1 - page_rect.x0)
        try:
            self.pixmap = self.page.new_pixmap_from_page_contents(
                    ctm=mupdf.Matrix(z, 0, 0, z, 0, 0),
                    cs=mupdf.Colorspace(mupdf.Colorspace.Fixed_RGB),
                    alpha=0,
                    )
        except Exception as e:
            print(f'self.page.new_pixmap_from_page_contents() failed: {e}')
            return
        # Need to preserve <pixmap> after we return because <image> will refer to
        # it.
        #print(f'pixmap.pixmap_width()={pixmap.pixmap_width()} pixmap.pixmap_height()={pixmap.pixmap_height()} pixmap.pixmap_stride()={pixmap.pixmap_stride()}')
        image = PyQt5.QtGui.QImage(
                int(self.pixmap.pixmap_samples()),
                self.pixmap.pixmap_width(),
                self.pixmap.pixmap_height(),
                self.pixmap.pixmap_stride(),
                PyQt5.QtGui.QImage.Format_RGB888,
                );
        qpixmap = PyQt5.QtGui.QPixmap.fromImage(image)
        self.central_widget.setPixmap(qpixmap)
        self.page_number = page_number
        self.zoom = zoom

    def open_path(self, path):
        try:
            self.document = mupdf.Document(path)
        except Exception:
            print(f'Failed to open path={path!r}')
            return
        self.goto_page(page_number=0, zoom=0)


app = PyQt5.QtWidgets.QApplication([])

main_window = MainWindow()

args = iter(sys.argv[1:])
while 1:
    try:
        arg = next(args)
    except StopIteration:
        break
    if arg.startswith('-'):
        if arg == '--html':
            main_window.show_html()
        else:
            raise Exception(f'Unrecognised arg: {arg}')
    else:
        main_window.open_path(arg)

#main_window.open_path('zlib.clean.pdf')
#main_window.show_html()

main_window.show()
app.exec_()
