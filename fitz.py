# Import Auto-generated MuPDF python bindings
#

import sys

sys.path.append('scripts')
import jlib

import mupdf

import weakref
import hashlib
import typing


def _swig_setattr_nondynamic(self, class_type, name, value, static=1):
    if name == "thisown":
        return self.this.own(value)
    if name == "this":
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name, None)
    if method:
        return method(self, value)
    if not static:
        object.__setattr__(self, name, value)
    else:
        raise AttributeError("You cannot add attributes to %s" % self)


def _swig_setattr(self, class_type, name, value):
    return _swig_setattr_nondynamic(self, class_type, name, value, 0)


def _swig_getattr(self, class_type, name):
    if (name == "thisown"):
        return self.this.own()
    method = class_type.__swig_getmethods__.get(name, None)
    if method:
        return method(self)
    raise AttributeError("'%s' object has no attribute '%s'" % (class_type.__name__, name))


def _swig_repr(self):
    try:
        strthis = "proxy of " + self.this.__repr__()
    except __builtin__.Exception:
        strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)


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

def JM_read_contents(pageref):
    assert isinstance(pageref, mupdf.PdfObj), f'{type(pageref)}'
    contents = pageref.dict_get(mupdf.PDF_ENUM_NAME_Contents)
    if contents.is_array():
        res = mupdf.Buffer(1024)
        for i in range(contents.array_len()):
            obj = contents.array_get(i)
            nres = obj.load_stream()
            res.append_buffer(nres)
    elif contents.m_internal:
        res = contents.load_stream()
    return res

def JM_BinFromBuffer(buffer_):
    assert isinstance(buffer_, mupdf.Buffer)
    print('JM_BinFromBuffer', file=sys.stderr)
    return buffer_.buffer_extract()


# compress char* into a new buffer
def JM_compress_buffer(inbuffer):
    data, compressed_length = mupdf.new_deflated_data_from_buffer(
            inbuffer.m_internal,
            mupdf.FZ_DEFLATE_BEST,
            )
    jlib.log('{data=} {compressed_length=}')
    if not data or compressed_length == 0:
        return None
    buf = mupdf.Buffer(mupdf.new_buffer_from_data(data, compressed_length))
    buf.resize_buffer(compressed_length)
    return buf;

def JM_update_stream(doc, obj, buffer_, compress):
    jlib.log('{dir(buffer_)}')
    len_, _ = buffer_.buffer_storage_raw()
    nlen = len_

    if len_ > 30:   # ignore small stuff
        nres = JM_compress_buffer(buffer_)
        assert isinstance(nres, mupdf.Buffer)
        nlen, _ = nres.buffer_storage_raw()

    if nlen < len_ and nres and compress==1:   # was it worth the effort?
        obj.dict_put(
                mupdf.PdfObj(mupdf.obj_enum_to_obj(mupdf.PDF_ENUM_NAME_Filter)),
                mupdf.PdfObj(mupdf.obj_enum_to_obj(mupdf.PDF_ENUM_NAME_FlateDecode)),
                )
        doc.update_stream(obj, nres, 1)
    else:
        doc.update_stream(obj, buffer_, 0);


# return a PDF page's /Rotate value: one of (0, 90, 180, 270)
def JM_page_rotation(page):
    rotate = 0

    obj = page.obj().dict_get_inheritable(
            mupdf.PdfObj(mupdf.obj_enum_to_obj(mupdf.PDF_ENUM_NAME_Rotate))
            )
    rotate = obj.to_int()
    rotate = JM_norm_rotation(rotate)
    return rotate


# return normalized /Rotate value
def JM_norm_rotation(rotate):
    while rotate < 0:
        rotate += 360
    while rotate >= 360:
        rotate -= 360
    if rotate % 90 != 0:
        return 0
    return rotate


TOOLS_JM_UNIQUE_ID = 0
class TOOLS:
    @staticmethod
    def gen_id():
        global TOOLS_JM_UNIQUE_ID
        TOOLS_JM_UNIQUE_ID += 1
        return TOOLS_JM_UNIQUE_ID

    @staticmethod
    def mupdf_warnings(reset=1):
        pass

    @staticmethod
    def _get_all_contents(page):
        page = page.this.page_from_fz_page()
        res = JM_read_contents(page.obj())
        result = JM_BinFromBuffer( res)
        return result

    fitz_config = {
                "plotter-g": True,
                "plotter-rgb": True,
                "plotter-cmyk": True,
                "plotter-n": True,
                "pdf": True,
                "xps": True,
                "svg": True,
                "cbz": True,
                "img": True,
                "html": True,
                "epub": True,
                "jpx": True,
                "js": True,
                "tofu": True,
                "tofu-cjk": True,
                "tofu-cjk-ext": True,
                "tofu-cjk-lang": True,
                "tofu-emoji": True,
                "tofu-historic": True,
                "tofu-symbol": True,
                "tofu-sil": True,
                "icc": True,
                "base14": True,
                "py-memory": True,
                }
    """PyMuPDF configuration parameters."""


    JM_annot_id_stem = 'fitz'

    @staticmethod
    def set_annot_stem( stem=None):
        if stem is None:
            return JM_annot_id_stem
        len_ = len(stem) + 1
        if len_ > 50:
            len_ = 50
        JM_annot_id_stem = stem[:50]
        return JM_annot_id_stem



def DUMMY(*args, **kw):
    return

def INRANGE(v, low, high):
    return low <= v and v <= high


def Page_set_contents(page0, xref):
    assert isinstance(page0, Page)
    page = page0.this.page_from_fz_page()
    assert isinstance(page, mupdf.PdfPage)
    #jlib.log(f'{page.doc=}')
    if not INRANGE(xref, 1, page.doc().xref_len() - 1):
        raise Exception('bad xref')
    contents = page.doc().new_indirect(xref, 0)
    if not contents.is_stream():
        raise Exception('xref is no stream')
    page.obj().dict_put_drop(
            mupdf.PdfObj(mupdf.obj_enum_to_obj(mupdf.PDF_ENUM_NAME_Contents)),
            contents,
            )
    # fixme: page.this.dirty = 1
    return

def Page_clean_contents(self, sanitize):
    assert isinstance(self, Page)
    page = self.this.page_from_fz_page()
    if not page:
        return
    assert isinstance(page, mupdf.PdfPage)
    filter_ = mupdf.pdf_filter_options()
    filter_.opaque = None
    filter_.image_filter = None
    filter_.text_filter = None
    filter_.after_text_object = None
    filter_.end_page = None
    filter_.recurse = 1
    filter_.instance_forms = 1
    filter_.sanitize = 1
    filter_.ascii = 0

    filter_.sanitize = sanitize
    filter2 = mupdf.PdfFilterOptions(filter_)
    page.doc().filter_page_contents(page, filter2)
    # fixme: page->doc->dirty = 1;

def JM_get_annot_xref_list(page_obj):
    '''
    Wrapper for PyMuPDF/fitz/helper-annot.i:
        PyObject *JM_get_annot_xref_list(fz_context *ctx, pdf_obj *page_obj)

    Not PyMuPDF/fitz/fitz_wrap.c:
        PyObject *JM_get_annot_xref_list(fz_context *ctx, pdf_page *page)
    '''
    names = []
    jlib.log('{page_obj=}')
    jlib.log('{mupdf.PDF_ENUM_NAME_Annots=}')
    jlib.log('calling page_obj.dict_get()')
    annots = page_obj.dict_get(mupdf.PDF_ENUM_NAME_Annots)
    jlib.log('{annots=}')
    if not annots:
        return names
    n = annots.array_len()
    jlib.log('{n=}')
    for i in range(n):
        annot_obj = mupdf.ppdf_array_get(annots, i)
        xref = mupdf.ppdf_to_num(annot_obj)
        jlib.log('{mupdf.PDF_ENUM_NAME_Subtype=}')
        subtype = mupdf.ppdf_dict_get(annot_obj, mupdf.PDF_ENUM_NAME_Subtype)
        type_ = mupdf.PDF_ANNOT_UNKNOWN
        if (subtype):
            name = mupdf.ppdf_to_name(subtype)
            type_ = mupdf.ppdf_annot_type_from_string(name)
        id_ = mupdf.ppdf_dict_gets(annot_obj, "NM")
        names.append( (xref, type_, mupdf.ppdf_to_text_string(id_)) )
    return names


def JM_BufferFromBytes(stream):
    '''
    Make fz_buffer from a PyBytes, PyByteArray, io.BytesIO object.
    '''
    if isinstance(stream, bytes):
        return mupdf.Buffer.new_buffer_from_copied_data(stream)
    assert 0, 'Only bytes supported at the moment.'
    if isinstance(stream, bytearray):
        return mupdf.Buffer.new_buffer_from_copied_data(stream, len(stream))
    if instance(stream, io.BytesIO):
        b = stream.getvalue()
        return mupdf.Buffer.new_buffer_from_copied_data(b, len(b))
    return mupdf.Buffer(None)


def pdf_dict_getl(doc, obj, *keys):
    jlib.log('{obj=} {len(keys)=}: {keys}')
    jlib.log('{doc.this.count_pages()=}')
    for i in range(len(keys)):
        if not obj:
            break
        obj = obj.dict_get(keys[i])
        jlib.log('{i=} {keys[i]=} {obj=} {doc.this.count_pages()=}')
    jlib.log('{obj=} {doc.this.count_pages()=}')
    return obj

def dir_str(x):
    ret = f'{x} {type(x)} ({len(dir(x))}):\n'
    for i in dir(x):
        ret += f'    {i}\n'
    return ret


def get_pdf_now():
    return getPDFnow()


def get_pdf_str(s: str) -> str:
    """ Return a PDF string depending on its coding.

    Notes:
        Returns a string bracketed with either "()" or "<>" for hex values.
        If only ascii then "(original)" is returned, else if only 8 bit chars
        then "(original)" with interspersed octal strings \nnn is returned,
        else a string "<FEFF[hexstring]>" is returned, where [hexstring] is the
        UTF-16BE encoding of the original.
    """
    if not bool(s):
        return "()"

    def make_utf16be(s):
        r = bytearray([254, 255]) + bytearray(s, "UTF-16BE")
        return "<" + r.hex() + ">"  # brackets indicate hex

    # The following either returns the original string with mixed-in
    # octal numbers \nnn for chars outside the ASCII range, or returns
    # the UTF-16BE BOM version of the string.
    r = ""
    for c in s:
        oc = ord(c)
        if oc > 255:  # shortcut if beyond 8-bit code range
            return make_utf16be(s)

        if oc > 31 and oc < 127:  # in ASCII range
            if c in ("(", ")", "\\"):  # these need to be escaped
                r += "\\"
            r += c
            continue

        if oc > 127:  # beyond ASCII
            r += "\\%03o" % oc
            continue

        # now the white spaces
        if oc == 8:  # backspace
            r += "\\b"
        elif oc == 9:  # tab
            r += "\\t"
        elif oc == 10:  # line feed
            r += "\\n"
        elif oc == 12:  # form feed
            r += "\\f"
        elif oc == 13:  # carriage return
            r += "\\r"
        else:
            r += "\\267"  # unsupported: replace by 0xB7

    return "(" + r + ")"


def get_text_length(text: str, fontname: str ="helv", fontsize: float =11, encoding: int =0) -> float:
    """Calculate length of a string for a built-in font.

    Args:
        fontname: name of the font.
        fontsize: font size points.
        encoding: encoding to use, 0=Latin (default), 1=Greek, 2=Cyrillic.
    Returns:
        (float) length of text.
    """
    fontname = fontname.lower()
    basename = Base14_fontdict.get(fontname, None)

    glyphs = None
    if basename == "Symbol":
        glyphs = symbol_glyphs
    if basename == "ZapfDingbats":
        glyphs = zapf_glyphs
    if glyphs is not None:
        w = sum([glyphs[ord(c)][1] if ord(c) < 256 else glyphs[183][1] for c in text])
        return w * fontsize

    if fontname in Base14_fontdict.keys():
        return TOOLS._measure_string(
            text, Base14_fontdict[fontname], fontsize, encoding
        )

    if fontname in (
        "china-t",
        "china-s",
        "china-ts",
        "china-ss",
        "japan",
        "japan-s",
        "korea",
        "korea-s",
    ):
        return len(text) * fontsize

    raise ValueError("Font '%s' is unsupported" % fontname)







class Matrix(object):
    """Matrix() - all zeros
    Matrix(a, b, c, d, e, f)
    Matrix(zoom-x, zoom-y) - zoom
    Matrix(shear-x, shear-y, 1) - shear
    Matrix(degree) - rotate
    Matrix(Matrix) - new copy
    Matrix(sequence) - from 'sequence'"""
    def __init__(self, *args):
        if not args:
            self.a = self.b = self.c = self.d = self.e = self.f = 0.0
            return None
        if len(args) > 6:
            raise ValueError("bad Matrix: sequ. length")
        if len(args) == 6:  # 6 numbers
            self.a, self.b, self.c, self.d, self.e, self.f = map(float, args)
            return None
        if len(args) == 1:  # either an angle or a sequ
            if hasattr(args[0], "__float__"):
                theta = math.radians(args[0])
                c = round(math.cos(theta), 8)
                s = round(math.sin(theta), 8)
                self.a = self.d = c
                self.b = s
                self.c = -s
                self.e = self.f = 0.0
                return None
            else:
                self.a, self.b, self.c, self.d, self.e, self.f = map(float, args[0])
                return None
        if len(args) == 2 or len(args) == 3 and args[2] == 0:
            self.a, self.b, self.c, self.d, self.e, self.f = float(args[0]), \
                0.0, 0.0, float(args[1]), 0.0, 0.0
            return None
        if len(args) == 3 and args[2] == 1:
            self.a, self.b, self.c, self.d, self.e, self.f = 1.0, \
                float(args[1]), float(args[0]), 1.0, 0.0, 0.0
            return None
        raise ValueError("bad Matrix constructor")

    def invert(self, src=None):
        """Calculate the inverted matrix. Return 0 if successful and replace
        current one. Else return 1 and do nothing.
        """
        if src is None:
            dst = TOOLS._invert_matrix(self)
        else:
            dst = TOOLS._invert_matrix(src)
        if dst[0] == 1:
            return 1
        self.a, self.b, self.c, self.d, self.e, self.f = dst[1]
        return 0

    def preTranslate(self, tx, ty):
        """Calculate pre translation and replace current matrix."""
        tx = float(tx)
        ty = float(ty)
        self.e += tx * self.a + ty * self.c
        self.f += tx * self.b + ty * self.d
        return self

    def preScale(self, sx, sy):
        """Calculate pre scaling and replace current matrix."""
        sx = float(sx)
        sy = float(sy)
        self.a *= sx
        self.b *= sx
        self.c *= sy
        self.d *= sy
        return self

    def preShear(self, h, v):
        """Calculate pre shearing and replace current matrix."""
        h = float(h)
        v = float(v)
        a, b = self.a, self.b
        self.a += v * self.c
        self.b += v * self.d
        self.c += h * a
        self.d += h * b
        return self

    def preRotate(self, theta):
        """Calculate pre rotation and replace current matrix."""
        theta = float(theta)
        while theta < 0: theta += 360
        while theta >= 360: theta -= 360
        if abs(0 - theta) < EPSILON:
            pass

        elif abs(90.0 - theta) < EPSILON:
            a = self.a
            b = self.b
            self.a = self.c
            self.b = self.d
            self.c = -a
            self.d = -b

        elif abs(180.0 - theta) < EPSILON:
            self.a = -self.a
            self.b = -self.b
            self.c = -self.c
            self.d = -self.d

        elif abs(270.0 - theta) < EPSILON:
            a = self.a
            b = self.b
            self.a = -self.c
            self.b = -self.d
            self.c = a
            self.d = b

        else:
            rad = math.radians(theta)
            s = math.sin(rad)
            c = math.cos(rad)
            a = self.a
            b = self.b
            self.a = c * a + s * self.c
            self.b = c * b + s * self.d
            self.c =-s * a + c * self.c
            self.d =-s * b + c * self.d

        return self

    def concat(self, one, two):
        """Multiply two matrices and replace current one."""
        if not len(one) == len(two) == 6:
            raise ValueError("bad Matrix: sequ. length")
        self.a, self.b, self.c, self.d, self.e, self.f = TOOLS._concat_matrix(one, two)
        return self

    def __getitem__(self, i):
        return (self.a, self.b, self.c, self.d, self.e, self.f)[i]

    def __setitem__(self, i, v):
        v = float(v)
        if   i == 0: self.a = v
        elif i == 1: self.b = v
        elif i == 2: self.c = v
        elif i == 3: self.d = v
        elif i == 4: self.e = v
        elif i == 5: self.f = v
        else:
            raise IndexError("index out of range")
        return

    def __len__(self):
        return 6

    def __repr__(self):
        return "Matrix" + str(tuple(self))

    def __invert__(self):
        """Calculate inverted matrix."""
        m1 = Matrix()
        m1.invert(self)
        return m1
    __inv__ = __invert__

    def __mul__(self, m):
        if hasattr(m, "__float__"):
            return Matrix(self.a * m, self.b * m, self.c * m,
                          self.d * m, self.e * m, self.f * m)
        m1 = Matrix(1,1)
        return m1.concat(self, m)

    def __truediv__(self, m):
        if hasattr(m, "__float__"):
            return Matrix(self.a * 1./m, self.b * 1./m, self.c * 1./m,
                          self.d * 1./m, self.e * 1./m, self.f * 1./m)
        m1 = TOOLS._invert_matrix(m)[1]
        if not m1:
            raise ZeroDivisionError("matrix not invertible")
        m2 = Matrix(1,1)
        return m2.concat(self, m1)
    __div__ = __truediv__

    def __add__(self, m):
        if hasattr(m, "__float__"):
            return Matrix(self.a + m, self.b + m, self.c + m,
                          self.d + m, self.e + m, self.f + m)
        if len(m) != 6:
            raise ValueError("bad Matrix: sequ. length")
        return Matrix(self.a + m[0], self.b + m[1], self.c + m[2],
                          self.d + m[3], self.e + m[4], self.f + m[5])

    def __sub__(self, m):
        if hasattr(m, "__float__"):
            return Matrix(self.a - m, self.b - m, self.c - m,
                          self.d - m, self.e - m, self.f - m)
        if len(m) != 6:
            raise ValueError("bad Matrix: sequ. length")
        return Matrix(self.a - m[0], self.b - m[1], self.c - m[2],
                          self.d - m[3], self.e - m[4], self.f - m[5])

    def __pos__(self):
        return Matrix(self)

    def __neg__(self):
        return Matrix(-self.a, -self.b, -self.c, -self.d, -self.e, -self.f)

    def __bool__(self):
        return not (max(self) == min(self) == 0)

    def __nonzero__(self):
        return not (max(self) == min(self) == 0)

    def __eq__(self, mat):
        if not hasattr(mat, "__len__"):
            return False
        return len(mat) == 6 and bool(self - mat) is False

    def __abs__(self):
        return math.sqrt(sum([c*c for c in self]))

    norm = __abs__

    @property
    def isRectilinear(self):
        """True if rectangles are mapped to rectangles."""
        return (abs(self.b) < EPSILON and abs(self.c) < EPSILON) or \
            (abs(self.a) < EPSILON and abs(self.d) < EPSILON);


class IdentityMatrix(Matrix):
    """Identity matrix [1, 0, 0, 1, 0, 0]"""
    def __init__(self):
        Matrix.__init__(self, 1.0, 1.0)
    def __setattr__(self, name, value):
        if name in "ad":
            self.__dict__[name] = 1.0
        elif name in "bcef":
            self.__dict__[name] = 0.0
        else:
            self.__dict__[name] = value

    def checkargs(*args):
        raise NotImplementedError("Identity is readonly")

    preRotate    = checkargs
    preShear     = checkargs
    preScale     = checkargs
    preTranslate = checkargs
    concat       = checkargs
    invert       = checkargs

    def __repr__(self):
        return "IdentityMatrix(1.0, 0.0, 0.0, 1.0, 0.0, 0.0)"

    def __hash__(self):
        return hash((1,0,0,1,0,0))


Identity = IdentityMatrix()

class Point(object):
    """Point() - all zeros\nPoint(x, y)\nPoint(Point) - new copy\nPoint(sequence) - from 'sequence'"""
    def __init__(self, *args):
        if not args:
            self.x = 0.0
            self.y = 0.0
            return None

        if len(args) > 2:
            raise ValueError("bad Point: sequ. length")
        if len(args) == 2:
            self.x = float(args[0])
            self.y = float(args[1])
            return None
        if len(args) == 1:
            l = args[0]
            if hasattr(l, "__getitem__") is False:
                raise ValueError("bad Point constructor")
            if len(l) != 2:
                raise ValueError("bad Point: sequ. length")
            self.x = float(l[0])
            self.y = float(l[1])
            return None
        raise ValueError("bad Point constructor")

    def transform(self, m):
        """Replace point by its transformation with matrix-like m."""
        if len(m) != 6:
            raise ValueError("bad Matrix: sequ. length")
        self.x, self.y = TOOLS._transform_point(self, m)
        return self

    @property
    def unit(self):
        """Unit vector of the point."""
        s = self.x * self.x + self.y * self.y
        if s < EPSILON:
            return Point(0,0)
        s = math.sqrt(s)
        return Point(self.x / s, self.y / s)

    @property
    def abs_unit(self):
        """Unit vector with positive coordinates."""
        s = self.x * self.x + self.y * self.y
        if s < EPSILON:
            return Point(0,0)
        s = math.sqrt(s)
        return Point(abs(self.x) / s, abs(self.y) / s)

    def distance_to(self, *args):
        """Return distance to rectangle or another point."""
        if not len(args) > 0:
            raise ValueError("at least one parameter must be given")

        x = args[0]
        if len(x) == 2:
            x = Point(x)
        elif len(x) == 4:
            x = Rect(x)
        else:
            raise ValueError("arg1 must be point-like or rect-like")

        if len(args) > 1:
            unit = args[1]
        else:
            unit = "px"
        u = {"px": (1.,1.), "in": (1.,72.), "cm": (2.54, 72.),
             "mm": (25.4, 72.)}
        f = u[unit][0] / u[unit][1]

        if type(x) is Point:
            return abs(self - x) * f

# from here on, x is a rectangle
# as a safeguard, make a finite copy of it
        r = Rect(x.top_left, x.top_left)
        r = r | x.bottom_right
        if self in r:
            return 0.0
        if self.x > r.x1:
            if self.y >= r.y1:
                return self.distance_to(r.bottom_right, unit)
            elif self.y <= r.y0:
                return self.distance_to(r.top_right, unit)
            else:
                return (self.x - r.x1) * f
        elif r.x0 <= self.x <= r.x1:
            if self.y >= r.y1:
                return (self.y - r.y1) * f
            else:
                return (r.y0 - self.y) * f
        else:
            if self.y >= r.y1:
                return self.distance_to(r.bottom_left, unit)
            elif self.y <= r.y0:
                return self.distance_to(r.top_left, unit)
            else:
                return (r.x0 - self.x) * f

    def __getitem__(self, i):
        return (self.x, self.y)[i]

    def __len__(self):
        return 2

    def __setitem__(self, i, v):
        v = float(v)
        if   i == 0: self.x = v
        elif i == 1: self.y = v
        else:
            raise IndexError("index out of range")
        return None

    def __repr__(self):
        return "Point" + str(tuple(self))

    def __pos__(self):
        return Point(self)

    def __neg__(self):
        return Point(-self.x, -self.y)

    def __bool__(self):
        return not (max(self) == min(self) == 0)

    def __nonzero__(self):
        return not (max(self) == min(self) == 0)

    def __eq__(self, p):
        if not hasattr(p, "__len__"):
            return False
        return len(p) == 2 and bool(self - p) is False

    def __abs__(self):
        return math.sqrt(self.x * self.x + self.y * self.y)

    norm = __abs__

    def __add__(self, p):
        if hasattr(p, "__float__"):
            return Point(self.x + p, self.y + p)
        if len(p) != 2:
            raise ValueError("bad Point: sequ. length")
        return Point(self.x + p[0], self.y + p[1])

    def __sub__(self, p):
        if hasattr(p, "__float__"):
            return Point(self.x - p, self.y - p)
        if len(p) != 2:
            raise ValueError("bad Point: sequ. length")
        return Point(self.x - p[0], self.y - p[1])

    def __mul__(self, m):
        if hasattr(m, "__float__"):
            return Point(self.x * m, self.y * m)
        p = Point(self)
        return p.transform(m)

    def __truediv__(self, m):
        if hasattr(m, "__float__"):
            return Point(self.x * 1./m, self.y * 1./m)
        m1 = TOOLS._invert_matrix(m)[1]
        if not m1:
            raise ZeroDivisionError("matrix not invertible")
        p = Point(self)
        return p.transform(m1)

    __div__ = __truediv__

    def __hash__(self):
        return hash(tuple(self))

class Rect(object):
    """Rect() - all zeros\nRect(x0, y0, x1, y1)\nRect(top-left, x1, y1)\nRect(x0, y0, bottom-right)\nRect(top-left, bottom-right)\nRect(Rect or IRect) - new copy\nRect(sequence) - from 'sequence'"""
    def __init__(self, *args):
        if not args:
            self.x0 = self.y0 = self.x1 = self.y1 = 0.0
            return None

        if len(args) > 4:
            raise ValueError("bad Rect: sequ. length")
        if len(args) == 4:
            self.x0, self.y0, self.x1, self.y1 = map(float, args)
            return None
        if len(args) == 1:
            l = args[0]
            if hasattr(l, "__getitem__") is False:
                raise ValueError("bad Rect constructor")
            if len(l) != 4:
                raise ValueError("bad Rect: sequ. length")
            self.x0, self.y0, self.x1, self.y1 = map(float, l)
            return None
        if len(args) == 2:                  # 2 Points provided
            self.x0 = float(args[0][0])
            self.y0 = float(args[0][1])
            self.x1 = float(args[1][0])
            self.y1 = float(args[1][1])
            return None
        if len(args) == 3:                  # 2 floats and 1 Point provided
            a0 = args[0]
            a1 = args[1]
            a2 = args[2]
            if hasattr(a0, "__float__"):    # (float, float, Point) provided
                self.x0 = float(a0)
                self.y0 = float(a1)
                self.x1 = float(a2[0])
                self.y1 = float(a2[1])
                return None
            self.x0 = float(a0[0])          # (Point, float, float) provided
            self.y0 = float(a0[1])
            self.x1 = float(a1)
            self.y1 = float(a2)
            return None
        raise ValueError("bad Rect constructor")

    def normalize(self):
        """Replace rectangle with its finite version."""
        if self.x1 < self.x0:
            self.x0, self.x1 = self.x1, self.x0
        if self.y1 < self.y0:
            self.y0, self.y1 = self.y1, self.y0
        return self

    @property
    def isEmpty(self):
        """True if rectangle area is empty."""
        return self.x0 == self.x1 or self.y0 == self.y1

    @property
    def isInfinite(self):
        """True if rectangle is infinite."""
        return self.x0 > self.x1 or self.y0 > self.y1

    @property
    def top_left(self):
        """Top-left corner."""
        return Point(self.x0, self.y0)

    @property
    def top_right(self):
        """Top-right corner."""
        return Point(self.x1, self.y0)

    @property
    def bottom_left(self):
        """Bottom-left corner."""
        return Point(self.x0, self.y1)

    @property
    def bottom_right(self):
        """Bottom-right corner."""
        return Point(self.x1, self.y1)

    tl = top_left
    tr = top_right
    bl = bottom_left
    br = bottom_right

    @property
    def quad(self):
        """Return Quad version of rectangle."""
        return Quad(self.tl, self.tr, self.bl, self.br)

    def morph(self, p, m):
        """Morph with matrix-like m and point-like p.

        Returns a new quad."""
        return self.quad.morph(p, m)

    def round(self):
        """Return the IRect."""
        return IRect(min(self.x0, self.x1), min(self.y0, self.y1),
                     max(self.x0, self.x1), max(self.y0, self.y1))

    irect = property(round)

    width  = property(lambda self: abs(self.x1 - self.x0))
    height = property(lambda self: abs(self.y1 - self.y0))

    def includePoint(self, p):
        """Extend to include point-like p."""
        if len(p) != 2:
            raise ValueError("bad Point: sequ. length")
        self.x0, self.y0, self.x1, self.y1 = TOOLS._include_point_in_rect(self, p)
        return self

    def includeRect(self, r):
        """Extend to include rect-like r."""
        if len(r) != 4:
            raise ValueError("bad Rect: sequ. length")
        self.x0, self.y0, self.x1, self.y1 = TOOLS._union_rect(self, r)
        return self

    def intersect(self, r):
        """Restrict to common rect with rect-like r."""
        if not len(r) == 4:
            raise ValueError("bad Rect: sequ. length")
        self.x0, self.y0, self.x1, self.y1 = TOOLS._intersect_rect(self, r)
        return self

    def contains(self, x):
        """Check if containing point-like or rect-like x."""
        return self.__contains__(x)

    def transform(self, m):
        """Replace with the transformation by matrix-like m."""
        if not len(m) == 6:
            raise ValueError("bad Matrix: sequ. length")
        self.x0, self.y0, self.x1, self.y1 = TOOLS._transform_rect(self, m)
        return self

    def __getitem__(self, i):
        return (self.x0, self.y0, self.x1, self.y1)[i]

    def __len__(self):
        return 4

    def __setitem__(self, i, v):
        v = float(v)
        if   i == 0: self.x0 = v
        elif i == 1: self.y0 = v
        elif i == 2: self.x1 = v
        elif i == 3: self.y1 = v
        else:
            raise IndexError("index out of range")
        return None

    def __repr__(self):
        return "Rect" + str(tuple(self))

    def __pos__(self):
        return Rect(self)

    def __neg__(self):
        return Rect(-self.x0, -self.y0, -self.x1, -self.y1)

    def __bool__(self):
        return not (max(self) == min(self) == 0)

    def __nonzero__(self):
        return not (max(self) == min(self) == 0)

    def __eq__(self, rect):
        if not hasattr(rect, "__len__"):
            return False
        return len(rect) == 4 and bool(self - rect) is False

    def __abs__(self):
        if self.isEmpty or self.isInfinite:
            return 0.0
        return (self.x1 - self.x0) * (self.y1 - self.y0)

    def norm(self):
        return math.sqrt(sum([c*c for c in self]))

    def __add__(self, p):
        if hasattr(p, "__float__"):
            r = Rect(self.x0 + p, self.y0 + p, self.x1 + p, self.y1 + p)
        else:
            if len(p) != 4:
                raise ValueError("bad Rect: sequ. length")
            r = Rect(self.x0 + p[0], self.y0 + p[1], self.x1 + p[2], self.y1 + p[3])
        return r

    def __sub__(self, p):
        if hasattr(p, "__float__"):
            return Rect(self.x0 - p, self.y0 - p, self.x1 - p, self.y1 - p)
        if len(p) != 4:
            raise ValueError("bad Rect: sequ. length")
        return Rect(self.x0 - p[0], self.y0 - p[1], self.x1 - p[2], self.y1 - p[3])

    def __mul__(self, m):
        if hasattr(m, "__float__"):
            return Rect(self.x0 * m, self.y0 * m, self.x1 * m, self.y1 * m)
        r = Rect(self)
        r = r.transform(m)
        return r

    def __truediv__(self, m):
        if hasattr(m, "__float__"):
            return Rect(self.x0 * 1./m, self.y0 * 1./m, self.x1 * 1./m, self.y1 * 1./m)
        im = TOOLS._invert_matrix(m)[1]
        if not im:
            raise ZeroDivisionError("matrix not invertible")
        r = Rect(self)
        r = r.transform(im)
        return r

    __div__ = __truediv__

    def __contains__(self, x):
        if hasattr(x, "__float__"):
            return x in tuple(self)
        l = len(x)
        r = Rect(self).normalize()
        if l == 4:
            if r.isEmpty: return False
            xr = Rect(x).normalize()
            if xr.isEmpty: return True
            if r.x0 <= xr.x0 and r.y0 <= xr.y0 and \
               r.x1 >= xr.x1 and r.y1 >= xr.y1:
               return True
            return False
        if l == 2:
            if r.x0 <= x[0] <= r.x1 and \
               r.y0 <= x[1] <= r.y1:
               return True
            return False
        return False

    def __or__(self, x):
        if not hasattr(x, "__len__"):
            raise ValueError("bad operand 2")

        r = Rect(self)
        if len(x) == 2:
            return r.includePoint(x)
        if len(x) == 4:
            return r.includeRect(x)
        raise ValueError("bad operand 2")

    def __and__(self, x):
        if not hasattr(x, "__len__"):
            raise ValueError("bad operand 2")

        r1 = Rect(x)
        r = Rect(self)
        return r.intersect(r1)

    def intersects(self, x):
        """Check if intersection with rectangle x is not empty."""
        r1 = Rect(x)
        if self.isEmpty or self.isInfinite or r1.isEmpty or r1.isInfinite:
            return False
        r = Rect(self)
        if r.intersect(r1).isEmpty:
            return False
        return True

    def __hash__(self):
        return hash(tuple(self))

