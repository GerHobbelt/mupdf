# Import Auto-generated MuPDF python bindings
#
import mupdf

import weakref
import hashlib
import typing

point_like = "point_like"
rect_like = "rect_like"
matrix_like = "matrix_like"
quad_like = "quad_like"
AnyType = typing.Any
OptInt = typing.Union[int, None]
OptFloat = typing.Optional[float]
OptStr = typing.Optional[str]
OptDict = typing.Optional[dict]
OptBytes = typing.Optional[typing.ByteString]
OptSeq = typing.Optional[typing.Sequence]

try:
    from pymupdf_fonts import fontdescriptors

    fitz_fontdescriptors = fontdescriptors.copy()
    del fontdescriptors
except ImportError:
    fitz_fontdescriptors = {}

def JM_rect_from_py(r):
    if not r or len(r) != 4:
        return mupdf.Rect(mupdf.Rect.Fixed_INFINITE)
    return mupdf.Rect(r[0], r[1], r[2], r[3])

TOOLS_JM_UNIQUE_ID = 0
class TOOLS:
    @staticmethod
    def gen_id():
        global TOOLS_JM_UNIQUE_ID
        TOOLS_JM_UNIQUE_ID += 1
        return TOOLS_JM_UNIQUE_ID

def DUMMY(*args, **kw):
    return


def JM_get_annot_xref_list(page_obj):
    '''
    Wrapper for PyMuPDF/fitz/helper-annot.i:
        PyObject *JM_get_annot_xref_list(fz_context *ctx, pdf_obj *page_obj)

    Not PyMuPDF/fitz/fitz_wrap.c:
        PyObject *JM_get_annot_xref_list(fz_context *ctx, pdf_page *page)
    '''
    names = []
    print(f'page_obj={page_obj}')
    print(f'mupdf.PDF_ENUM_NAME_Annots={mupdf.PDF_ENUM_NAME_Annots}')
    print(f'calling page_obj.dict_get()')
    annots = page_obj.dict_get(mupdf.PDF_ENUM_NAME_Annots)
    print(f'annots={annots}')
    if not annots:
        return names
    n = annots.array_len()
    print(f'n={n}')
    for i in range(n):
        annot_obj = mupdf.ppdf_array_get(annots, i)
        xref = mupdf.ppdf_to_num(annot_obj)
        print(f'mupdf.PDF_ENUM_NAME_Subtype={mupdf.PDF_ENUM_NAME_Subtype}')
        subtype = mupdf.ppdf_dict_get(annot_obj, mupdf.PDF_ENUM_NAME_Subtype)
        type_ = mupdf.PDF_ANNOT_UNKNOWN
        if (subtype):
            name = mupdf.ppdf_to_name(subtype)
            type_ = mupdf.ppdf_annot_type_from_string(name)
        id_ = mupdf.ppdf_dict_gets(annot_obj, "NM")
        names.append( (xref, type_, mupdf.ppdf_to_text_string(id_)) )
    return names

def dir_str(x):
    ret = f'{x} {type(x)} ({len(dir(x))}):\n'
    for i in dir(x):
        ret += f'    {i}\n'
    return ret