class IRect(Rect):
    """IRect() - all zeros\nIRect(x0, y0, x1, y1)\nIRect(Rect or IRect) - new copy\nIRect(sequence) - from 'sequence'"""
    def __init__(self, *args):
        Rect.__init__(self, *args)
        self.x0 = math.floor(self.x0 + 0.001)
        self.y0 = math.floor(self.y0 + 0.001)
        self.x1 = math.ceil(self.x1 - 0.001)
        self.y1 = math.ceil(self.y1 - 0.001)
        return None

    @property
    def round(self):
        pass

    irect = round

    @property
    def rect(self):
        return Rect(self)

    def __repr__(self):
        return "IRect" + str(tuple(self))

    def includePoint(self, p):
        """Extend rectangle to include point p."""
        return Rect.includePoint(self, p).round()

    def includeRect(self, r):
        """Extend rectangle to include rectangle r."""
        return Rect.includeRect(self, r).round()

    def intersect(self, r):
        """Restrict rectangle to intersection with rectangle r."""
        return Rect.intersect(self, r).round()

    def __setitem__(self, i, v):
        v = int(v)
        if   i == 0: self.x0 = v
        elif i == 1: self.y0 = v
        elif i == 2: self.x1 = v
        elif i == 3: self.y1 = v
        else:
            raise IndexError("index out of range")
        return None

    def __pos__(self):
        return IRect(self)

    def __neg__(self):
        return IRect(-self.x0, -self.y0, -self.x1, -self.y1)

    def __add__(self, p):
        return Rect.__add__(self, p).round()

    def __sub__(self, p):
        return Rect.__sub__(self, p).round()

    def transform(self, m):
        return Rect.transform(self, m).round()

    def __mul__(self, m):
        return Rect.__mul__(self, m).round()

    def __truediv__(self, m):
        return Rect.__truediv__(self, m).round()

    def __or__(self, x):
        return Rect.__or__(self, x).round()

    def __and__(self, x):
        return Rect.__and__(self, x).round()

class Quad(object):
    """Quad() - all zero points\nQuad(ul, ur, ll, lr)\nQuad(quad) - new copy\nQuad(sequence) - from 'sequence'"""
    def __init__(self, *args):
        if not args:
            self.ul = self.ur = self.ll = self.lr = Point()
            return None

        if len(args) > 4:
            raise ValueError("bad Quad: sequ. length")
        if len(args) == 4:
            self.ul, self.ur, self.ll, self.lr = map(Point, args)
            return None
        if len(args) == 1:
            l = args[0]
            if hasattr(l, "__getitem__") is False:
                raise ValueError("bad Quad constructor")
            if len(l) != 4:
                raise ValueError("bad Quad: sequ. length")
            self.ul, self.ur, self.ll, self.lr = map(Point, l)
            return None
        raise ValueError("bad Quad constructor")

    @property
    def isRectangular(self):
        """Check if quad is rectangular.

        Notes:
            Some rotation matrix can thus transform it into a rectangle.
            This is equivalent to three corners enclose 90 degrees.
        Returns:
            True or False.
        """

        sine = TOOLS._sine_between(self.ul, self.ur, self.lr)
        if abs(sine - 1) > EPSILON:  # the sine of the angle
            return False

        sine = TOOLS._sine_between(self.ur, self.lr, self.ll)
        if abs(sine - 1) > EPSILON:
            return False

        sine = TOOLS._sine_between(self.lr, self.ll, self.ul)
        if abs(sine - 1) > EPSILON:
            return False

        return True


    @property
    def isConvex(self):
        """Check if quad is convex and not degenerate.

        Notes:
            For convexity, every line connecting two points of the quad must be
            inside the quad. This is equivalent to that every corner encloses
            an angle with 0 < angle < 180 degrees.
            Excluding the "degenerate" case (all points on the same line),
            it suffices to check that the sines of three angles are > 0.
        Returns:
            True or False.
        """
        count = 0
        sine = TOOLS._sine_between(self.ul, self.ur, self.lr)
        if sine > 0:
            count += 1
        elif sine < 0:
            return False

        sine = TOOLS._sine_between(self.ur, self.lr, self.ll)
        if sine > 0:
            count += 1
        elif sine < 0:
            return False

        sine = TOOLS._sine_between(self.lr, self.ll, self.ul)
        if sine > 0:
            count += 1
        elif sine < 0:
            return False

        sine = TOOLS._sine_between(self.ll, self.ul, self.ur)
        if sine > 0:
            count += 1
        elif sine < 0:
            return False

        if count >= 2:
            return True

        return False


    @property
    def isEmpty(self):
        """Check whether all quad corners are on the same line.

        The is the case exactly if more than one corner angle is zero.
        """
        count = 0
        if abs(TOOLS._sine_between(self.ul, self.ur, self.lr)) < EPSILON:
            count += 1
        if abs(TOOLS._sine_between(self.ur, self.lr, self.ll)) < EPSILON:
            count += 1
        if abs(TOOLS._sine_between(self.lr, self.ll, self.ul)) < EPSILON:
            count += 1
        if abs(TOOLS._sine_between(self.ll, self.ul, self.ur)) < EPSILON:
            count += 1
        if count <= 2:
            return False
        return True

    width  = property(lambda self: max(abs(self.ul - self.ur), abs(self.ll - self.lr)))
    height = property(lambda self: max(abs(self.ul - self.ll), abs(self.ur - self.lr)))

    @property
    def rect(self):
        r = Rect()
        r.x0 = min(self.ul.x, self.ur.x, self.lr.x, self.ll.x)
        r.y0 = min(self.ul.y, self.ur.y, self.lr.y, self.ll.y)
        r.x1 = max(self.ul.x, self.ur.x, self.lr.x, self.ll.x)
        r.y1 = max(self.ul.y, self.ur.y, self.lr.y, self.ll.y)
        return r

    def __getitem__(self, i):
        return (self.ul, self.ur, self.ll, self.lr)[i]

    def __len__(self):
        return 4

    def __setitem__(self, i, v):
        if   i == 0: self.ul = Point(v)
        elif i == 1: self.ur = Point(v)
        elif i == 2: self.ll = Point(v)
        elif i == 3: self.lr = Point(v)
        else:
            raise IndexError("index out of range")
        return None

    def __repr__(self):
        return "Quad" + str(tuple(self))

    def __pos__(self):
        return Quad(self)

    def __neg__(self):
        return Quad(-self.ul, -self.ur, -self.ll, -self.lr)

    def __bool__(self):
        return not self.isEmpty

    def __nonzero__(self):
        return not self.isEmpty

    def __eq__(self, quad):
        if not hasattr(quad, "__len__"):
            return False
        return len(quad) == 4 and (
            self.ul == quad[0] and
            self.ur == quad[1] and
            self.ll == quad[2] and
            self.lr == quad[3]
        )

    def __abs__(self):
        if self.isEmpty:
            return 0.0
        return abs(self.ul - self.ur) * abs(self.ul - self.ll)


    def morph(self, p, m):
        """Morph the quad with matrix-like 'm' and point-like 'p'.

        Return a new quad."""

        delta = Matrix(1, 1).preTranslate(p.x, p.y)
        q = self * ~delta * m * delta
        return q


    def transform(self, m):
        """Replace quad by its transformation with matrix m."""
        if len(m) != 6:
            raise ValueError("bad Matrix: sequ. length")
        self.ul *= m
        self.ur *= m
        self.ll *= m
        self.lr *= m
        return self

    def __mul__(self, m):
        r = Quad(self)
        r = r.transform(m)
        return r

    def __truediv__(self, m):
        if hasattr(m, "__float__"):
            im = 1. / m
        else:
            im = TOOLS._invert_matrix(m)[1]
            if not im:
                raise ZeroDivisionError("matrix not invertible")
        r = Quad(self)
        r = r.transform(im)
        return r

    __div__ = __truediv__

    def __hash__(self):
        return hash(tuple(self))



#------------------------------------------------------------------------------
# Class describing a PDF form field ("widget")
#------------------------------------------------------------------------------
class Widget(object):
    def __init__(self):
        self.border_color = None
        self.border_style = "S"
        self.border_width = 0
        self.border_dashes = None
        self.choice_values = None  # choice fields only

        self.field_name = None  # field name
        self.field_label = None  # field label
        self.field_value = None
        self.field_flags = None
        self.field_display = 0
        self.field_type = 0  # valid range 1 through 7
        self.field_type_string = None  # field type as string

        self.fill_color = None
        self.button_caption = None  # button caption
        self.is_signed = None  # True / False if signature
        self.text_color = (0, 0, 0)
        self.text_font = "Helv"
        self.text_fontsize = 0
        self.text_maxlen = 0  # text fields only
        self.text_format = 0  # text fields only
        self._text_da = ""  # /DA = default apparance

        self.script = None  # JavaScript (/A)
        self.script_stroke = None  # JavaScript (/AA/K)
        self.script_format = None  # JavaScript (/AA/F)
        self.script_change = None  # JavaScript (/AA/V)
        self.script_calc = None  # JavaScript (/AA/C)

        self.rect = None  # annot value
        self.xref = 0  # annot value


    def _validate(self):
        """Validate the class entries.
        """
        if (self.rect.isInfinite
            or self.rect.isEmpty
           ):
            raise ValueError("bad rect")

        if not self.field_name:
            raise ValueError("field name missing")

        if self.field_label == "Unnamed":
            self.field_label = None
        CheckColor(self.border_color)
        CheckColor(self.fill_color)
        if not self.text_color:
            self.text_color = (0, 0, 0)
        CheckColor(self.text_color)

        if not self.border_width:
            self.border_width = 0

        if not self.text_fontsize:
            self.text_fontsize = 0

        self.border_style = self.border_style.upper()[0:1]

# standardize content of JavaScript entries
        btn_type = self.field_type in (
            PDF_WIDGET_TYPE_BUTTON,
            PDF_WIDGET_TYPE_CHECKBOX,
            PDF_WIDGET_TYPE_RADIOBUTTON
        )
        if not self.script:
            self.script = None
        elif type(self.script) is not str:
            raise ValueError("script content must be string")

# buttons cannot have the following script actions
        if btn_type or not self.script_calc:
            self.script_calc = None
        elif type(self.script_calc) is not str:
            raise ValueError("script_calc content must be string")

        if btn_type or not self.script_change:
            self.script_change = None
        elif type(self.script_change) is not str:
            raise ValueError("script_change content must be string")

        if btn_type or not self.script_format:
            self.script_format = None
        elif type(self.script_format) is not str:
            raise ValueError("script_format content must be string")

        if btn_type or not self.script_stroke:
            self.script_stroke = None
        elif type(self.script_stroke) is not str:
            raise ValueError("script_stroke content must be string")

        self._checker()  # any field_type specific checks


    def _adjust_font(self):
        """Ensure text_font is from our list and correctly spelled.
        """
        if not self.text_font:
            self.text_font = "Helv"
            return
        valid_fonts = ("Cour", "TiRo", "Helv", "ZaDb")
        for f in valid_fonts:
            if self.text_font.lower() == f.lower():
                self.text_font = f
                return
        self.text_font = "Helv"
        return


    def _parse_da(self):
        """Extract font name, size and color from default appearance string (/DA object).

        Equivalent to 'pdf_parse_default_appearance' function in MuPDF's 'pdf-annot.c'.
        """
        if not self._text_da:
            return
        font = "Helv"
        fsize = 0
        col = (0, 0, 0)
        dat = self._text_da.split()  # split on any whitespace
        for i, item in enumerate(dat):
            if item == "Tf":
                font = dat[i - 2][1:]
                fsize = float(dat[i - 1])
                dat[i] = dat[i-1] = dat[i-2] = ""
                continue
            if item == "g":  # unicolor text
                col = [(float(dat[i - 1]))]
                dat[i] = dat[i-1] = ""
                continue
            if item == "rg":  # RGB colored text
                col = [float(f) for f in dat[i - 3:i]]
                dat[i] = dat[i-1] = dat[i-2] = dat[i-3] = ""
                continue
        self.text_font = font
        self.text_fontsize = fsize
        self.text_color = col
        self._text_da = ""
        return


    def _checker(self):
        """Any widget type checks.
        """
        if self.field_type not in range(1, 8):
            raise ValueError("bad field type")


    def update(self):
        """Reflect Python object in the PDF.
        """
        doc = self.parent.parent
        self._validate()

        self._adjust_font()  # ensure valid text_font name

# now create the /DA string
        self._text_da = ""
        if   len(self.text_color) == 3:
            fmt = "{:g} {:g} {:g} rg /{f:s} {s:g} Tf" + self._text_da
        elif len(self.text_color) == 1:
            fmt = "{:g} g /{f:s} {s:g} Tf" + self._text_da
        elif len(self.text_color) == 4:
            fmt = "{:g} {:g} {:g} {:g} k /{f:s} {s:g} Tf" + self._text_da
        self._text_da = fmt.format(*self.text_color, f=self.text_font,
                                    s=self.text_fontsize)
# finally update the widget

        TOOLS._save_widget(self._annot, self)
        self._text_da = ""

    def reset(self):
        """Reset the field value to its default.
        """
        TOOLS._reset_widget(self._annot)

    def __repr__(self):
        return "'%s' widget on %s" % (self.field_type_string, str(self.parent))

    def __del__(self):
        self._annot.__del__()

    @property
    def next(self):
        return self._annot.next


# ------------------------------------------------------------------------------
# link kinds and link flags
# ------------------------------------------------------------------------------
LINK_NONE = 0
LINK_GOTO = 1
LINK_URI = 2
LINK_LAUNCH = 3
LINK_NAMED = 4
LINK_GOTOR = 5
LINK_FLAG_L_VALID = 1
LINK_FLAG_T_VALID = 2
LINK_FLAG_R_VALID = 4
LINK_FLAG_B_VALID = 8
LINK_FLAG_FIT_H = 16
LINK_FLAG_FIT_V = 32
LINK_FLAG_R_IS_ZOOM = 64

# ------------------------------------------------------------------------------
# Text handling flags
# ------------------------------------------------------------------------------
TEXT_ALIGN_LEFT = 0
TEXT_ALIGN_CENTER = 1
TEXT_ALIGN_RIGHT = 2
TEXT_ALIGN_JUSTIFY = 3

TEXT_OUTPUT_TEXT = 0
TEXT_OUTPUT_HTML = 1
TEXT_OUTPUT_JSON = 2
TEXT_OUTPUT_XML = 3
TEXT_OUTPUT_XHTML = 4

TEXT_PRESERVE_LIGATURES = 1
TEXT_PRESERVE_WHITESPACE = 2
TEXT_PRESERVE_IMAGES = 4
TEXT_INHIBIT_SPACES = 8
TEXT_DEHYPHENATE = 16
TEXT_PRESERVE_SPANS = 32

# ------------------------------------------------------------------------------
# Simple text encoding options
# ------------------------------------------------------------------------------
TEXT_ENCODING_LATIN = 0
TEXT_ENCODING_GREEK = 1
TEXT_ENCODING_CYRILLIC = 2
# ------------------------------------------------------------------------------
# Stamp annotation icon numbers
# ------------------------------------------------------------------------------
STAMP_Approved = 0
STAMP_AsIs = 1
STAMP_Confidential = 2
STAMP_Departmental = 3
STAMP_Experimental = 4
STAMP_Expired = 5
STAMP_Final = 6
STAMP_ForComment = 7
STAMP_ForPublicRelease = 8
STAMP_NotApproved = 9
STAMP_NotForPublicRelease = 10
STAMP_Sold = 11
STAMP_TopSecret = 12
STAMP_Draft = 13

# ------------------------------------------------------------------------------
# Base 14 font names and dictionary
# ------------------------------------------------------------------------------
Base14_fontnames = (
    "Courier",
    "Courier-Oblique",
    "Courier-Bold",
    "Courier-BoldOblique",
    "Helvetica",
    "Helvetica-Oblique",
    "Helvetica-Bold",
    "Helvetica-BoldOblique",
    "Times-Roman",
    "Times-Italic",
    "Times-Bold",
    "Times-BoldItalic",
    "Symbol",
    "ZapfDingbats",
)

Base14_fontdict = {}
for f in Base14_fontnames:
    Base14_fontdict[f.lower()] = f
    del f
Base14_fontdict["helv"] = "Helvetica"
Base14_fontdict["heit"] = "Helvetica-Oblique"
Base14_fontdict["hebo"] = "Helvetica-Bold"
Base14_fontdict["hebi"] = "Helvetica-BoldOblique"
Base14_fontdict["cour"] = "Courier"
Base14_fontdict["coit"] = "Courier-Oblique"
Base14_fontdict["cobo"] = "Courier-Bold"
Base14_fontdict["cobi"] = "Courier-BoldOblique"
Base14_fontdict["tiro"] = "Times-Roman"
Base14_fontdict["tibo"] = "Times-Bold"
Base14_fontdict["tiit"] = "Times-Italic"
Base14_fontdict["tibi"] = "Times-BoldItalic"
Base14_fontdict["symb"] = "Symbol"
Base14_fontdict["zadb"] = "ZapfDingbats"

annot_skel = {
    "goto1": "<</A<</S/GoTo/D[%i 0 R/XYZ %g %g 0]>>/Rect[%s]/BS<</W 0>>/Subtype/Link>>",
    "goto2": "<</A<</S/GoTo/D%s>>/Rect[%s]/BS<</W 0>>/Subtype/Link>>",
    "gotor1": "<</A<</S/GoToR/D[%i /XYZ %g %g 0]/F<</F(%s)/UF(%s)/Type/Filespec>>>>/Rect[%s]/BS<</W 0>>/Subtype/Link>>",
    "gotor2": "<</A<</S/GoToR/D%s/F(%s)>>/Rect[%s]/BS<</W 0>>/Subtype/Link>>",
    "launch": "<</A<</S/Launch/F<</F(%s)/UF(%s)/Type/Filespec>>>>/Rect[%s]/BS<</W 0>>/Subtype/Link>>",
    "uri": "<</A<</S/URI/URI(%s)>>/Rect[%s]/BS<</W 0>>/Subtype/Link>>",
    "named": "<</A<</S/Named/N/%s/Type/Action>>/Rect[%s]/BS<</W 0>>/Subtype/Link>>",
}


def getTextlength(text: str, fontname: str ="helv", fontsize: float =11, encoding: int =0) -> float:
    """Calculate length of a string for a given built-in font.

    Args:
        fontname: name of the font.
        fontsize: size of font in points.
        encoding: encoding to use (0=Latin, 1=Greek, 2=Cyrillic).
    Returns:
        (float) length of text.
    """
    fontname = fontname.lower()
    basename = Base14_fontdict.get(fontname, None)

    glyphs = None
    if basename == "Symbol":
        glyphs = symbol_glyphs
    if basename == "ZapfDingbats":
        glyphs = zapf_glyphs
    if glyphs is not None:
        w = sum([glyphs[ord(c)][1] if ord(c) < 256 else glyphs[183][1] for c in text])
        return w * fontsize

    if fontname in Base14_fontdict.keys():
        return TOOLS._measure_string(
            text, Base14_fontdict[fontname], fontsize, encoding
        )

    if fontname in (
        "china-t",
        "china-s",
        "china-ts",
        "china-ss",
        "japan",
        "japan-s",
        "korea",
        "korea-s",
    ):
        return len(text) * fontsize

    raise ValueError("Font '%s' is unsupported" % fontname)


# ------------------------------------------------------------------------------
# Glyph list for the built-in font 'ZapfDingbats'
# ------------------------------------------------------------------------------
zapf_glyphs = (
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (32, 0.278),
    (33, 0.974),
    (34, 0.961),
    (35, 0.974),
    (36, 0.98),
    (37, 0.719),
    (38, 0.789),
    (39, 0.79),
    (40, 0.791),
    (41, 0.69),
    (42, 0.96),
    (43, 0.939),
    (44, 0.549),
    (45, 0.855),
    (46, 0.911),
    (47, 0.933),
    (48, 0.911),
    (49, 0.945),
    (50, 0.974),
    (51, 0.755),
    (52, 0.846),
    (53, 0.762),
    (54, 0.761),
    (55, 0.571),
    (56, 0.677),
    (57, 0.763),
    (58, 0.76),
    (59, 0.759),
    (60, 0.754),
    (61, 0.494),
    (62, 0.552),
    (63, 0.537),
    (64, 0.577),
    (65, 0.692),
    (66, 0.786),
    (67, 0.788),
    (68, 0.788),
    (69, 0.79),
    (70, 0.793),
    (71, 0.794),
    (72, 0.816),
    (73, 0.823),
    (74, 0.789),
    (75, 0.841),
    (76, 0.823),
    (77, 0.833),
    (78, 0.816),
    (79, 0.831),
    (80, 0.923),
    (81, 0.744),
    (82, 0.723),
    (83, 0.749),
    (84, 0.79),
    (85, 0.792),
    (86, 0.695),
    (87, 0.776),
    (88, 0.768),
    (89, 0.792),
    (90, 0.759),
    (91, 0.707),
    (92, 0.708),
    (93, 0.682),
    (94, 0.701),
    (95, 0.826),
    (96, 0.815),
    (97, 0.789),
    (98, 0.789),
    (99, 0.707),
    (100, 0.687),
    (101, 0.696),
    (102, 0.689),
    (103, 0.786),
    (104, 0.787),
    (105, 0.713),
    (106, 0.791),
    (107, 0.785),
    (108, 0.791),
    (109, 0.873),
    (110, 0.761),
    (111, 0.762),
    (112, 0.762),
    (113, 0.759),
    (114, 0.759),
    (115, 0.892),
    (116, 0.892),
    (117, 0.788),
    (118, 0.784),
    (119, 0.438),
    (120, 0.138),
    (121, 0.277),
    (122, 0.415),
    (123, 0.392),
    (124, 0.392),
    (125, 0.668),
    (126, 0.668),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (183, 0.788),
    (161, 0.732),
    (162, 0.544),
    (163, 0.544),
    (164, 0.91),
    (165, 0.667),
    (166, 0.76),
    (167, 0.76),
    (168, 0.776),
    (169, 0.595),
    (170, 0.694),
    (171, 0.626),
    (172, 0.788),
    (173, 0.788),
    (174, 0.788),
    (175, 0.788),
    (176, 0.788),
    (177, 0.788),
    (178, 0.788),
    (179, 0.788),
    (180, 0.788),
    (181, 0.788),
    (182, 0.788),
    (183, 0.788),
    (184, 0.788),
    (185, 0.788),
    (186, 0.788),
    (187, 0.788),
    (188, 0.788),
    (189, 0.788),
    (190, 0.788),
    (191, 0.788),
    (192, 0.788),
    (193, 0.788),
    (194, 0.788),
    (195, 0.788),
    (196, 0.788),
    (197, 0.788),
    (198, 0.788),
    (199, 0.788),
    (200, 0.788),
    (201, 0.788),
    (202, 0.788),
    (203, 0.788),
    (204, 0.788),
    (205, 0.788),
    (206, 0.788),
    (207, 0.788),
    (208, 0.788),
    (209, 0.788),
    (210, 0.788),
    (211, 0.788),
    (212, 0.894),
    (213, 0.838),
    (214, 1.016),
    (215, 0.458),
    (216, 0.748),
    (217, 0.924),
    (218, 0.748),
    (219, 0.918),
    (220, 0.927),
    (221, 0.928),
    (222, 0.928),
    (223, 0.834),
    (224, 0.873),
    (225, 0.828),
    (226, 0.924),
    (227, 0.924),
    (228, 0.917),
    (229, 0.93),
    (230, 0.931),
    (231, 0.463),
    (232, 0.883),
    (233, 0.836),
    (234, 0.836),
    (235, 0.867),
    (236, 0.867),
    (237, 0.696),
    (238, 0.696),
    (239, 0.874),
    (183, 0.788),
    (241, 0.874),
    (242, 0.76),
    (243, 0.946),
    (244, 0.771),
    (245, 0.865),
    (246, 0.771),
    (247, 0.888),
    (248, 0.967),
    (249, 0.888),
    (250, 0.831),
    (251, 0.873),
    (252, 0.927),
    (253, 0.97),
    (183, 0.788),
    (183, 0.788),
)

# ------------------------------------------------------------------------------
# Glyph list for the built-in font 'Symbol'
# ------------------------------------------------------------------------------
symbol_glyphs = (
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (32, 0.25),
    (33, 0.333),
    (34, 0.713),
    (35, 0.5),
    (36, 0.549),
    (37, 0.833),
    (38, 0.778),
    (39, 0.439),
    (40, 0.333),
    (41, 0.333),
    (42, 0.5),
    (43, 0.549),
    (44, 0.25),
    (45, 0.549),
    (46, 0.25),
    (47, 0.278),
    (48, 0.5),
    (49, 0.5),
    (50, 0.5),
    (51, 0.5),
    (52, 0.5),
    (53, 0.5),
    (54, 0.5),
    (55, 0.5),
    (56, 0.5),
    (57, 0.5),
    (58, 0.278),
    (59, 0.278),
    (60, 0.549),
    (61, 0.549),
    (62, 0.549),
    (63, 0.444),
    (64, 0.549),
    (65, 0.722),
    (66, 0.667),
    (67, 0.722),
    (68, 0.612),
    (69, 0.611),
    (70, 0.763),
    (71, 0.603),
    (72, 0.722),
    (73, 0.333),
    (74, 0.631),
    (75, 0.722),
    (76, 0.686),
    (77, 0.889),
    (78, 0.722),
    (79, 0.722),
    (80, 0.768),
    (81, 0.741),
    (82, 0.556),
    (83, 0.592),
    (84, 0.611),
    (85, 0.69),
    (86, 0.439),
    (87, 0.768),
    (88, 0.645),
    (89, 0.795),
    (90, 0.611),
    (91, 0.333),
    (92, 0.863),
    (93, 0.333),
    (94, 0.658),
    (95, 0.5),
    (96, 0.5),
    (97, 0.631),
    (98, 0.549),
    (99, 0.549),
    (100, 0.494),
    (101, 0.439),
    (102, 0.521),
    (103, 0.411),
    (104, 0.603),
    (105, 0.329),
    (106, 0.603),
    (107, 0.549),
    (108, 0.549),
    (109, 0.576),
    (110, 0.521),
    (111, 0.549),
    (112, 0.549),
    (113, 0.521),
    (114, 0.549),
    (115, 0.603),
    (116, 0.439),
    (117, 0.576),
    (118, 0.713),
    (119, 0.686),
    (120, 0.493),
    (121, 0.686),
    (122, 0.494),
    (123, 0.48),
    (124, 0.2),
    (125, 0.48),
    (126, 0.549),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (183, 0.46),
    (160, 0.25),
    (161, 0.62),
    (162, 0.247),
    (163, 0.549),
    (164, 0.167),
    (165, 0.713),
    (166, 0.5),
    (167, 0.753),
    (168, 0.753),
    (169, 0.753),
    (170, 0.753),
    (171, 1.042),
    (172, 0.713),
    (173, 0.603),
    (174, 0.987),
    (175, 0.603),
    (176, 0.4),
    (177, 0.549),
    (178, 0.411),
    (179, 0.549),
    (180, 0.549),
    (181, 0.576),
    (182, 0.494),
    (183, 0.46),
    (184, 0.549),
    (185, 0.549),
    (186, 0.549),
    (187, 0.549),
    (188, 1),
    (189, 0.603),
    (190, 1),
    (191, 0.658),
    (192, 0.823),
    (193, 0.686),
    (194, 0.795),
    (195, 0.987),
    (196, 0.768),
    (197, 0.768),
    (198, 0.823),
    (199, 0.768),
    (200, 0.768),
    (201, 0.713),
    (202, 0.713),
    (203, 0.713),
    (204, 0.713),
    (205, 0.713),
    (206, 0.713),
    (207, 0.713),
    (208, 0.768),
    (209, 0.713),
    (210, 0.79),
    (211, 0.79),
    (212, 0.89),
    (213, 0.823),
    (214, 0.549),
    (215, 0.549),
    (216, 0.713),
    (217, 0.603),
    (218, 0.603),
    (219, 1.042),
    (220, 0.987),
    (221, 0.603),
    (222, 0.987),
    (223, 0.603),
    (224, 0.494),
    (225, 0.329),
    (226, 0.79),
    (227, 0.79),
    (228, 0.786),
    (229, 0.713),
    (230, 0.384),
    (231, 0.384),
    (232, 0.384),
    (233, 0.384),
    (234, 0.384),
    (235, 0.384),
    (236, 0.494),
    (237, 0.494),
    (238, 0.494),
    (239, 0.494),
    (183, 0.46),
    (241, 0.329),
    (242, 0.274),
    (243, 0.686),
    (244, 0.686),
    (245, 0.686),
    (246, 0.384),
    (247, 0.549),
    (248, 0.384),
    (249, 0.384),
    (250, 0.384),
    (251, 0.384),
    (252, 0.494),
    (253, 0.494),
    (254, 0.494),
    (183, 0.46),
)


class linkDest(object):
    """link or outline destination details"""

    def __init__(self, obj, rlink):
        isExt = obj.isExternal
        isInt = not isExt
        self.dest = ""
        self.fileSpec = ""
        self.flags = 0
        self.isMap = False
        self.isUri = False
        self.kind = LINK_NONE
        self.lt = Point(0, 0)
        self.named = ""
        self.newWindow = ""
        self.page = obj.page
        self.rb = Point(0, 0)
        self.uri = obj.uri
        if rlink and not self.uri.startswith("#"):
            self.uri = "#%i,%g,%g" % (rlink[0] + 1, rlink[1], rlink[2])
        if obj.isExternal:
            self.page = -1
            self.kind = LINK_URI
        if not self.uri:
            self.page = -1
            self.kind = LINK_NONE
        if isInt and self.uri:
            if self.uri.startswith("#"):
                self.named = ""
                self.kind = LINK_GOTO
                ftab = self.uri[1:].split(",")
                if len(ftab) == 3:
                    self.page = int(ftab[0]) - 1
                    self.lt = Point(float(ftab[1]), float(ftab[2]))
                    self.flags = self.flags | LINK_FLAG_L_VALID | LINK_FLAG_T_VALID
                else:
                    try:
                        self.page = int(ftab[0]) - 1
                    except:
                        self.kind = LINK_NAMED
                        self.named = self.uri[1:]
            else:
                self.kind = LINK_NAMED
                self.named = self.uri
        if obj.isExternal:
            if self.uri.startswith(("http://", "https://", "mailto:", "ftp://")):
                self.isUri = True
                self.kind = LINK_URI
            elif self.uri.startswith("file://"):
                self.fileSpec = self.uri[7:]
                self.isUri = False
                self.uri = ""
                self.kind = LINK_LAUNCH
                ftab = self.fileSpec.split("#")
                if len(ftab) == 2:
                    if ftab[1].startswith("page="):
                        self.kind = LINK_GOTOR
                        self.fileSpec = ftab[0]
                        self.page = int(ftab[1][5:]) - 1
            else:
                self.isUri = True
                self.kind = LINK_LAUNCH


# -------------------------------------------------------------------------------
# "Now" timestamp in PDF Format
# -------------------------------------------------------------------------------
def getPDFnow() -> str:
    import time

    tz = "%s'%s'" % (
        str(abs(time.altzone // 3600)).rjust(2, "0"),
        str((abs(time.altzone // 60) % 60)).rjust(2, "0"),
    )
    tstamp = time.strftime("D:%Y%m%d%H%M%S", time.localtime())
    if time.altzone > 0:
        tstamp += "-" + tz
    elif time.altzone < 0:
        tstamp += "+" + tz
    else:
        pass
    return tstamp


def getPDFstr(s: str) -> str:
    """ Return a PDF string depending on its coding.

    Notes:
        Returns a string bracketed with either "()" or "<>" for hex values.
        If only ascii then "(original)" is returned, else if only 8 bit chars
        then "(original)" with interspersed octal strings \nnn is returned,
        else a string "<FEFF[hexstring]>" is returned, where [hexstring] is the
        UTF-16BE encoding of the original.
    """
    if not bool(s):
        return "()"

    def make_utf16be(s):
        r = bytearray([254, 255]) + bytearray(s, "UTF-16BE")
        return "<" + r.hex() + ">"  # brackets indicate hex

# The following either returns the original string with mixed-in
# octal numbers \nnn for chars outside the ASCII range, or returns
# the UTF-16BE BOM version of the string.
    r = ""
    for c in s:
        oc = ord(c)
        if oc > 255:  # shortcut if beyond 8-bit code range
            return make_utf16be(s)

        if oc > 31 and oc < 127:  # in ASCII range
            if c in ("(", ")", "\\"):  # these need to be escaped
                r += "\\"
            r += c
            continue

        if oc > 127:  # beyond ASCII
            r += "\\%03o" % oc
            continue

# now the white spaces
        if oc == 8:  # backspace
            r += "\\b"
        elif oc == 9:  # tab
            r += "\\t"
        elif oc == 10:  # line feed
            r += "\\n"
        elif oc == 12:  # form feed
            r += "\\f"
        elif oc == 13:  # carriage return
            r += "\\r"
        else:
            r += "\\267"  # unsupported: replace by 0xB7

    return "(" + r + ")"


def getTJstr(text: str, glyphs: typing.Union[list, tuple, None], simple: bool, ordering: int) -> str:
    """ Return a PDF string enclosed in [] brackets, suitable for the PDF TJ
    operator.

    Notes:
        The input string is converted to either 2 or 4 hex digits per character.
    Args:
        simple: no glyphs: 2-chars, use char codes as the glyph
                glyphs: 2-chars, use glyphs instead of char codes (Symbol,
                ZapfDingbats)
        not simple: ordering < 0: 4-chars, use glyphs not char codes
                    ordering >=0: a CJK font! 4 chars, use char codes as glyphs
    """
    if text.startswith("[<") and text.endswith(">]"):  # already done
        return text

    if not bool(text):
        return "[<>]"

    if simple:  # each char or its glyph is coded as a 2-byte hex
        if glyphs is None:  # not Symbol, not ZapfDingbats: use char code
            otxt = "".join(["%02x" % ord(c) if ord(c) < 256 else "b7" for c in text])
        else:  # Symbol or ZapfDingbats: use glyphs
            otxt = "".join(
                ["%02x" % glyphs[ord(c)][0] if ord(c) < 256 else "b7" for c in text]
            )
        return "[<" + otxt + ">]"

# non-simple fonts: each char or its glyph is coded as 4-byte hex
    if ordering < 0:  # not a CJK font: use the glyphs
        otxt = "".join(["%04x" % glyphs[ord(c)][0] for c in text])
    else:  # CJK: use the char codes
        otxt = "".join(["%04x" % ord(c) for c in text])

    return "[<" + otxt + ">]"


"""
Information taken from the following web sites:
www.din-formate.de
www.din-formate.info/amerikanische-formate.html
www.directtools.de/wissen/normen/iso.htm
"""
paperSizes = {  # known paper formats @ 72 dpi
    "a0": (2384, 3370),
    "a1": (1684, 2384),
    "a10": (74, 105),
    "a2": (1191, 1684),
    "a3": (842, 1191),
    "a4": (595, 842),
    "a5": (420, 595),
    "a6": (298, 420),
    "a7": (210, 298),
    "a8": (147, 210),
    "a9": (105, 147),
    "b0": (2835, 4008),
    "b1": (2004, 2835),
    "b10": (88, 125),
    "b2": (1417, 2004),
    "b3": (1001, 1417),
    "b4": (709, 1001),
    "b5": (499, 709),
    "b6": (354, 499),
    "b7": (249, 354),
    "b8": (176, 249),
    "b9": (125, 176),
    "c0": (2599, 3677),
    "c1": (1837, 2599),
    "c10": (79, 113),
    "c2": (1298, 1837),
    "c3": (918, 1298),
    "c4": (649, 918),
    "c5": (459, 649),
    "c6": (323, 459),
    "c7": (230, 323),
    "c8": (162, 230),
    "c9": (113, 162),
    "card-4x6": (288, 432),
    "card-5x7": (360, 504),
    "commercial": (297, 684),
    "executive": (522, 756),
    "invoice": (396, 612),
    "ledger": (792, 1224),
    "legal": (612, 1008),
    "legal-13": (612, 936),
    "letter": (612, 792),
    "monarch": (279, 540),
    "tabloid-extra": (864, 1296),
}


def PaperSize(s: str) -> tuple:
    """Return a tuple (width, height) for a given paper format string.

    Notes:
        'A4-L' will return (842, 595), the values for A4 landscape.
        Suffix '-P' and no suffix return the portrait tuple.
    """
    size = s.lower()
    f = "p"
    if size.endswith("-l"):
        f = "l"
        size = size[:-2]
    if size.endswith("-p"):
        size = size[:-2]
    rc = paperSizes.get(size, (-1, -1))
    if f == "p":
        return rc
    return (rc[1], rc[0])


def PaperRect(s: str) -> Rect:
    """Return a Rect for the paper size indicated in string 's'. Must conform to the argument of method 'PaperSize', which will be invoked.
    """
    width, height = PaperSize(s)
    return Rect(0.0, 0.0, width, height)


def CheckParent(o: typing.Any):
    if not hasattr(o, "parent") or o.parent is None:
        raise ValueError("orphaned object: parent is None")


def CheckColor(c: OptSeq):
    if c:
        if (
            type(c) not in (list, tuple)
            or len(c) not in (1, 3, 4)
            or min(c) < 0
            or max(c) > 1
        ):
            raise ValueError("need 1, 3 or 4 color components in range 0 to 1")


def ColorCode(c: typing.Union[list, tuple, float, None], f: str) -> str:
    if not c:
        return ""
    if hasattr(c, "__float__"):
        c = (c,)
    CheckColor(c)
    if len(c) == 1:
        s = "%g " % c[0]
        return s + "G " if f == "c" else s + "g "

    if len(c) == 3:
        s = "%g %g %g " % tuple(c)
        return s + "RG " if f == "c" else s + "rg "

    s = "%g %g %g %g " % tuple(c)
    return s + "K " if f == "c" else s + "k "


def JM_TUPLE(o: typing.Sequence) -> tuple:
    return tuple(map(lambda x: round(x, 5) if abs(x) >= 1e-4 else 0, o))


def CheckRect(r: typing.Any) -> bool:
    """Check whether an object is non-degenerate rect-like.

    It must be a sequence of 4 numbers.
    """
    try:
        r = Rect(r)
    except:
        return False
    return not (r.isEmpty or r.isInfinite)


def CheckQuad(q: typing.Any) -> bool:
    """Check whether an object is convex, not empty  quad-like.

    It must be a sequence of 4 number pairs.
    """
    try:
        q0 = Quad(q)
    except:
        return False
    return q0.isConvex


def CheckMarkerArg(quads: typing.Any) -> tuple:
    if CheckRect(quads):
        r = Rect(quads)
        return (r.quad,)
    if CheckQuad(quads):
        return (quads,)
    for q in quads:
        if not (CheckRect(q) or CheckQuad(q)):
            raise ValueError("bad quads entry")
    return quads


def CheckMorph(o: typing.Any) -> bool:
    if not bool(o):
        return False
    if not (type(o) in (list, tuple) and len(o) == 2):
        raise ValueError("morph must be a sequence of length 2")
    if not (len(o[0]) == 2 and len(o[1]) == 6):
        raise ValueError("invalid morph parm 0")
    if not o[1][4] == o[1][5] == 0:
        raise ValueError("invalid morph parm 1")
    return True


def CheckFont(page: "struct Page *", fontname: str) -> tuple:
    """Return an entry in the page's font list if reference name matches.
    """
    for f in page.get_fonts():
        if f[4] == fontname:
            return f
        if f[3].lower() == fontname.lower():
            return f


def CheckFontInfo(doc: "struct Document *", xref: int) -> list:
    """Return a font info if present in the document.
    """
    for f in doc.FontInfos:
        if xref == f[0]:
            return f


def UpdateFontInfo(doc: "struct Document *", info: typing.Sequence):
    xref = info[0]
    found = False
    for i, fi in enumerate(doc.FontInfos):
        if fi[0] == xref:
            found = True
            break
    if found:
        doc.FontInfos[i] = info
    else:
        doc.FontInfos.append(info)


def DUMMY(*args, **kw):
    return


def planishLine(p1: point_like, p2: point_like) -> Matrix:
    """Return matrix which flattens out the line from p1 to p2.

    Args:
        p1, p2: point_like
    Returns:
        Matrix which maps p1 to Point(0,0) and p2 to a point on the x axis at
        the same distance to Point(0,0). Will always combine a rotation and a
        transformation.
    """
    p1 = Point(p1)
    p2 = Point(p2)
    return Matrix(TOOLS._hor_matrix(p1, p2))


def ImageProperties(img: typing.ByteString) -> dict:
    """ Return basic properties of an image.

    Args:
        img: bytes, bytearray, io.BytesIO object or an opened image file.
    Returns:
        A dictionary with keys width, height, colorspace.n, bpc, type, ext and size,
        where 'type' is the MuPDF image type (0 to 14) and 'ext' the suitable
        file extension.
    """
    if type(img) is io.BytesIO:
        stream = img.getvalue()
    elif hasattr(img, "read"):
        stream = img.read()
    elif type(img) in (bytes, bytearray):
        stream = img
    else:
        raise ValueError("bad argument 'img'")

    return TOOLS.image_profile(stream)


def ConversionHeader(i: str, filename: OptStr ="unknown"):
    t = i.lower()
    html = """<!DOCTYPE html>
<html>
<head>
<style>
body{background-color:gray}
div{position:relative;background-color:white;margin:1em auto}
p{position:absolute;margin:0}
img{position:absolute}
</style>
</head>
<body>\n"""

    xml = (
        """<?xml version="1.0"?>
<document name="%s">\n"""
        % filename
    )

    xhtml = """<?xml version="1.0"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<style>
body{background-color:gray}
div{background-color:white;margin:1em;padding:1em}
p{white-space:pre-wrap}
</style>
</head>
<body>\n"""

    text = ""
    json = '{"document": "%s", "pages": [\n' % filename
    if t == "html":
        r = html
    elif t == "json":
        r = json
    elif t == "xml":
        r = xml
    elif t == "xhtml":
        r = xhtml
    else:
        r = text

    return r


def ConversionTrailer(i: str):
    t = i.lower()
    text = ""
    json = "]\n}"
    html = "</body>\n</html>\n"
    xml = "</document>\n"
    xhtml = html
    if t == "html":
        r = html
    elif t == "json":
        r = json
    elif t == "xml":
        r = xml
    elif t == "xhtml":
        r = xhtml
    else:
        r = text

    return r


def DerotateRect(cropbox: rect_like, rect: rect_like, deg: float) -> Rect:
    """Calculate the non-rotated rect version.

    Args:
        cropbox: the page's /CropBox
        rect: rectangle
        deg: the page's /Rotate value
    Returns:
        Rectangle in original (/CropBox) coordinates
    """
    while deg < 0:
        deg += 360
    while deg >= 360:
        deg -= 360
    if deg % 90 > 0:
        deg = 0
    if deg == 0:  # no rotation: no-op
        return rect
    points = []  # store the new rect points here
    for p in rect.quad:  # run through the rect's quad points
        if deg == 90:
            q = (p.y, cropbox.height - p.x)
        elif deg == 270:
            q = (cropbox.width - p.y, p.x)
        else:
            q = (cropbox.width - p.x, cropbox.height - p.y)
        points.append(q)

    r = Rect(points[0], points[0])
    for p in points[1:]:
        r |= p
    return r


def get_highlight_selection(page, start: point_like =None, stop: point_like =None, clip: rect_like =None) -> list:
    """Return rectangles of text lines between two points.

    Notes:
        The default of 'start' is top-left of 'clip'. The default of 'stop'
        is bottom-reight of 'clip'.

    Args:
        start: start point_like
        stop: end point_like, must be 'below' start
        clip: consider this rect_like only, default is page rectangle
    Returns:
        List of line bbox intersections with the area established by the
        parameters.
    """
# validate and normalize arguments
    if clip is None:
        clip = page.rect
    clip = Rect(clip)
    if start is None:
        start = clip.tl
    if stop is None:
        stop = clip.br
    clip.y0 = start.y
    clip.y1 = stop.y
    if clip.isEmpty or clip.isInfinite:
        return []

# extract text of page, clip only, no images, expand ligatures
    blocks = page.getText(
        "dict", flags=0, clip=clip,
    )["blocks"]

    lines = []  # will return this list of rectangles
    for b in blocks:
        for line in b["lines"]:
            lines.append(Rect(line["bbox"]))

    if lines == []:  # did not select anything
        return lines

    lines.sort(key=lambda bbox: bbox.y1)  # sort by vertical positions

# cut off prefix from first line if start point is close to its top
    bboxf = lines.pop(0)
    if bboxf.y0 - start.y <= 0.1 * bboxf.height:  # close enough?
        r = Rect(start.x, bboxf.y0, bboxf.br)  # intersection rectangle
        if not (r.isEmpty or r.isInfinite):
            lines.insert(0, r)  # insert again if not empty
    else:
        lines.insert(0, bboxf)  # insert again

    if lines == []:  # the list might have been emptied
        return lines

# cut off suffix from last line if stop point is close to its bottom
    bboxl = lines.pop()
    if stop.y - bboxl.y1 <= 0.1 * bboxl.height:  # close enough?
        r = Rect(bboxl.tl, stop.x, bboxl.y1)  # intersection rectangle
        if not (r.isEmpty or r.isInfinite):
            lines.append(r)  # append if not empty
    else:
        lines.append(bboxl)  # append again

    return lines


def annot_preprocess(page: "Page") -> int:
    """Prepare for annotation insertion on the page.

    Returns:
        Old page rotation value. Temporarily sets rotation to 0 when required.
    """
    CheckParent(page)
    if not page.parent.isPDF:
        raise ValueError("not a PDF")
    old_rotation = page.rotation
    if old_rotation != 0:
        page.setRotation(0)
    return old_rotation


def annot_postprocess(page: "Page", annot: "Annot") -> None:
    """Clean up after annotation inertion.

    Set ownership flag and store annotation in page annotation dictionary.
    """
    annot.parent = weakref.proxy(page)
    page._annot_refs[id(annot)] = annot
    annot.thisown = True


def sRGB_to_rgb(srgb: int) -> tuple:
    """Convert sRGB color code to an RGB color triple.

    There is **no error checking** for performance reasons!

    Args:
        srgb: (int) RRGGBB (red, green, blue), each color in range(255).
    Returns:
        Tuple (red, green, blue) each item in intervall 0 <= item <= 255.
    """
    r = srgb >> 16
    g = (srgb - (r << 16)) >> 8
    b = srgb - (r << 16) - (g << 8)
    return (r, g, b)


def sRGB_to_pdf(srgb: int) -> tuple:
    """Convert sRGB color code to a PDF color triple.

    There is **no error checking** for performance reasons!

    Args:
        srgb: (int) RRGGBB (red, green, blue), each color in range(255).
    Returns:
        Tuple (red, green, blue) each item in intervall 0 <= item <= 1.
    """
    t = sRGB_to_rgb(srgb)
    return t[0] / 255.0, t[1] / 255.0, t[2] / 255.0


def make_table(rect: rect_like =(0, 0, 1, 1), cols: int =1, rows: int =1) -> list:
    """Return a list of (rows x cols) equal sized rectangles.

    Notes:
        A utility to fill a given area with table cells of equal size.
    Args:
        rect: rect_like to use as the table area
        rows: number of rows
        cols: number of columns
    Returns:
        A list with <rows> items, where each item is a list of <cols>
        PyMuPDF Rect objects of equal sizes.
    """
    rect = Rect(rect)  # ensure this is a Rect
    if rect.isEmpty or rect.isInfinite:
        raise ValueError("rect must be finite and not empty")
    tl = rect.tl

    height = rect.height / rows  # height of one table cell
    width = rect.width / cols  # width of one table cell
    delta_h = (width, 0, width, 0)  # diff to next right rect
    delta_v = (0, height, 0, height)  # diff to next lower rect

    r = Rect(tl, tl.x + width, tl.y + height)  # first rectangle

# make the first row
    row = [r]
    for i in range(1, cols):
        r += delta_h  # build next rect to the right
        row.append(r)

# make result, starts with first row
    rects = [row]
    for i in range(1, rows):
        row = rects[i - 1]  # take previously appended row
        nrow = []  # the new row to append
        for r in row:  # for each previous cell add its downward copy
            nrow.append(r + delta_v)
        rects.append(nrow)  # append new row to result

    return rects


def repair_mono_font(page: "Page", font: "Font") -> None:
    """Repair character spacing for mono fonts.

    Notes:
        Some mono-spaced fonts are displayed with a too large character
        distance, e.g. "a b c" instead of "abc". This utility adds an entry
        "/W[0 65535 w]" to the descendent font(s) of font. The float w is
        taken to be the width of 0x20 (space).
        This should enforce viewers to use 'w' as the character width.

    Args:
        page: fitz.Page object.
        font: fitz.Font object.
    """
    if not font.flags["mono"]:  # font not flagged as monospaced
        return None
    doc = page.parent  # the document
    fontlist = page.get_fonts()  # list of fonts on page
    xrefs = [  # list of objects referring to font
        f[0]
        for f in fontlist
        if (f[3] == font.name and f[4].startswith("F") and f[5].startswith("Identity"))
    ]
    if xrefs == []:  # our font does not occur
        return
    xrefs = set(xrefs)  # drop any double counts
    width = int(round((font.glyph_advance(32) * 1000)))
    for xref in xrefs:
        if not TOOLS.set_font_width(doc, xref, width):
            print("Cannot set width for '%s' in xref %i" % (font.name, xref))


# Adobe Glyph List functions
import base64, gzip

_adobe_glyphs = {}
_adobe_unicodes = {}
def unicode_to_glyph_name(ch: int) -> str:
    if _adobe_glyphs == {}:
        for line in _get_glyph_text():
            if line.startswith("#"):
                continue
            name, unc = line.split(";")
            uncl = unc.split()
            for unc in uncl:
                c = int(unc[:4], base=16)
                _adobe_glyphs[c] = name
    return _adobe_glyphs.get(ch, ".notdef")


def glyph_name_to_unicode(name: str) -> int:
    if _adobe_unicodes == {}:
        for line in _get_glyph_text():
            if line.startswith("#"):
                continue
            gname, unc = line.split(";")
            c = int(unc[:4], base=16)
            _adobe_unicodes[gname] = c
    return _adobe_unicodes.get(name, 65533)

def adobe_glyph_names() -> tuple:
    if _adobe_unicodes == {}:
        for line in _get_glyph_text():
            if line.startswith("#"):
                continue
            gname, unc = line.split(";")
            c = int("0x" + unc[:4], base=16)
            _adobe_unicodes[gname] = c
    return tuple(_adobe_unicodes.keys())

def adobe_glyph_unicodes() -> tuple:
    if _adobe_unicodes == {}:
        for line in _get_glyph_text():
            if line.startswith("#"):
                continue
            gname, unc = line.split(";")
            c = int("0x" + unc[:4], base=16)
            _adobe_unicodes[gname] = c
    return tuple(_adobe_unicodes.values())

def _get_glyph_text() -> bytes:
    return gzip.decompress(base64.b64decode(
    b'H4sIABmRaF8C/7W9SZfjRpI1useviPP15utzqroJgBjYWhEkKGWVlKnOoapVO0YQEYSCJE'
    b'IcMhT569+9Ppibg8xevHdeSpmEXfPBfDZ3N3f/t7u//r//k/zb3WJ4eTv2T9vzXTaZZH/N'
    b'Junsbr4Z7ru7/7s9n1/+6z//8/X19T/WRP7jYdj/57//R/Jv8Pax2/Sn87G/v5z74XC3Pm'
    b'zuLqfurj/cnYbL8aEzyH1/WB/f7h6H4/70l7vX/ry9G47wzK/hcr7bD5v+sX9YM4i/3K2P'
    b'3d1Ld9z353O3uXs5Dl/7DT7O2/UZ/3Tw9zjsdsNrf3i6exgOm57eTsbbvjv/1w2xTnfDo5'
    b'fnYdjA3eV0vjt25zXkRJB36/vhKwN+kEw4DOf+ofsLuP3pboewGISO7bAxPkUU+EaUD7t1'
    b'v++O/3FTCESmcsILgQRuLhDs/w857lz6NsPDZd8dzmtfSP85HO8GcI53+/W5O/br3QkeJa'
    b'9NERmPKgE2Ue+73vgj97Ded5TH1pPDEFCT4/35RFFtAMORMezXb3dwiioCsYe77rABjjCO'
    b'jHs/nLs7mx3wuYFYX+HsEQyTfHg/DY/nVxa0rzmnl+6BVQfeegTyemSlOdjqczqJ0J9/ev'
    b'fp7tOH1ed/zj+2d/j+9eOHf7xbtsu75jcw27vFh19/+/jux58+3/304edl+/HT3fz9kq3i'
    b'w/vPH981Xz5/APR/5p/g9/+Qhb+/3bX/8+vH9tOnuw8f79798uvP7xAcwv84f//5XfvpL/'
    b'D97v3i5y/Ld+9//Msdgrh7/+Hz3c/vfnn3GQ4/f/iLifja492HFbz+0n5c/ARg3rz7+d3n'
    b'30ycq3ef3zO+FSKc3/06//j53eLLz/OPd79++fjrh0/tHRIHr8t3nxY/z9/90i7/AxIg1r'
    b'v2H+37z3effpr//PPN1CIF47Q2LUSdNz+3NjakdvnuY7v4/BcEGb4WyEPI+DMT++nXdvEO'
    b'n8iWFomaf/ztL8wZhPqp/e8vcAbm3XL+y/xHpPH/xlnDejXKHJTQ4svH9hdK/mF19+lL8+'
    b'nzu89fPrd3P374sDSZ/qn9+I93i/bTD/D+8wcWxOruy6f2L4jl89xEjkCQaZ9+4Hfz5dM7'
    b'k33v3n9uP3788uvndx/e/zu8/vThn8ggSDqH56XJ6Q/vTZKRVx8+/sZgmRemIP5y98+fWu'
    b'Ao8vc+z+bMjE/Iu8Vn7RBxIis/q7TevW9//Pndj+37RWuz/AND+ue7T+2/o+zefaKTdzbq'
    b'f84R7xeTdJYYJLOf7z4xq11N/osp2bt3q7v58h/vKLxzjtrw6Z2rOSbzFj+5rEd7+P84UL'
    b'xH8/6vO/lj2/6Pu7eX7d3P6C3Y2tb3u+7ua3dkA/yvu+w/JqyV6GeUt0/dy7nb36MjySZ/'
    b'MUMO3Hz5+LNycsdx54SB5wmN/XJvRh0z/vz1/PaCf4Zhd/rP9dPur/j7eDDtfIV+dX3+r7'
    b'vz63B36vb9w7AbDn/ddLseown7kr7bbU4YIhD6/03//e7JiM0O669/vbyg1/hPdKLd8WGN'
    b'PmnXoSs52h5200OGk/WW/fvdl0NvhpHTw3q3Pt59Xe8uCOARA8ydCcX433Z/rjfonfbrnf'
    b'hP5j9MJtM0mbf4XZT4XT9czt0Pk3S1ALFfPxyHA6g2A3WCz90Pq6qFO+dsskjdtzAB3B+7'
    b'rwwDeWi/reu0nbcOeMBostv1Dz9MpsuJwzbD+b5DcuGuKR32dFx/pcfGO9oOw7MZlAj64M'
    b'/9bmOAaTJ/WFuJF0t898eHXfdDNmV4JC77x133J8XONCDiTTWq5JkvNMMLNY9C1ZLNa82R'
    b'rIki9ULP50AZ/6pczOyn92DSE3IqRSZs7nc2+gmqKMi+O3an/sQkTQOpszcLsBTnsg2gSE'
    b'f/KskTQ4YaANrFPFn4b/ELIEo/Iu2jQkbg/QEtEJXe1Y6MtWP3sl3/MMlnqf08D4cBaclr'
    b'5KzEzHTuyXhZPyCXVhkcD0/DoXsmEwEfoWVQqsJ+Sg2eW9qniOGQFqHh3n+XCNMWCMLJ3b'
    b'c4BPB2vz5CYenXkKjI06Rhu8mSJlSxKmmQX+uHB6g1jC0ztEQ+TRqdISmC6A46TLiH/sfM'
    b'wBczE0mo4WrXHzoJpUyaKCvglLnpJC1XiEWSBN55eIHcDChLFpQ4TxZrHWkL2mUXwl6Yto'
    b'N6OLefEmyRLHy7mizwDT1yt1szryqhfCOa1AJJBtKVZFRtCd8WU3pATvFrbr5cHlo6Dome'
    b'tzoF0xmAbn3/vF2fgKgcbhbkKCCrCKBYETp0uZt+2siJ5pSGc92+kOVgbLVIOREE/rw+jc'
    b'JfNGSxGWBysYMmOzxrCU3qelSBOUV1VQCf456kXEGaqB4gykGJUKTJQupBnixZ9NNk+S+2'
    b'ihS/0kkCjOoD6ccjhCO3niVLKfYW367Y0xY90TIU6MwSVkRfVdMM6HFYsxzpPGobc0NLrV'
    b'4ky6htQIoOA9rLmWTeIupuh6aRZaij5vPp2LH15zO49PmEMH1niBrcCCWd60KgH00/Bmgp'
    b'kM8t9NzL/mm930scS/j7XYuHlr2MGiXkiwoDQvnESoFVyfKEarx1uSGFA7ehkULobywiRP'
    b'BNiqgAcbOCo9MFRwtGp1GVn6wSDuzTImllwJ65b2mcAPyAjZxvfcTpHN+2xC0bZboApKt6'
    b'joBDPZhbIgyyEeD7B7Sx9kZ1qTWqKgeUkvZ66MUI1N4eejGytzeG3kgUP/QumFyVWyD1+E'
    b'pSja9NICVYYqbrSkvzJV2Xo0WhQfIedV+EsGU0rd23hAogyuUKtNZ7kBjOxTEPBT9LS/Cv'
    b'BlfE32OqDgVzo+JFfWt3uqkhATv4OEhYCFtGXrRhR/jCY7Is4kuCVWavQ0QdiVoDqoiute'
    b'kS9K0eFjpDy3E8nc75EdVjKGbtgVmg+1KkWtQAVp/hpaPQM1SNl1O/YwryWeEJUS3gUkeb'
    b'wTnzDLP+DdtgG0jtClLrXh86SHu6mQoIb1r5HM1KWjmksEN7xQ9VsjVpEQ1ezvA7gUqMD+'
    b'97RcpruAv3Le0G8V2Oww/ZBDpq+40xQxPBh2/G6D1BqRSiKq7YJ5TJKjTdJlnpDjptk1U0'
    b'phVwrbvkabJy/S5Ut1UPnyELqgwIovM1Cm6jCoGgMDERdp6sJJ/K5EeKViU/Nqc/Lutj90'
    b'OeYwD8UVS6Kb7RNzMrc/sZhqsZmYenfh3EnCc/StfWJj9KniAe0WFSKFE/hpxYWEK0k5TA'
    b'wIh806Z72+hRd37UjZ50NJBBxu16o3UD+N1iHrjZ7LpRfab42+5KJ5gZH5eX8+WomxFq+Y'
    b'++BBALJnWqVgGIRywArlFjJgefUXkgf/142NpPKQ84le/KfdtYs1kD2gjLDJ0mP7Hg6uSn'
    b'tEb8P2TFYmW+p/xGo+B3kfK7SX7CQF4ZPE1++lUKGh3sT+tbAx3G5J/WN5WyDIzj5tQ/ae'
    b'cZYrMDKqraT6b8fWshK2gxGcINBb+0hBQ8uuifpPuHY4SlmwhqwU+qg6frKFcRttbIphPQ'
    b'R9WCwJesxfcF85bjZb9bX84siFWEiBYBh98kv1AF3jHTZ8k7PUvMVsm7v0F+TCjefdF4m7'
    b'wTJWDpvmXIAeBbSrZI3on2gcBCFrWWCAN8BEhYRFXlK5N3elStQapRdRVIP8hQ0huaNirZ'
    b'u6sBmN5NW8wn5kvaoqNFjZgn77qrpQeIFrXXInn3eFw/o62hZ8IU7Z2M0Qv3LREDiNQOJK'
    b'vXQZEej8mQoT9th+NZO0TxyYCL+ukInW4UZFS14AO1SrX3Jnk36ByH4DIyMjMHO/jMzJfq'
    b'MEsDhNLI0VCJyIAEUiopfEt7xzj2zk2XU9T0d9GQxPrzbdufT9GgMPWgrwuaWSZ/Y02eJ3'
    b'+L5nZp8rdQ+VaWkPaJucrfok6uTv42mog1yd+ijEP4kpx58ndG2SR/V0NNkfz976E/WiZ/'
    b'X99DZ3/uoxF+AtjV1Nx8q8JEqDd7qhkZYwUmB/byYoqG7OuuvwX63cnibJH8XQa0Gt8yoO'
    b'UlKJ9v0JT/Ho9fZKuWgX7i7/FYPwUQLU2skr9vdTKh0/19q9UBhOgHI0gSjz0QU8+WUGx/'
    b'jwoFJTAgF5SXemIhmYEhH066cZUEfEE2yc8syEXyM3s9aIU//4yuEtXlZ6815DN87+83Jq'
    b'fh3OdavsR3yDVyJNdSS8STlByRjPISnlz/szJfgWNp8VoGUoZiqH8/969RViOG35kMcOJs'
    b'RBqibJwnP0fZCI9+gol2Y79l3IBnya9F8gvza5n8oip+mfxihVqVUD7tt0yJVwRchW+TX0'
    b'ImZckvekjEGPeLSjJ0nV+iejSdJr9EMkMGEQvfVHGMioqq/cuFhbVI3lPWNnlvynaevPdl'
    b'Os2T974coS++D+WIye77IGJuibgc0dG8j8uRnqKkTA0tHsrkPSv4rnuk69kyeY+yEBW2Tt'
    b'6bQmvwGxUa4tGFBv3ofZQBSNjwqnMI8UiOgOmXJJep+5Y5AQCTQ8vkA3NolXzARD8tMvxK'
    b'qc+TD37AX+buWwIAACXpGM1y0I048Nbwi+C8ioAS+eBzH7J9YK7Bw8aPCTPIE8pgaglRG5'
    b'YR4KsW6t2HmysAy1oz/LxzmWlUD8Vx8JLgCPXzKWgAH3T/jXRhfPKVrJgYUlSXBcigutDv'
    b'rXxSsEROTCkjCMiMz1JUDQCnajBhkaqxAhD1zwXoPeodVNIPkQ7Skj6yUDBImU/J3LmllR'
    b'BtZiHJ0IWlo6x0IfrsahmsVlVtHvWMEcFdKTzwLroNeugP8WICa2u8mMDA9t3T2iWOn7rb'
    b'd1w/LmCKbejjcDnoalzNLX7uzzutF1ULh3v1BrV031vx8pkQwqZz3VrhQjV6CCNKFtuGJc'
    b'J+CXy7FQn0rh9c3zxhZTbfMqVtHSDFTRe+D0CUduDXzrX6WJH2vUThvn0GM8sNoOYxU+9B'
    b'4iuSX+EZWf+rFMw0+TU0X/B111iUya+R0rwCHaldcwA3p7hzeLXr2/ywCsMccRkI8fevR1'
    b'3P8+RXnf9Qtn49Gac1P3QmkOOSg+//ZnLS5L9DEsrkv6OQwBT3afKR7rPkY6R7LkD7bmCa'
    b'fPS9XVHjW8Ya5MXHEEsFIhpVyFb9RzoBqXOyNrRvkMU8kKIiFJAj1s4QiJqjgL0dmCdIRt'
    b'jbKlcLknFrTJFEPRoVbfIxyhXwJVf8tw8E/ut0hJ0uLx2tXMBryuQTczFPPq24YzeZYHqP'
    b'/hJU5qh0Sir31ITU1FM1qcJRufFXOiozVOV5JpTa+zO8mXdJnoncxM4YUpElI+VdlimozL'
    b'ssycu8SxQaKC81OltQXuqS6cu81IUJxUtdVKS81MWSlJe6oJyZl7poQOXisiUlLlekxOWc'
    b'lJe6YPqmIvWMlJe6pNRTL3XJtE+91IWhvNQlZZl6qUtKPfWylCyHqZelNPF5WUrmxFRkYe'
    b'yFl6Wgv0JykPlZSA4yzwrJQaa9EFmQPmll/ls3EYqw3r/0vsvHAPTJN8XSf0ceSgdKS0BB'
    b'qAaLzH7YvvITvb/51OsBtYVubaNDutDSa0vIXJTlGzX9jDU6kmtiaN/2WOU8GTmDt7gzhf'
    b'jR+jzSF2+AVgT05AxBbB9iCIUVzdcQ+zZy0SB5236vlk6Rov7JrLTOUYD9nyIAqkHUa4A7'
    b'PJ7Ha3DwLn0JXJwZlszn5slndhbT5POaSiyGgM92wQ6p+yzFCzQUHDLsc8j/mSVirR49/+'
    b'e4/6WnKHfnhpZCWCSfow1iOL+5+Tunw1AEiL07n6KNW8i6dbv3NT7d0LbgJ/WxCRQp8ymD'
    b'Lmlkh4SJqNWgXJIfzwyh4n/WvTemB5+jcoAIesERk97PUEgee6OwNwtDnXrW1npqiPPrQC'
    b'Gr5POxg47h1WhiCDtKH5Sxz6d4Z7EB4gsY4b12O7XkD+brIFSafGFxF8kXmY7M3bfkBwA/'
    b'uUCxfJHJRY5vKfa5JcJEotGA1INSoxID3aoUIWCl6aPufNEj9RSk0vQXgfQ+llXAJOYsYJ'
    b'KCmcKU2cAkwC7WlMm5NtUpAihpoTxKk4e0MnuYuW9xC0Cr9JiefPGThJX99Gofpn9fRpME'
    b'iqknCVB0v4wnCegqvkSThBZ0PElg9mpIZwTy7EpTgYxab6wgmGQIGvGX6zXS1oNK1a3oUj'
    b'cRZKWo7Cwr2SacF55I2T8Jy+QM03p6298PO+nAcnEgi6lN6jG9ntqMwRuBTb2bwIuEkPkI'
    b'0mhNnVI0/i/jheQJMd8ikR7MG9bcJdb9WBvga+MTlJGfv2MY+hLNJCoPSFWfJv9goy6Tf4'
    b'T22ST/UHUHU5N/RBOFDHS02gEHrsdpwIuKCuFG2yd18g9JHHi+rmFK90+KUSX/9KLWWfLP'
    b'INLCEjJSQ+5/qipSk1QjBKZq/1RJqOvkn77q15Pkn5GIiFNEqpL/oRh18j8h6mXyPzqmBU'
    b'gd0zz5n2ikz+Ges5tZm/xPFA8ClXjq5DfGM0t+k6506b6lwRPQpY6x5bcgVWuJkCFl8luo'
    b'sSljuOpuVsC06K2hpY+YJr9hHqA714bI5Va3h+B9hqLl/+aLP7efvktZQSi9wzEtQOu6Xo'
    b'GOhkfonL9FuYYsklzDt68wFOByuu+fdAbNHXbLYGJB3q4/n3e6LkNREfiWrzr5F8tpnvwr'
    b'Mq8qQfsRZ5aIGVa1dN8y/K8ASJE5whVZ2s4myb/sonPVmC9ReBztS2aWJf+KWmAF+ub2RE'
    b'3GDa23BW7VGoi+7XRa5gTGO2qLlKiO0vi7Gafl3Ih0kfxLazqzafKvqGgRsxQtv/2uVFMk'
    b'tEmEvrFe33cYbXZoTzM06bVvLC1Zm+4rnM0mxJ8uv6+P6zPczWtLH/eXZ65RzA1/v0Z3qc'
    b'C8BXi8yML5JAf9dYD2QwU4RNq0Gncx5hGooqbre2Zlb87D7NfHZ121VxFXBYhhVScUyb8f'
    b'Xob98Dj8kNN+ay2G2Ln7FkvnlQN0vqcO03ZLlcPEENs7igySfPBipgJRZAsZiZO6vJxYQl'
    b'Q4TEXWNwyxC41qq+SlZoghdqXRyBB5pjlict0kvkZAczefJoKH/T2qelpZyFKT1FFDRLoS'
    b'KJx3LtkMXCRBYzUABm0XwJQ+Qi7nyAG9pgzuZrN+VnWsIuTqKPJB6aFQ9G7OTfMAB70Rgu'
    b'iMSw0ZlidBmxaBWh4WF5G73fNw7FDvcq7srrvgAZE89v2EO/g/QOzCkvVsmtL4aGrIdII+'
    b'yFqqe7K2xs6enFlFwJHZxFrJeDK11p+ezOyevCdzu7ftyantXjxZ2A7Ok6XdhPdkZbfaPV'
    b'nbzVpPzqwpnCPzibVj82RqzdY8mdmNAk/mdg3Uk1NrU+bJwhqLebK000xPVnYm4snaWgZ6'
    b'cma3Wh05ndiJmCdTa9LsycxO/T2Z22m/J6fWLsaThR2kPVnaGbsnK2vw5snaGo94cmZtTB'
    b'xZTKwxkidTayDrycxaH3kyt1aWnpxao1VPFtZaxJOlHeg9Wdk9fk/WdlPUkzO73ebIcmKn'
    b'qJ5M7Ua0JzOrLnsyp8WNSFVOSYpUZeEarSMpVS4FWlKqXNJbUqpc0ltSqlxCrihVLiFXlK'
    b'qQoCpKlUvyK+ZVLsmvmFe5JL8yUknyKyOVJL8yUknyKyOVJL8yUkn51kYqyY2aUuVSvjWl'
    b'mkrya0o1FZlrSjWV5NeUairJrynVVJJfU6qpJL+mVFNJb02pppLeGaWaSnpnlGoq6Z0ZqS'
    b'S9MyOVpHdmpJL0zoxUkt6ZkUrSOzNSSXpnlGomCZxRqsInEADJXEhTglMhKVVRCEmpilJI'
    b'SlVUQlKqohaSUhUzISlVMReSUhWNkEYqn8A0NVL5FKWmdU9WQpZ2DuDJyppoerK2xjmORM'
    b'ai8ovMJmMLCcpkbCnJNxlbBZIRVT75NbpNBFUJaUL26a2NVEub3gy5nE1cg8y5MDxx4mO4'
    b'JWHLrqhyVs6ynAsJ4UvXrkGyVpTlRMicZCrklGQmZEEyF7IkORWyIlkIyYjKUsgZycqRU9'
    b'aKsqyFNELOhKQYbnAhyZDdeEGSQWVeyCmLsswyIRlUlgvJBGZTIRlyVgjJBGalkExgJkKm'
    b'TGAmQnKYLjMRksN0mc2FNFKJzJmRaiGkkWoppJGqFdJIJQnkMF3mEyEpVS7p5TBd5pJeDt'
    b'NlLunlMF3mkl4O02Uu6eUwXeaSXg7TZS7p5TBd5pJeDtNlLunNjVSSXo6t5VSE5NhaTkVI'
    b'jq3lVITk2FpORUiOreVUhGTrK6ciJOt5ORUh2dzKqUjFwbScilSFEUOkKowYUgqFEUNKoT'
    b'BiSCkURgwphcKIIaXAwbQsJIEcTMtCEsjBtCwkgZURw+dkwZ6qnE+FZFBVKySDqkshGdSs'
    b'FpIJnHsxClOfq5mQTFEtjk19nqVCMkXNXEgGtfRCFqYElz6fUQ+ohXrHJUuhaLyQJRNYLH'
    b'yRoZ2DXE6EpONlKmRJMhOyIhn8MqjlVMgZSRGDWVcsSyFTkpWQGclayJzkTEgjlSShMlI1'
    b'QhqpFkIaqZZCGqkkvZWRymd7ySG+aCW97EWLVtLLIb5oJb0c4otW0sshvmglvRzii1bSyy'
    b'G+aCW9HOKLVtLL/rloJb0c4otW0jszUkl60T+vmiyQBUmf/Ap97KqZBpJc6UUrdm7FaiIk'
    b'xVilQlKMlU9ghQ5q1Ug3UnGYKJqpkExvE7imIpVCMqJGxOAwUTS1kIyoqYRkehsvVc1hom'
    b'gyIVkKTSokS6HJhaRUi+CYUi2CYyPGTEgjhq8bdW7i9XWjnpqIVkIyooWXasZONXN+yzRD'
    b'B5WlTicHiSLLUjdBK9McXVCWujlXmRY04p9kCyGnJJdCFiRbR7LRYSh3jvO0NCOsczydcS'
    b'qUUWa/kcHqqldniiRanAG57Y/rp/Vh/UPOk7jraNoPifuwMsL5Sa+XRiBU76bYnKrGR5UR'
    b'dK9iNp5V1MbDeF2IXTpvUlnfMwwz0PSHRyA7h61ogQ4M/517jTZE990mAhcER7ZUTNKNlS'
    b'aqVP14pWkagSoxdP28PuOvybd5Fsjtevf42m/O2x9WKy5ByDoAR5Fd9+i6THxJMqldgN6s'
    b'n7rT1iwGvrJpWVdx6uvWgNv1/tvalFIIJB9xRh6ngW0WM4LHYsQZeawt24olwu/WyGyR1a'
    b'VtzzWYkVjZiDMK3bOfT5fjWnxxLA9w7GU10bxxRVjlmjuqECubCS8oqpDPmc3SP7hIeQqo'
    b'SdHLFg2Vfdxu1/1xWe9+yDJqDu64PXsdfdx+DlY4bg+mXm6lHrR/6Y6n9WHzAxdWAqmdTR'
    b'TuV2eN22BPjyw7qFbIHD48aWBK4Hm7PjxvL+ftGhWWRlHAuHaYcVWFn/fH9cNzdza2uJgt'
    b'1FeoN5lHxnEiq7jmCiN6ml3DytfUxWSiyPLMuba+QRuZuOxsrDDRgg/DGY575m2NNnG4bN'
    b'bns1/Eo2J1uJy+sjTDYm0A/VpfQHS/BzRcdoACfVmj2ML684TIsTv8kPFAwPploFgv0Uo9'
    b's1Bwu0rJ/v7lBbm6qlcrfh6H9cO2OyGXqSSS/lPqTa2B4Yi+74nFwWQZnJ1ht3sT9xDyuO'
    b'7UQiLbPpEAoJ8/PiAnuRJocpWdj9nbTNvZnJi50YF6RnSjQ2NpOXmNqnk8Dq/3w5n1fTa1'
    b'5GZ92m6GV9oeUI/xkC1NXmQhkCtRXm8i2OWFgAt5c79zgS+ngriwl7kgLujlRBAf8jITyA'
    b'S89AHbMGZ5IF0gs1mAfChUqD32uu2RGRDRuUNZb4i79ecioAzQoVlATZgOzgN8eXGYS+cW'
    b'Jf2t+xM1hPocES/fJJBIlUq2Q9x+TMYrWARHB3r0qeH6gsclNQ6TFGeKjgJdKQYE//r2Q1'
    b'bNWgUyKierT4zBJSqXmWfeCmSrxFQQqREuH02hzVJPbEyhFYG8PzHIeS0ISuJ+PQJ9zpUa'
    b'GB5dHVhIcJL4yiMis0OMTmAKBWGdHvrebm5wr7HVQLRf5jjeTLjStHZogzj2LzRg4+zQEv'
    b'5Yhmnx9gio0rxSh2mtYoxp1YLLJife8HZ65mgyF2q9456JjKRUDT3nBoY+B60yS0No0WAU'
    b'gnVjUcuFIAuh0zYKo5ivrkq2pdPb/uU8mCFAdWZoIWcesEAV9/nHPuUcGYaTKfGgjwo5Bs'
    b'5F6aFTkmrAI9vroeRptdPSQe0kvUNQ5y33B0OgnF5ervRRdPCXW9pihHttMQK1tgjGV2rk'
    b'Wz9Icdk4ugqH2frWH9wM8o0KD4sxqCMTg4oWBlf33KPFjxoNoYDcYyT2RvKFIqOaTNxJkv'
    b'FbyTq3tOSA4auKWk1In51aAb3gXivCS3KPbBz0doxaBRBVZhiD78N2ZprcRxeb5IaW8Qlu'
    b'O+pyp/7PcwcnWyoKGGXLEoF2D+sLO4ospzO9RYhQaRriNdGaZKxLohMGNtYhZ8ajSvOM9E'
    b'iXRM9qwG4/8r6YrYRzGnYY1DfCmhgZDsMQT2oWaJH3nc5HxqjtMljQ3dmur9xbU4LGQOuR'
    b'FRQTdLYzCc4h0kCGiYUBg0JvSGjZobahJt9vdb1akvY1xhC6yjgg1BkC9nh7gZLsdVaS1g'
    b'klvUMurHcPKDVzIh551B82eq4Ine6+V+YCTMEONdtXIJ6SNwBKCHVuQ6R0CAaHl6E/nKHv'
    b'QEF1SjBn+YbNEcSzzW93pOfpNVd5xqzfscF5uKAYY106/d/4WqtuvuPO69dp+r850CH55P'
    b'CWO8aipEU/G3jGo2ZmlnnsHs4em7vAjNvrzGnmN9g6a13Om57cFZm5u8Ch/Q7uH9kpZKXP'
    b'geDMZd3pjG4kK9nySZrb98bpmireVbqCRyehEUeLOR270EyTLYdn9E0Zs09fU1SBHlBTsw'
    b'JT4/toigdfwz1XNXrXP6ZI9aCrP7J20NUftMw70Gr+CLM8RIuy7oyWgnmrIey5yUnVBPL+'
    b'TH4egH2/IZIpRPfCyqsfajV2fqHnNAC6klUWtrUTYiwVbeVoFeIE0Y4iSTRDRFko0MqiES'
    b'1MnehGh8Gu0YAVZ6Ihq++tNBQNipF/E3fbJlGDRCTLCLGxNBFmC2weYVE8cRA2keju3frU'
    b'sk7CVRvW8iVrLeQMaUpLycKWcriKWc4OJ43RzXCBwm55JXn95imKbu6wGzHk5GECcbCj/B'
    b'yyiNlYjdzWuiCchiu5UEEvuh3A40W3A9KY/p251Jm5bxM/R3au9VtoQPCYtx+pss4Mdure'
    b'TJfcJg/Uh/LkQVsKloDVOIY58YPc01fh2yuNxLXSaOmgNJLehWPeNcjDhoP3YaP00jrVuM'
    b'v9icb8GkXkUC9TkPFysv0Lj0M+IMbh0a4lO0uwbFHZT11mCwu5KmIo9GZP3bGjEg3/Dfzr'
    b'pVskQe6kW+JbriLEFOlhfBXhDJDoapklwr2D5F6OO472iMRdQdiYr3AFIenQucGdRNjUnn'
    b'BpgQDGE5dV+dU/cXGHeZBb+vDoK9lyZRDdvtqJgYbd5nR+49JM5YLRdRNuotM/0PAetMIz'
    b'a0j72mEIXT0cEOoHAZ27U9C3b1NckvPwzLkHJtxpbsjAn1YE/vfLFVeRE82xnm+YCxdkaC'
    b'vpykR8+3LFBVnfv1yRWUUDa1bDbd9deEbKVA6/LpVVgWMGN2Gkwhj5KGeeEZbL5x6Kw2B1'
    b'2w4ImlM4M8hO5h7xQG2BPjhxnobOA0yku/EQrhnPVSpKh4/S4OBxClwoQX4HjKR36GUUKM'
    b'QRXbZx3/vL7ty/7N7Q2c0qh6FxgZo56mV34VrjrPD0AL1pZ+pWjs7dobxTnWMalw+MysMe'
    b'daKYsnQo3DTRTTxblMnofJBrqkuFu74HjW3XUXkzDZk6/Xr3tcM8iOPAIrPQhnfW7whMLM'
    b'Bp0tEiqUXkMBUx1Nbd5Z4TPvt1uvRnJ6yG3DIPbUoe9g/omUOXM0eTjHQ1+HJr6soRpNHH'
    b'JdgdD+ZoywQjn/nc88TX+vjGbfJUIAk2dc64AqCciH5TWNqqmlTome12xXCZjnkOp1Dmsj'
    b'buEdqTedxIceNLriBTkA4vEn2Ib1UuvEM/H574wNQS99JCqodtUwtFy0LOp78NT4szjVlu'
    b'ndyFK9ngkqS75MxCds1HhxgxXHgNsRd0XZxDUJrD0/HCdJp1c75NMFyOnLA8Hc36E1Qo82'
    b'DBAILG5o6YL3h5ETQqRzct78ChZuBoHsZmk7XkYs5rVNJA88Q7R09LLhcp2WmgM9JZoHPS'
    b'eaCnpKdCm9irldA/89JRKhCWbnnhDNQeT77nAf1JIfQHngadSHDtJ15VzKHJ0Z952XJaBZ'
    b'pnbUJmrHidoSlaSzLtqZA/GlLS+pOJS2T52fide/L9nPmaimgfjWcpg0+8b20i6fzEq1cm'
    b'gWvTIdn2ycop2frpi0mHRPbpN1MqUohfTGQS+j9MaMwF9/QGFYtZIE/rw4m6voZQKR+pXR'
    b'BDrRtN700ejeBoaTa75utdsTRmy2ba8gYehZvfcKADNvG+DEd7vsF3aqZCBdWL5Q9Pz08B'
    b'QtbJJBTFcLx863p7FyZChALQnalWcGkGnqHpvXELM6ONvqGMOk4F/HJEIA9vzGDUwrejuV'
    b'Ob+ZiSWrEvX9H0CMS9ZxmHj45VJNwaLafJJlLiSavFqBLkJtgIGNItTZnveImvaYmNl/ig'
    b'RAEd2wtMErdyZsxAomUzjzxxDWSSTdy32bmZZClJtSJWGjosiJFW05+S3tX0x0S8CyuVFG'
    b'5nl/ty+xlW9CIgrOk5eItA7f628XxnLGVGnLDyd8U/dU88Nek46Zgz8un5AXVAf+z/EFdT'
    b'BY4C8CxoB3sBZwocuXesOH2VAkfuHctu7Qtaa3Tkw/Mu9xflo9HoyIfjxTlXKnDk3rO2ps'
    b'o6cKLAkXvHYqfUCVgocOTesOImMJ8D00P/dGUBbQbisfP6MNpCmi4CJ8IOvApuZprn8SnI'
    b'Pa8sYPrFCMRM4+XQcZdFjvKYQX5aQ+r7nb8/lfWIy2/XRgrzWwy9KrQcO5DetbnJ0X5b4+'
    b'LIecP10or1rvZv0XN5RG1Sc1vb54tJ05NPUymUU5RXBLSOsiCAGLnayKNBlaLd8ovJGLMx'
    b'GzATzsux33ujBJNJPmFcf8k4OiqMnpWGNWHC1c4MWtl9GBzQImShAFGpy+vR/MOqQG6J0W'
    b'3kRP3l9XAedeOG9h23IXQP6oDQhRog9JGYtW3GFb2pIfpmIxP3Ajm6ifYxskSxM0vpWD0S'
    b'oiWid6YaQ8tiMOqbfQrm1L2szdJU2GVtrni06zFjmmOqvSrUpo6bOFwQQZPvtn1oOktDh9'
    b'EDFUPfQoJS0XtHC7LROYjZTeNosbspCdg9pKn9lCsDa8Z1GPbIVsiLn8sJXcHhsrfrbiEr'
    b'V8j/jvdkZxjr40yuEpXHhtBZ7ICQwwTcZhE+MR6/nblD5E/rFyPMnQacJrLXwxMFjogmgS'
    b'i6cOZvXifx1RNoklUS3TzhWvpUUNc8gk9pzAGK5NSFxNh1qZA+nwc3OYfaven5JhtEW1Xu'
    b'm3P5zDL4wpLdxs0y6NGb6D7EAmE9n7ZmUayYwUO0P4HqEJYqobFtwj30aEPRHBhJPchmBg'
    b'guomzWfokE3cKAmuW3MsjXCURb01sZC9I7M82fMA/Nt55I5g6LZpLeoVquE89iCuBD1tNF'
    b'Ojo8UUdF9R7U3iBrd1h4zJazQLryrBLfgl2J5wEYFKISt2IkGGxOvDgtzVNP/c4rUluh7G'
    b'KZq80mQ8/OwGJRkOCavCzzoHMyK/Fvw8YqNMYSO8ZEvzOc1wMS8qyP2LaCurUCRCOqPLzo'
    b'HEMSzuveLNMii8LSPOTQS/MctvTSPCU3r2kgT75ZzYCNnpQcTS5J2CXgOZ3ffmcjJUdXYz'
    b'qNVj+LVcIGARE6OWo+w/eReciTJJ1abIdbveS6SDq5ox7+7fq6X29fekCvtQt4ZchRXHG0'
    b'NYfhuhbV4Hv0uAeD1UutTM3D9i2+Z6GuAMrgObVEOM0914C8+LHSqIyxM43q2zErzZAXP1'
    b'KNRtde5pojb3tQelVCEFUfuwbX5zGk02eskTPuSY8q6aInPSwtR+Mhf6f3+hFOd2WHAz/6'
    b'3Q/0XJ1YuNf4VsUK/1H2w2u0No/y0YZX8B2dwYfckY07gnOrBnltP8MI74BQKdvWIlK0jD'
    b'0AbkeLSw52jSGrZql14HKxdAF0mEj7MKpUMN+2MdoIxAa+YXufWUzlhRdH5aSPYIs+4yoh'
    b'XFT/th0uyJfMQzS1sdY3HFMbi2KwGpD/L9verRzkWeZSKl1+NqldGNECqcNUh+/z1Seucp'
    b'FIyuqVAE59Wjkv/m6sykUu/V02qZwTbwBNcnwWgL5u3DqCzNVmeHUgI+N+1MHn4YBc1JcO'
    b'GNCf/AehX4nJkbBdt7frlFArOvNkTKgrc4dIRrQekDLOHCIJp59d/8JGl9Go3FMyscky1o'
    b'KgA+SekLdoKo/IWzTIAP0WTY6+db8xygiXK+23njmhgkZ6Bf2/cAA4je/gaMg5v506kwVw'
    b'F1myQzY9YmA21x18vLn71vFmxG5dNEfH5g2chh86CkY5ehSH0PhOeRTOwSbHPGHZhRdy0M'
    b'qGUMKIyN5OmzFp/HzYDSe7WDa3QHgzBoN+DInboo0ZXiFGBvjKMJ/g21+0hVl+F99qhUmC'
    b'NbZEP+U+o2bnMNGpSkerBrMg1H/FvP3AdGclivWo8w5+dC5PIZFOXB1I7Qox671IjuK3n/'
    b'xBBnLpLatzfjh9oi5JDEffQUIrtfTVoG0cegF2w/DCq9nmBKkbnpWk7D2vDHArh+mWP8ai'
    b'1VgGfTZG+xseX6BcSttCZtoZVsUPNRzVpKXU4Ms8VbRCXsqtL0v3LUM8cuaM2M/rxwH9jE'
    b'wMOXYoPFpvCbwb0LVLP/9bIu6LVG/WAHkVqbtlB1sp2BeExrTeBPzPB7PSxwVT+637hoXD'
    b'7JpqLiTNuyfcSgu03KnvwWhS4UE5P0MAUzXaDpgeEbMvO3dlf6reeFoZyla8mXGjH3yaEb'
    b'AqdNrMk0dqqmXyKKsNLb7VUGBoBHDYdj1XhyYz0OetWoVrLRCtwjksWmtrkke9PlMnj0F1'
    b'LJLH6MWpVfKobF7R2B4jbQjN6XFsBLvMiI1XyJc50dEKOTTVR730gNgxdlASHvt+fMRMZc'
    b'Lfnh8I4HHHD3gyAITpHyPVBtqIg0SzyQSRQQ8y0xq080MBnex2GMeHP63JoCVpw2jNF036'
    b'nteP9iCwp8Ia+hgLy+iBE5ZVAxYWkud2sThmKC8xWxZ753ZFN8JHvhx33+3tyWRPBWcOO1'
    b'wO9nSyp4ILh7109giyI4LxuIP4ikxvzyEHOrgiejydzRVMqB7diToTpvmPPeS2Vlck4kfL'
    b'GLRRy/PCfAUd09JKV24MEOrCVNE3NOW6NXyvKFvfVkeF7pMWSwNo7bdxSFB+LRLrvoXDgu'
    b'prkVs6rhVRq7jWbTTUWkgruBYRta62pKi3C0977da6Fx3PxqqHauvAq7agTDtDu+DBMvMm'
    b'Eb4jlQxtKBwhxFThcXgUexl2GsOjX/eBqvAIXXAv7CnZR3alvM474XPYLN+p+Qr5aGlVvn'
    b'MDhPLNFX2rfJeG78vX+tbF6ZFQnBaJi3PqsFCcFrlVnFYiXZzWbVScFrq1BFoZji5o61YK'
    b'2joIBd142he0dS8FbeXRBW0dxH3mUjDpNNMASa9ZWMzVERfQdtSaIZEomAjkuH7g3jFP9k'
    b'xJHR449ucJTxFiKvukTeRI+gOFBb69tRzxcLZ5viIZL9NjaH3iod5owGlmU6LxgNPMGLI2'
    b'vasMHSzvSGs1bgFaq3Ck7UuHTW4/dwjJKRCYMDlQ3cHfTgDF7x82iZ5DTJYg/VITkifqA2'
    b'RRzyEi5DBMl5YIzyEijNFziHDvnkNMzVfggI72CuBSL2EUGWiV5ob0sOcOV3QIq2A4x45v'
    b'ZjDkoAAuHC7IKnfI/vLHRu3CzpbEUVl5kpCXpq5II8A33nkeB9oGVggXRQzt162BY0r3FB'
    b'ld1qT1M49VZhBXsQxb1wUHhMpgAH1/wNwCoxsEWote3SGwsvhY50F9+N5bkwVZ10+KMWE3'
    b'3ppE/m/D5tTcUFphJGInfiXjVE8UIkC9uQAt8UlvLsxJa12a1brfdzt7A4v5DNpPBATVx8'
    b'FBiwAQbzsg0N1wxvRBXq6QK0NbzzqdOfHK2JgDoF6/gDKnGO6s7ERjaqLG/L1mOE/pLZ5u'
    b'x5EIXtRsnl7DKso5Uh3e+ITbaBRFC9d7IOhVn/QeSANautOM38G0EI3syOsl7eJPlfjlSx'
    b'Y1P/WyfpnojWLnwN+c6UhfjXJLhpszWwtEcjs/6jZNIh2NLjmUt57wXQWUIo0MR25vAF82'
    b'Ho+GSPE/HGUJgcms8sBwIVSVQF9VfILKAgUkkEO0mIc+hUdSwdEbFgWScuEEYD/4syDzJk'
    b'De5qux2Kk/PLlz5pN8FiC3OUo7zye9/dEw9ON6HzaY2Mu8hf3xWcL5O6b129uPrs7IiA0q'
    b'UHV1v9fQyU177jwJJ0bpSN91a+lwoy5pddhxSXJkBpIRG/d689ygYf9nRXrUB86nAPuz2m'
    b'WbJ9vIgmmlaL1MUtPhDrqkXs2ncLymRKRNLRBbqWTpnTFLCSw9K7bcheXGE2vLahXr2mNj'
    b'udFFKKlgz+vTcRQeqlnEvQ7Spep0eb6MWAVznja9ZqJ65MoKM/Tqyd0pM+v4MgzmEoP79f'
    b'HenJtvFh62p448vqBIoSbSs7L+ajJFm5udIiTLr5DHMRJs3zR6cJcd3OJRGLTi20zUie6K'
    b'I3NqU9sFSO+voKy+gvLpFRQiiOCx0BHzSuqIG4vtWN7eq0kVbS7MipBsOkbyyRgJYWt0LL'
    b'DmXcmrmbG44LhHnKtEb4NN0K7iN53RItSbzuhOgvZaWSK86VwkW/2mM/jRm865oSVkuO7s'
    b'bW+8UOXMfaTCfkZ2/AoTGw6I3wXNZSpUUFuIbW90sHoVrCIpeo3xYbtG7W3VzCvNOb8O0v'
    b'9h7rkdL5tZ7Dv3LTXzIuaOj4I3cyOG741HgtSaJxE2Bg2H6Iwr11OPApgplvhHNwI5OhRc'
    b'6DUqBqpP4tWKjjryJRmXc3Rve14CPIjWyvw7XtQwwVHJ2rGSpSxFQXpPpf3Ur6Ch+Prucn'
    b'2uqHH46PCMg8cncpYWDidyWguMTuTQmc5V9EvRCXVNRxnCaK2hK/Q+85lOFZGlmtgoIrRO'
    b'B4zbuoOvmrnD4xYOMLrmH/kZ6X4oUH2mpcKgAR32xS0MsNlHJ5RJ6+RrOko+ctPZ7VIX4W'
    b'c6U0RWKiLPFBFEd8A4+Q6+Sr7D4+QTPAzP24s3VMoomNvQ9zrzzEAPmnjhQgAUsG+xnWdq'
    b'mHL4SLMysoJd/ZS0fop+ZuhvA482ObPLgpA7lclqOpxPL7x5ydxdwYIxN1fw0NRW5g3oPH'
    b'VbQHHJPSjsIqNjtKT7Xl1klcN3dLC2UHRUfOgMoseFsuUyQlxmQeivXE9EOG8vW+508mpC'
    b'+62tuzw/2ojxDkWpzz2gdspKh/EdrYzHXXrq07OkFxOgJb+VlrRK1KWEdZVoe42MpFucga'
    b'C9vB+FcMOAVid9bHDTJvpdlKJMem3lAmH86qExRnIB5Vm9CpzH/tgFRpOoBUea3GJW0PmF'
    b'x3yluWQLZx5xkCsqUIwpmsnNY5oSlhFqjorlPC8zRs2sZ7WC6hlxuO1/vuzMoRERo4rdHL'
    b'm3EuTINdfkiCypRikzzxmjwp9CypcR/8+Hbse5ogQ9i/iP3GHFbNL7xqxVczHgHh54c4j4'
    b'Lm/yJfIR+yhiZVFxbddfg8BZxIH+HbIhysieBxj9syMsgKiwduiOjkHO+oon8cUsFFmILy'
    b'oU9kvCiRLGYf+B9uHCnsXsc8gSdJaaNYQqkEU18bDehyyJ0u0WnHOaSWiYx+9CgqNoMPI+'
    b'SI2Z5jHrBVolaoRENovZJ24hBFHicJXpFVId5eSpe+A5JhFoFjN3jyJPlIzT8NB35zeJLx'
    b'LW9nN8kjNGu6jSRfXgdB4enoWVxqzLJkQUVcjTJbTMOC72o191+1po9itXVKRAY9YwbIQT'
    b'Nbpv3XFgolRtM1Um9G0q01ljAkNVGVaYkNuqxiAtAVeJMbKGoJSwFDUwjKzWFIQSKovDVS'
    b'C9bVOmMG2KyjJRlpLI7KsnmKCiRvfZshw7jo9jpdTjI6XUwWOltLJwUEodMFJKgYp9I7JC'
    b'2zeSpcwlQeqVYeR0ZNSJeq4HS7QJPdCxt5Hs5LeOyNIhJtJXhpkowSuzOmRnP35Wj+345r'
    b'27E417E5II1DYkYPxOC2y0Q73+PU1uqujQ5ftgzAI/5ua5bIkc3V3ewgEL0GIgx6Hg+l3E'
    b'PDH3dQ7Hm3d1FoY9euIKVS/Sw5EBB/RB3vwPXfbB7IHxfH+KJnXQL7WVkEIdDQrU/cBDBD'
    b'zFkQbsHNP2CppCaC7Jw8EkAIo+ome0e35ZRhHPfbgVlUF89Rez8BYWkGLAvqTrr7zPqQu3'
    b'OfX6ofgCIonhHJviYE2iZuZLve+4mEeIt45i9wDYbNhR+7X+xHYKAYrSjApw1JWVJX9l4p'
    b'U7TNecMRaZeCHBp9N2rfd8IalsJRi+0mTRNXklQEU7U7A+UkDYvRPJjI8svtgjRzccwsFF'
    b'q8CoL7eeS1slV20p15heQAb+bdufT5H5RuFBOaymmFXyO1XzefJ7dHdKClrt4i1A+i07fu'
    b'sdO0uHDTvQ2tZ6kvzu9fUVv0Vfn1lCFqDQGf+OJno6df5MA3L5d3cMQ8qnWCXxBlYNutuH'
    b'tdmFoUdXArYGvLoTcGXg8bo4pFQLTTNGsB2dSWuS36NdziVpn0GG0DnkgJBFBOKrWxAgWk'
    b'3Oo/6/Rz0MCkYaBDJIzyKzhNeEolfByLA+bZ/7yPIyJRwkLEC6ATQnS3fjc9A3nyFsDMOm'
    b'igE82mcXnpUtABpgZIbVJDcssAw4MlBjpMogyzi5slcz6HjvdkEwvttwCUjneGHokOGkda'
    b'/BcMfmwVNguhdpFB0NQCUYLy+m15vbz/i+RlRzoG/dcDnsoQfsZbSqUmG8cNXqJaxj1dPA'
    b'Iif4qYVxOq2hU8TcGbjH4dirDp55cdr2mzUm/EMop4mGUcF69kz2CunYzag3XTHvwjVZlF'
    b'PvoxST5GrrxBTH9Q76KmGwLAYMtztjjnR8jnKWYX33kiI0o2e92N0mz9EFXjPSzmqD32K1'
    b'gYnvc+h2UGSxkQbZSnGEGvIcm1dOCai9SZRiZJqh6Sg5kCK+8BM5cGWQvEJ1Ys057NaHDR'
    b'OaQoF7jnqXkrQeKQoCvmEarq78Dgi13wBqH7E19Ggj0Tq62kmsDDzuIimhthmlq2AFMTOU'
    b'toIggor7fL38WwtnpGsLY6xtzz0j6NuNh0YaN50Oz1u5uhHTWQMMcqtUYYHL2p8pmeQWeQ'
    b'2epkT2Fzl1wtjsNVMzpgv647O+uYoZqcw8UDsiZR61OFJzNR3VHuRpfxzGG9WFQfddd9YH'
    b'JFnEgAMNmXt0Gs/j/C5bzxhllcfH7icOl8zm6GGQUQDe4akfTsExcjMertF565VtDPrP6m'
    b'QrCn18xxNSFg2IyP3rO55QrpENR05aPa8A4ZBkKdHUkKEF54qOygAVaECXE/IV2TSgw1cp'
    b'qhkYk3s685KA48Y9U466vSJnOPhDxxwqZSwv+R0SgIhOehLHruIc5CflF4yhzDzrBeMpmH'
    b'p5eK7pKDXI3a8SZgPqNVBtwmMm5SLZaSuGDKSzB4SWsBPDBeJa77R0mCeRfjat4m09eJPT'
    b'IuHhgKvnT1YLj3/vnZNVfe1ivPfWrqrI0Y1XT1bzaxfXwcy8o2tW41nfe/kEffmVi+tgbD'
    b'7IYDkleb8x+kTjvsUwZmYQljsfuDKfQdeKgKBtOTjoVh7wV7Is7L0rAZQbchzrztyMM+ar'
    b'AG+6GvPJGil9LbHrYWaxMEVzpf6tiN7Q3BcLE/jzrZBMhhlptuOsX65YL8f6fjuxYHdDsG'
    b'Vde+ZVRAvPuTW1WK7uEPL0zkwnnLtb46tyx5iOT2I7X7RIvd3mnyF3UFuN1RRi1UoQSK/0'
    b'5MhcpfSQI0pPY4n4lHG+BBqrQvBk7VWhCu60vaqjxWsVSLGsy1Eo3aO9clpf9jY38PiYO5'
    b'JL67EJDwXxS8zGpoEcjt6gLcuWc4NHNmrW59hALXNo8AuV3UDaOs1CsovFWM3xIYyQvDTR'
    b'XaCAGKK9QzpAtqH3tS877+Ij4CwermWxfsbjHgC+Xo+RaBe60ZyE7kcJ6NER5aacI7rd1w'
    b'FKb/+gTPLTgHo7ewXdWFFo8xts7xU8axbr1jEyzC+jU4dTJDGMrEukZ3jYcqvJ7dSCPTxR'
    b'gbcXimWVpw+DMeNbKFpsNDPeqetwc/VYhuox7MJlnxk6zYF7rJMUw6q/QMfsRZmrdVbttE'
    b'3ie3UyT/OIEeKAE5Tc8A35YM65oD7JaAwh3QML6RT+/NXlPFm706tBiOMsl3Qgl/1TTBlq'
    b'01XJsPLEBTMJyK1yyZLvFgtYf4ZMzxMeuENF3Os7WtrEL3hSB7Df+p7n1GFuF3jqyGBlun'
    b'RIdPVuTtAtHDBUfwkMY9N3wFg6XAFDmkq9Ots4nwoW3yNlcLUFTr/cskOn8UrjPNN/MKdX'
    b'Nab2Me8oB8LBnGqm1zsaDYZb550Xpq/vnuNYUHQe1eHXjYV9yLUlx2HWc+LQfrh+oPGpwv'
    b'1rGyyV/rzuMQnRTmcB9rFVBsJQG4u6CnAka+tw733m6Ctpl4aBrirO6CzAUR6nDvfhzh19'
    b'lbMTMt7W+0HyqwSiDRlaRUeGDEyTPYFIKQ6nN22jwXz4Q60dNQzmePKu0fO7WU+oYAwvrB'
    b'SgyPUYivDC3VhLlFEYN1ENRtMRVD9tFjdNDe07bKj4e70aCZ13f7UaiXZ+Q6FoW+t3rJ1M'
    b'HXqtgSzTwBo/SsKqOZojovfb63WMmt77b7HlGLJSr220qaJ1CbF22NOM9LEPOqkig0ZqwK'
    b'AektSjZsU0cikoFFjhkOfuEWNLwMsIj3sRz4tRhOSs0iokRs/MkQQz0qlrgaKdgsLwzajV'
    b'oI5wKe9q+SJz+GjxwsHjyfQ0iRcEWXsIvKCK62lzNfF4NMV23uMlQOgrBo0CwPRxHxnAkd'
    b'YtT9NRuTLmg7mB2iQCn9pcynF9A6FxhgHcTUWVpdwV1hg8SdLoE17xfezvI0tDdh0AA40u'
    b'iqP8rnuS2S6zQi0QIL5xi0QskX6Can61QDBDevUCQZ2RVgsEKAi9IsAmenNFgMPFEORZQp'
    b'5hL7oPQ6FGE4SrIkRJjfYp2of5DiwMMiEEqIR7rYEgIcF0DMSFtRM19ZL6D9XRIRWXh23Q'
    b'g6HLEXDHNkpk/+UxuEZnd/Fr2I0hAg+ZqtccapSKXnNoNR3lF7LkosqPArob0CcT1peLOs'
    b'FK6Q7KQp1FSyBu0ARPToE09sRzDZiLBkqTUGCP6BXttd18IM1A3Pt78RgzUOU180utkKBw'
    b'L2qJBFnydd89hfzFFHevnCM1rzEfwSv/y4SqGdrrQWttNUlM2cwBooNfbZlO8e1VLTrRqp'
    b'alg6pFWp/2mCeH6ByHpqNhtgBDnr9krDMAodDTRN/kMmlA2lYGBXOSHPzEE2PNIUw8MciH'
    b'c63LpSXiiSc0skM88aSnaFgtDC0ekDPRbYkINroeUdNRCiFa9wr1/w+rTtuH0A+q0kOU6A'
    b'TsjLRfWjeEXlp3QFhaJ4Aey+toLEK9TZwn5hYae4SJo8VhPJus4ITGIlcLtSuHj8YAB8fv'
    b'EuSFR+MwUgvHJtN5adEATC0wHoXK2uORBC7Q2GllwXP/3F3OAWZUutyQ29EFipqOyo0ezX'
    b'qJ1p+Z/Q71GiUKntO/Cc998SucGbe0ml2tDBCOXNeKvnWJV2b4fgJmfeuj6x4JR9ctEh9d'
    b'nzksHF23yK2j61YifXTduo3WPCykD6hbRA6oLywpZ8YnnvYH1K17OaBuY9UH1K2D+L6yTD'
    b'A5oF4GSCKbW8ztlCAgsxoCkeLVEDjTW2B5IKPBA6ULXcDMPqgXcCkMvadeIWGPFY3+4KsR'
    b'BfFEnW1O2nerhtD9qgNCx0oguEdU0WWZiCq6LFPTUWWmxwOGr/UzzcRVD8prWP0NDTlJ34'
    b'+wlIdB7aiWydUDg21rwaftBUKK02au0NEZ/ZVh3TqGUt2ZsyRkX/MMfGsZdpkF1tUMpDG8'
    b'8XSmduiNwIrAugqsNbzrRxahmGDU57MA6/5ApWbCRJzVlWwzRfPVJY/4dUAWw1mpSCtFHw'
    b'ZZL8TkIcL90VcTWL8xj/nZAJknZ69itZ7QQZkoeX3wbtcZU7DSAEdeO2kujK2Ni9Pl3t6p'
    b'Vk8tidERKiSB1AJs1NYF8+5VT6kQpOiXkFEpOfCrGzvS619vXYF1ofKHTI2uD0WeRteHaj'
    b'qq6RUZZ72DtLCIX8J0pF7zFChsHxHa37PHejKHE3JFR4cRNEMeIlkl9mIPax3lFFrMMRVq'
    b'3k0UVmFZAxf8kG/mDh5otPiQee1UkcHsxIDhch2QSh1EqEr5Q2t403pGS9rrGYbQeoYDgp'
    b'7RJgN1x1Uy+BMU6DSHsOucLZPhfn082jlT4Qlt7jjz4C3j2QbMIByC1iZcZLrjF1NIEF3D'
    b'mqYe0PILeGUFOrviaFNQw3WHOzJ8ix7ZWkIOd6ymGvALlMtUo0qBXM40w9+JuMw1qk1s0R'
    b'cN1/emYr6iTSFzCMXr4p3KXqSGlAMmKBGfR4hHGTWvykDqMkDo2oAZ/k2w8Kyun5wn3vqS'
    b'B/ftt5uc18ng7YtXyDxdHggjMmlB8vQOMgKNDIxXpI8shXlqPyWHG0srQdvcQpKrS0tH+e'
    b'lC9DnZMtjoqJLJPl7EjFF4uLI+hne9wz1Pbm/XI1khp5CdegkQgos9MNTGIb4wk7kcX5hJ'
    b'efbeomWCb8zsaNY6s58pH+Yt7bfet08tZOxb5SrIqrLocUAfoq0vG4ufoebqmlUtHe7MYq'
    b'FaDHtVnkvK09vEcJbpCHG+AKKVIriwSnKaRO+IG1KpyBXpoCFPAnnrbqc52V4/Nl5RKzpo'
    b'bOgbzIMqU2L2Ni9e5tWQfOx5YzbvW1+Q1Ap1ZYGgTxsgVqdTC+14UR+GqSFWrQ33lmZtUq'
    b'IVa+My0qsNcutGKJMKrW8bl6JuG3a4Dqp2pFe2jWN36pEym1SL7m3kCjadk2ZGwKvPqSX6'
    b'Iy+jZA0Vw2v215aQOt0uCakhg+6vTPvpz91tCsFFQ0BRAhWrcGiWNO2iAXmeoVEdN49GXz'
    b'OViI6Pm/369HDZWaQhct5SIKPgpKhv+n7PNHP01WgAj/5h81XtvuUCKoYyNveeOUz3BmMs'
    b'WsRFgq0xRRRsWFBboQj0mQboQ4PoQ4X79r0E+w0DqIPybFyRWTdKzT3mwXXPVqh4t3KexE'
    b'9+TAoBwn7lLGD3u9f11zeCCwE90hjk9DAcO7v3N9w6lNEo2Oe/xvQ43CQvfLZskrys1/uX'
    b'oDzWBuFZrmATlcGxnmPNQfpetcC3nz4Rf+rMzZ9ZigGBlLnyAoP7SzQPMy7VNIy0XsxOQf'
    b'dva0wH/CZUxuD0+jaduLPAxkh/9DTNlOzhYRvZQS+YuNFCPMNFxOxOWNHLRKvtTN2xO7gL'
    b'ajD+Chkf3V/mbWCZ94XRWAWwbxgvAqD7KeUuUnxVXKL3zhSmFHwVhH0BuQmAvnjZpcbfrZ'
    b'PNFD1Oz0rx7IPJtULsWZVKITpJrcKjNOkIJVFzDapU6VDse8ulQnS6DM6Z5qZ/NPO/DMCp'
    b'Cyf2Tbmfolt1KUpYkCfl7l+p7GeaamKjiGytiLBF6YDxqXgHX52Kd3h8Kp7gN+UKutmLXp'
    b'9FQoPCjBLSC6rQhuzNoaj50Qk4uAuXcUynQoVJDrHuW9ilyVF/rN3b2GUORjAzZhHFhxzm'
    b'ib6wlOGOzlUYKceLE01RGzS0fxPO6FJB1v7ozgs6unnB25yRxMcHKOnRPVDMVm2JoHXMPR'
    b'TVV3EoRkTGHRUBBNO6b612zxxmhwKqhtxZtFg0aqUO1KfxvcNIBh+LtJfMA2rPqDbYCTUF'
    b'kphZrzNINY4x8G/6B75NisYxN4milcDJ2O9gYAJw4r3XGe/OflFL50ht9EZQQ9r39obQnb'
    b'oDQq9OwLw5XPLD6NNF4s5FXO2zzoUz2mkVxnjte5GMz1hg9HbQaEXbOPUn0qqa1OEsdhe5'
    b'iSI+4mEktTbgc/P5El4qxlzdABeZnKeMYDiteX++N8eASvpiUs9fyHSV4tzho/Q6OF7/r0'
    b'qPxnlQWHhkwV1lSbyFPHXAKFucbzMgjkKYKpaEosDRPkDlgjoz+8+hRDAvsvjIOROpGzxD'
    b'1m2b9KhAmAOvR93YEAj3odEUG/OljQ9XBgnb2IWh7c73hCc6DGk3tUtHqFZnA5Rmn1lSjU'
    b'6oMtoD5o8vymYONSy6ngX1cuAhzcNTD83sT6pI/rIkSqp5HLSFt4h5ZuQTZhszLy/CYXQ6'
    b'N0m/iAFfisTpJ6ehvAf60R6OZ+WVuQPch5VLphyasbnkz8wfUgqiHrKbWSpY/vFS6ZfjsL'
    b'k8mOXaFYnfeXz1q7lFxTC5+N9t/G7BgtBLtzOWgjQkNeQxLJdmgoQF0txgmIPYY7F5pWg7'
    b'aUE2nEyLrPmhpwQpgV3/nWcOUT/U6ipyJrrNBfFEd7eAVmuEqMhqjXCe/EGtO03+kKM0Nb'
    b'/3ygCGgDp9l5EcGVmXxK4MjSui46N0DM1f1ea/00lErSPqQVNZFVEzTeW5pjidClRQaTwy'
    b'1os8/gfPlX0H/l/9XGlUETfWq4T1PT/Xzo+Hjtc6KI1xlfyhl0xRhqKLtZPkD2eCNMdn1D'
    b'HA3cBTlRjd8REUMUUGNcWA0X2AbWVfe43woGKNuP5+O4unMT7yZbkBM6S7Gsu6mAo08moZ'
    b'7rCBhWYCjdwaRpyaSqCRW8OQ+mqxOmAj15bj33y1WBOwkWvDifOnFGjk1jLc9f8Wmgg0cm'
    b'sY/p1XCxUCjdyCIZ3qInG10Ru5IKN8Wiis+U5rTWWFpvJUU6H2emTcejx+1Qg8I24ERHmR'
    b'j7E2xiTCU9IzpRoL74G0gronQJpVhPjnPRQs2zTBb7RwF1x6z0YeZwuE4T8T6n59Mq+wto'
    b'K4W2PThSDRQB+8mlGLw2EbQzKQ5XxJ3bP8zbMe8tHUgVQjYNpY+BbkA5op+mBNdQxgLrr1'
    b'6ZorjEtBWaWBKGVVwvVGqILH6Nz/ArTavZuA9NsbRSKbPjnxjdvwRKyOsCsZxt3IDK4dYc'
    b'oQbkVWIJcJp2asYqtETdIcrfcNJ0l8NwdpbaI2A61N1DQdWRkgK9ZmQxBjo1nCVIu/KXjO'
    b'SvSayRj3J7tTQuNOcx8ElYsy0W8spSD9rhamqcdgK4X5bnhLoUVcsVUU2WpHCYPKMZrTzw'
    b'zt92GKJpByJqdAfnaYQ/L5J6PQQd9qCKGwgsJUChIUJsTdPfGBHTtPZRE6mpsALOg6IGZL'
    b'YFVi0n1UKwB5asmgk08IjA4eM2BdbgvSb52x49UH5fL0btWucvxTt3fm3NwxMlVeKDoqXw'
    b'plTrcZiU/b8bBq0Xhcre3IGTNCfz1my8hR27EzZoz8OXYALe0H19qOoYKNfDuOH15rO4oK'
    b'NnJtOXGyqoCNXFtOGGJrO5AGcOTesWSQre1QGsCRe8uKM6sM2Mi14/iBtrbjqWAj15YjQ2'
    b'1tR1TBRq7JsZ2tXezPeIsdoF6pdJUFaBS7VuVlcXWoyRxeOvIFHW9o3gZSXUNfoQfTCyaY'
    b'eB3DoXkSA6cfKT9sOEv7GYyhGw3ou0AKMkbXUJiAzv0Dfbi5LATDfHt3tdiQOny02ODg8b'
    b'JCbuHRTawTi46Pi881HBsNzhxL3DogNpJnf0X0yjxx4fFo1cIJN178gU5g8WjlI18oNA7d'
    b'xRofZ19acLyOkbt8HZs/urQj5cd+ZIVZMiiurJuh2uyZ2bXs0THJmYOPvXfJgVCvjtSMRX'
    b'eEmo46QjTXnlZ0PEvJL23ZXxjE7UVZNv06y1UTZ0C0RjeLOFr0RcQJa57ZMheO223ImjaG'
    b'9Lm1WczSAWVkxbYCKQM/RydfMMs6aqPBAqlx5wzYqBZChYaGHIjmaYgoOj+A0ovOC2g6yn'
    b'NUI4giJwQgnOj48KOVreWCtNewUhL6Cg1y9bVEqaFH9xIxyOsTopOA+u16BekteAXf2kKc'
    b'3mD7rcRbPL2lCL7edoX4Z3/KdoZoQ9bPPKH7N/iOzh8gW6PzB5qO8h+hIRij+yjNLbNonL'
    b'xVTrTnq90l+2Y53InIrw93NskoTycB0TfuBfRWjubJdzP0BkvnZ55wqbLCj1bY6+QkCnvj'
    b'vrXOWBYAN0GnMqSrcvS7iZWzZk5svJbUMOTNaC2pWQDU+nlt6KCfk9Z3dDBqfQmHpiOrHs'
    b'YGfRn/b4cLYnzbdq9rA+3DyX4Kuu+ejZaTuu+wnBIjQfXzeNAOiGBK5Btsnlna22RMHb/f'
    b'8/+dXCmC6h/wS3hmLbfw3gfnaE9ODCmBW7Lv9enM0mHeS2Fp7cRB3oUVRc592hRcuk57qT'
    b'3oPVUO0I485t1YUWRfxIUh9Cw56VkPSD/rKVP3HVVFBK+mQitQ29c1LVNm9lNf3OmgG2Zz'
    b'y8ay/PO6qAhhSpVZQu6Yg5Z1iuZYGcWMpEoN7YcK6DpCRs7grUP13u30SIUm0D0Mdt8sd9'
    b'+jx9nmib+bccL9tFPXqaetckOPmmBmwKs2aN2OGyHK3j9iUdrPNNfEoyKyB0WEebYDxgtE'
    b'Dr5aH3K43j3PkhuPVtBdtBu8JKD6A5RjdK2WpqP+oAVj3z8MO7v41AQyrD4pMFosUrhsmU'
    b'4N9nXoURs5TjgBZosbeDS2oMp2+m7NLEtGpjEspK/mgnU2MH6GTWUHqHF6aZFggFdq4NYZ'
    b'lYl14Ed1F4B6QLO1iB7jlx4KhnYOik3tKg8G+zoH3bKwc6JqQw/nOsp/h2lzOgeJQd3c0W'
    b'JS1wrgjeqcFzGjc5HrHTjnJD7EMgmgnGKZKkyOsdQOdIZ4COzxLHflQ3E7baNVs4qAGoVL'
    b'0vrCtpoAbwSSa/NSh+jnkVaLMoLDnXqrBUvScPSzSPAw0bC+hK9wTyJZtr60D74yDUfRrB'
    b'K538I64ikMo6TlltzZFUlef2Fo9kCXvXJvlQmTBVodcEDQBwyww1R+px4RMbHoUQRj2/Yh'
    b'zkx0vduo25xaYNRvlha96jgri497ThaRvtKOgvDYoD0yaL+dmB4x6xLNxH5CVE1pIss00S'
    b'kidI8OGPe6Dr7qdR0ed7EEo6xiH7rlzceSKlbd3pxvmJmvoCJpOihIGjVfwxlwtriGxU/M'
    b'FC/LKzT4cLwh1INFaqCgl1lBlAhzDYSgHCzOGkUHV0StvlCj1vZP5jFRqtT8pCnKwsGmTi'
    b'l6dzmsz91ooYU8PZKhhukJeaPpaCRDTvW7i3o7ZmmB6MCzAfe9tc+hijHKKcY+nK6WdKYW'
    b'Hq3oWHRkPdI6MF7lKZNblh/zJDb6KAwdHyilxt6zz48WZmx4o/tLl8ktcxEmkqc82Ef0f4'
    b'YhyZBqwDTuwnBZBPKWvfqKbD9UGq96WHRAGBQNEA+JpYXCgGiAW8OhEUUPhsZlNBQaRA+E'
    b'BpBhcGYoGQSXjvRDoHEsA6CJTg9/hh0/MbwS6HLkfsDbBuPwHvU7NnefeWcyQuaCyPhYGc'
    b'iNjojL2XBnK/sZ7TQRs4c3K/epFekZ6oq+bhz1K1p4QeTcDT6pVrIwWDwec0d19O4eyi+6'
    b'E5KudKvUdNQqIeWw6zcXI6uxtV6/OQW/9ixjzh7zkCdcdBKTZGQk2l+4GIt+T35WNmlIhX'
    b'UhJNudC80m9lPXPAduzE6w+4yeWVOYPLM2TU6y1IQWbnRSPVlpHPbwwAswpp7a89zs0lF+'
    b'08vcyw394mHL1w4x2M9nzkV4HslzfEjPTzQSXHnKhNsK9bB+6eGJUXtwd6BxVOqpgf6XmS'
    b'P3JjTvFDWGzMKTJvCFp5zs3E70oYXzCddJKZ2bcIHRYLYDzWqjd1RpR3ZJ1rqiB++odo68'
    b'+bHHvZymbF5RQ8zcw5Ueb7Q4HYN1GMolWtKpSHu1yhBarTIAn6TQPTqHbaLxkjPXCYjGj1'
    b'XUE4uO1+0zC8c9e+mCGNkP5haNR4bSgqO+nU1IrwMiGnsqgs+RMyccFd1BhlI0ZziuG2Tp'
    b'ODfaI0RVFmH2Wx38recOCwdz2UmHQ7YcxS4PW6rVNEwjpbsTZHH0pqymo+5kmcSvhxYUht'
    b'q9tURLkbgLLyPh0B4ZrHlKC90IqsRGHQg2ZUsE8zZcXtfRvU6LhLbNUAr04dw5yYdneyQj'
    b'c5Q1VeB7UHJqNyNH2/JaOpjyklbbvhXJ0fvcGbGr17nz5BytCa5IjzTzBUPvmaYoRcvkHC'
    b'0frhQdnUmegHF+7bqdvuf8vOZBZxP0V6qXc34Y5ZRab6C2IzJoxgYM+ilIe1kn5s1nbZUP'
    b'hiyDFfjG6Mu3DdBXnMPqV4mMeNDPW6IqGiBe30eVNOjYQp7F+3D1OGTDPLLw1Wl7eDEXjy'
    b'bnsFiWWyK+q6VKgUZWCZRVnX+CLnCOVsYaQ8sCGmTQBw6mqAjdrccG5nSoLimfkxw941AS'
    b'u3Hp6zzzjPHFAZMFOVcPP1QGDQfcTcC3bjjAAOI5V0E3ZO35cO9ZvSs8U+hI/KlhxbV7Vl'
    b'vwRtRT4VxF3ZJ1fRtChaKJ7sUpFR01CjrcdS9bngvNeGZNSK9TmDh2PSft3WbQd7BNPOOP'
    b'jksHgcGkK4XTkLeUY8MQRXdpKFEtKUpY2aFTqpZ8KO1sXx1lhp3DhXOKDBfOGTBcOGfIk6'
    b'6GDZpi97UPM+pZY4Fo6kUwOuJQkPa9oiF0t+iA0C8aIPQ7+cTQI/uXBUEuNT1jpBndwViP'
    b'eNFFjJVm+tX+KLSrKxlRH3QvkzWGHlXTuQGv2ox1O66+jA99Qfdnfzqb+zdyCzzyMGLGd+'
    b'VA2ieCavtpTnqk9ntkxE/U7KxfzWZnwhlNaIUxnr42yXiX3uSNgUYzU+P0GM+WFoLJPGgS'
    b'IKmtTB60SqOvhLs2UybEHQ9Z8vPFnCYRdkaMVmOTVZtYb+r8SOUgASYWGMKBktoi6ogJS9'
    b'Ye2tF302eCnsx7cpzrhens4gY3TDENGyXDeXhuP4NXB6i5+MwiIQczDdyaj7vw/YzcBaAW'
    b'r50DPUufeSjM0x0Uz9RzD4a5uoNudUhOVD1fd66jGbvDbh0SLy1LT+eda+nnnJMwpZ8L4C'
    b'f1zotb7TNHUdoY4t2aJ7NB7RjSU7o06MPkLjg/Tyeprr9E1Y3u5kKdje7m0nQ0dhgGmtFV'
    b'I514xqiNenzcRLNkPDmoHDJqoHQoz7yFR7Wcoj+xkLNdyR01RORmuNzvnJPSeeARERajXV'
    b'azUDSDmFrQz+Yciozv9506PEShedIxDBulQ+LBxKAv0YtmlERd/eBOlFDm6FrxCsqtNmAp'
    b'QUerJJBUvwfNNhFdVYX+IrqqStNR2TIgxIPs//NMc9qnrbUca4uIIXdGs0FaXLktPRac1R'
    b'7a9xsHVQZ67M29Ms3SUGbZjxNVEnw8GB2o8WrutbDShd01hkAzRn+/8ATZwmlgj45m22GC'
    b'fUSf0Jkb5GiePf0uV7YCl991ok8Uz266sqZMOR+I/i5bImq/70bHhC4CqrWMGwjZHWv3o0'
    b'uTnGWRB6mn/ZA1803ZqXnSW+zOFeRNdhGC3Efo18SR5cd+/bRBsHziwRC7R16aPrXEkTtA'
    b'zdwSPMRPa1jagPLZWr4013NO5D7DRCoCwlTKwWEyRSCaNBjAGHZSceNnmmlCc7J7RYRVdA'
    b'eMN1gcfLXB4vB4g4XgNrrIDrmnVzPQcvUEe7Yi7W/BMIS+lccB4coOAvoE9czQ8RyQ88vr'
    b'KU3DJn41u2jYEcQa7MQAXoW1lNZhPRKUWCLeOKtG5NHNYKgP0c1gmo46FlSPy/g2D47Sl/'
    b'F1HosrMDoZjSx67XZflZ7ROEQGWu8kaGm5Q2SwNH4O57ewNZw7RDSGIp9OHSYaYOUBCZkB'
    b'8WauPONH0D8MqbSjmnSQOQ3kLc3IhOr1IuN1dLNO4bDvIboPmZCjdajaAkGDMkCsP2UWCt'
    b'qTAW7pTiYpWnMyLiO9ySC3tCYjtNaZjEspSMMO+tLMkV5bMo6lSI0c8m5OY7JQK0PGtVeF'
    b'HNEfN0bRnCa8RhnxXeR2tXlyMes5GaK9KLM/UuqylxqkuxqtXCYXubwMIYaFFUeEy8saDc'
    b'hKS5VEz4HmyWWzDt1HkYIOt41VlpSzIZDd2yFCRH3b2CKQ3jMmxIJJ9HnAJBlzhQXRVmmA'
    b'nQDpUkUjdxItS4DqpjAIKTeUQUptJmnI8C4xSH3tD8LR14lBd7i4C8qaif30V860M0uraC'
    b'muvqCsbSwdhbi0mFxQtgIdX1DGHNeQzhDk3ZUdMmTUtxSVye3lYXjVt1Ogz7+EO8yQqZKZ'
    b'6Ogu148YrzyoluQq43J08xOkj1RGlAVX4PytQcVK0eYS7QlTIJD2m2u3uqvJFe4vJ6Jb9x'
    b'TxnJ/s7cyy9QQlJxdaMRt8u2eRvsgLPCTQiqMtbzQonsg2158tCk/ox4ebMeh1SBO44fgL'
    b'HzAPc4jcn4bK8DI2xPeYO0kBEaL8ZQKsdT0v37+Mn8qGwnc1/E2L5Gr0m4+xaPBD3UAPtz'
    b'ZW8GrldBXgq1czG5S7f5KY/qP7rCoPSCeA6HVvh6yRboXfusVaOjRZ0le1LgN4y+45wr3F'
    b'cwRqW2cwbgWSJtdhaEwHkSZf2cWXyVfZSyvwrbfSLB0MlEjrW4or0NwsWJIRtgdyRZbFCA'
    b'hLkgYMS5KWNKe4oAE3QgWt2GDaz2pC5G0IL7uhZ/sahhkEqXo9qEHRS88YW78q3XI+JTlS'
    b'LRtiV5rlguhYsVwC1JkzA23ejeDuiu8TzAg6qRYCcBKrngabLCOOPo8yizjhjaI4LAfWAK'
    b'Pbb9vkq5/LIE16WWMFt2iC+uEkNHcL+TrkaV1/iJ3WR31XPObpDvNNRADdTgBGHS+qoJ6r'
    b'VxDImJjefGe8HTN1UjxTG602yf9isEoPOoB58lU6XVQlP/hVSGxQ+ZHjeiyeoeLogW01TV'
    b'5ZyFXy6rsVJPl1re4snYHUhzdWoPXhDU1H8i7IkGBqUOM+tG49qAMkeFZ2uAWF+2ou1uME'
    b'ncF+fbs9hCE169ewU8g4R89ImtBfw0uUYTV9GjNib3WZvKpnhpbJa2i5pSXETB3d8Ksaz2'
    b'uSaosN85BX1dKhO73q3axZChq+OSbwFuo0RSqixkoHIV+Rnk7dmwrJvKZUwyFNFvTFkAaQ'
    b'Rwox0CrAzWWAL2cOh07VHeOFmEn7HZ4qB2i/1278Cstk9T2mDmFqHaHb2huT/GJRRYi7NJ'
    b'zn4LjlZSqRclw7x8PrwV+kY5yEk3g8kn7lRrOXls2kfS+IRX7tRrNTz+b94ryja7SmVX6H'
    b'L4tRLs2G/m46Zjccab4LxPjzb+PxRl2H9jTYCAZcFhVnLgmnMw0Yy4mTWG0/lr48/7fFu/'
    b'r7TiStLhnQF7+X0GLsQjNRFHpBfDYBrVuNoaWZQOaoW0ce6SXXWQZa+9Z0pNQhQwbzMMmM'
    b'H5HdC1noSf1GUIY4pL9GeEbfTLmF/KrPysFV6L1RB98OZqK0Sjj3xHDzpxqB82Xypza3zp'
    b'JgT4lZ1p+6F4LTqBdqkj+jEx3QCf7kBUpNm0SWjui4xawRmfynkrXNEz4EBD30bb3ehA57'
    b'2ib6tnRouG8yM18mcnF6Rlz1ZFkSXaNuvOmlLNJ68JiC1uOGpqOByDAkmhTUfs3h1e+6Ut'
    b'yroSn3oI7iCozqwgJcrdqXcB7Ko7ZEGCaq5E3P9JG8qIAsLdPgInlTCuB0TtLcCB+GsGUW'
    b'wFg3ZF6Od4pXxvWtkbCMGaORcB5zxzvNqFgRf7TlDIXk7Xp7GlPwt6vdaegmb7eNKzD+vn'
    b'3HuALV9e2WccXMBGa3LIezXTcJGYc6oSoi029MU5nncZsmokZbQ16dDq8ZwHG9RRN4Q9sM'
    b'JhbzCI8fxjI8fXHZlBl5vLmCgwYHKDYETAUbH7VnVXasGGcFOPdhijKDDF55YIm4bYpmaj'
    b'/9agumUm+91oGRC1rwgvxgdIhY+sMb+mmMFWzD8eYYhYi6G6RtMA9mm48wT1NkmJYZMEzL'
    b'DBlNsTKH6PsyVk0KMaID4ag0QxC5Zji62deKjnqWkgypDSiwqzuvoe29XV163V6BUT+C/s'
    b'g8VmLPJ6AgBt1PGmFVh2ZieJNttIxJfgtv72KWJkvgLMmX4alDIe9ZAryXaR5D+oJRlCtt'
    b'4uZIpR+skDN6sIIoftrBShkGLiQhOvGNIC4qg9EJRAfAS0VHGVyQIVVpAup03z/pPrZxWD'
    b'+c+8c+ejQDQxp4u/4MPUTDVYBv+ZqRPS7GwoNa7CswKkbGrroVdowX3XuwJ9Xj5HJF2i8Y'
    b'r5JvHFvnyTd9WA36xjdZRCbPO2/wrS8cIK2MOmuSI6NOBnVt1FkZNBh1Gldjo04G16szXJ'
    b'mhR0e4JgC1jSdD+qN7xIRbHVhFCRs0visQvfW39fEPtSnPGN/M2adlaT9D1xABoXNwcOge'
    b'AGhtCSn1S+VVi28ZqWeWcCM1an0KwBp+8tO+sV4tzJcYVjraj9ezPPkWLeAgtpuWk2hS37'
    b'pbJ6NRAaITtgg/OmFL+mh2rybmK2z/WFrtX5UG8FtSltJ7Sh4Jm0oWiXeVbLB6s8gi0W6R'
    b'hfSukEXUzo8F9HkXi/jtHUuZZvT7wLfOqAusAngYDg7PJpNFwK0MwFD3ndEakhGdR0ShbD'
    b'vdnOYEzKK/vko+I6oLj+HcLr3KcG4U3zL5Fh0rQwWOjpWRPgzqPnBUQW0lwoYRDYwQNToR'
    b'A/fRiRjQ0s/D79gsABOib2GDDQmK7OEReGQPP0/+7a59v0z+H+SUGTTsMAEA'
    )).decode().splitlines()





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
                jlib.log('mupdf.Document() => {this=}')
            else:
                if filename:
                    if not filetype:
                        doc = mupdf.Document(filename)
                        jlib.log('mupdf.Document(filename) => {doc=}')
                    else:
                        assert 0, 'recognize_document() not yet supported'
                else:
                    doc = mupdf.PdfDocument()
                    jlib.log('mupdf.PdfDocument() => {doc=}')
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
            print(f'{doc=} {this=}')

        #try:
        #    self.this.append(this)
        #except __builtin__.Exception:
        #    self.this = this
        self.this = this
        jlib.log('{self.this=}')

        # fixme: not sure where self.thisown gets initialised in PyMuPDF.
        #
        self.thisown = True

        if self.thisown:
            self._graft_id = TOOLS.gen_id()
            if self.needsPass is True:
                self.isEncrypted = True
            else: # we won't init until doc is decrypted
                self.initData()


    def new_page(
            self,
            pno: int = -1,
            width: float = 595,
            height: float = 842,
            ):# -> Page:
        """Create and return a new page object.

        Args:
            pno: (int) insert before this page. Default: after last page.
            width: (float) page width in points. Default: 595 (ISO A4 width).
            height: (float) page height in points. Default 842 (ISO A4 height).
        Returns:
            A Page object.
        """
        #doc._newPage(pno, width=width, height=height)
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        #val = _fitz.Document__newPage(self, pno, width, height)
        if isinstance(self.this, mupdf.PdfDocument):
            pdf = self.this
        else:
            pdf = self.this.specifics()
        assert isinstance(pdf, mupdf.PdfDocument)
        mediabox = mupdf.Rect(mupdf.Rect.Fixed_UNIT)
        mediabox.x1 = width
        mediabox.y1 = height
        #pdf_obj *resources = NULL, *page_obj = NULL;
        #fz_buffer *contents = NULL;
        contents = mupdf.Buffer(0)
        #fz_try(gctx) {
        if pno < -1:
            raise Exception("bad page number(s)")
        # create /Resources and /Contents objects
        resources = pdf.add_object_drop(pdf.new_dict(1))
        page_obj = pdf.add_page(mediabox, 0, resources, contents)
        pdf.insert_page(pno, page_obj)
        # fixme: pdf->dirty = 1;

        self._reset_page_refs()
        return self[pno]



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

        #val = _fitz.this.load_page(page_id)
        if isinstance(page_id, int):
            page = self.this.load_page(page_id)
        else:
            chapter, pagenum = page_id
            page = self.this.load_chapter_page(chapter, pagenum)
        val = Page(page)

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
        jlib.log('{self.this=}')
        jlib.log('{self.this.count_pages=}')
        jlib.log('{self.this.m_internal=}')
        jlib.log('{self.this.m_internal.refs=}')
        ret = self.this.count_pages()
        jlib.log('{ret=}')
        return ret

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
        #jlib.log('{key!r=}')
        #jlib.log('{self=}')
        #jlib.log('{self.this=}')
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
        jlib.log('isPDF')
        if self.isClosed:
            raise ValueError("document closed")
        jlib.log('calling self.this.specifics()')
        jlib.log('calling self.this.specifics() {self.this=}')
        if isinstance(self.this, mupdf.PdfDocument):
            return True
        p = self.this.specifics()
        jlib.log('{p=}')
        jlib.log('{self.this=}')
        pp = self.this.specifics()
        jlib.log('{pp=}')
        jlib.log('{pp.m_internal=}')
        if pp.m_internal:
            jlib.log(f'Returning true')
            return True
        jlib.log(f'Returning false')
        return False

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

        pdf = self.this.document_from_fz_document()
        if not pdf.m_internal:
            return False
        return pdf.can_be_saved_incrementally()


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

        #return _fitz.Document_update_stream(self, xref, stream, new)
        pdf = self.this.specifics()
        xreflen = pdf.xref_len()
        if xref < 1 or xref > xreflen:
            raise Exception(f'bad xref={xref} xreflen={xreflen}')
        obj = pdf.new_indirect(xref, 0)
        if not new and not obj.is_stream():
            raise Exception(f'no stream object at xref={xref}')
        res = JM_BufferFromBytes(stream)
        if not res:
            raise Exception('bad type: "stream"')
        JM_update_stream(pdf, obj, res, 1)
        pdf.dirty = 1


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
        page_count = self.this.count_pages()
        jlib.log('{loc=} {self.pageCount=} {page_count=}')
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
            raise IndexError(f"page {i} not in document")
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
        self.isClosed = True

    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.close()

    def page_annot_xrefs(self, n):
        page_count = self.this.count_pages()
        jlib.log('*** {page_count=}')
        while n < 0:
            n += page_count
        jlib.log('self.this is: {self.this}')
        jlib.log('{self.this.m_internal=}')
        if 0:
            jlib.log('dir(self.this):')
            for i in dir(self.this):
                jlib.log('    {i}')
        jlib.log('self.this.specifics is: {self.this.specifics}')
        if isinstance(self.this, mupdf.PdfDocument):
            jlib.log('self.this is a mupdf.PdfDocument')
            pdf_document = self.this
        else:
            jlib.log('self.this is not a mupdf.PdfDocument: {self.this}')
            jlib.log('{self.this.m_internal=}')
            pdf_document = self.this.specifics()
            jlib.log('self.this.specifics() => {pdf_document=}')
        page_obj = pdf_document.lookup_page_obj(n)
        #jlib.log('page_obj: {dir_str(page_obj)}')
        #jlib.log('page_obj.m_internal: {dir_str(page_obj.m_internal)}')
        annots = JM_get_annot_xref_list(page_obj)
        jlib.log('{self.this.count_pages()=} {self.pageCount=}')
        return annots


    def has_links(self):
        """Check whether there are links on any page."""
        jlib.log('has_links()')
        if self.isClosed:
            raise ValueError("document closed")
        if not self.isPDF:
            raise ValueError("not a PDF")
        jlib.log('{self.pageCount=}')
        jlib.log('calling self.pageCount')
        for i in range(self.pageCount):
            jlib.log('{i=}')
            for item in self.page_annot_xrefs(i):
                if item[1] == PDF_ANNOT_LINK:
                    jlib.log('Returning true')
                    return True
        jlib.log('Returning false')
        return False

    def has_annots(self):
        """Check whether there are annotations on any page."""
        jlib.log('has_annots()')
        if self.isClosed:
            raise ValueError("document closed")
        if not self.isPDF:
            raise ValueError("not a PDF")
        for i in range(self.pageCount):
            for item in self.page_annot_xrefs(i):
                if not (item[1] == PDF_ANNOT_LINK or item[1] == PDF_ANNOT_WIDGET):
                    return True
        return False

    @property
    def is_form_pdf(self):
        """Either False or PDF field count."""
        jlib.log('{self.this.count_pages()=}')
        pdf = self.this.specifics()
        if not pdf.m_internal:
            return False
        jlib.log('{self.this.count_pages()=}')
        count = -1;
        try:
            jlib.log('{self.this.count_pages()=}')
            fields = pdf_dict_getl(self, pdf.trailer(),
                    mupdf.PDF_ENUM_NAME_Root,
                    mupdf.PDF_ENUM_NAME_AcroForm,
                    mupdf.PDF_ENUM_NAME_Fields,
                    )
            jlib.log('{fields=} {fields.is_array()=} {self.this.count_pages()=}')
            if fields.is_array():
                count = fields.array_len()
            jlib.log('{self.this.count_pages()=}')
        except Exception:
            return False
        jlib.log('{count=}')
        jlib.log('{self.this.count_pages()=}')
        if count >= 0:
            return count
        return False

    @property
    def is_repaired(self):
        """Check whether PDF was repaired."""
        jlib.log('{self.this.count_pages()=}')
        pdf = self.this.document_from_fz_document()
        if not pdf.m_internal:
            return False
        r = pdf.was_repaired()
        jlib.log('{r=}')
        if r:
            jlib.log('returning true')
            return True
        jlib.log('returning false')
        return False

    @property
    def is_dirty(self):
        jlib.log('{self.this.count_pages()=}')
        pdf = self.this.specifics()
        jlib.log('{pdf.m_internal=}')
        if not pdf.m_internal:
            return False
        r = pdf.has_unsaved_changes()
        jlib.log('{r=}')
        return True if r else False


open = Document






class Page:
    #__swig_setmethods__ = {}
    #__setattr__ = lambda self, name, value: _swig_setattr(self, Page, name, value)
    #__swig_getmethods__ = {}
    #__getattr__ = lambda self, name: _swig_getattr(self, Page, name)

    def __init__(self, page):
        assert isinstance(page, (mupdf.Page, mupdf.PdfPage)), f'page is: {page}'
        self.this = page
        self.thisown = True

    def bound(self):
        """Get page rectangle."""
        CheckParent(self)

        val = self.this.bound_page()
        val = Rect(val)

        return val

    rect = property(bound, doc="page rectangle")

    def add_caret_annot(self, point: point_like) -> "struct Annot *":
        """Add a 'Caret' annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_caret_annot(point)
        finally:
            if old_rotation != 0:
                self.set_rotation(old_rotation)
        annot_postprocess(self, annot)
        return annot

    def getImageBbox(self, name):

        """Get rectangle occupied by image 'name'.

        'name' is either an item of the image full list, or the referencing
        name string - elem[7] of the resp. item."""
        CheckParent(self)
        doc = self.parent
        if doc.isClosed or doc.isEncrypted:
            raise ValueError("document closed or encrypted")
        inf_rect = Rect(1, 1, -1, -1)
        if type(name) in (list, tuple):
            if not type(name[-1]) is int:
                raise ValueError("need a full page image list item")
            item = name
        else:
            imglist = [i for i in doc.getPageImageList(self.number, True) if name == i[-3]]
            if len(imglist) == 1:
                item = imglist[0]
            elif imglist == []:
                raise ValueError("no valid image found")
            else:
                raise ValueError("found more than one image of that name.")

        val = _fitz.Page_getImageBbox(self, name)

        if not bool(val):
            return inf_rect
        rc = inf_rect
        for v in val:
            if v[0] == item[-3]:
                rc = Quad(v[1]).rect
                break
        val = rc * self.transformationMatrix

        return val


    def run(self, dw, m):
        """Run page through a device."""
        CheckParent(self)

        return _fitz.Page_run(self, dw, m)


    def _get_text_page(self, clip=None, flags=0):
        val = _fitz.Page__get_text_page(self, clip, flags)
        val.thisown = True

        return val


    def getTextPage(self, clip: rect_like =None, flags: int =0) -> "TextPage":
        CheckParent(self)
        old_rotation = self.rotation
        if old_rotation != 0:
            self.setRotation(0)
        try:
    #if clip is None:
    #    clip = self.rect
            textpage = self._get_text_page(clip, flags=flags)

        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        return textpage

    @property

    def language(self):
        """Page language."""

        return _fitz.Page_language(self)


    def setLanguage(self, language=None):
        """Set PDF page default language."""
        CheckParent(self)

        return _fitz.Page_setLanguage(self, language)


    def getSVGimage(self, matrix=None, text_as_path=1):
        """Make SVG image from page."""
        CheckParent(self)

        return _fitz.Page_getSVGimage(self, matrix, text_as_path)


    def _set_opacity(self, gstate=None, CA=1, ca=1):

        if min(CA, ca) >= 1:
            return
        tCA = int(round(max(CA , 0) * 100))
        if tCA >= 100:
            tCA = 99
        tca = int(round(max(ca, 0) * 100))
        if tca >= 100:
            tca = 99
        gstate = "fitzca%02i%02i" % (tCA, tca)


        return _fitz.Page__set_opacity(self, gstate, CA, ca)


    def _add_caret_annot(self, point):
        return _fitz.Page__add_caret_annot(self, point)

    def _add_redact_annot(self, quad, text=None, da_str=None, align=0, fill=None, text_color=None):
        return _fitz.Page__add_redact_annot(self, quad, text, da_str, align, fill, text_color)

    def _add_line_annot(self, p1, p2):
        return _fitz.Page__add_line_annot(self, p1, p2)

    def _add_text_annot(self, point, text, icon=None):
        return _fitz.Page__add_text_annot(self, point, text, icon)

    def _add_ink_annot(self, list):
        return _fitz.Page__add_ink_annot(self, list)

    def _add_stamp_annot(self, rect, stamp=0):
        return _fitz.Page__add_stamp_annot(self, rect, stamp)

    def _add_file_annot(self, point, buffer, filename, ufilename=None, desc=None, icon=None):
        return _fitz.Page__add_file_annot(self, point, buffer, filename, ufilename, desc, icon)

    def _add_text_marker(self, quads, annot_type):

        CheckParent(self)
        if not self.parent.isPDF:
            raise ValueError("not a PDF")

        val = _fitz.Page__add_text_marker(self, quads, annot_type)

        if not val:
            return None
        val.parent = weakref.proxy(self)
        self._annot_refs[id(val)] = val

        return val


    def _add_square_or_circle(self, rect, annot_type):
        return _fitz.Page__add_square_or_circle(self, rect, annot_type)

    def _add_multiline(self, points, annot_type):
        return _fitz.Page__add_multiline(self, points, annot_type)

    def _add_freetext_annot(self, rect, text, fontsize=11, fontname=None, text_color=None, fill_color=None, align=0, rotate=0):
        return _fitz.Page__add_freetext_annot(self, rect, text, fontsize, fontname, text_color, fill_color, align, rotate)

    @property
    def rotationMatrix(self) -> Matrix:
        """Reflects page rotation."""
        return Matrix(TOOLS._rotate_matrix(self))

    @property
    def derotationMatrix(self) -> Matrix:
        """Reflects page de-rotation."""
        return Matrix(TOOLS._derotate_matrix(self))

    def addCaretAnnot(self, point: point_like) -> "struct Annot *":
        """Add a 'Caret' annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_caret_annot(point)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot


    def addStrikeoutAnnot(self, quads=None, start=None, stop=None, clip=None) -> "struct Annot *":
        """Add a 'StrikeOut' annotation."""
        if quads is None:
            q = get_highlight_selection(self, start=start, stop=stop, clip=clip)
        else:
            q = CheckMarkerArg(quads)
        return self._add_text_marker(q, PDF_ANNOT_STRIKE_OUT)


    def addUnderlineAnnot(self, quads=None, start=None, stop=None, clip=None) -> "struct Annot *":
        """Add a 'Underline' annotation."""
        if quads is None:
            q = get_highlight_selection(self, start=start, stop=stop, clip=clip)
        else:
            q = CheckMarkerArg(quads)
        return self._add_text_marker(q, PDF_ANNOT_UNDERLINE)


    def addSquigglyAnnot(self, quads=None, start=None,
                         stop=None, clip=None) -> "struct Annot *":
        """Add a 'Squiggly' annotation."""
        if quads is None:
            q = get_highlight_selection(self, start=start, stop=stop, clip=clip)
        else:
            q = CheckMarkerArg(quads)
        return self._add_text_marker(q, PDF_ANNOT_SQUIGGLY)


    def addHighlightAnnot(self, quads=None, start=None,
                          stop=None, clip=None) -> "struct Annot *":
        """Add a 'Highlight' annotation."""
        if quads is None:
            q = get_highlight_selection(self, start=start, stop=stop, clip=clip)
        else:
            q = CheckMarkerArg(quads)
        return self._add_text_marker(q, PDF_ANNOT_HIGHLIGHT)


    def addRectAnnot(self, rect: rect_like) -> "struct Annot *":
        """Add a 'Square' (rectangle) annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_square_or_circle(rect, PDF_ANNOT_SQUARE)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot


    def addCircleAnnot(self, rect: rect_like) -> "struct Annot *":
        """Add a 'Circle' (ellipse, oval) annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_square_or_circle(rect, PDF_ANNOT_CIRCLE)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot


    def addTextAnnot(self, point: point_like, text: str, icon: str ="Note") -> "struct Annot *":
        """Add a 'Text' (sticky note) annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_text_annot(point, text, icon=icon)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot


    def addLineAnnot(self, p1: point_like, p2: point_like) -> "struct Annot *":
        """Add a 'Line' annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_line_annot(p1, p2)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot


    def addPolylineAnnot(self, points: list) -> "struct Annot *":
        """Add a 'PolyLine' annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_multiline(points, PDF_ANNOT_POLY_LINE)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot


    def addPolygonAnnot(self, points: list) -> "struct Annot *":
        """Add a 'Polygon' annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_multiline(points, PDF_ANNOT_POLYGON)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot


    def addStampAnnot(self, rect: rect_like, stamp: int =0) -> "struct Annot *":
        """Add a ('rubber') 'Stamp' annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_stamp_annot(rect, stamp)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot


    def addInkAnnot(self, handwriting: list) -> "struct Annot *":
        """Add a 'Ink' ('handwriting') annotation.

        The argument must be a list of lists of point_likes.
        """
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_ink_annot(handwriting)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot


    def addFileAnnot(self, point: point_like,
        buffer: typing.ByteString,
        filename: str,
        ufilename: OptStr =None,
        desc: OptStr =None,
        icon: OptStr =None) -> "struct Annot *":
        """Add a 'FileAttachment' annotation."""

        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_file_annot(point,
                        buffer,
                        filename,
                        ufilename=ufilename,
                        desc=desc,
                        icon=icon)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot


    def addFreetextAnnot(self, rect: rect_like, text: str, fontsize: float =11,
                         fontname: OptStr =None, text_color: OptSeq =None,
                         fill_color: OptSeq =None, align: int =0, rotate: int =0) -> "struct Annot *":
        """Add a 'FreeText' annotation."""

        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_freetext_annot(rect, text, fontsize=fontsize,
                    fontname=fontname, text_color=text_color,
                    fill_color=fill_color, align=align, rotate=rotate)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot


    def addRedactAnnot(self, quad, text: OptStr =None, fontname: OptStr =None,
                       fontsize: float =11, align: int =0, fill: OptSeq =None, text_color: OptSeq =None,
                       cross_out: bool =True) -> "struct Annot *":
        """Add a 'Redact' annotation."""
        da_str = None
        if text:
            CheckColor(fill)
            CheckColor(text_color)
            if not fontname:
                fontname = "Helv"
            if not fontsize:
                fontsize = 11
            if not text_color:
                text_color = (0, 0, 0)
            if hasattr(text_color, "__float__"):
                text_color = (text_color, text_color, text_color)
            if len(text_color) > 3:
                text_color = text_color[:3]
            fmt = "{:g} {:g} {:g} rg /{f:s} {s:g} Tf"
            da_str = fmt.format(*text_color, f=fontname, s=fontsize)
            if fill is None:
                fill = (1, 1, 1)
            if fill:
                if hasattr(fill, "__float__"):
                    fill = (fill, fill, fill)
                if len(fill) > 3:
                    fill = fill[:3]

        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_redact_annot(quad, text=text, da_str=da_str,
                       align=align, fill=fill)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
    #-------------------------------------------------------------
    # change appearance to show a crossed-out rectangle
    #-------------------------------------------------------------
        if cross_out:
            ap_tab = annot._getAP().splitlines()[:-1]  # get the 4 commands only
            _, LL, LR, UR, UL = ap_tab
            ap_tab.append(LR)
            ap_tab.append(LL)
            ap_tab.append(UR)
            ap_tab.append(LL)
            ap_tab.append(UL)
            ap_tab.append(b"S")
            ap = b"\n".join(ap_tab)
            annot._setAP(ap, 0)
        return annot


    def _load_annot(self, name, xref):
        return _fitz.Page__load_annot(self, name, xref)

    def _get_resource_properties(self):
        return _fitz.Page__get_resource_properties(self)

    def _set_resource_property(self, name, xref):
        return _fitz.Page__set_resource_property(self, name, xref)

    def _get_optional_content(self, oc: OptInt) -> OptStr:
        if oc == None or oc == 0:
            return None
        doc = self.parent
        check = doc.xrefObject(oc, compressed=True)
        if not ("/Type/OCG" in check or "/Type/OCMD" in check):
            raise ValueError("bad optional content: 'oc'")
        props = {}
        for p, x in self._get_resource_properties():
            props[x] = p
        if oc in props.keys():
            return props[oc]
        i = 0
        mc = "MC%i" % i
        while mc in props.values():
            i += 1
            mc = "MC%i" % i
        self._set_resource_property(mc, oc)
        return mc

    def get_oc_items(self) -> list:
        """Get OCGs and OCMDs used in the page's contents.

        Returns:
            List of items (name, xref, type), where type is one of "ocg" / "ocmd",
            and name is the property name.
        """
        rc = []
        for pname, xref in self._get_resource_properties():
            text = self.parent.xrefObject(xref, compressed=True)
            if "/Type/OCG" in text:
                octype = "ocg"
            elif "/Type/OCMD" in text:
                octype = "ocmd"
            else:
                continue
            rc.append((pname, xref, octype))
        return rc


    def annot_names(self):
        """List of names of annotations, fields and links."""
        CheckParent(self)

        return _fitz.Page_annot_names(self)


    def annot_xrefs(self):
        """List of xref numbers of annotations, fields and links."""
        CheckParent(self)

        return _fitz.Page_annot_xrefs(self)


    def loadAnnot(self, ident: typing.Union[str, int]) -> "struct Annot *":
        """Load an annot by name (/NM key) or xref.

        Args:
            ident: identifier, either name (str) or xref (int).
        """

        CheckParent(self)
        if type(ident) is str:
            xref = 0
            name = ident
        elif type(ident) is int:
            xref = ident
            name = None
        else:
            raise ValueError("identifier must be string or integer")
        val = self._load_annot(name, xref)
        if not val:
            return val
        val.thisown = True
        val.parent = weakref.proxy(self)
        self._annot_refs[id(val)] = val
        return val

    load_annot = loadAnnot


    #---------------------------------------------------------------------
    # page addWidget
    #---------------------------------------------------------------------
    def addWidget(self, widget: Widget) -> "struct Annot *":
        """Add a 'Widget' (form field)."""
        CheckParent(self)
        doc = self.parent
        if not doc.isPDF:
            raise ValueError("not a PDF")
        widget._validate()
        annot = self._addWidget(widget.field_type, widget.field_name)
        if not annot:
            return None
        annot.thisown = True
        annot.parent = weakref.proxy(self) # owning page object
        self._annot_refs[id(annot)] = annot
        widget.parent = annot.parent
        widget._annot = annot
        widget.update()
        return annot


    def _addWidget(self, field_type, field_name):
        return _fitz.Page__addWidget(self, field_type, field_name)

    def getDisplayList(self, annots=1):

        """Make a DisplayList from the page for Pixmap generation.

        Include (default) or exclude annotations."""

        CheckParent(self)


        val = _fitz.Page_getDisplayList(self, annots)
        val.thisown = True

        return val


    def getDrawings(self):
        """Get page draw paths."""

        CheckParent(self)
        val = self._getDrawings()  # read raw list from trace device
        paths = []

        def new_path():
            """Return empty path dict to use as template."""
            return {
                "color": None,
                "fill": None,
                "width": 1.0,
                "lineJoin": 0,
                "lineCap": (0, 0, 0),
                "dashes": "[] 0",
                "closePath": False,
                "even_odd": False,
                "rect": Rect(),
                "items": [],
                "opacity": 1.0,
            }

        def is_rectangle(path):
            """Check if path represents a rectangle.

            For this, it must be exactly three connected lines, of which
            the first and the last one must be horizontal and line two
            must be vertical.
            """
            if not path["closePath"]:
                return False
            if [item[0] for item in path["items"]] != ["l", "l", "l"]:
                return False
            p1, p2 = path["items"][0][1:]  # first line
            p3, p4 = path["items"][1][1:]  # second line
            p5, p6 = path["items"][2][1:]  # third line
            if p2 != p3 or p4 != p5:  # must be connected
                return False
            if p1.y != p2.y or p3.x != p4.x or p5.y != p6.y:
                return False
            return True

        def check_and_merge(this, prev):
            """Check if "this" is the "stroke" version of "prev".

            If so, update "prev" with appropriate values and return True,
            else do nothing and return False.
            """
            if prev is None:
                return False
            if this["items"] != prev["items"]:  # must have same items
                return False
            if this["closePath"] != prev["closePath"]:
                return False
            if this["color"] is not None:
                prev["color"] = this["color"]
            if this["width"] != 1:
                prev["width"] = this["width"]
            if this["dashes"] != "[] 0":
                prev["dashes"] = this["dashes"]
            if this["lineCap"] != (0, 0, 0):
                prev["lineCap"] = this["lineCap"]
            if this["lineJoin"] != 0:
                prev["lineJoin"] = this["lineJoin"]
            return True

        for item in val:
            if type(item) is list:
                if item[0] in ("fill", "stroke", "clip", "clip-stroke"):
    # this begins a new path
                    path = new_path()
                    ctm = Matrix(1, 1)
                    factor = 1  # modify width and dash length
                    current = None  # the current point
                    for x in item[1:]:  # loop through path parms that follow
                        if x == "non-zero":
                            path["even_odd"] = False
                        elif x == "even-odd":
                            path["even_odd"] = True
                        elif x[0] == "matrix":
                            ctm = Matrix(x[1])
                            if abs(ctm.a) == abs(ctm.d):
                                factor = abs(ctm.a)
                        elif x[0] == "w":
                            path["width"] = x[1] * factor
                        elif x[0] == "lineCap":
                            path["lineCap"] = x[1:]
                        elif x[0] == "lineJoin":
                            path["lineJoin"] = x[1]
                        elif x[0] == "color":
                            if item[0] == "fill":
                                path["fill"] = x[1:]
                            else:
                                path["color"] = x[1:]
                        elif x[0] == "dashPhase":
                            dashPhase = x[1] * factor
                        elif x[0] == "dashes":
                            dashes = x[1:]
                            l = list(map(lambda y: float(y) * factor, dashes))
                            l = list(map(str, l))
                            path["dashes"] = "[%s] %g" % (" ".join(l), dashPhase)
                        elif x[0] == "alpha":
                            path["opacity"] = round(x[1], 2)

                if item[0] == "m":
                    p = Point(item[1]) * ctm
                    current = p
                    path["rect"] = Rect(p, p)
                elif item[0] == "l":
                    p2 = Point(item[1]) * ctm
                    path["items"].append(("l", current, p2))
                    current = p2
                    path["rect"] |= p2
                elif item[0] == "c":
                    p2 = Point(item[1]) * ctm
                    p3 = Point(item[2]) * ctm
                    p4 = Point(item[3]) * ctm
                    path["items"].append(("c", current, p2, p3, p4))
                    current = p4
                    path["rect"] |= p2
                    path["rect"] |= p3
                    path["rect"] |= p4
            elif item == "closePath":
                path["closePath"] = True
            elif item in ("estroke", "efill", "eclip", "eclip-stroke"):
                if is_rectangle(path):
                    path["items"] = [("re", path["rect"])]
                    path["closePath"] = False

                try:  # check if path is "stroke" duplicate of previous
                    prev = paths.pop()  # get previous path in list
                except IndexError:
                    prev = None  # we are the first
                if prev is None:
                    paths.append(path)
                elif check_and_merge(path, prev) is False:  # no duplicates
                    paths.append(prev)  # re-append old one
                    paths.append(path)  # append new one
                else:
                    paths.append(prev)  # append modified old one

                path = None
            else:
                print("unexpected item:", item)

        return paths


    def _getDrawings(self):
        return _fitz.Page__getDrawings(self)

    def _apply_redactions(self, *args):
        return _fitz.Page__apply_redactions(self, *args)

    def _makePixmap(self, doc, ctm, cs, alpha=0, annots=1, clip=None):
        return _fitz.Page__makePixmap(self, doc, ctm, cs, alpha, annots, clip)

    def setMediaBox(self, rect):
        """Set the MediaBox."""
        CheckParent(self)

        return _fitz.Page_setMediaBox(self, rect)


    def setCropBox(self, rect):
        """Set the CropBox."""
        CheckParent(self)

        return _fitz.Page_setCropBox(self, rect)


    def loadLinks(self):
        """Get first Link."""
        CheckParent(self)

        val = _fitz.Page_loadLinks(self)

        if val:
            val.thisown = True
            val.parent = weakref.proxy(self) # owning page object
            self._annot_refs[id(val)] = val
            if self.parent.isPDF:
                link_id = [x for x in self.annot_xrefs() if x[1] == PDF_ANNOT_LINK][0]
                val.xref = link_id[0]
                val.id = link_id[2]
            else:
                val.xref = 0
                val.id = ""


        return val

    firstLink = property(loadLinks, doc="First link on page")
    @property

    def firstAnnot(self):
        """First annotation."""
        CheckParent(self)

        val = _fitz.Page_firstAnnot(self)

        if val:
            val.thisown = True
            val.parent = weakref.proxy(self) # owning page object
            self._annot_refs[id(val)] = val


        return val

    @property

    def firstWidget(self):
        """First widget/field."""
        CheckParent(self)

        val = _fitz.Page_firstWidget(self)

        if val:
            val.thisown = True
            val.parent = weakref.proxy(self) # owning page object
            self._annot_refs[id(val)] = val
            widget = Widget()
            TOOLS._fill_widget(val, widget)
            val = widget


        return val


    def deleteLink(self, linkdict):
        """Delete a Link."""
        CheckParent(self)

        val = _fitz.Page_deleteLink(self, linkdict)
        if linkdict["xref"] == 0: return
        try:
            linkid = linkdict["id"]
            linkobj = self._annot_refs[linkid]
            linkobj._erase()
        except:
            pass


        return val


    def deleteAnnot(self, annot):

        """Delete annot and return next one."""
        CheckParent(self)
        CheckParent(annot)

        val = _fitz.Page_deleteAnnot(self, annot)

        if val:
            val.thisown = True
            val.parent = weakref.proxy(self) # owning page object
            val.parent._annot_refs[id(val)] = val
        annot._erase()


        return val

    @property

    def MediaBox(self):
        """The MediaBox."""
        CheckParent(self)

        val = _fitz.Page_MediaBox(self)
        val = Rect(val)

        return val

    @property

    def CropBox(self):
        """The CropBox."""
        CheckParent(self)

        val = _fitz.Page_CropBox(self)
        val = Rect(val)

        return val


    @property
    def CropBoxPosition(self):
        return self.CropBox.tl

    @property

    def rotation(self):
        """Page rotation."""
        CheckParent(self)

        #return _fitz.Page_rotation(self)
        #pdf_page *page = pdf_page_from_fz_page(gctx, (fz_page *) self);
        page = self.this if isinstance(self.this, mupdf.PdfPage) else self.this.page_from_fz_page()
        if not page:
            return 0
        return JM_page_rotation(page);



    def setRotation(self, rotation):
        """Set page rotation."""
        CheckParent(self)

        return _fitz.Page_setRotation(self, rotation)


    def _addAnnot_FromString(self, linklist):
        """Add links from list of object sources."""
        CheckParent(self)

        return _fitz.Page__addAnnot_FromString(self, linklist)


    def clean_contents(self, sanitize=1):
        """Clean page /Contents into one object."""
        CheckParent(self)
        if not sanitize and not self.is_wrapped:
            self.wrap_contents()

        return Page_clean_contents(self, sanitize)


    def _showPDFpage(self, fz_srcpage, overlay=1, matrix=None, xref=0, oc=0, clip=None, graftmap=None, _imgname=None):
        return _fitz.Page__showPDFpage(self, fz_srcpage, overlay, matrix, xref, oc, clip, graftmap, _imgname)

    def _insertImage(self, filename=None, pixmap=None, stream=None, imask=None, overlay=1, oc=0, xref=0, matrix=None, _imgname=None, _imgpointer=None):
        return _fitz.Page__insertImage(self, filename, pixmap, stream, imask, overlay, oc, xref, matrix, _imgname, _imgpointer)

    def refresh(self):
        """Refresh page after link/annot/widget updates."""
        CheckParent(self)

        return _fitz.Page_refresh(self)


    def insertFont(self, fontname="helv", fontfile=None, fontbuffer=None,
                   set_simple=False, wmode=0, encoding=0):
        doc = self.parent
        if doc is None:
            raise ValueError("orphaned object: parent is None")
        idx = 0

        if fontname.startswith("/"):
            fontname = fontname[1:]

        font = CheckFont(self, fontname)
        if font is not None:                    # font already in font list of page
            xref = font[0]                      # this is the xref
            if CheckFontInfo(doc, xref):        # also in our document font list?
                return xref                     # yes: we are done
    # need to build the doc FontInfo entry - done via getCharWidths
            doc.getCharWidths(xref)
            return xref

    #--------------------------------------------------------------------------
    # the font is not present for this page
    #--------------------------------------------------------------------------

        bfname = Base14_fontdict.get(fontname.lower(), None) # BaseFont if Base-14 font

        serif = 0
        CJK_number = -1
        CJK_list_n = ["china-t", "china-s", "japan", "korea"]
        CJK_list_s = ["china-ts", "china-ss", "japan-s", "korea-s"]

        try:
            CJK_number = CJK_list_n.index(fontname)
            serif = 0
        except:
            pass

        if CJK_number < 0:
            try:
                CJK_number = CJK_list_s.index(fontname)
                serif = 1
            except:
                pass

        if fontname.lower() in fitz_fontdescriptors.keys():
            import pymupdf_fonts
            fontbuffer = pymupdf_fonts.myfont(fontname)  # make a copy
            del pymupdf_fonts

    # install the font for the page
        val = self._insertFont(fontname, bfname, fontfile, fontbuffer, set_simple, idx,
                               wmode, serif, encoding, CJK_number)

        if not val:                   # did not work, error return
            return val

        xref = val[0]                 # xref of installed font

        if CheckFontInfo(doc, xref):  # check again: document already has this font
            return xref               # we are done

    # need to create document font info
        doc.getCharWidths(xref)
        return xref



    def _insertFont(self, fontname, bfname, fontfile, fontbuffer, set_simple, idx, wmode, serif, encoding, ordering):
        return _fitz.Page__insertFont(self, fontname, bfname, fontfile, fontbuffer, set_simple, idx, wmode, serif, encoding, ordering)
    @property

    def transformationMatrix(self):
        """Page transformation matrix."""
        CheckParent(self)

        val = _fitz.Page_transformationMatrix(self)

        if self.rotation % 360 == 0:
            val = Matrix(val)
        else:
            val = Matrix(1, 0, 0, -1, 0, self.CropBox.height)


        return val


    def get_contents(self):
        """Get xrefs of /Contents objects."""
        CheckParent(self)

        #return _fitz.Page_get_contents(self)
        ret = []
        page = self.this.page_from_fz_page()
        obj = page.obj()
        contents = obj.dict_get(mupdf.PDF_ENUM_NAME_Contents)
        if contents.is_array():
            n = contents.array_len()
            for i in range(n):
                icont = contents.array_get(i)
                xref = icont.to_num()
                ret.append(xref)
        elif contents.m_internal:
            xref = contents.to_num()
            ret.append( xref)
        return ret


    def set_contents(self, xref):
        """Set an xref as the (only) /Contents object."""
        CheckParent(self)

        return Page_set_contents(self, xref)


    @property
    def is_wrapped(self):
        """Check if /Contents is wrapped with string pair "q" / "Q"."""
        if getattr(self, "was_wrapped", False):  # costly checks only once
            return True
        cont = self.read_contents().split()
        if cont == []:  # no contents treated as okay
            self.was_wrapped = True
            return True
        if cont[0] != b"q" or cont[-1] != b"Q":
            return False  # potential "geometry" issue
        self.was_wrapped = True  # cheap check next time
        return True

    _isWrapped = is_wrapped

    def wrap_contents(self):
        if self.is_wrapped:  # avoid unnecessary wrapping
            return
        TOOLS._insert_contents(self, b"q\n", False)
        TOOLS._insert_contents(self, b"\nQ", True)
        self.was_wrapped = True  # indicate not needed again

    wrapContents = wrap_contents


    def links(self, kinds=None):
        """ Generator over the links of a page.

        Args:
            kinds: (list) link kinds to subselect from. If none,
                   all links are returned. E.g. kinds=[LINK_URI]
                   will only yield URI links.
        """
        all_links = self.getLinks()
        for link in all_links:
            if kinds is None or link["kind"] in kinds:
                yield (link)


    def annots(self, types=None):
        """ Generator over the annotations of a page.

        Args:
            types: (list) annotation types to subselect from. If none,
                   all annotations are returned. E.g. types=[PDF_ANNOT_LINE]
                   will only yield line annotations.
        """
        annot = self.firstAnnot
        while annot:
            if types is None or annot.type[0] in types:
                yield (annot)
            annot = annot.next


    def widgets(self, types=None):
        """ Generator over the widgets of a page.

        Args:
            types: (list) field types to subselect from. If none,
                    all fields are returned. E.g. types=[PDF_WIDGET_TYPE_TEXT]
                    will only yield text fields.
        """
        widget = self.firstWidget
        while widget:
            if types is None or widget.field_type in types:
                yield (widget)
            widget = widget.next


    def __str__(self):
        CheckParent(self)
        x = self.parent.name
        if self.parent.stream is not None:
            x = "<memory, doc# %i>" % (self.parent._graft_id,)
        if x == "":
            x = "<new PDF, doc# %i>" % self.parent._graft_id
        return "page %s of %s" % (self.number, x)

    def __repr__(self):
        CheckParent(self)
        x = self.parent.name
        if self.parent.stream is not None:
            x = "<memory, doc# %i>" % (self.parent._graft_id,)
        if x == "":
            x = "<new PDF, doc# %i>" % self.parent._graft_id
        return "page %s of %s" % (self.number, x)

    def _forget_annot(self, annot):
        """Remove an annot from reference dictionary."""
        aid = id(annot)
        if aid in self._annot_refs:
            self._annot_refs[aid] = None

    def _reset_annot_refs(self):
        """Invalidate / delete all annots of this page."""
        self._annot_refs.clear()

    @property
    def xref(self):
        """PDF xref number of page."""
        CheckParent(self)
        return self.parent.page_xref(self.number)

    def _erase(self):
        self._reset_annot_refs()
        try:
            self.parent._forget_page(self)
        except:
            pass
        self.parent = None
        self.thisown = False
        self.number = None

    #def __del__(self):
    #    self._erase()

    def get_fonts(self, full=False):
        """List of fonts defined in the page object."""
        CheckParent(self)
        return self.parent.get_page_fonts(self.number, full=full)

    getFontList = get_fonts

    def get_images(self, full=False):
        """List of images defined in the page object."""
        CheckParent(self)
        return self.parent.get_page_images(self.number, full=full)

    getImageList = get_images

    def readContents(self):
        """All /Contents streams concatenated to one bytes object."""
        return TOOLS._get_all_contents(self)


    def read_contents(self):
        """All /Contents streams concatenated to one bytes object."""
        return TOOLS._get_all_contents(self)


    @property
    def MediaBoxSize(self):
        return Point(self.MediaBox.width, self.MediaBox.height)

    cleanContents = clean_contents
    getContents = get_contents

class Pixmap:
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Pixmap, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Pixmap, name)

    def __init__(self, *args):
        """Pixmap(colorspace, irect, alpha) - empty pixmap.
        Pixmap(colorspace, src) - copy changing colorspace.
        Pixmap(src, width, height,[clip]) - scaled copy, float dimensions.
        Pixmap(src, alpha=1) - copy and add or drop alpha channel.
        Pixmap(filename) - from an image in a file.
        Pixmap(image) - from an image in memory (bytes).
        Pixmap(colorspace, width, height, samples, alpha) - from samples data.
        Pixmap(PDFdoc, xref) - from an image at xref in a PDF document.
        """

        this = _fitz.new_Pixmap(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this

    def shrink(self, factor):
        """Divide width and height by 2**factor.
        E.g. factor=1 shrinks to 25% of original size (in place)."""

        return _fitz.Pixmap_shrink(self, factor)


    def tintWith(self, black, white):
        return _fitz.Pixmap_tintWith(self, black, white)

    def clearWith(self, *args):
        """Fill all color components with same value."""

        return _fitz.Pixmap_clearWith(self, *args)


    def copyPixmap(self, src, bbox):
        """Copy bbox from another Pixmap."""

        return _fitz.Pixmap_copyPixmap(self, src, bbox)


    def setAlpha(self, alphavalues=None, premultiply=1):
        """Set alphas to values contained in a byte array.
        If omitted, set alphas to 255."""

        return _fitz.Pixmap_setAlpha(self, alphavalues, premultiply)


    def _getImageData(self, format):
        return _fitz.Pixmap__getImageData(self, format)

    def getImageData(self, output="png"):
        """Convert to binary image stream of desired type.

        Can be used as input to GUI packages like tkinter.

        Args:
            output: (str) image type, default is PNG. Others are PNM, PGM, PPM,
                    PBM, PAM, PSD, PS.
        Returns:
            Bytes object.
        """
        valid_formats = {"png": 1, "pnm": 2, "pgm": 2, "ppm": 2, "pbm": 2,
                         "pam": 3, "tga": 4, "tpic": 4,
                         "psd": 5, "ps": 6}
        idx = valid_formats.get(output.lower(), 1)
        if self.alpha and idx in (2, 6):
            raise ValueError("'%s' cannot have alpha" % output)
        if self.colorspace and self.colorspace.n > 3 and idx in (1, 2, 4):
            raise ValueError("unsupported colorspace for '%s'" % output)
        barray = self._getImageData(idx)
        return barray

    def getPNGdata(self):
        """Wrapper for Pixmap.getImageData("png")."""
        barray = self._getImageData(1)
        return barray

    def getPNGData(self):
        """Wrapper for Pixmap.getImageData("png")."""
        barray = self._getImageData(1)
        return barray


    def _writeIMG(self, filename, format):
        return _fitz.Pixmap__writeIMG(self, filename, format)

    def writeImage(self, filename, output=None):
        """Output as image in format determined by filename extension.

        Args:
            output: (str) only use to override filename extension. Default is PNG.
                    Others are PNM, PGM, PPM, PBM, PAM, PSD, PS.
        Returns:
            Bytes object.
        """
        valid_formats = {"png": 1, "pnm": 2, "pgm": 2, "ppm": 2, "pbm": 2,
                         "pam": 3, "tga": 4, "tpic": 4,
                         "psd": 5, "ps": 6}
        if output is None:
            _, ext = os.path.splitext(filename)
            output = ext[1:]

        idx = valid_formats.get(output.lower(), 1)

        if self.alpha and idx in (2, 6):
            raise ValueError("'%s' cannot have alpha" % output)
        if self.colorspace and self.colorspace.n > 3 and idx in (1, 2, 4):
            raise ValueError("unsupported colorspace for '%s'" % output)

        return self._writeIMG(filename, idx)

    def writePNG(self, filename):
        """Wrapper for Pixmap.writeImage(filename, "png")."""
        return self._writeIMG(filename, 1)


    def pillowWrite(self, *args, **kwargs):
        """Write to image file using Pillow.

        Arguments are passed to Pillow's Image.save() method.
        Use instead of writeImage when other output formats are desired.
        """
        try:
            from PIL import Image
        except ImportError:
            print("PIL/Pillow not instralled")
            raise

        cspace = self.colorspace
        if cspace is None:
            mode = "L"
        elif cspace.n == 1:
            mode = "L" if self.alpha == 0 else "LA"
        elif cspace.n == 3:
            mode = "RGB" if self.alpha == 0 else "RGBA"
        else:
            mode = "CMYK"

        img = Image.frombytes(mode, (self.width, self.height), self.samples)

        if "dpi" not in kwargs.keys():
            kwargs["dpi"] = (self.xres, self.yres)

        img.save(*args, **kwargs)

    def pillowData(self, *args, **kwargs):
        """Convert to binary image stream using pillow.

        Arguments are passed to Pillow's Image.save() method.
        Use it instead of writeImage when other output formats are needed.
        """
        from io import BytesIO
        bytes_out = BytesIO()
        self.pillowWrite(bytes_out, *args, **kwargs)
        return bytes_out.getvalue()



    def invertIRect(self, bbox=None):
        """Invert the colors inside a bbox."""

        return _fitz.Pixmap_invertIRect(self, bbox)


    def pixel(self, x, y):
        """Get color tuple of pixel (x, y).
        Last item is the alpha if Pixmap.alpha is true."""

        return _fitz.Pixmap_pixel(self, x, y)


    def setPixel(self, x, y, color):
        """Set color of pixel (x, y)."""

        return _fitz.Pixmap_setPixel(self, x, y, color)


    def setOrigin(self, x, y):
        """Set top-left coordinates."""

        return _fitz.Pixmap_setOrigin(self, x, y)


    def setResolution(self, xres, yres):
        """Set resolution in both dimensions.

        Use pillowWrite to reflect this in output image."""

        return _fitz.Pixmap_setResolution(self, xres, yres)


    def setRect(self, bbox, color):
        """Set color of all pixels in bbox."""

        return _fitz.Pixmap_setRect(self, bbox, color)

    @property

    def stride(self):
        """Length of one image line (width * n)."""

        return _fitz.Pixmap_stride(self)

    @property

    def xres(self):
        """Resolution in x direction."""

        return _fitz.Pixmap_xres(self)

    @property

    def yres(self):
        """Resolution in y direction."""

        return _fitz.Pixmap_yres(self)

    @property

    def w(self):
        """The width."""

        return _fitz.Pixmap_w(self)

    @property

    def h(self):
        """The height."""

        return _fitz.Pixmap_h(self)

    @property

    def x(self):
        """x component of Pixmap origin."""

        return _fitz.Pixmap_x(self)

    @property

    def y(self):
        """y component of Pixmap origin."""

        return _fitz.Pixmap_y(self)

    @property

    def n(self):
        """The size of one pixel."""

        return _fitz.Pixmap_n(self)

    @property

    def alpha(self):
        """Indicates presence of alpha channel."""

        return _fitz.Pixmap_alpha(self)

    @property

    def colorspace(self):
        """Pixmap Colorspace."""

        return _fitz.Pixmap_colorspace(self)

    @property

    def irect(self):
        """Pixmap bbox - an IRect object."""

        val = _fitz.Pixmap_irect(self)
        val = IRect(val)

        return val

    @property

    def size(self):
        """Pixmap size."""

        return _fitz.Pixmap_size(self)

    @property

    def samples(self):
        """The area of all pixels."""

        return _fitz.Pixmap_samples(self)


    width  = w
    height = h

    def __len__(self):
        return self.size

    def __repr__(self):
        if not type(self) is Pixmap: return
        if self.colorspace:
            return "Pixmap(%s, %s, %s)" % (self.colorspace.name, self.irect, self.alpha)
        else:
            return "Pixmap(%s, %s, %s)" % ('None', self.irect, self.alpha)


class Colorspace:
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Colorspace, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Colorspace, name)
    __del__ = lambda self: None

    def __init__(self, type):
        """Supported are GRAY, RGB and CMYK."""

        this = _fitz.new_Colorspace(type)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
    @property

    def n(self):
        """Size of one pixel."""

        return _fitz.Colorspace_n(self)


    def _name(self):
        return _fitz.Colorspace__name(self)

    @property
    def name(self):
        """Name of the Colorspace."""

        if self.n == 1:
            return csGRAY._name()
        elif self.n == 3:
            return csRGB._name()
        elif self.n == 4:
            return csCMYK._name()
        return self._name()

    def __repr__(self):
        x = ("", "GRAY", "", "RGB", "CMYK")[self.n]
        return "Colorspace(CS_%s) - %s" % (x, self.name)

class Device:
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Device, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Device, name)

    def __init__(self, *args):
        this = _fitz.new_Device(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this

class Outline:
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Outline, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Outline, name)

    def __init__(self, *args, **kwargs):
        raise AttributeError("No constructor defined")
    @property

    def uri(self):
        return _fitz.Outline_uri(self)
    @property

    def next(self):
        return _fitz.Outline_next(self)
    @property

    def down(self):
        return _fitz.Outline_down(self)
    @property

    def isExternal(self):
        return _fitz.Outline_isExternal(self)
    @property

    def page(self):
        return _fitz.Outline_page(self)
    @property

    def x(self):
        return _fitz.Outline_x(self)
    @property

    def y(self):
        return _fitz.Outline_y(self)
    @property

    def title(self):
        return _fitz.Outline_title(self)
    @property

    def is_open(self):
        return _fitz.Outline_is_open(self)
    isOpen = is_open

    @property
    def dest(self):
        '''outline destination details'''
        return linkDest(self, None)


class Annot:
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Annot, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Annot, name)

    def __init__(self, *args, **kwargs):
        raise AttributeError("No constructor defined")
    @property

    def rect(self):
        """annotation rectangle"""
        CheckParent(self)

        val = _fitz.Annot_rect(self)

        val = Rect(val)
        val *= self.parent.derotationMatrix


        return val

    @property

    def xref(self):
        """annotation xref"""
        CheckParent(self)

        return _fitz.Annot_xref(self)

    @property

    def apn_matrix(self):
        """annotation appearance matrix"""
        CheckParent(self)

        val = _fitz.Annot_apn_matrix(self)
        val = Matrix(val)

        return val

    @property

    def apn_bbox(self):
        """annotation appearance bbox"""
        CheckParent(self)

        val = _fitz.Annot_apn_bbox(self)

        val = Rect(val) * self.parent.transformationMatrix
        val *= self.parent.derotationMatrix

        return val


    def set_apn_matrix(self, matrix):
        """Set annotation appearance matrix."""
        CheckParent(self)

        return _fitz.Annot_set_apn_matrix(self, matrix)


    def set_apn_bbox(self, bbox):

        """Set annotation appearance bbox."""

        CheckParent(self)
        page = self.parent
        rot = page.rotationMatrix
        mat = page.transformationMatrix
        bbox *= rot * ~mat


        return _fitz.Annot_set_apn_bbox(self, bbox)


    def blendMode(self):
        """annotation BlendMode"""
        CheckParent(self)

        return _fitz.Annot_blendMode(self)


    def set_blendmode(self, blend_mode):
        """Set annotation BlendMode."""
        CheckParent(self)

        return _fitz.Annot_set_blendmode(self, blend_mode)


    def get_oc(self):
        """Get annotation optional content reference."""
        CheckParent(self)

        return _fitz.Annot_get_oc(self)


    def set_open(self, is_open):
        """Set 'open' status of annotation or its Popup."""
        CheckParent(self)

        return _fitz.Annot_set_open(self, is_open)

    @property

    def is_open(self):
        """Get 'open' status of annotation or its Popup."""
        CheckParent(self)

        return _fitz.Annot_is_open(self)

    @property

    def has_popup(self):
        """Check if annotation has a Popup."""
        CheckParent(self)

        return _fitz.Annot_has_popup(self)

    @property

    def popup_rect(self):
        """annotation 'Popup' rectangle"""
        CheckParent(self)

        val = _fitz.Annot_popup_rect(self)

        val = Rect(val) * self.parent.transformationMatrix
        val *= self.parent.derotationMatrix

        return val

    @property

    def popup_xref(self):
        """annotation 'Popup' xref"""
        CheckParent(self)

        return _fitz.Annot_popup_xref(self)


    def set_oc(self, oc=0):
        """Set / remove annotation OC xref."""
        CheckParent(self)

        return _fitz.Annot_set_oc(self, oc)

    setOC = set_oc
    @property

    def language(self):
        """annotation language"""

        return _fitz.Annot_language(self)


    def set_language(self, language=None):
        """Set annotation language."""
        CheckParent(self)

        return _fitz.Annot_set_language(self, language)


    def _getAP(self):
        return _fitz.Annot__getAP(self)

    def _setAP(self, ap, rect=0):
        return _fitz.Annot__setAP(self, ap, rect)

    def _get_redact_values(self):
        val = _fitz.Annot__get_redact_values(self)

        if not val:
            return val
        val["rect"] = self.rect
        text_color, fontname, fontsize = TOOLS._parse_da(self)
        val["text_color"] = text_color
        val["fontname"] = fontname
        val["fontsize"] = fontsize
        fill = self.colors["fill"]
        val["fill"] = fill



        return val


    def get_textpage(self, clip=None, flags=0):
        """Make annotation TextPage."""
        CheckParent(self)

        return _fitz.Annot_get_textpage(self, clip, flags)


    def set_name(self, name):
        """Set /Name (icon) of annotation."""
        CheckParent(self)

        return _fitz.Annot_set_name(self, name)


    def set_rect(self, rect):
        """Set annotation rectangle."""
        CheckParent(self)

        return _fitz.Annot_set_rect(self, rect)


    def set_rotation(self, rotate=0):
        """Set annotation rotation."""
        CheckParent(self)

        return _fitz.Annot_set_rotation(self, rotate)

    @property

    def rotation(self):
        """annotation rotation"""
        CheckParent(self)

        return _fitz.Annot_rotation(self)

    @property

    def vertices(self):
        """annotation vertex points"""
        CheckParent(self)

        return _fitz.Annot_vertices(self)

    @property

    def colors(self):
        """Color definitions."""
        CheckParent(self)

        return _fitz.Annot_colors(self)


    def _update_appearance(self, opacity=-1, blend_mode=None, fill_color=None, rotate=-1):
        return _fitz.Annot__update_appearance(self, opacity, blend_mode, fill_color, rotate)

    def update(self,
               blend_mode: OptStr =None,
               opacity: OptFloat =None,
               fontsize: float =0,
               fontname: OptStr =None,
               text_color: OptSeq =None,
               border_color: OptSeq =None,
               fill_color: OptSeq =None,
               cross_out: bool =True,
               rotate: int =-1,
               ):

        """Update annot appearance.

        Notes:
            Depending on the annot type, some parameters make no sense,
            while others are only available in this method to achieve the
            desired result - especially for 'FreeText' annots.
        Args:
            blend_mode: set the blend mode, all annotations.
            opacity: set the opacity, all annotations.
            fontsize: set fontsize, 'FreeText' only.
            fontname: set the font, 'FreeText' only.
            border_color: set border color, 'FreeText' only.
            text_color: set text color, 'FreeText' only.
            fill_color: set fill color, all annotations.
            cross_out: draw diagonal lines, 'Redact' only.
            rotate: set rotation, 'FreeText' and some others.
        """
        CheckParent(self)
        def color_string(cs, code):
            """Return valid PDF color operator for a given color sequence.
            """
            if not cs:
                return b""
            if hasattr(cs, "__float__") or len(cs) == 1:
                app = " g\n" if code == "f" else " G\n"
            elif len(cs) == 3:
                app = " rg\n" if code == "f" else " RG\n"
            elif len(cs) == 4:
                app = " k\n" if code == "f" else " K\n"
            else:
                return b""

            if hasattr(cs, "__len__"):
                col = " ".join(map(str, cs)) + app
            else:
                col = "%g" % cs + app

            return col.encode()

        type = self.type[0]  # get the annot type
        dt = self.border["dashes"]  # get the dashes spec
        bwidth = self.border["width"]  # get border line width
        stroke = self.colors["stroke"]  # get the stroke color
        if fill_color != None:
            fill = fill_color
        else:
            fill = self.colors["fill"]

        rect = None  # self.rect  # prevent MuPDF fiddling with it
        apnmat = self.apn_matrix  # prevent MuPDF fiddling with it
        if rotate != -1:  # sanitize rotation value
            while rotate < 0:
                rotate += 360
            while rotate >= 360:
                rotate -= 360
            if type == PDF_ANNOT_FREE_TEXT and rotate % 90 != 0:
                rotate = 0

    #------------------------------------------------------------------
    # handle opacity and blend mode
    #------------------------------------------------------------------
        if blend_mode is None:
            blend_mode = self.blendmode
        if not hasattr(opacity, "__float__"):
            opacity = self.opacity

        if 0 <= opacity < 1 or blend_mode is not None:
            opa_code = "/H gs\n"  # then we must reference this 'gs'
        else:
            opa_code = ""

    #------------------------------------------------------------------
    # now invoke MuPDF to update the annot appearance
    #------------------------------------------------------------------
        val = self._update_appearance(
            opacity=opacity,
            blend_mode=blend_mode,
            fill_color=fill,
            rotate=rotate,
        )
        if not val:  # something went wrong, skip the rest
            return val

        bfill = color_string(fill, "f")
        bstroke = color_string(stroke, "s")

        p_ctm = self.parent.transformationMatrix
        imat = ~p_ctm  # inverse page transf. matrix

        if dt:
            dashes = "[" + " ".join(map(str, dt)) + "] 0 d\n"
            dashes = dashes.encode("utf-8")
        else:
            dashes = None

        if self.lineEnds:
            line_end_le, line_end_ri = self.lineEnds
        else:
            line_end_le, line_end_ri = 0, 0  # init line end codes

    # read contents as created by MuPDF
        ap = self._getAP()
        ap_tab = ap.splitlines()  # split in single lines
        ap_updated = False  # assume we did nothing

        if type == PDF_ANNOT_REDACT:
            if cross_out:  # create crossed-out rect
                ap_updated = True
                ap_tab = ap_tab[:-1]
                _, LL, LR, UR, UL = ap_tab
                ap_tab.append(LR)
                ap_tab.append(LL)
                ap_tab.append(UR)
                ap_tab.append(LL)
                ap_tab.append(UL)
                ap_tab.append(b"S")

            if bwidth > 0 or bstroke != b"":
                ap_updated = True
                ntab = [b"%g w" % bwidth] if bwidth > 0 else []
                for line in ap_tab:
                    if line.endswith(b"w"):
                        continue
                    if line.endswith(b"RG") and bstroke != b"":
                        line = bstroke[:-1]
                    ntab.append(line)
                ap_tab = ntab

            ap = b"\n".join(ap_tab)

        if type == PDF_ANNOT_FREE_TEXT:
            CheckColor(border_color)
            CheckColor(text_color)
            tcol, fname, fsize = TOOLS._parse_da(self)

    # read and update default appearance as necessary
            update_default_appearance = False
            if fsize <= 0:
                fsize = 12
                update_default_appearance = True
            if text_color is not None:
                tcol = text_color
                update_default_appearance = True
            if fontname is not None:
                fname = fontname
                update_default_appearance = True
            if fontsize > 0:
                fsize = fontsize
                update_default_appearance = True

            da_str = ""
            if len(tcol) == 3:
                fmt = "{:g} {:g} {:g} rg /{f:s} {s:g} Tf"
            elif len(tcol) == 1:
                fmt = "{:g} g /{f:s} {s:g} Tf"
            elif len(tcol) == 4:
                fmt = "{:g} {:g} {:g} {:g} k /{f:s} {s:g} Tf"
            da_str = fmt.format(*tcol, f=fname, s=fsize)
            TOOLS._update_da(self, da_str)

            for i, item in enumerate(ap_tab):
                if (
                    item.endswith(b" w") and bwidth > 0 and border_color is not None
                ):  # update border color
                    ap_tab[i + 1] = color_string(border_color, "s")
                    continue
                if item == b"BT":  # update text color
                    ap_tab[i + 1] = color_string(tcol, "f")
                    continue
                if not fill:
                    if item.endswith((b" re")) and ap_tab[i + 1] == b"f":
                        ap_tab[i + 1] = b"n"

            if dashes is not None:  # handle dashes
                ap_tab.insert(0, dashes)
                dashes = None

            ap = b"\n".join(ap_tab)         # updated AP stream
            ap_updated = True

        if type in (PDF_ANNOT_POLYGON, PDF_ANNOT_POLY_LINE):
            ap = b"\n".join(ap_tab[:-1]) + b"\n"
            ap_updated = True
            if bfill != b"":
                if type == PDF_ANNOT_POLYGON:
                    ap = ap + bfill + b"b"  # close, fill, and stroke
                elif type == PDF_ANNOT_POLY_LINE:
                    ap = ap + b"S"  # stroke
            else:
                if type == PDF_ANNOT_POLYGON:
                    ap = ap + b"s"  # close and stroke
                elif type == PDF_ANNOT_POLY_LINE:
                    ap = ap + b"S"  # stroke

        if dashes is not None:  # handle dashes
            ap = dashes + ap
    # reset dashing - only applies for LINE annots with line ends given
            ap = ap.replace(b"\nS\n", b"\nS\n[] 0 d\n", 1)
            ap_updated = True

        if opa_code:
            ap = opa_code.encode("utf-8") + ap
            ap_updated = True

        ap = b"q\n" + ap + b"\nQ\n"
    #----------------------------------------------------------------------
    # the following handles line end symbols for 'Polygon' and 'Polyline'
    #----------------------------------------------------------------------
        if line_end_le + line_end_ri > 0 and type in (PDF_ANNOT_POLYGON, PDF_ANNOT_POLY_LINE):

            le_funcs = (None, TOOLS._le_square, TOOLS._le_circle,
                        TOOLS._le_diamond, TOOLS._le_openarrow,
                        TOOLS._le_closedarrow, TOOLS._le_butt,
                        TOOLS._le_ropenarrow, TOOLS._le_rclosedarrow,
                        TOOLS._le_slash)
            le_funcs_range = range(1, len(le_funcs))
            d = 2 * max(1, self.border["width"])
            rect = self.rect + (-d, -d, d, d)
            ap_updated = True
            points = self.vertices
            if line_end_le in le_funcs_range:
                p1 = Point(points[0]) * imat
                p2 = Point(points[1]) * imat
                left = le_funcs[line_end_le](self, p1, p2, False, fill_color)
                ap += left.encode()
            if line_end_ri in le_funcs_range:
                p1 = Point(points[-2]) * imat
                p2 = Point(points[-1]) * imat
                left = le_funcs[line_end_ri](self, p1, p2, True, fill_color)
                ap += left.encode()

        if ap_updated:
            if rect:                        # rect modified here?
                self.set_rect(rect)
                self._setAP(ap, rect=1)
            else:
                self._setAP(ap, rect=0)

    #-------------------------------
    # handle annotation rotations
    #-------------------------------
        if type not in (  # only these types are supported
            PDF_ANNOT_CARET,
            PDF_ANNOT_CIRCLE,
            PDF_ANNOT_FILE_ATTACHMENT,
            PDF_ANNOT_INK,
            PDF_ANNOT_LINE,
            PDF_ANNOT_POLY_LINE,
            PDF_ANNOT_POLYGON,
            PDF_ANNOT_SQUARE,
            PDF_ANNOT_STAMP,
            PDF_ANNOT_TEXT,
            ):
            return

        rot = self.rotation  # get value from annot object
        if rot == -1:  # nothing to change
            return

        M = (self.rect.tl + self.rect.br) / 2  # center of annot rect

        if rot == 0:  # undo rotations
            if abs(apnmat - Matrix(1, 1)) < 1e-5:
                return  # matrix already is a no-op
            quad = self.rect.morph(M, ~apnmat)  # derotate rect
            self.setRect(quad.rect)
            self.set_apn_matrix(Matrix(1, 1))  # appearance matrix = no-op
            return

        mat = Matrix(rot)
        quad = self.rect.morph(M, mat)
        self.set_rect(quad.rect)
        self.set_apn_matrix(apnmat * mat)


    def set_colors(self, colors=None, fill=None, stroke=None):

        """Set 'stroke' and 'fill' colors.

        Use either a dict or the direct arguments.
        """
        CheckParent(self)
        if type(colors) is not dict:
            colors = {"fill": fill, "stroke": stroke}


        return _fitz.Annot_set_colors(self, colors, fill, stroke)

    @property

    def line_ends(self):
        """Line end codes."""
        CheckParent(self)

        return _fitz.Annot_line_ends(self)


    def set_line_ends(self, start, end):
        """Set line end codes."""
        CheckParent(self)

        return _fitz.Annot_set_line_ends(self, start, end)

    @property

    def type(self):
        """annotation type"""
        CheckParent(self)

        return _fitz.Annot_type(self)

    @property

    def opacity(self):
        """Opacity."""
        CheckParent(self)

        return _fitz.Annot_opacity(self)


    def set_opacity(self, opacity):
        """Set opacity."""
        CheckParent(self)

        return _fitz.Annot_set_opacity(self, opacity)


    def fileInfo(self):
        """Attached file information."""
        CheckParent(self)

        return _fitz.Annot_fileInfo(self)


    def get_file(self):
        """Retrieve attached file content."""
        CheckParent(self)

        return _fitz.Annot_get_file(self)


    def get_sound(self):
        """Retrieve sound stream."""
        CheckParent(self)

        return _fitz.Annot_get_sound(self)


    def update_file(self, buffer=None, filename=None, ufilename=None, desc=None):
        """Update attached file."""
        CheckParent(self)

        return _fitz.Annot_update_file(self, buffer, filename, ufilename, desc)

    @property

    def info(self):
        """Various information details."""
        CheckParent(self)

        return _fitz.Annot_info(self)


    def set_info(self, info=None, content=None, title=None, creationDate=None, modDate=None, subject=None):

        """Set various properties."""
        CheckParent(self)
        if type(info) is dict:  # build the args from the dictionary
            content = info.get("content", None)
            title = info.get("title", None)
            creationDate = info.get("creationDate", None)
            modDate = info.get("modDate", None)
            subject = info.get("subject", None)
            info = None


        return _fitz.Annot_set_info(self, info, content, title, creationDate, modDate, subject)

    @property

    def border(self):
        """Border information."""
        CheckParent(self)

        return _fitz.Annot_border(self)


    def set_border(self, border=None, width=0, style=None, dashes=None):

        """Set border properties.

        Either a dict, or direct arguments width, style and dashes."""
        CheckParent(self)
        if type(border) is not dict:
            border = {"width": width, "style": style, "dashes": dashes}


        return _fitz.Annot_set_border(self, border, width, style, dashes)

    @property

    def flags(self):
        """Flags field."""
        CheckParent(self)

        return _fitz.Annot_flags(self)


    def clean_contents(self, sanitize=1):
        """Clean appearance contents stream."""
        CheckParent(self)

        return _fitz.Annot_clean_contents(self, sanitize)


    def set_flags(self, flags):
        """Set annotation flags."""
        CheckParent(self)

        return _fitz.Annot_set_flags(self, flags)


    def delete_responses(self):
        """Delete 'Popup' and responding annotations."""
        CheckParent(self)

        return _fitz.Annot_delete_responses(self)

    @property

    def next(self):
        """Next annotation."""
        CheckParent(self)

        val = _fitz.Annot_next(self)

        if not val:
            return None
        val.thisown = True
        val.parent = self.parent  # copy owning page object from previous annot
        val.parent._annot_refs[id(val)] = val

        if val.type[0] == PDF_ANNOT_WIDGET:
            widget = Widget()
            TOOLS._fill_widget(val, widget)
            val = widget


        return val


    def get_pixmap(self, matrix=None, colorspace=None, alpha=0):
        """annotation Pixmap"""

        CheckParent(self)
        cspaces = {"gray": csGRAY, "rgb": csRGB, "cmyk": csCMYK}
        if type(colorspace) is str:
            colorspace = cspaces.get(colorspace.lower(), None)


        return _fitz.Annot_get_pixmap(self, matrix, colorspace, alpha)


    blendmode = property(blendMode, doc="annotation BlendMode")
    file_info = property(fileInfo, doc="Attached file information")
    fileGet = get_file
    fileUpd = update_file
    getPixmap = get_pixmap
    getTextPage = get_textpage
    lineEnds=line_ends
    setBlendMode = set_blendmode
    setBorder = set_border
    setColors = set_colors
    setFlags = set_flags
    setInfo = set_info
    setLineEnds = set_line_ends
    setName = set_name
    setOpacity = set_opacity
    setRect = set_rect
    setRotation = set_rotation
    soundGet = get_sound

    def _erase(self):
        try:
            self.parent._forget_annot(self)
        except:
            return
        if getattr(self, "thisown", False):
            self.thisown = False
        self.parent = None

    def __str__(self):
        CheckParent(self)
        return "'%s' annotation on %s" % (self.type[1], str(self.parent))

    def __repr__(self):
        CheckParent(self)
        return "'%s' annotation on %s" % (self.type[1], str(self.parent))

    def __del__(self):
        if self.parent is None:
            return
        self._erase()

class Link:
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Link, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Link, name)

    def __init__(self, *args, **kwargs):
        raise AttributeError("No constructor defined")

    def _border(self, doc, xref):
        return _fitz.Link__border(self, doc, xref)

    def _setBorder(self, border, doc, xref):
        return _fitz.Link__setBorder(self, border, doc, xref)

    def _colors(self, doc, xref):
        return _fitz.Link__colors(self, doc, xref)

    def _setColors(self, colors, doc, xref):
        return _fitz.Link__setColors(self, colors, doc, xref)

    @property
    def border(self):
        return self._border(self.parent.parent.this, self.xref)

    def setBorder(self, border=None, width=0, dashes=None, style=None):
        if type(border) is not dict:
            border = {"width": width, "style": style, "dashes": dashes}
        return self._setBorder(border, self.parent.parent.this, self.xref)

    @property
    def colors(self):
        return self._colors(self.parent.parent.this, self.xref)

    def setColors(self, colors=None, stroke=None, fill=None):
        if type(colors) is not dict:
            colors = {"fill": fill, "stroke": stroke}
        return self._setColors(colors, self.parent.parent.this, self.xref)

    @property

    def uri(self):
        """Uri string."""
        CheckParent(self)

        return _fitz.Link_uri(self)

    @property

    def isExternal(self):
        """External indicator."""
        CheckParent(self)

        return _fitz.Link_isExternal(self)


    page = -1
    @property
    def dest(self):
        """Create link destination details."""
        if hasattr(self, "parent") and self.parent is None:
            raise ValueError("orphaned object: parent is None")
        if self.parent.parent.isClosed or self.parent.parent.isEncrypted:
            raise ValueError("document closed or encrypted")
        doc = self.parent.parent

        if self.isExternal or self.uri.startswith("#"):
            uri = None
        else:
            uri = doc.resolveLink(self.uri)

        return linkDest(self, uri)

    @property

    def rect(self):
        """Rectangle ('hot area')."""
        CheckParent(self)

        val = _fitz.Link_rect(self)
        val = Rect(val)

        return val

    @property

    def next(self):
        """Next link."""
        CheckParent(self)

        val = _fitz.Link_next(self)

        if val:
            val.thisown = True
            val.parent = self.parent  # copy owning page from prev link
            val.parent._annot_refs[id(val)] = val
            if self.xref > 0:  # prev link has an xref
                link_xrefs = [x[0] for x in self.parent.annot_xrefs() if x[1] == PDF_ANNOT_LINK]
                link_ids = [x[2] for x in self.parent.annot_xrefs() if x[1] == PDF_ANNOT_LINK]
                idx = link_xrefs.index(self.xref)
                val.xref = link_xrefs[idx + 1]
                val.id = link_ids[idx + 1]
            else:
                val.xref = 0
                val.id = ""


        return val


    def _erase(self):
        try:
            self.parent._forget_annot(self)
        except:
            pass
        self.parent = None
        self.thisown = False

    def __str__(self):
        CheckParent(self)
        return "link on " + str(self.parent)

    def __repr__(self):
        CheckParent(self)
        return "link on " + str(self.parent)

    def __del__(self):
        self._erase()

class DisplayList:
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, DisplayList, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, DisplayList, name)

    def __init__(self, mediabox):
        this = _fitz.new_DisplayList(mediabox)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
        self.thisown = True



    def run(self, dw, m, area):
        return _fitz.DisplayList_run(self, dw, m, area)
    @property

    def rect(self):
        val = _fitz.DisplayList_rect(self)
        val = Rect(val)

        return val


    def getPixmap(self, matrix=None, colorspace=None, alpha=0, clip=None):
        val = _fitz.DisplayList_getPixmap(self, matrix, colorspace, alpha, clip)
        val.thisown = True

        return val


    def getTextPage(self, flags=3):
        val = _fitz.DisplayList_getTextPage(self, flags)
        val.thisown = True

        return val


    def __del__(self):
        if not type(self) is DisplayList: return
        self.thisown = False


class TextPage:
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TextPage, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TextPage, name)

    def __init__(self, mediabox):
        this = _fitz.new_TextPage(mediabox)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
        self.thisown = True



    def search(self, needle, hit_max=0, quads=1):
        """Locate 'needle' returning rects or quads."""

        val = _fitz.TextPage_search(self, needle, hit_max, quads)

        if not val:
            return val
        items = len(val)
        for i in range(items):  # change entries to quads or rects
            q = Quad(val[i])
            if quads:
                val[i] = q
            else:
                val[i] = q.rect
        if quads:
            return val
        i = 0  # join overlapping rects on the same line
        while i < items - 1:
            v1 = val[i]
            v2 = val[i + 1]
            if v1.y1 != v2.y1 or (v1 & v2).isEmpty:
                i += 1
                continue  # no overlap on same line
            val[i] = v1 | v2  # join rectangles
            del val[i + 1]  # remove v2
            items -= 1  # reduce item count


        return val


    def _getNewBlockList(self, page_dict, raw):
        return _fitz.TextPage__getNewBlockList(self, page_dict, raw)

    def _textpage_dict(self, raw=False):
        page_dict = {"width": self.rect.width, "height": self.rect.height}
        self._getNewBlockList(page_dict, raw)
        return page_dict


    def extractBLOCKS(self):
        """Return a list with text block information."""

        return _fitz.TextPage_extractBLOCKS(self)


    def extractWORDS(self):
        """Return a list with text word information."""

        return _fitz.TextPage_extractWORDS(self)

    @property

    def rect(self):
        """Page rectangle."""

        val = _fitz.TextPage_rect(self)
        val = Rect(val)

        return val


    def _extractText(self, format):
        return _fitz.TextPage__extractText(self, format)

    def extractSelection(self, pointa, pointb):
        return _fitz.TextPage_extractSelection(self, pointa, pointb)

    def extractText(self) -> str:
        """Return simple, bare text on the page."""
        return self._extractText(0)

    extractTEXT = extractText

    def extractHTML(self) -> str:
        """Return page content as a HTML string."""
        return self._extractText(1)

    def extractJSON(self) -> str:
        """Return 'extractDICT' converted to JSON format."""
        import base64, json
        val = self._textpage_dict(raw=False)

        class b64encode(json.JSONEncoder):
            def default(self, s):
                if type(s) in (bytes, bytearray):
                    return base64.b64encode(s).decode()

        val = json.dumps(val, separators=(",", ":"), cls=b64encode, indent=1)
        return val

    def extractRAWJSON(self) -> str:
        """Return 'extractRAWDICT' converted to JSON format."""
        import base64, json
        val = self._textpage_dict(raw=True)

        class b64encode(json.JSONEncoder):
            def default(self,s):
                if type(s) in (bytes, bytearray):
                    return base64.b64encode(s).decode()

        val = json.dumps(val, separators=(",", ":"), cls=b64encode, indent=1)
        return val

    def extractXML(self) -> str:
        """Return page content as a XML string."""
        return self._extractText(3)

    def extractXHTML(self) -> str:
        """Return page content as a XHTML string."""
        return self._extractText(4)

    def extractDICT(self) -> dict:
        """Return page content as a Python dict of images and text spans."""
        return self._textpage_dict(raw=False)

    def extractRAWDICT(self) -> dict:
        """Return page content as a Python dict of images and text characters."""
        return self._textpage_dict(raw=True)

    def __del__(self):
        if not type(self) is TextPage: return
        self.thisown = False


class Graftmap:
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Graftmap, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Graftmap, name)

    def __init__(self, doc):
        this = _fitz.new_Graftmap(doc)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this
        self.thisown = True



    def __del__(self):
        if not type(self) is Graftmap:
            return
        self.thisown = False


class TextWriter:
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TextWriter, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TextWriter, name)

    def __init__(self, page_rect, opacity=1, color=None):
        """Stores text spans for later output on compatible PDF pages."""

        this = _fitz.new_TextWriter(page_rect, opacity, color)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this

        self.opacity = opacity
        self.color = color
        self.rect = Rect(page_rect)
        self.ctm = Matrix(1, 0, 0, -1, 0, self.rect.height)
        self.ictm = ~self.ctm
        self.lastPoint = Point()
        self.lastPoint.__doc__ = "Position following last text insertion."
        self.textRect = Rect(0, 0, -1, -1)
        self.textRect.__doc__ = "Accumulated area of text spans."
        self.used_fonts = set()




    def append(self, pos, text, font=None, fontsize=11, language=None):

        """Store 'text' at point 'pos' using 'font' and 'fontsize'."""

        pos = Point(pos) * self.ictm
        if font is None:
            font = Font("helv")
        if not font.isWritable:
            raise ValueError("Unsupported font '%s'." % font.name)

        val = _fitz.TextWriter_append(self, pos, text, font, fontsize, language)

        self.lastPoint = Point(val[-2:]) * self.ctm
        self.textRect = self._bbox * self.ctm
        val = self.textRect, self.lastPoint
        if font.flags["mono"] == 1:
            self.used_fonts.add(font)


        return val


    def appendv(self, pos, text, font=None, fontsize=11,
        language=None):
        lheight = fontsize * 1.2
        for c in text:
            self.append(pos, c, font=font, fontsize=fontsize,
                language=language)
            pos.y += lheight
        return self.textRect, self.lastPoint

    @property

    def _bbox(self):
        val = _fitz.TextWriter__bbox(self)
        val = Rect(val)

        return val


    def writeText(self, page, color=None, opacity=-1, overlay=1, morph=None, render_mode=0, oc=0):

        """Write the text to a PDF page having the TextWriter's page size.

        Args:
            page: a PDF page having same size.
            color: override text color.
            opacity: override transparency.
            overlay: put in foreground or background.
            morph: tuple(Point, Matrix), apply Matrix with fixpoint Point.
            render_mode: (int) PDF render mode operator 'Tr'.
        """

        CheckParent(page)
        if abs(self.rect - page.rect) > 1e-3:
            raise ValueError("incompatible page rect")
        if morph != None:
            if (type(morph) not in (tuple, list)
                or type(morph[0]) is not Point
                or type(morph[1]) is not Matrix
                ):
                raise ValueError("morph must be (Point, Matrix) or None")
        if getattr(opacity, "__float__", None) is None or opacity == -1:
            opacity = self.opacity
        if color is None:
            color = self.color


        val = _fitz.TextWriter_writeText(self, page, color, opacity, overlay, morph, render_mode, oc)

        max_nums = val[0]
        content = val[1]
        max_alp, max_font = max_nums
        old_cont_lines = content.splitlines()

        optcont = page._get_optional_content(oc)
        if optcont != None:
            bdc = "/OC /%s BDC" % optcont
            emc = "EMC"
        else:
            bdc = emc = ""

        new_cont_lines = ["q"]
        if bdc:
            new_cont_lines.append(bdc)

        cb = page.CropBoxPosition
        if bool(cb):
            new_cont_lines.append("1 0 0 1 %g %g cm" % (cb.x, cb.y))

        if morph:
            p = morph[0] * self.ictm
            delta = Matrix(1, 1).preTranslate(p.x, p.y)
            matrix = ~delta * morph[1] * delta
            new_cont_lines.append("%g %g %g %g %g %g cm" % JM_TUPLE(matrix))

        for line in old_cont_lines:
            if line.endswith(" cm"):
                continue
            if line == "BT":
                new_cont_lines.append(line)
                new_cont_lines.append("%i Tr" % render_mode)
                continue
            if line.endswith(" gs"):
                alp = int(line.split()[0][4:]) + max_alp
                line = "/Alp%i gs" % alp
            elif line.endswith(" Tf"):
                temp = line.split()
                font = int(temp[0][2:]) + max_font
                line = " ".join(["/F%i" % font] + temp[1:])
            elif line.endswith(" rg"):
                new_cont_lines.append(line.replace("rg", "RG"))
            elif line.endswith(" g"):
                new_cont_lines.append(line.replace(" g", " G"))
            elif line.endswith(" k"):
                new_cont_lines.append(line.replace(" k", " K"))
            new_cont_lines.append(line)
        if emc:
            new_cont_lines.append(emc)
        new_cont_lines.append("Q\n")
        content = "\n".join(new_cont_lines).encode("utf-8")
        TOOLS._insert_contents(page, content, overlay=overlay)
        val = None
        for font in self.used_fonts:
            repair_mono_font(page, font)


        return val


    def __del__(self):
        if not type(self) is TextWriter:
            return


class Font:
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Font, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Font, name)

    def __init__(self, fontname=None, fontfile=None, fontbuffer=None, script=0, language=None, ordering=-1, is_bold=0, is_italic=0, is_serif=0):

        if fontname:
            if "/" in fontname or "\\" in fontname or "." in fontname:
                print("Warning: did you mean a fontfile?")

            if fontname.lower() in ("china-t", "china-s", "japan", "korea","china-ts", "china-ss", "japan-s", "korea-s", "cjk"):
                ordering = 0

            elif fontname.lower() in fitz_fontdescriptors.keys():
                import pymupdf_fonts  # optional fonts
                fontbuffer = pymupdf_fonts.myfont(fontname)  # make a copy
                fontname = None  # ensure using fontbuffer only
                del pymupdf_fonts  # remove package again

            elif ordering < 0:
                fontname = Base14_fontdict.get(fontname.lower(), fontname)


        this = _fitz.new_Font(fontname, fontfile, fontbuffer, script, language, ordering, is_bold, is_italic, is_serif)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this

    def glyph_advance(self, chr, language=None, script=0, wmode=0):
        """Return the glyph width of a unicode (font size 1)."""

        return _fitz.Font_glyph_advance(self, chr, language, script, wmode)


    def glyph_bbox(self, chr, language=None, script=0):
        """Return the glyph bbox of a unicode (font size 1)."""

        val = _fitz.Font_glyph_bbox(self, chr, language, script)
        val = Rect(val)

        return val


    def has_glyph(self, chr, language=None, script=0, fallback=0):
        """Check whether font has a glyph for this unicode."""

        return _fitz.Font_has_glyph(self, chr, language, script, fallback)


    def valid_codepoints(self):
        from array import array
        gc = self.glyph_count
        cp = array("l", (0,) * gc)
        arr = cp.buffer_info()
        self._valid_unicodes(arr)
        return array("l", sorted(set(cp))[1:])


    def _valid_unicodes(self, arr):
        return _fitz.Font__valid_unicodes(self, arr)
    @property

    def flags(self):
        return _fitz.Font_flags(self)
    @property

    def isWritable(self):
        return _fitz.Font_isWritable(self)
    @property

    def name(self):
        return _fitz.Font_name(self)
    @property

    def glyph_count(self):
        return _fitz.Font_glyph_count(self)
    @property

    def buffer(self):
        return _fitz.Font_buffer(self)
    @property

    def bbox(self):
        val = _fitz.Font_bbox(self)
        val = Rect(val)

        return val

    @property

    def ascender(self):
        """Return the glyph ascender value."""

        return _fitz.Font_ascender(self)

    @property

    def descender(self):
        """Return the glyph descender value."""

        return _fitz.Font_descender(self)


    def glyph_name_to_unicode(self, name):
        """Return the unicode for a glyph name."""
        return glyph_name_to_unicode(name)

    def unicode_to_glyph_name(self, ch):
        """Return the glyph name for a unicode."""
        return unicode_to_glyph_name(ch)

    def text_length(self, text, fontsize=11, wmode=0):
        """Calculate the length of a string for this font."""
        return fontsize * sum([self.glyph_advance(ord(c), wmode=wmode) for c in text])

    def __repr__(self):
        return "Font('%s')" % self.name

    def __del__(self):
        if type(self) is not Font:
            return None

class _unused_Tools:
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Tools, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Tools, name)

    def gen_id(self):
        """Return a unique positive integer."""

        return _fitz.Tools_gen_id(self)


    def set_icc(self, on=0):
        """Set ICC color handling on or off."""

        return _fitz.Tools_set_icc(self, on)


    def set_annot_stem(self, stem=None):
        """Get / set id prefix for annotations."""

        return _fitz.Tools_set_annot_stem(self, stem)


    def set_small_glyph_heights(self, on=None):
        """Set / unset small glyph heights."""

        return _fitz.Tools_set_small_glyph_heights(self, on)


    def store_shrink(self, percent):
        """Free 'percent' of current store size."""

        return _fitz.Tools_store_shrink(self, percent)

    @property

    def store_size(self):
        """MuPDF current store size."""

        return _fitz.Tools_store_size(self)

    @property

    def store_maxsize(self):
        """MuPDF store size limit."""

        return _fitz.Tools_store_maxsize(self)


    def show_aa_level(self):
        """Show anti-aliasing values."""

        val = _fitz.Tools_show_aa_level(self)

        temp = {"graphics": val[0], "text": val[1], "graphics_min_line_width": val[2]}
        val = temp

        return val


    def set_aa_level(self, level):
        """Set anti-aliasing level."""

        return _fitz.Tools_set_aa_level(self, level)


    def set_graphics_min_line_width(self, min_line_width):
        """Set the graphics minimum line width."""

        return _fitz.Tools_set_graphics_min_line_width(self, min_line_width)


    def image_profile(self, stream, keep_image=0):
        """Metadata of an image binary stream."""

        return _fitz.Tools_image_profile(self, stream, keep_image)


    def _rotate_matrix(self, page):
        return _fitz.Tools__rotate_matrix(self, page)

    def _derotate_matrix(self, page):
        return _fitz.Tools__derotate_matrix(self, page)
    @property

    def fitz_config(self):
        """PyMuPDF configuration parameters."""

        return _fitz.Tools_fitz_config(self)


    def glyph_cache_empty(self):
        """Empty the glyph cache."""

        return _fitz.Tools_glyph_cache_empty(self)


    def _fill_widget(self, annot, widget):
        val = _fitz.Tools__fill_widget(self, annot, widget)

        widget.rect = Rect(annot.rect)
        widget.xref = annot.xref
        widget.parent = annot.parent
        widget._annot = annot  # backpointer to annot object
        if not widget.script:
            widget.script = None
        if not widget.script_stroke:
            widget.script_stroke = None
        if not widget.script_format:
            widget.script_format = None
        if not widget.script_change:
            widget.script_change = None
        if not widget.script_calc:
            widget.script_calc = None


        return val


    def _save_widget(self, annot, widget):
        return _fitz.Tools__save_widget(self, annot, widget)

    def _reset_widget(self, annot):
        return _fitz.Tools__reset_widget(self, annot)

    def _parse_da(self, annot):
        val = _fitz.Tools__parse_da(self, annot)

        if not val:
            return ((0,), "", 0)
        font = "Helv"
        fsize = 12
        col = (0, 0, 0)
        dat = val.split()  # split on any whitespace
        for i, item in enumerate(dat):
            if item == "Tf":
                font = dat[i - 2][1:]
                fsize = float(dat[i - 1])
                dat[i] = dat[i-1] = dat[i-2] = ""
                continue
            if item == "g":            # unicolor text
                col = [(float(dat[i - 1]))]
                dat[i] = dat[i-1] = ""
                continue
            if item == "rg":           # RGB colored text
                col = [float(f) for f in dat[i - 3:i]]
                dat[i] = dat[i-1] = dat[i-2] = dat[i-3] = ""
                continue
            if item == "k":           # CMYK colored text
                col = [float(f) for f in dat[i - 4:i]]
                dat[i] = dat[i-1] = dat[i-2] = dat[i-3] = dat[i-4] = ""
                continue

        val = (col, font, fsize)


        return val


    def _update_da(self, annot, da_str):
        return _fitz.Tools__update_da(self, annot, da_str)

    def _get_all_contents(self, fzpage):
        """Concatenate all /Contents objects of a page into a bytes object."""

        return _fitz.Tools__get_all_contents(self, fzpage)


    def _insert_contents(self, page, newcont, overlay=1):
        """Add bytes as a new /Contents object for a page, and return its xref."""

        return _fitz.Tools__insert_contents(self, page, newcont, overlay)


    def mupdf_version(self):
        """Get version of MuPDF binary build."""

        return _fitz.Tools_mupdf_version(self)


    def mupdf_warnings(self, reset=1):
        """Get the MuPDF warnings/errors with optional reset (default)."""

        val = _fitz.Tools_mupdf_warnings(self, reset)

        val = "\n".join(val)
        if reset:
            self.reset_mupdf_warnings()

        return val


    def _int_from_language(self, language):
        return _fitz.Tools__int_from_language(self, language)

    def reset_mupdf_warnings(self):
        """Empty the MuPDF warnings/errors store."""

        return _fitz.Tools_reset_mupdf_warnings(self)


    def mupdf_display_errors(self, value=None):
        """Set MuPDF error display to True or False."""

        return _fitz.Tools_mupdf_display_errors(self, value)


    def _transform_rect(self, rect, matrix):
        return _fitz.Tools__transform_rect(self, rect, matrix)

    def _intersect_rect(self, r1, r2):
        return _fitz.Tools__intersect_rect(self, r1, r2)

    def _include_point_in_rect(self, r, p):
        return _fitz.Tools__include_point_in_rect(self, r, p)

    def _transform_point(self, point, matrix):
        return _fitz.Tools__transform_point(self, point, matrix)

    def _union_rect(self, r1, r2):
        return _fitz.Tools__union_rect(self, r1, r2)

    def _concat_matrix(self, m1, m2):
        return _fitz.Tools__concat_matrix(self, m1, m2)

    def _invert_matrix(self, matrix):
        return _fitz.Tools__invert_matrix(self, matrix)

    def _measure_string(self, text, fontname, fontsize, encoding=0):
        return _fitz.Tools__measure_string(self, text, fontname, fontsize, encoding)

    def _sine_between(self, C, P, Q):
        return _fitz.Tools__sine_between(self, C, P, Q)

    def _hor_matrix(self, C, P):
        return _fitz.Tools__hor_matrix(self, C, P)

    def set_font_width(self, doc, xref, width):
        return _fitz.Tools_set_font_width(self, doc, xref, width)

    def _le_annot_parms(self, annot, p1, p2, fill_color):
        """Get common parameters for making annot line end symbols.

        Returns:
            m: matrix that maps p1, p2 to points L, P on the x-axis
            im: its inverse
            L, P: transformed p1, p2
            w: line width
            scol: stroke color string
            fcol: fill color store_shrink
            opacity: opacity string (gs command)
        """
        w = annot.border["width"]  # line width
        sc = annot.colors["stroke"]  # stroke color
        if not sc:  # black if missing
            sc = (0,0,0)
        scol = " ".join(map(str, sc)) + " RG\n"
        if fill_color:
            fc = fill_color
        else:
            fc = annot.colors["fill"]  # fill color
        if not fc:
            fc = (1,1,1)  # white if missing
        fcol = " ".join(map(str, fc)) + " rg\n"
    # nr = annot.rect
        np1 = p1                   # point coord relative to annot rect
        np2 = p2                   # point coord relative to annot rect
        m = Matrix(self._hor_matrix(np1, np2))  # matrix makes the line horizontal
        im = ~m                            # inverted matrix
        L = np1 * m                        # converted start (left) point
        R = np2 * m                        # converted end (right) point
        if 0 <= annot.opacity < 1:
            opacity = "/H gs\n"
        else:
            opacity = ""
        return m, im, L, R, w, scol, fcol, opacity

    def _oval_string(self, p1, p2, p3, p4):
        """Return /AP string defining an oval within a 4-polygon provided as points
        """
        def bezier(p, q, r):
            f = "%f %f %f %f %f %f c\n"
            return f % (p.x, p.y, q.x, q.y, r.x, r.y)

        kappa = 0.55228474983              # magic number
        ml = p1 + (p4 - p1) * 0.5          # middle points ...
        mo = p1 + (p2 - p1) * 0.5          # for each ...
        mr = p2 + (p3 - p2) * 0.5          # polygon ...
        mu = p4 + (p3 - p4) * 0.5          # side
        ol1 = ml + (p1 - ml) * kappa       # the 8 bezier
        ol2 = mo + (p1 - mo) * kappa       # helper points
        or1 = mo + (p2 - mo) * kappa
        or2 = mr + (p2 - mr) * kappa
        ur1 = mr + (p3 - mr) * kappa
        ur2 = mu + (p3 - mu) * kappa
        ul1 = mu + (p4 - mu) * kappa
        ul2 = ml + (p4 - ml) * kappa
    # now draw, starting from middle point of left side
        ap = "%f %f m\n" % (ml.x, ml.y)
        ap += bezier(ol1, ol2, mo)
        ap += bezier(or1, or2, mr)
        ap += bezier(ur1, ur2, mu)
        ap += bezier(ul1, ul2, ml)
        return ap

    def _le_diamond(self, annot, p1, p2, lr, fill_color):
        """Make stream commands for diamond line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = self._le_annot_parms(annot, p1, p2, fill_color)
        shift = 2.5             # 2*shift*width = length of square edge
        d = shift * max(1, w)
        M = R - (d/2., 0) if lr else L + (d/2., 0)
        r = Rect(M, M) + (-d, -d, d, d)         # the square
    # the square makes line longer by (2*shift - 1)*width
        p = (r.tl + (r.bl - r.tl) * 0.5) * im
        ap = "q\n%s%f %f m\n" % (opacity, p.x, p.y)
        p = (r.tl + (r.tr - r.tl) * 0.5) * im
        ap += "%f %f l\n"   % (p.x, p.y)
        p = (r.tr + (r.br - r.tr) * 0.5) * im
        ap += "%f %f l\n"   % (p.x, p.y)
        p = (r.br + (r.bl - r.br) * 0.5) * im
        ap += "%f %f l\n"   % (p.x, p.y)
        ap += "%g w\n" % w
        ap += scol + fcol + "b\nQ\n"
        return ap

    def _le_square(self, annot, p1, p2, lr, fill_color):
        """Make stream commands for square line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = self._le_annot_parms(annot, p1, p2, fill_color)
        shift = 2.5             # 2*shift*width = length of square edge
        d = shift * max(1, w)
        M = R - (d/2., 0) if lr else L + (d/2., 0)
        r = Rect(M, M) + (-d, -d, d, d)         # the square
    # the square makes line longer by (2*shift - 1)*width
        p = r.tl * im
        ap = "q\n%s%f %f m\n" % (opacity, p.x, p.y)
        p = r.tr * im
        ap += "%f %f l\n"   % (p.x, p.y)
        p = r.br * im
        ap += "%f %f l\n"   % (p.x, p.y)
        p = r.bl * im
        ap += "%f %f l\n"   % (p.x, p.y)
        ap += "%g w\n" % w
        ap += scol + fcol + "b\nQ\n"
        return ap

    def _le_circle(self, annot, p1, p2, lr, fill_color):
        """Make stream commands for circle line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = self._le_annot_parms(annot, p1, p2, fill_color)
        shift = 2.5             # 2*shift*width = length of square edge
        d = shift * max(1, w)
        M = R - (d/2., 0) if lr else L + (d/2., 0)
        r = Rect(M, M) + (-d, -d, d, d)         # the square
        ap = "q\n" + opacity + self._oval_string(r.tl * im, r.tr * im, r.br * im, r.bl * im)
        ap += "%g w\n" % w
        ap += scol + fcol + "b\nQ\n"
        return ap

    def _le_butt(self, annot, p1, p2, lr, fill_color):
        """Make stream commands for butt line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = self._le_annot_parms(annot, p1, p2, fill_color)
        shift = 3
        d = shift * max(1, w)
        M = R if lr else L
        top = (M + (0, -d/2.)) * im
        bot = (M + (0, d/2.)) * im
        ap = "\nq\n%s%f %f m\n" % (opacity, top.x, top.y)
        ap += "%f %f l\n" % (bot.x, bot.y)
        ap += "%g w\n" % w
        ap += scol + "s\nQ\n"
        return ap

    def _le_slash(self, annot, p1, p2, lr, fill_color):
        """Make stream commands for slash line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = self._le_annot_parms(annot, p1, p2, fill_color)
        rw = 1.1547 * max(1, w) * 1.0         # makes rect diagonal a 30 deg inclination
        M = R if lr else L
        r = Rect(M.x - rw, M.y - 2 * w, M.x + rw, M.y + 2 * w)
        top = r.tl * im
        bot = r.br * im
        ap = "\nq\n%s%f %f m\n" % (opacity, top.x, top.y)
        ap += "%f %f l\n" % (bot.x, bot.y)
        ap += "%g w\n" % w
        ap += scol + "s\nQ\n"
        return ap

    def _le_openarrow(self, annot, p1, p2, lr, fill_color):
        """Make stream commands for open arrow line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = self._le_annot_parms(annot, p1, p2, fill_color)
        shift = 2.5
        d = shift * max(1, w)
        p2 = R + (d/2., 0) if lr else L - (d/2., 0)
        p1 = p2 + (-2*d, -d) if lr else p2 + (2*d, -d)
        p3 = p2 + (-2*d, d) if lr else p2 + (2*d, d)
        p1 *= im
        p2 *= im
        p3 *= im
        ap = "\nq\n%s%f %f m\n" % (opacity, p1.x, p1.y)
        ap += "%f %f l\n" % (p2.x, p2.y)
        ap += "%f %f l\n" % (p3.x, p3.y)
        ap += "%g w\n" % w
        ap += scol + "S\nQ\n"
        return ap

    def _le_closedarrow(self, annot, p1, p2, lr, fill_color):
        """Make stream commands for closed arrow line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = self._le_annot_parms(annot, p1, p2, fill_color)
        shift = 2.5
        d = shift * max(1, w)
        p2 = R + (d/2., 0) if lr else L - (d/2., 0)
        p1 = p2 + (-2*d, -d) if lr else p2 + (2*d, -d)
        p3 = p2 + (-2*d, d) if lr else p2 + (2*d, d)
        p1 *= im
        p2 *= im
        p3 *= im
        ap = "\nq\n%s%f %f m\n" % (opacity, p1.x, p1.y)
        ap += "%f %f l\n" % (p2.x, p2.y)
        ap += "%f %f l\n" % (p3.x, p3.y)
        ap += "%g w\n" % w
        ap += scol + fcol + "b\nQ\n"
        return ap

    def _le_ropenarrow(self, annot, p1, p2, lr, fill_color):
        """Make stream commands for right open arrow line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = self._le_annot_parms(annot, p1, p2, fill_color)
        shift = 2.5
        d = shift * max(1, w)
        p2 = R - (d/3., 0) if lr else L + (d/3., 0)
        p1 = p2 + (2*d, -d) if lr else p2 + (-2*d, -d)
        p3 = p2 + (2*d, d) if lr else p2 + (-2*d, d)
        p1 *= im
        p2 *= im
        p3 *= im
        ap = "\nq\n%s%f %f m\n" % (opacity, p1.x, p1.y)
        ap += "%f %f l\n" % (p2.x, p2.y)
        ap += "%f %f l\n" % (p3.x, p3.y)
        ap += "%g w\n" % w
        ap += scol + fcol + "S\nQ\n"
        return ap

    def _le_rclosedarrow(self, annot, p1, p2, lr, fill_color):
        """Make stream commands for right closed arrow line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = self._le_annot_parms(annot, p1, p2, fill_color)
        shift = 2.5
        d = shift * max(1, w)
        p2 = R - (2*d, 0) if lr else L + (2*d, 0)
        p1 = p2 + (2*d, -d) if lr else p2 + (-2*d, -d)
        p3 = p2 + (2*d, d) if lr else p2 + (-2*d, d)
        p1 *= im
        p2 *= im
        p3 *= im
        ap = "\nq\n%s%f %f m\n" % (opacity, p1.x, p1.y)
        ap += "%f %f l\n" % (p2.x, p2.y)
        ap += "%f %f l\n" % (p3.x, p3.y)
        ap += "%g w\n" % w
        ap += scol + fcol + "b\nQ\n"
        return ap


    def __init__(self):
        this = _fitz.new_Tools()
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this















if __name__ == '__main__':
    pass