class Document:
    def __init__(self, filename=None, stream=None, filetype=None, rect=None, width=0, height=0, fontsize=11):
        """Creates a document. Use 'open' as a synonym.

        Notes:
            Basic usages:
            open() - new PDF document
            open(filename) - string or pathlib.Path, must have supported
                    file extension.
            open(type, buffer) - type: valid extension, buffer: bytes object.
            open(stream=buffer, filetype=type) - keyword version of previous.
            open(filename, fileype=type) - filename with unrecognized extension.
            rect, width, height, fontsize: layout reflowable document
            on open (e.g. EPUB). Ignored if n/a.
        """

        if not filename or type(filename) is str:
            pass
        else:
            filename = str(filename)  # takes care of pathlib.Path

        if stream:
            if not (filename or filetype):
                raise ValueError("need filetype for opening a stream")

            if type(stream) is bytes:
                self.stream = stream
            elif type(stream) is bytearray:
                self.stream = bytes(stream)
            elif type(stream) is io.BytesIO:
                self.stream = stream.getvalue()
            else:
                raise ValueError("bad type: 'stream'")
            stream = self.stream
        else:
            self.stream = None

        if filename and not stream:
            self.name = filename
        else:
            self.name = ""

        self.isClosed    = False
        self.isEncrypted = False
        self.metadata    = None
        self.FontInfos   = []
        self.Graftmaps   = {}
        self.ShownPages  = {}
        self.InsertedImages  = {}
        self._page_refs  = weakref.WeakValueDictionary()

        # this = _fitz.new_Document(filename, stream, filetype, rect, width, height, fontsize)
        #
        if 1:
            r = JM_rect_from_py(rect)
            if r.is_infinite_rect():
                w = width
                h = height
            else:
                w = r.x1 - r.x0
                h = r.y1 - r.y0

            if stream:
                this = mupdf.Document(filename if filename else filetype, stream)
                print(f'mupdf.Document() => this={this}')
            else:
                if filename:
                    if not filetype:
                        doc = mupdf.Document(filename)
                        print(f'mupdf.Document(filename) => doc={doc}')
                    else:
                        assert 0, 'recognize_document() not yet supported'
                else:
                    doc = mupdf.PdfDocument()
                    print(f'mupdf.PdfDocument() => doc={doc}')
                    doc.dirty = 1
            if w > 0 and h > 0:
                if isinstance(doc, mupdf.PdfDocument):
                    mupdf.layout_document(doc.m_internal.super, w, h, fontsize)
                else:
                    doc.layout_document(doc.m_internal.super, w, h, fontsize)
            else:
                if isinstance(doc, mupdf.PdfDocument):
                    reflowable = mupdf.is_document_reflowable(doc.m_internal.super)
                else:
                    reflowable = doc.is_document_reflowable()
                if reflowable:
                    if isinstance(doc, mupdf.PdfDocument):
                        mupdf.layout_document(doc.m_internal.super, 400, 600, 11)
                    else:
                        doc.layout_document(400, 600, 11)
            this = doc
            print( f'doc={doc} this={this}')

        #try:
        #    self.this.append(this)
        #except __builtin__.Exception:
        #    self.this = this
        self.this = this
        print( f'self.this={self.this}')

        # fixme: not sure where self.thisown gets initialised in PyMuPDF.
        #
        self.thisown = True

        if self.thisown:
            self._graft_id = TOOLS.gen_id()
            if self.needsPass is True:
                self.isEncrypted = True
            else: # we won't init until doc is decrypted
                self.initData()




    def close(self):

        """Close document."""
        if self.isClosed:
            raise ValueError("document closed")
        if hasattr(self, "_outline") and self._outline:
            self._outline = None
        self._reset_page_refs()
        self.metadata    = None
        self.stream      = None
        self.isClosed    = True
        self.FontInfos   = []
        self.Graftmaps = {}
        self.ShownPages = {}
        self.InsertedImages  = {}

        self.this = None
        #val = self.this.document_close()
        #val = _fitz.Document_close(self)
        #self.thisown = False
        #
        #return val


    def loadPage(self, page_id):

        """Load a page.

        'page_id' is either a 0-based page number or a tuple (chapter, pno),
        with chapter number and page number within that chapter.
        """

        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        if page_id is None:
            page_id = 0
        if page_id not in self:
            raise ValueError("page not in document")
        if type(page_id) is int and page_id < 0:
            np = self.pageCount
            while page_id < 0:
                page_id += np

        val = _fitz.this.load_page(page_id)

        val.thisown = True
        val.parent = weakref.proxy(self)
        self._page_refs[id(val)] = val
        val._annot_refs = weakref.WeakValueDictionary()
        val.number = page_id


        return val


    def _remove_links_to(self, first, last):
        return _fitz.Document__remove_links_to(self, first, last)

    def _loadOutline(self):
        """Load first outline."""
        if self.isClosed:
            raise ValueError("document closed")

        return self.this.load_outline()


    def _dropOutline(self, ol):
        return _fitz.Document__dropOutline(self, ol)

    def get_outline_xrefs(self):
        """Get list of outline xref numbers."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_get_outline_xrefs(self)


    def _extend_toc_items(self, items):
        """Add color info to all items of an extended TOC list."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document__extend_toc_items(self, items)


    def _embeddedFileNames(self, namelist):
        """Get list of embedded file names."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document__embeddedFileNames(self, namelist)


    def _embeddedFileDel(self, idx):
        return _fitz.Document__embeddedFileDel(self, idx)

    def _embeddedFileInfo(self, idx, infodict):
        return _fitz.Document__embeddedFileInfo(self, idx, infodict)

    def _embeddedFileUpd(self, idx, buffer=None, filename=None, ufilename=None, desc=None):
        return _fitz.Document__embeddedFileUpd(self, idx, buffer, filename, ufilename, desc)

    def _embeddedFileGet(self, idx):
        return _fitz.Document__embeddedFileGet(self, idx)

    def _embeddedFileAdd(self, name, buffer, filename=None, ufilename=None, desc=None):
        return _fitz.Document__embeddedFileAdd(self, name, buffer, filename, ufilename, desc)

    def embeddedFileNames(self) -> list:
        """Get list of names of EmbeddedFiles."""
        filenames = []
        self._embeddedFileNames(filenames)
        return filenames

    def _embeddedFileIndex(self, item: typing.Union[int, str]) -> int:
        filenames = self.embeddedFileNames()
        msg = "'%s' not in EmbeddedFiles array." % str(item)
        if item in filenames:
            idx = filenames.index(item)
        elif item in range(len(filenames)):
            idx = item
        else:
            raise ValueError(msg)
        return idx

    def embeddedFileCount(self) -> int:
        """Get number of EmbeddedFiles."""
        return len(self.embeddedFileNames())

    def embeddedFileDel(self, item: typing.Union[int, str]):
        """Delete an entry from EmbeddedFiles.

        Notes:
            The argument must be name or index of an EmbeddedFiles item.
            Physical deletion of data will happen on save to a new
            file with appropriate garbage option.
        Args:
            item: name or number of item.
        Returns:
            None
        """
        idx = self._embeddedFileIndex(item)
        return self._embeddedFileDel(idx)

    def embeddedFileInfo(self, item: typing.Union[int, str]) -> dict:
        """Get information of an item in the EmbeddedFiles array.

        Args:
            item: number or name of item.
        Returns:
            Information dictionary.
        """
        idx = self._embeddedFileIndex(item)
        infodict = {"name": self.embeddedFileNames()[idx]}
        self._embeddedFileInfo(idx, infodict)
        return infodict

    def embeddedFileGet(self, item: typing.Union[int, str]) -> bytes:
        """Get the content of an item in the EmbeddedFiles array.

        Args:
            item: number or name of item.
        Returns:
            (bytes) The file content.
        """
        idx = self._embeddedFileIndex(item)
        return self._embeddedFileGet(idx)

    def embeddedFileUpd(self, item: typing.Union[int, str],
                             buffer: OptBytes =None,
                             filename: OptStr =None,
                             ufilename: OptStr =None,
                             desc: OptStr =None,) -> None:
        """Change an item of the EmbeddedFiles array.

        Notes:
            Only provided parameters are changed. If all are omitted,
            the method is a no-op.
        Args:
            item: number or name of item.
            buffer: (binary data) the new file content.
            filename: (str) the new file name.
            ufilename: (unicode) the new filen ame.
            desc: (str) the new description.
        """
        idx = self._embeddedFileIndex(item)
        return self._embeddedFileUpd(idx, buffer=buffer,
                                     filename=filename,
                                     ufilename=ufilename,
                                     desc=desc)

    def embeddedFileAdd(self, name: str, buffer: typing.ByteString,
                              filename: OptStr =None,
                              ufilename: OptStr =None,
                              desc: OptStr =None,) -> None:
        """Add an item to the EmbeddedFiles array.

        Args:
            name: name of the new item, must not already exist.
            buffer: (binary data) the file content.
            filename: (str) the file name, default: the name
            ufilename: (unicode) the file name, default: filename
            desc: (str) the description.
        """
        filenames = self.embeddedFileNames()
        msg = "Name '%s' already in EmbeddedFiles array." % str(name)
        if name in filenames:
            raise ValueError(msg)

        if filename is None:
            filename = name
        if ufilename is None:
            ufilename = unicode(filename, "utf8") if str is bytes else filename
        if desc is None:
            desc = name
        return self._embeddedFileAdd(name, buffer=buffer,
                                     filename=filename,
                                     ufilename=ufilename,
                                     desc=desc)


    def convertToPDF(self, from_page=0, to_page=-1, rotate=0):
        """Convert document to a PDF, selecting page range and optional rotation. Output bytes object."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document_convertToPDF(self, from_page, to_page, rotate)


    def get_oc(self, xref):
        """Get xref of optional content object."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_get_oc(self, xref)


    def set_oc(self, xref, oc):
        """Attach optional content object to image or form xobject."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_set_oc(self, xref, oc)

    @property

    def pageCount(self):
        """Number of pages."""
        if self.isClosed:
            raise ValueError("document closed")
        return self.this.count_pages()

    @property

    def chapterCount(self):
        """Number of chapters."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_chapterCount(self)

    @property

    def lastLocation(self):
        """Id (chapter, page) of last page."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_lastLocation(self)


    def chapterPageCount(self, chapter):
        """Page count of chapter."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_chapterPageCount(self, chapter)


    def previousLocation(self, page_id):

        """Get (chapter, page) of previous page."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        if type(page_id) is int:
            page_id = (0, page_id)
        if page_id not in self:
            raise ValueError("page id not in document")
        if page_id  == (0, 0):
            return ()


        return _fitz.Document_previousLocation(self, page_id)


    def nextLocation(self, page_id):

        """Get (chapter, page) of next page."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        if type(page_id) is int:
            page_id = (0, page_id)
        if page_id not in self:
            raise ValueError("page id not in document")
        if tuple(page_id)  == self.lastLocation:
            return ()


        return _fitz.Document_nextLocation(self, page_id)


    def location_from_page_number(self, pno):
        """Convert pno to (chapter, page)."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_location_from_page_number(self, pno)


    def page_number_from_location(self, page_id):

        """Convert (chapter, pno) to page number."""
        if type(page_id) is int:
            np = self.pageCount
            while page_id < 0:
                page_id += np
            page_id = (0, page_id)
        if page_id not in self:
            raise ValueError("page id not in document")


        return _fitz.Document_page_number_from_location(self, page_id)


    def _getMetadata(self, key):
        """Get metadata."""
        if self.isClosed:
            raise ValueError("document closed")

        # return self.this _fitz.Document__getMetadata(self, key)
        if 1:
            return self.this.lookup_metadata(key)

    @property
    def needsPass(self):
        """Indicate password required."""
        if self.isClosed:
            raise ValueError("document closed")

        return self.this.needs_password()

    @property

    def language(self):
        """Document language."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_language(self)


    def setLanguage(self, language=None):
        return _fitz.Document_setLanguage(self, language)

    def resolveLink(self, uri=None, chapters=0):

        """Calculate internal link destination.

        Args:
            uri: (str) some Link.uri
            chapters: (bool) whether to use (chapter, page) format
        Returns:
            (page_id, x, y) where x, y are point coordinates on the page.
            page_id is either page number (if chapters=0), or (chapter, pno).
        """


        return _fitz.Document_resolveLink(self, uri, chapters)


    def layout(self, rect=None, width=0, height=0, fontsize=11):
        """Re-layout a reflowable document."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        val = _fitz.Document_layout(self, rect, width, height, fontsize)

        self._reset_page_refs()
        self.initData()

        return val


    def makeBookmark(self, loc):
        """Make a page pointer before layouting document."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document_makeBookmark(self, loc)


    def findBookmark(self, bm):
        """Find new location after layouting a document."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document_findBookmark(self, bm)

    @property

    def isReflowable(self):
        """Check if document is layoutable."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_isReflowable(self)


    def _deleteObject(self, xref):
        """Delete object."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document__deleteObject(self, xref)


    def pdf_catalog(self):
        """Get xref of PDF catalog."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_pdf_catalog(self)


    def _getPDFfileid(self):
        """Get PDF file id."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document__getPDFfileid(self)

    @property

    def isPDF(self):
        """Check for PDF."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_isPDF(self)

    @property

    def _hasXrefStream(self):
        """Check if xref table is a stream."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document__hasXrefStream(self)

    @property

    def _hasXrefOldStyle(self):
        """Check if xref table is old style."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document__hasXrefOldStyle(self)

    @property

    def isDirty(self):
        """True if PDF has unsaved changes."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_isDirty(self)


    def can_save_incrementally(self):
        """Check whether incremental saves are possible."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_can_save_incrementally(self)


    def authenticate(self, password):
        """Decrypt document."""
        if self.isClosed:
            raise ValueError("document closed")

        val = _fitz.Document_authenticate(self, password)

        if val:  # the doc is decrypted successfully and we init the outline
            self.isEncrypted = False
            self.initData()
            self.thisown = True


        return val


    def save(self, filename, garbage=0, clean=0, deflate=0, deflate_images=0, deflate_fonts=0, incremental=0, ascii=0, expand=0, linear=0, pretty=0, encryption=1, permissions=-1, owner_pw=None, user_pw=None):

        """Save PDF to filename."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        if type(filename) == str:
            pass
        elif str is bytes and type(filename) == unicode:
            filename = filename.encode('utf8')
        else:
            filename = str(filename)
        if filename == self.name and not incremental:
            raise ValueError("save to original must be incremental")
        if self.pageCount < 1:
            raise ValueError("cannot save with zero pages")
        if incremental:
            if self.name != filename or self.stream:
                raise ValueError("incremental needs original file")


        return _fitz.Document_save(self, filename, garbage, clean, deflate, deflate_images, deflate_fonts, incremental, ascii, expand, linear, pretty, encryption, permissions, owner_pw, user_pw)


    def write(self, garbage=0, clean=0, deflate=0, deflate_images=0, deflate_fonts=0, ascii=0, expand=0, pretty=0, encryption=1, permissions=-1, owner_pw=None, user_pw=None):

        """Write the PDF to a bytes object."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        if self.pageCount < 1:
            raise ValueError("cannot write with zero pages")

        return _fitz.Document_write(self, garbage, clean, deflate, deflate_images, deflate_fonts, ascii, expand, pretty, encryption, permissions, owner_pw, user_pw)


    def insertPDF(self, docsrc, from_page=-1, to_page=-1, start_at=-1, rotate=-1, links=1, annots=1, show_progress=0, final=1, _gmap=None):

        """Insert a page range from another PDF.

        Args:
            docsrc: PDF to copy from. Must be different object, but may be same file.
            from_page: (int) first source page to copy, 0-based, default 0.
            to_page: (int) last source page to copy, 0-based, default last page.
            start_at: (int) from_page will become this page number in target.
            rotate: (int) rotate copied pages, default -1 is no change.
            links: (int/bool) whether to also copy links.
            annots: (int/bool) whether to also copy annotations.
            show_progress: (int) progress message interval, 0 is no messages.
            final: (bool) indicates last insertion from this source PDF.
            _gmap: internal use only

        Copy sequence reversed if from_page > to_page."""

        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        if self._graft_id == docsrc._graft_id:
            raise ValueError("source and target cannot be same object")
        sa = start_at
        if sa < 0:
            sa = self.pageCount
        if len(docsrc) > show_progress > 0:
            inname = os.path.basename(docsrc.name)
            if not inname:
                inname = "memory PDF"
            outname = os.path.basename(self.name)
            if not outname:
                outname = "memory PDF"
            print("Inserting '%s' at '%s'" % (inname, outname))

        # retrieve / make a Graftmap to avoid duplicate objects
        isrt = docsrc._graft_id
        _gmap = self.Graftmaps.get(isrt, None)
        if _gmap is None:
            _gmap = Graftmap(self)
            self.Graftmaps[isrt] = _gmap


        val = _fitz.Document_insertPDF(self, docsrc, from_page, to_page, start_at, rotate, links, annots, show_progress, final, _gmap)

        self._reset_page_refs()
        if links:
            self._do_links(docsrc, from_page = from_page, to_page = to_page,
                        start_at = sa)
        if final == 1:
            self.Graftmaps[isrt] = None

        return val


    def _newPage(self, pno=-1, width=595, height=842):
        """Make a new PDF page."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        val = _fitz.Document__newPage(self, pno, width, height)
        self._reset_page_refs()

        return val


    def select(self, pyliste):
        """Build sub-pdf with page numbers in the list."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        if not self.isPDF:
            raise ValueError("not a PDF")
        if not hasattr(pyliste, "__getitem__"):
            raise ValueError("sequence required")
        if len(pyliste) == 0 or min(pyliste) not in range(len(self)) or max(pyliste) not in range(len(self)):
            raise ValueError("bad page number(s)")

        val = _fitz.Document_select(self, pyliste)
        self._reset_page_refs()

        return val


    def _deletePage(self, pno):
        return _fitz.Document__deletePage(self, pno)
    @property

    def permissions(self):

        """Document permissions."""

        if self.isEncrypted:
            return 0


        return _fitz.Document_permissions(self)


    def _getCharWidths(self, xref, bfname, ext, ordering, limit, idx=0):
        """Return list of glyphs and glyph widths of a font."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document__getCharWidths(self, xref, bfname, ext, ordering, limit, idx)


    def page_xref(self, pno):
        """Get xref of page number."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_page_xref(self, pno)


    def pageCropBox(self, pno):
        """Get CropBox of page number (without loading page)."""
        if self.isClosed:
            raise ValueError("document closed")

        val = _fitz.Document_pageCropBox(self, pno)
        val = Rect(val)

        return val


    def _getPageInfo(self, pno, what):
        """List fonts, images, XObjects used on a page."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document__getPageInfo(self, pno, what)


    def extractFont(self, xref=0, info_only=0):
        """Get a font by xref."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document_extractFont(self, xref, info_only)


    def extractImage(self, xref):
        """Get image by xref. Returns a dictionary."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document_extractImage(self, xref)


    def _delToC(self):
        """Delete the TOC."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        val = _fitz.Document__delToC(self)
        self.initData()

        return val


    def isStream(self, xref=0):
        """Check if xref is a stream object."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_isStream(self, xref)


    def need_appearances(self, value=None):
        """Get/set the NeedAppearances value."""
        if self.isClosed:
            raise ValueError("document closed")
        if not self.isFormPDF:
            return None


        return _fitz.Document_need_appearances(self, value)


    def getSigFlags(self):
        """Get the /SigFlags value."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_getSigFlags(self)

    @property

    def isFormPDF(self):
        """Check if PDF Form document."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_isFormPDF(self)

    @property

    def FormFonts(self):
        """Get list of field font resource names."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_FormFonts(self)


    def _addFormFont(self, name, font):
        """Add new form font."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document__addFormFont(self, name, font)


    def _getOLRootNumber(self):
        """Get xref of Outline Root, create it if missing."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document__getOLRootNumber(self)


    def get_new_xref(self):
        """Make new xref."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document_get_new_xref(self)


    def xref_length(self):
        """Get length of xref table."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_xref_length(self)


    def getXmlMetadata(self):
        """Get document XML metadata."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_getXmlMetadata(self)


    def xref_xml_metadata(self):
        """Get xref of document XML metadata."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_xref_xml_metadata(self)


    def del_xml_metadata(self):
        """Delete XML metadata."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document_del_xml_metadata(self)


    def set_xml_metadata(self, metadata):
        """Store XML document level metadata."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document_set_xml_metadata(self, metadata)

    setXmlMetadata = set_xml_metadata

    def xref_object(self, xref, compressed=0, ascii=0):
        """Get xref object source as a string."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_xref_object(self, xref, compressed, ascii)


    def pdf_trailer(self, compressed=0, ascii=0):
        """Get PDF trailer as a string."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_pdf_trailer(self, compressed, ascii)


    def xref_stream_raw(self, xref):
        """Get xref stream without decompression."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document_xref_stream_raw(self, xref)


    def xref_stream(self, xref):
        """Get decompressed xref stream."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document_xref_stream(self, xref)


    def update_object(self, xref, text, page=None):
        """Replace object definition source."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document_update_object(self, xref, text, page)


    def update_stream(self, xref=0, stream=None, new=0):
        """Replace xref stream part."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document_update_stream(self, xref, stream, new)


    def _setMetadata(self, text):
        """Set old style metadata."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document__setMetadata(self, text)


    def _make_page_map(self):
        """Make an array page number -> page object."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document__make_page_map(self)


    def fullcopyPage(self, pno, to=-1):
        """Make full page duplication."""
        if self.isClosed:
            raise ValueError("document closed")

        val = _fitz.Document_fullcopyPage(self, pno, to)
        self._reset_page_refs()

        return val


    def _move_copy_page(self, pno, nb, before, copy):
        """Move or copy a PDF page reference."""
        if self.isClosed:
            raise ValueError("document closed")

        val = _fitz.Document__move_copy_page(self, pno, nb, before, copy)
        self._reset_page_refs()

        return val


    def _remove_toc_item(self, xref):
        return _fitz.Document__remove_toc_item(self, xref)

    def _update_toc_item(self, xref, action=None, title=None, flags=0, collapse=None, color=None):
        return _fitz.Document__update_toc_item(self, xref, action, title, flags, collapse, color)

    def _get_page_labels(self):
        return _fitz.Document__get_page_labels(self)

    def _set_page_labels(self, labels):
        val = _fitz.Document__set_page_labels(self, labels)

        xref = self.pdf_catalog()
        text = self.xref_object(xref, compressed=True)
        text = text.replace("/Nums[]", "/Nums[%s]" % labels)
        self.update_object(xref, text)

        return val


    def get_layers(self):
        """Show optional OC layers."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_get_layers(self)


    def switch_layer(self, config, as_default=0):
        """Activate an OC layer."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_switch_layer(self, config, as_default)


    def get_layer(self, config=-1):
        """Content of ON, OFF, RBGroups of an OC layer."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_get_layer(self, config)


    def set_layer(self, config, basestate=None, on=None, off=None, rbgroups=None):
        """Set the PDF keys /ON, /OFF, /RBGroups of an OC layer."""
        if self.isClosed:
            raise ValueError("document closed")
        ocgs = set(self.get_ocgs().keys())
        if ocgs == set():
            raise ValueError("document has no optional content")

        if on:
            if type(on) not in (list, tuple):
                raise ValueError("bad type: 'on'")
            s = set(on).difference(ocgs)
            if s != set():
                raise ValueError("bad OCGs in 'on': %s" % s)

        if off:
            if type(off) not in (list, tuple):
                raise ValueError("bad type: 'off'")
            s = set(off).difference(ocgs)
            if s != set():
                raise ValueError("bad OCGs in 'off': %s" % s)

        if rbgroups:
            if type(rbgroups) not in (list, tuple):
                raise ValueError("bad type: 'rbgroups'")
            for x in rbgroups:
                if not type(x) in (list, tuple):
                    raise ValueError("bad RBGroup '%s'" % x)
                s = set(x).difference(ocgs)
                if f != set():
                    raise ValueError("bad OCGs in RBGroup: %s" % s)

        if basestate:
            basestate = str(basestate).upper()
            if basestate == "UNCHANGED":
                basestate = "Unchanged"
            if basestate not in ("ON", "OFF", "Unchanged"):
                raise ValueError("bad 'basestate'")


        return _fitz.Document_set_layer(self, config, basestate, on, off, rbgroups)


    def add_layer(self, name, creator=None, on=None):
        """Add a new OC layer."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_add_layer(self, name, creator, on)


    def layer_ui_configs(self):
        """Show OC visibility status modifyable by user."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_layer_ui_configs(self)


    def set_layer_ui_config(self, number, action=0):
        """Set / unset OC intent configuration."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_set_layer_ui_config(self, number, action)


    def get_ocgs(self):
        """Show existing optional content groups."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_get_ocgs(self)

    getOCGs = get_ocgs

    def add_ocg(self, name, config=-1, on=1, intent=None, usage=None):
        """Add new optional content group."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_add_ocg(self, name, config, on, intent, usage)

    addOCG = add_ocg

    def initData(self):
        if self.isEncrypted:
            raise ValueError("cannot initData - document still encrypted")
        self._outline = self._loadOutline()
        self.metadata = dict([(k,self._getMetadata(v)) for k,v in {'format':'format', 'title':'info:Title', 'author':'info:Author','subject':'info:Subject', 'keywords':'info:Keywords','creator':'info:Creator', 'producer':'info:Producer', 'creationDate':'info:CreationDate', 'modDate':'info:ModDate', 'trapped':'info:Trapped'}.items()])
        self.metadata['encryption'] = None if self._getMetadata('encryption')=='None' else self._getMetadata('encryption')

    outline = property(lambda self: self._outline)
    _getPageXref = page_xref
    pageXref = page_xref



    def get_page_fonts(self, pno: int, full: bool =False) -> list:
        """Retrieve a list of fonts used on a page.
        """
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        if not self.isPDF:
            return ()
        val = self._getPageInfo(pno, 1)
        if full is False:
            return [v[:-1] for v in val]
        return val

    getPageFontList = get_page_fonts

    def get_page_images(self, pno: int, full: bool =False) -> list:
        """Retrieve a list of images used on a page.
        """
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        if not self.isPDF:
            return ()
        val = self._getPageInfo(pno, 2)
        if full is False:
            return [v[:-1] for v in val]
        return val

    getPageImageList = get_page_images

    def get_page_xobjects(self, pno: int) -> list:
        """Retrieve a list of XObjects used on a page.
        """
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        if not self.isPDF:
            return ()
        val = self._getPageInfo(pno, 3)
        return val

    getPageXObjectList = get_page_xobjects

    def copyPage(self, pno: int, to: int =-1):
        """Copy a page within a PDF document.

        Args:
            pno: source page number
            to: put before this page, '-1' means after last page.
        """
        if self.isClosed:
            raise ValueError("document closed")

        pageCount = len(self)
        if (
            pno not in range(pageCount) or
            to not in range(-1, pageCount)
           ):
            raise ValueError("bad page number(s)")
        before = 1
        copy = 1
        if to == -1:
            to = pageCount - 1
            before = 0

        return self._move_copy_page(pno, to, before, copy)

    def movePage(self, pno: int, to: int =-1):
        """Move a page within a PDF document.

        Args:
            pno: source page number.
            to: put before this page, '-1' means after last page.
        """
        if self.isClosed:
            raise ValueError("document closed")

        pageCount = len(self)
        if (
            pno not in range(pageCount) or
            to not in range(-1, pageCount)
           ):
            raise ValueError("bad page number(s)")
        before = 1
        copy = 0
        if to == -1:
            to = pageCount - 1
            before = 0

        return self._move_copy_page(pno, to, before, copy)

    def deletePage(self, pno: int =-1):
        """ Delete one page from a PDF.
        """
        if not self.isPDF:
            raise ValueError("not a PDF")
        if self.isClosed:
            raise ValueError("document closed")

        pageCount = self.pageCount
        while pno < 0:
            pno += pageCount

        if not pno in range(pageCount):
            raise ValueError("bad page number(s)")

    # remove TOC bookmarks pointing to deleted page
        old_toc = self.getToC()
        for i, item in enumerate(old_toc):
            if item[2] == pno + 1:
                xref = self.outline_xref(i)
                self._remove_toc_item(xref)

        self._remove_links_to(pno, pno)
        self._deletePage(pno)
        self._reset_page_refs()



    def deletePageRange(self, from_page: int =-1, to_page: int =-1):
        """Delete pages from a PDF.
        """
        if not self.isPDF:
            raise ValueError("not a PDF")
        if self.isClosed:
            raise ValueError("document closed")

        pageCount = self.pageCount  # page count of document
        f = from_page  # first page to delete
        t = to_page  # last page to delete
        while f < 0:
            f += pageCount
        while t < 0:
            t += pageCount
        if not f <= t < pageCount:
            raise ValueError("bad page number(s)")

        old_toc = self.getToC()
        for i, item in enumerate(old_toc):
            if f + 1 <= item[2] <= t + 1:
                xref = self.outline_xref(i)
                self._remove_toc_item(xref)

        self._remove_links_to(f, t)

        for i in range(t, f - 1, -1):  # delete pages, last to first
            self._deletePage(i)

        self._reset_page_refs()


    def saveIncr(self):
        """ Save PDF incrementally"""
        return self.save(self.name, incremental=True, encryption=PDF_ENCRYPT_KEEP)


    def reload_page(self, page: "struct Page *") -> "struct Page *":
        """Make a fresh copy of a page."""
        old_annots = {}  # copy annot references to here
        pno = page.number  # save the page number
        for k, v in page._annot_refs.items():  # save the annot dictionary
            old_annots[k] = v
        page._erase()  # remove the page
        page = None
        page = self.loadPage(pno)  # reload the page

    # copy annot refs over to the new dictionary
        page_proxy = weakref.proxy(page)
        for k, v in old_annots.items():
            annot = old_annots[k]
            annot.parent = page_proxy  # refresh parent to new page
            page._annot_refs[k] = annot
        return page


    updateObject = update_object
    updateStream = update_stream
    xrefStream = xref_stream
    xrefStreamRaw = xref_stream_raw
    xrefObject = xref_object
    xrefLength = xref_length
    PDFTrailer = pdf_trailer
    PDFCatalog = pdf_catalog
    metadataXML = xref_xml_metadata


    def __repr__(self) -> str:
        m = "closed " if self.isClosed else ""
        if self.stream is None:
            if self.name == "":
                return m + "Document(<new PDF, doc# %i>)" % self._graft_id
            return m + "Document('%s')" % (self.name,)
        return m + "Document('%s', <memory, doc# %i>)" % (self.name, self._graft_id)


    def __contains__(self, loc) -> bool:
        if type(loc) is int:
            if loc < self.pageCount:
                return True
            return False
        if type(loc) not in (tuple, list) or len(loc) != 2:
            return False

        chapter, pno = loc
        if (type(chapter) != int or
            chapter < 0 or
            chapter >= self.chapterCount
            ):
            return False
        if (type(pno) != int or
            pno < 0 or
            pno >= self.chapterPageCount(chapter)
            ):
            return False

        return True


    def __getitem__(self, i: int =0):
        if i not in self:
            raise IndexError("page not in document")
        return self.loadPage(i)

    def pages(self, start: OptInt =None, stop: OptInt =None, step: OptInt =None) -> "struct Page *":
        """Return a generator iterator over a page range.

        Arguments have the same meaning as for the range() built-in.
        """
    # set the start value
        start = start or 0
        while start < 0:
            start += self.pageCount
        if start not in range(self.pageCount):
            raise ValueError("bad start page number")

    # set the stop value
        stop = stop if stop is not None and stop <= self.pageCount else self.pageCount

    # set the step value
        if step == 0:
            raise ValueError("arg 3 must not be zero")
        if step is None:
            if start > stop:
                step = -1
            else:
                step = 1

        for pno in range(start, stop, step):
            yield (self.loadPage(pno))


    def __len__(self) -> int:
        return self.pageCount

    def _forget_page(self, page: "struct Page *"):
        """Remove a page from document page dict."""
        pid = id(page)
        if pid in self._page_refs:
            self._page_refs[pid] = None

    def _reset_page_refs(self):
        """Invalidate all pages in document dictionary."""
        if self.isClosed:
            return
        self._page_refs.clear()

    def __del__(self):
        if hasattr(self, "_reset_page_refs"):
            self._reset_page_refs()
        if hasattr(self, "this") and self.thisown:
            self.thisown = False

        self.Graftmaps = {}
        self.ShownPages = {}
        self.InsertedImages  = {}
        self.stream = None
        self._reset_page_refs = DUMMY
        self.__swig_destroy__ = DUMMY
        self.isClosed = True

    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.close()

    def page_annot_xrefs(self, n):
        page_count = self.this.count_pages()
        while n < 0:
            n += page_count
        print(f'self.this is: {self.this}')
        print(f'self.this.m_internal={self.this.m_internal}')
        if 0:
            print(f'dir(self.this):')
            for i in dir(self.this):
                print(f'    {i}')
        print(f'self.this.specifics is: {self.this.specifics}')
        if isinstance(self.this, mupdf.PdfDocument):
            print(f'self.this is a mupdf.PdfDocument')
            pdf_document = self.this
        else:
            print(f'self.this is not a mupdf.PdfDocument: {self.this}')
            print(f'self.this.m_internal={self.this.m_internal}')
            pdf_document = self.this.specifics()
            print(f'self.this.specifics() => pdf_document={pdf_document}')
        page_obj = pdf_document.lookup_page_obj(n)
        #print(f'page_obj: {dir_str(page_obj)}')
        #print(f'page_obj.m_internal: {dir_str(page_obj.m_internal)}')
        annots = JM_get_annot_xref_list(page_obj)
        return annots


    def has_links(self):
        """Check whether there are links on any page."""
        print('has_links()')
        if self.isClosed:
            raise ValueError("document closed")
        #if not self.this.is_pdf:
        #    raise ValueError("not a PDF")
        for i in range(self.pageCount):
            for item in self.page_annot_xrefs(i):
                if item[1] == PDF_ANNOT_LINK:
                    print('Returning true')
                    return True
        print('Returning false')
        return False


open = Document

if __name__ == '__main__':
    pass
