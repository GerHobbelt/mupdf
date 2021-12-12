# Import Auto-generated MuPDF python bindings
#

import base64
import binascii
import gzip
import hashlib
import inspect
import math
import sys
import textwrap
import typing
import weakref

sys.path.append('scripts')
import jlib

import mupdf

# Names required by class method typing annotations.
OptBytes = typing.Optional[typing.ByteString]
OptDict = typing.Optional[dict]
OptFloat = typing.Optional[float]
OptInt = typing.Union[int, None]
OptSeq = typing.Optional[typing.Sequence]
OptStr = typing.Optional[str]

Page = 'Page_forward_decl'
Point = 'Point_forward_decl'

matrix_like = "matrix_like"
point_like = "point_like"
quad_like = "quad_like"
rect_like = "rect_like"


# Classes
#

class Annot:

    #def __del__(self):
    #    if self.parent is None:
    #        return
    #    self._erase()

    def __init__(self, annot):
        #jlib.log('{annot=} {annot.m_internal=} {annot.annot_refs()=}')
        #assert isinstance(annot, mupdf.PdfAnnot), f'type(annot)={type(annot)}'
        #assert isinstance(page, Page), f'page is: {page}'
        self.this = annot
        #self.parent = page

    def __repr__(self):
        #CheckParent(self)
        parent = getattr(self, 'parent', '<>')
        return "'%s' annotation on %s" % (self.type[1], str(parent))

    def __str__(self):
        return self.__repr__()

    def _erase(self):
        try:
            self.parent._forget_annot(self)
        except:
            return
        if getattr(self, "thisown", False):
            self.thisown = False
        #self.parent = None

    def _get_redact_values(self):
        #jlib.log('')
        #val = _fitz.Annot__get_redact_values(self)
        annot = self.this
        if mupdf.mpdf_annot_type(annot) != mupdf.PDF_ANNOT_REDACT:
            return

        values = dict()
        #pdf_obj *obj = NULL;
        #const char *text = NULL;
        try:
            obj = mupdf.mpdf_dict_gets(annot.annot_obj(), "RO")
            if obj.m_internal:
                JM_Warning("Ignoring redaction key '/RO'.")
                xref = mupdf.mpdf_to_num(obj)
                values[dictkey_xref] = xref
            obj = mupdf.mpdf_dict_gets(annot.annot_obj(), "OverlayText")
            if obj.m_internal:
                text = mupdf.mpdf_to_text_string(obj)
                values[dictkey_text] = JM_UnicodeFromStr(text)
            else:
                values[dictkey_text] = ''
            obj = mupdf.mpdf_dict_get(annot.annot_obj(), PDF_NAME('Q'))
            align = 0;
            if obj.m_internal:
                align = mupdf.mpdf_to_int(obj)
            values[dictkey_align] = align
        except Exception as e:
            #jlib.log('{e=}')
            return
        val = values

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

    def _getAP(self):
        #return _fitz.Annot__getAP(self)
        r = None
        res = None
        annot = self.this
        ap = annot.annot_obj().dict_getl(
                mupdf.PDF_ENUM_NAME_AP,
                mupdf.PDF_ENUM_NAME_N,
                )

        if ap.is_stream():
            res = ap.load_stream()
        if not res or res.m_internal:
            r = JM_BinFromBuffer(res)
        return r

    def _setAP(self, ap, rect=0):
        #return _fitz.Annot__setAP(self, ap, rect)
        try:
            annot = self.this
            apobj = mupdf.mpdf_dict_getl(annot.obj(), PDF_NAME('AP'), PDF_NAME('N'))
            if not apobj.m_internal:
                THROWMSG("annot has no AP/N object")
            if not mupdf.mpdf_is_stream(apobj):
                THROWMSG("AP/N object is no stream")
            res = JM_BufferFromBytes(ap)
            if not res.m_internal:
                THROWMSG("invalid /AP stream argument")
            JM_update_stream(annot.page().doc(), apobj, res, 1)
            if rect:
                bbox = mupdf.mpdf_dict_get_rect(annot.obj(), PDF_NAME('Rect'))
                mupdf.mpdf_dict_put_rect(apobj, PDF_NAME('BBox'), bbox)
                #annot->ap = NULL;
        except Exception:
            return
        return

    def _update_appearance(self, opacity=-1, blend_mode=None, fill_color=None, rotate=-1):
        #return _fitz.Annot__update_appearance(self, opacity, blend_mode, fill_color, rotate)
        annot = self.this
        assert annot.m_internal
        type_ = annot.annot_type()
        fcol = [1, 1, 1, 1] # std fill color: white
        nfcol = 0   # number of color components
        nfcol = JM_color_FromSequence(fill_color, fcol);

        annot.dirty_annot() # enforce MuPDF /AP formatting
        if type_ == mupdf.PDF_ANNOT_FREE_TEXT:
            if fill_color:
                # Use mupdf python custom fn. fixme: make this available as a
                # class method and as mpdf_set_annot_color().
                mupdf.mpdf_set_annot_color(annot, fcol[:nfcol])
            else:
                annot.annot_obj().dict_del(mupdf.PDF_ENUM_NAME_IC)
        else:
            if fill_color:
                annot.set_annot_interior_color(fcol[:nfcol])
            elif fill_color is not None:
                annot.annot_obj().dict_del(mupdf.PDF_ENUM_NAME_IC)

        insert_rot = 1 if rotate >= 0 else 0
        if type not in (
                mupdf.PDF_ANNOT_CARET,
                mupdf.PDF_ANNOT_CIRCLE,
                mupdf.PDF_ANNOT_FREE_TEXT,
                mupdf.PDF_ANNOT_FILE_ATTACHMENT,
                mupdf.PDF_ANNOT_INK,
                mupdf.PDF_ANNOT_LINE,
                mupdf.PDF_ANNOT_POLY_LINE,
                mupdf.PDF_ANNOT_POLYGON,
                mupdf.PDF_ANNOT_SQUARE,
                mupdf.PDF_ANNOT_STAMP,
                mupdf.PDF_ANNOT_TEXT,
                ):
            insert_rot = 0

        if insert_rot:
            annot.annot_obj().dict_put_int(mupdf.PDF_ENUM_NAME_Rotate, rotate)
        self.needs_new_ap = 1   # re-create appearance stream
        annot.update_annot()    # update the annotation

        if (opacity < 0 or opacity >= 1) and not blend_mode:    # no opacity, no blend_mode
            return True

        try:    # create or update /ExtGState
            ap = mupdf.pdf_dict_getl(
                    annot.annot.obj(),
                    mupdf.PDF_ENUM_NAME_AP,
                    mupdf.PDF_ENUM_NAME_N
                    )
            if not ap.m_internal:   # should never happen
                raise Exception("annot has no /AP object")

            resources = ap.dict_get(mupdf.PDF_ENUM_NAME_Resources)

            if not resources.m_internal:    # no Resources yet: make one
                resources = ap.dict_put_dict(mupdf.PDF_ENUM_NAME_Resources, 2)
            alp0 = annot.page().doc().new_dict(3)
            if opacity >= 0 and opacity < 1:
                alp0.dict_put_real(mupdf.PDF_ENUM_NAME_CA, opacity)
                alp0.dict_put_real(mupdf.PDF_ENUM_NAME_ca, opacity)
                annot.annot_obj().dict_put_real(mupdf.PDF_ENUM_NAME_CA, opacity)

            if blend_mode:
                alp0.dict_put_name(mupdf.PDF_ENUM_NAME_BM, blend_mode)
                annot.annot_obj().dict_put_name(mupdf.PDF_ENUM_NAME_BM, blend_mode)

            extg = resources.dict_get(mupdf.PDF_ENUM_NAME_ExtGState)
            if not extg.m_internal: # no ExtGState yet: make one
                extg = resources.dict_put_dict(mupdf.PDF_ENUM_NAME_ExtGState, 2)

            #extg.dict_put_drop(mupdf.PDF_ENUM_NAME_H, alp0)
            extg.dict_put(mupdf.PDF_ENUM_NAME_H, alp0)

        except Exception:
            PySys_WriteStderr("could not set opacity or blend mode\n");
            return False

        return True

    @property
    def apn_bbox(self):
        """annotation appearance bbox"""
        CheckParent(self)

        #val = _fitz.Annot_apn_bbox(self)
        annot = self.this
        annot_obj = mupdf.mpdf_annot_obj(annot)
        ap = mupdf.mpdf_dict_getl(annot_obj, PDF_NAME('AP'), PDF_NAME('N'))
        if not ap.m_internal:
            val = JM_py_from_rect(mupdf.Rect(mupdf.Rect.Fixed_INFINITE))
        else:
            rect = mupdf.mpdf_dict_get_rect(ap, PDF_NAME('BBox'))
            val = JM_py_from_rect(rect)

        val = Rect(val) * self.parent.transformationMatrix
        val *= self.parent.derotationMatrix
        return val

    @property
    def apn_matrix(self):
        """annotation appearance matrix"""
        try:
            CheckParent(self)

            #val = _fitz.Annot_apn_matrix(self)
            annot = self.this
            assert isinstance(annot, mupdf.PdfAnnot)
            ap = annot.annot_obj().dict_getl(mupdf.PDF_ENUM_NAME_AP, mupdf.PDF_ENUM_NAME_N);
            if not ap.m_internal:
                return JM_py_from_matrix(mupdf.Matrix())
            mat = ap.dict_get_matrix(mupdf.PDF_ENUM_NAME_Matrix)
            val = JM_py_from_matrix(mat)

            val = Matrix(val)

            return val
        except Exception:
            #jlib.log(jlib.exception_info())
            raise

    def blendMode(self):
        """annotation BlendMode"""
        CheckParent(self)

        #return _fitz.Annot_blendMode(self)
        blend_mode = None
        annot = self.this
        #pdf_obj *obj, *obj1, *obj2;
        obj = annot.annot_obj().dict_get(mupdf.PDF_ENUM_NAME_BM)
        if obj.m_internal:
            blend_mode = obj.to_name()
            return blend_mode

        # loop through the /AP/N/Resources/ExtGState objects
        obj = annot.annot_obj().dict_getl(
                mupdf.PDF_ENUM_NAME_AP,
                mupdf.PDF_ENUM_NAME_N,
                mupdf.PDF_ENUM_NAME_Resources,
                mupdf.PDF_ENUM_NAME_ExtGState,
                )
        if obj.is_dict():
            n = obj.dict_len()
            for i in range(n):
                obj1 = obj.dict_get_val(i)
                if obj1.is_dict():
                    m = obj1.dict_len()
                    for j in range(m):
                        obj2 = obj1.dict_get_key(j)
                        if obj2.objcmp(mupdf.PDF_ENUM_NAME_BM) == 0:
                            blend_mode = obj1.dict_get_val(j).to_name()
                            return blend_mode
        return blend_mode;

    @property
    def border(self):
        """Border information."""
        CheckParent(self)
        #return _fitz.Annot_border(self)
        ao = self.this.annot_obj()
        ret = JM_annot_border(ao)
        return ret

    def clean_contents(self, sanitize=1):
        """Clean appearance contents stream."""
        CheckParent(self)
        #return _fitz.Annot_clean_contents(self, sanitize)
        annot = self.this
        pdf = mupdf.mpdf_get_bound_document(mupdf.mpdf_annot_obj(annot))
        filter = mupdf.PdfFilterOptions()
        filter.recurse = 1
        filter.instance_forms = 1
        filter.sanitize = sanitize
        filter.ascii = 0
        mupdf.mpdf_filter_annot_contents(pdf, annot, filter)
        mupdf.mpdf_dirty_annot(annot)

    @property
    def colors(self):
        """Color definitions."""
        try:
            CheckParent(self)

            #return _fitz.Annot_colors(self)
            annot = self.this
            assert isinstance(annot, mupdf.PdfAnnot)
            return JM_annot_colors(annot.annot_obj())
        except Exception as e:
            #jlib.log(jlib.exception_info())
            raise

    def delete_responses(self):
        """Delete 'Popup' and responding annotations."""
        CheckParent(self)
        #return _fitz.Annot_delete_responses(self)
        annot = self.this
        annot_obj = mupdf.mpdf_annot_obj(annot)
        page = mupdf.mpdf_annot_page(annot)
        #pdf_annot *irt_annot = NULL;
        while 1:
            irt_annot = JM_find_annot_irt(annot)
            if not irt_annot.m_internal:
                break
            mupdf.mpdf_delete_annot(page, irt_annot)
        mupdf.mpdf_dict_del(annot_obj, PDF_NAME('Popup'));

        annots = mupdf.mpdf_dict_get(page.obj(), PDF_NAME('Annots'))
        n = mupdf.mpdf_array_len(annots)
        found = 0
        for i in range(n-1, -1, -1):
            o = mupdf.mpdf_array_get(annots, i)
            p = mupdf.mpdf_dict_get(o, PDF_NAME('Parent'))
            if not o.m_internal:
                continue
            if not mupdf.mpdf_objcmp(p, annot_obj):
                mupdf.mpdf_array_delete(annots, i)
                found = 1
        if found:
            mupdf.mpdf_dict_put(page.obj(), PDF_NAME('Annots'), annots)
        mupdf.mpdf_dirty_annot(annot)

    def fileInfo(self):
        """Attached file information."""
        CheckParent(self)

        # fixme: no Annot_fileInfo() in PyMuPDF?
        return _fitz.Annot_fileInfo(self)

    @property
    def flags(self):
        """Flags field."""
        CheckParent(self)
        #return _fitz.Annot_flags(self)
        annot = self.this
        return mupdf.mpdf_annot_flags(annot)

    def get_file(self):
        """Retrieve attached file content."""
        CheckParent(self)
        #return _fitz.Annot_get_file(self)
        #PyObject *res = NULL;
        #pdf_obj *stream = NULL;
        #fz_buffer *buf = NULL;
        annot = self.this
        annot_obj = mupdf.mpdf_annot_obj(annot)
        type = mupdf.mpdf_annot_type(annot)
        if type != mupdf.PDF_ANNOT_FILE_ATTACHMENT:
            THROWMSG("bad annot type")
        stream = annot_obj.dict_getl(PDF_NAME('FS'), PDF_NAME('EF'), PDF_NAME('F'))
        if not stream.m_internal:
            THROWMSG("bad PDF: file entry not found")
        buf = mupdf.mpdf_load_stream(stream)
        res = JM_BinFromBuffer(buf)
        return res

    def get_oc(self):
        """Get annotation optional content reference."""
        CheckParent(self)
        #return _fitz.Annot_get_oc(self)
        oc = 0
        annot = self.this
        annot_obj = mupdf.mpdf_annot_obj(annot)
        obj = mupdf.mpdf_dict_get(annot_obj, PDF_NAME('OC'))
        if obj.m_internal:
            oc = mupdf.mpdf_to_num(obj)
        return oc

    def get_pixmap(self, matrix=None, colorspace=None, alpha=0):
        """annotation Pixmap"""

        CheckParent(self)
        #cspaces = {"gray": mupdf.csGRAY, "rgb": mupdf.csRGB, "cmyk": mupdf.csCMYK}
        #if type(colorspace) is str:
        #    colorspace = cspaces.get(colorspace.lower(), None)

        #return _fitz.Annot_get_pixmap(self, matrix, colorspace, alpha)
        ctm = JM_matrix_from_py(matrix)
        cs = None
        if isinstance(colorspace, str):
            colorspace = colorspace.toupper()
            f = getattr(f'mupdf.Colorspace.Fixed_{colorspace}', None)
            if f:
                cs = mupdf.Colorspace(f)
        if cs is None:
            cs = mupdf.Colorspace(mupdf.Colorspace.Fixed_RGB)
        try:
            pix = mupdf.mpdf_new_pixmap_from_annot(self.this, ctm, cs, mupdf.Separations(0), alpha)
        except Exception as e:
            #jlib.log('{e=}')
            return
        #jlib.log('{pix=}')
        return Pixmap(pix)

    def get_sound(self):
        """Retrieve sound stream."""
        CheckParent(self)

        #return _fitz.Annot_get_sound(self)
        annot = self.this
        annot_obj = mupdf.mpdf_annot_obj(annot)
        type = mupdf.mpdf_annot_type(annot)
        sound = mupdf.mpdf_dict_get(annot_obj, PDF_NAME('Sound'))
        if type != mupdf.PDF_ANNOT_SOUND or not sound.m_internal:
            THROWMSG("bad annot type")
        if pdf_dict_get(sound, PDF_NAME(F)).m_internal:
            THROWMSG("unsupported sound stream")
        res = dict()
        obj = mupdf.mpdf_dict_get(sound, PDF_NAME('R'))
        if obj.m_internal:
            res['rate'] = mupdf.mpdf_to_real(obj)
        obj = mupdf.mpdf_dict_get(sound, PDF_NAME('C'))
        if obj.m_internal:
            res['channels'] = mupdf.mpdf_to_int(obj)
        obj = mupdf.mpdf_dict_get(sound, PDF_NAME('B'))
        if obj.m_internal:
            res['bps'] = mupdf.mpdf_to_int(obj)
        obj = mupdf.mpdf_dict_get(sound, PDF_NAME('E'))
        if obj.m_internal:
            res['encoding'] = mupdf.mpdf_to_name(obj)
        obj = mupdf.mpdf_dict_gets(sound, "CO");
        if obj.m_internal:
            res['compression'] = mupdf.mpdf_to_name(obj)
        buf = mupdf.mpdf_load_stream(sound)
        stream = JM_BinFromBuffer(buf)
        res['stream'] = stream
        return res

    def get_textpage(self, clip=None, flags=0):
        """Make annotation TextPage."""
        CheckParent(self)
        #return _fitz.Annot_get_textpage(self, clip, flags)
        options = mupdf.StextOptions()
        options.flags = flags
        annot = self.this
        stextpage = mupdf.mpdf_new_stext_page_from_annot(annot, options)
        return TextPage(stextpage)

    @property
    def has_popup(self):
        """Check if annotation has a Popup."""
        CheckParent(self)

        #return _fitz.Annot_has_popup(self)
        annot = self.this
        obj = mupdf.mpdf_dict_get(annot.annot_obj(), PDF_NAME('Popup'))
        return True if obj.m_internal else False

    @property
    def info(self):
        """Various information details."""
        CheckParent(self)

        #return _fitz.Annot_info(self)
        annot = self.this
        res = dict()

        res[dictkey_content] = JM_UnicodeFromStr(mupdf.mpdf_annot_contents(annot))

        o = mupdf.mpdf_dict_get(annot.annot_obj(), PDF_NAME('Name'))
        res[dictkey_name] = JM_UnicodeFromStr(mupdf.mpdf_to_name(o))

        # Title (= author)
        o = mupdf.mpdf_dict_get(annot.annot_obj(), PDF_NAME('T'))
        res[dictkey_title] = JM_UnicodeFromStr(mupdf.mpdf_to_text_string(o))

        # CreationDate
        o = mupdf.mpdf_dict_gets(annot.annot_obj(), "CreationDate")
        res[dictkey_creationDate] = JM_UnicodeFromStr(mupdf.mpdf_to_text_string(o))

        # ModDate
        o = mupdf.mpdf_dict_get(annot.annot_obj(), PDF_NAME('M'))
        res[dictkey_modDate] = JM_UnicodeFromStr(mupdf.mpdf_to_text_string(o))

        # Subj
        o = mupdf.mpdf_dict_gets(annot.annot_obj(), "Subj")
        res[dictkey_subject] = mupdf.mpdf_to_text_string(o)

        # Identification (PDF key /NM)
        o = mupdf.mpdf_dict_gets(annot.annot_obj(), "NM")
        res[dictkey_id] = JM_UnicodeFromStr(mupdf.mpdf_to_text_string(o))

        return res

    @property
    def is_open(self):
        """Get 'open' status of annotation or its Popup."""
        CheckParent(self)
        #return _fitz.Annot_is_open(self)
        return mupdf.mpdf_annot_is_open(self.this)

    @property
    def language(self):
        """annotation language"""
        #return _fitz.Annot_language(self)
        assert 0, 'Not implemented yet'
        this_annot = self.this
        lang = mupdf.mpdf_annot_language(this_annot)
        if not lang:
            return
        #return Py_BuildValue("s", fz_string_from_text_language(buf, lang));

    @property
    def line_ends(self):
        """Line end codes."""
        CheckParent(self)

        #return _fitz.Annot_line_ends(self)
        annot = self.this
        # return nothing for invalid annot types
        if not annot.annot_has_line_ending_styles():
            return

        lstart = annot.annot_line_start_style()
        lend = annot.annot_line_end_style()
        return lstart, lend

    @property
    def next(self):
        """Next annotation."""
        CheckParent(self)

        #val = _fitz.Annot_next(self)
        this_annot = self.this
        assert isinstance(this_annot, mupdf.PdfAnnot)
        assert(this_annot.m_internal)
        type_ = mupdf.mpdf_annot_type(this_annot)
        if type_ != mupdf.PDF_ANNOT_WIDGET:
            annot = mupdf.mpdf_next_annot(this_annot)
        else:
            #annot = (pdf_widget *) pdf_next_widget(gctx, (pdf_widget *) this_annot);
            annot = mupdf.mpdf_next_widget(this_annot)

        val = Annot(annot) if annot.m_internal else None

        if not val:
            return None
        val.thisown = True

        assert val.parent.this.m_internal_value() == self.parent.this.m_internal_value(), jlib.expand_nv('{val.parent.this.m_internal_value()=} {self.parent.this.m_internal_value()=}')
        #val.parent = self.parent  # copy owning page object from previous annot
        val.parent._annot_refs[id(val)] = val

        if val.type[0] == mupdf.PDF_ANNOT_WIDGET:
            widget = Widget()
            TOOLS._fill_widget(val, widget)
            val = widget
        return val

    @property
    def opacity(self):
        """Opacity."""
        CheckParent(self)

        #return _fitz.Annot_opacity(self)
        annot = self.this
        opy = -1
        ca = annot.annot_obj().dict_get(mupdf.PDF_ENUM_NAME_CA)
        if ca.is_number():
            opy = ca.to_real()
        return opy

    @property
    def parent(self):
        return Page(self.this.annot_page())

    @property
    def popup_rect(self):
        """annotation 'Popup' rectangle"""
        CheckParent(self)

        #val = _fitz.Annot_popup_rect(self)
        rect = mupdf.Rect(mupdf.Rect.Fixed_INFINITE)
        #jlib.log('{rect=}')
        annot = self.this
        obj = mupdf.mpdf_dict_get(annot.annot_obj(), PDF_NAME('Popup'))
        #jlib.log('{obj.m_internal=}')
        if obj.m_internal:
            if 1:
                rect = mupdf.mpdf_dict_get_rect(obj, PDF_NAME('Rect'))
            else:
                # fixme, this fixes assert in PyMuPDF/tests/test_annots.py:test_redact().
                rect = mupdf.mpdf_annot_popup(annot)
            #jlib.log('{rect=}')
        val = JM_py_from_rect(rect)
        #jlib.log('{val=}')
        val = Rect(val) * self.parent.transformationMatrix
        #jlib.log('{val=}')
        val *= self.parent.derotationMatrix
        #jlib.log('{val=}')

        return val

    @property
    def popup_xref(self):
        """annotation 'Popup' xref"""
        CheckParent(self)
        #return _fitz.Annot_popup_xref(self)
        xref = 0
        annot = self.this
        annot_obj = mupdf.mpdf_annot_obj(annot)
        obj = mupdf.mpdf_dict_get(annot_obj, PDF_NAME('Popup'))
        if obj.m_internal:
            xref = mupdf.mpdf_to_num(obj)
        return xref

    @property
    def rect(self):
        """annotation rectangle"""
        CheckParent(self)

        #val = _fitz.Annot_rect(self)
        val = mupdf.mpdf_bound_annot(self.this)
        val = Rect(val)
        #jlib.log('{val=} {self.parent=}')
        val *= self.parent.derotation_matrix
        return val

    @property
    def rotation(self):
        """annotation rotation"""
        CheckParent(self)

        #return _fitz.Annot_rotation(self)
        annot = self.this
        rotation = annot.annot_obj().dict_get(mupdf.PDF_ENUM_NAME_Rotate)
        if not rotation.m_internal:
            return -1
        return rotation.to_int(g)

    def set_apn_bbox(self, bbox):
        """
        Set annotation appearance bbox.
        """
        CheckParent(self)
        page = self.parent
        rot = page.rotationMatrix
        mat = page.transformationMatrix
        bbox *= rot * ~mat

        #return _fitz.Annot_set_apn_bbox(self, bbox)
        pannot = self.this
        annot_obj = mupdf.mpdf_annot_obj(annot)
        ap = annot_obj.dict_getl(PDF_NAME('AP'), PDF_NAME('N'))
        if not ap.m_internal:
            THROWMSG("annot has no appearance stream")
        rect = JM_rect_from_py(bbox)
        mupdf.mpdf_dict_put_rect(ap, PDF_NAME('BBox'), rect)

    def set_apn_matrix(self, matrix):
        """Set annotation appearance matrix."""
        CheckParent(self)
        #return _fitz.Annot_set_apn_matrix(self, matrix)
        annot = self.this
        annot_obj = mupdf.mpdf_annot_obj(annot)
        ap = annot_obj.dict_getl(PDF_NAME('AP'), PDF_NAME('N'))
        if not ap.m_internal:
            THROWMSG("annot has no appearance stream")
        mat = JM_matrix_from_py(matrix)
        mupdf.mpdf_dict_put_matrix(ap, PDF_NAME('Matrix'), mat)

    def set_blendmode(self, blend_mode):
        """Set annotation BlendMode."""
        CheckParent(self)
        #return _fitz.Annot_set_blendmode(self, blend_mode)
        annot = self.this
        annot_obj = mupdf.mpdf_annot_obj(annot)
        mupdf.mpdf_dict_put_name(annot_obj, PDF_NAME('BM'), blend_mode)

    def set_border(self, border=None, width=0, style=None, dashes=None):
        """Set border properties.

        Either a dict, or direct arguments width, style and dashes."""
        CheckParent(self)
        if type(border) is not dict:
            border = {"width": width, "style": style, "dashes": dashes}


        #return _fitz.Annot_set_border(self, border, width, style, dashes)
        annot = self.this
        return JM_annot_set_border(border, annot.annot_page().doc(), annot.annot_obj())

    def set_colors(self, colors=None, fill=None, stroke=None):

        """Set 'stroke' and 'fill' colors.

        Use either a dict or the direct arguments.
        """
        CheckParent(self)
        if type(colors) is not dict:
            colors = {"fill": fill, "stroke": stroke}
        assert 0, 'no Annot_set_colors'
        return _fitz.Annot_set_colors(self, colors, fill, stroke)

    def set_flags(self, flags):
        """Set annotation flags."""
        CheckParent(self)
        #return _fitz.Annot_set_flags(self, flags)
        annot = self.this
        mupdf.mpdf_set_annot_flags(annot, flags)

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

        #return _fitz.Annot_set_info(self, info, content, title, creationDate, modDate, subject)
        annot = self.this
        # use this to indicate a 'markup' annot type
        is_markup = mupdf.mpdf_annot_has_author(annot)
        # contents
        if content:
            mupdf.mpdf_set_annot_contents(annot, content)

        if is_markup:
            # title (= author)
            if title:
                mupdf.mpdf_set_annot_author(annot, title)
            # creation date
            if creationDate:
                mupdf.mpdf_dict_put_text_string(annot.annot_obj(), PDF_NAME('CreationDate'), creationDate)
            # mod date
            if modDate:
                mupdf.mpdf_dict_put_text_string(annot.annot_obj(), PDF_NAME('M'), modDate)
            # subject
            if subject:
                mupdf.mpdf_dict_puts(annot.annot_obj(), "Subj", mupdf.mpdf_new_text_string(subject))

    def set_language(self, language=None):
        """Set annotation language."""
        CheckParent(self)
        #return _fitz.Annot_set_language(self, language)
        this_annot = self.this
        if not language:
            lang = mupdf.FZ_LANG_UNSET;
        else:
            lang = mupdf.mfz_text_language_from_string(language);
        mupdf.mpdf_set_annot_language(this_annot, lang)

    def set_line_ends(self, start, end):
        """Set line end codes."""
        CheckParent(self)
        #return _fitz.Annot_set_line_ends(self, start, end)
        annot = self.this
        if mupdf.mpdf_annot_has_line_ending_styles(annot):
            mupdf.mpdf_set_annot_line_ending_styles(annot, start, end)
        else:
            JM_Warning("bad annot type for line ends")

    def set_name(self, name):
        """Set /Name (icon) of annotation."""
        CheckParent(self)
        #return _fitz.Annot_set_name(self, name)
        annot = self.this
        annot_obj = mupdf.mpdf_annot_obj(annot)
        mupdf.mpdf_dict_put_name(annot_obj, PDF_NAME('Name'), name)
        mupdf.mpdf_dirty_annot(annot)

    def set_oc(self, oc=0):
        """Set / remove annotation OC xref."""
        CheckParent(self)
        #return _fitz.Annot_set_oc(self, oc)
        annot = self.this
        annot_obj = mupdf.mpdf_annot_obj(annot)
        if not oc:
            mupdf.mpdf_dict_del(annot_obj, PDF_NAME('OC'))
        else:
            JM_add_oc_object(mupdf.mpdf_get_bound_document(annot_obj), annot_obj, oc)

    def set_opacity(self, opacity):
        """Set opacity."""
        CheckParent(self)
        #return _fitz.Annot_set_opacity(self, opacity)
        annot = self.this
        if not INRANGE(opacity, 0.0, 1.0):
            mupdf.mpdf_set_annot_opacity(annot, 1)
            return;
        mupdf.mpdf_set_annot_opacity(annot, opacity)
        if opacity < 1.0:
            page = mupdf.mpdf_annot_page(annot)
            page.transparency = 1

    def set_open(self, is_open):
        """Set 'open' status of annotation or its Popup."""
        CheckParent(self)
        #return _fitz.Annot_set_open(self, is_open)
        annot = self.this
        mupdf.mpdf_set_annot_is_open(annot, is_open)

    def set_popup(self, rect):
        '''
        Create annotation 'Popup' or update rectangle.
        '''
        CheckParent(self)
        #jlib.log('{rect=}')
        annot = self.this
        pdfpage = annot.annot_page()
        rot = JM_rotate_page_matrix(pdfpage)
        #jlib.log('{rot=}')
        r = mupdf.mfz_transform_rect(JM_rect_from_py(rect), rot)
        #jlib.log('{r=}')
        mupdf.mpdf_set_annot_popup(annot, r)

    def set_rect(self, rect):
        """Set annotation rectangle."""
        CheckParent(self)
        #return _fitz.Annot_set_rect(self, rect)
        try:
            annot = self.this
            pdfpage = annot.annot_page()
            rot = JM_rotate_page_matrix(pdfpage)
            r = mupdf.mfz_transform_rect(JM_rect_from_py(rect), rot)
            mupdf.mpdf_set_annot_rect(annot, r)
        except Exception as e:
            jlib.log('{e=}')

    def set_rotation(self, rotate=0):
        """Set annotation rotation."""
        CheckParent(self)
        #return _fitz.Annot_set_rotation(self, rotate)
        annot = self.this
        type = mupdf.mpdf_annot_type(annot)
        if type not in (
                mupdf.PDF_ANNOT_CARET,
                mupdf.PDF_ANNOT_CIRCLE,
                mupdf.PDF_ANNOT_FREE_TEXT,
                mupdf.PDF_ANNOT_FILE_ATTACHMENT,
                mupdf.PDF_ANNOT_INK,
                mupdf.PDF_ANNOT_LINE,
                mupdf.PDF_ANNOT_POLY_LINE,
                mupdf.PDF_ANNOT_POLYGON,
                mupdf.PDF_ANNOT_SQUARE,
                mupdf.PDF_ANNOT_STAMP,
                mupdf.PDF_ANNOT_TEXT,
                ):
            return
        rot = rotate
        while rot < 0:
            rot += 360
        while rot >= 360:
            rot -= 360
        if type == mupdf.PDF_ANNOT_FREE_TEXT and rot % 90 != 0:
            rot = 0
        annot_obj = mupdf.mpdf_annot_obj(annot)
        mupdf.mpdf_dict_put_int(annot_obj, PDF_NAME('Rotate'), rot)

    @property
    def type(self):
        """annotation type"""
        CheckParent(self)
        #return _fitz.Annot_type(self)
        assert self.this.m_internal, f'self.this={self.this} self.this.m_internal={self.this.m_internal}'
        type_ = self.this.annot_type()
        c = mupdf.ppdf_string_from_annot_type(type_)
        o = self.this.annot_obj().dict_gets("IT")
        if not o.m_internal or o.is_name():
            return (type_, c)
        it = o.to_name()
        return (type_, c, it)

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
            if type == mupdf.PDF_ANNOT_FREE_TEXT and rotate % 90 != 0:
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

        if type == mupdf.PDF_ANNOT_REDACT:
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

        if type == mupdf.PDF_ANNOT_FREE_TEXT:
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

        if type in (mupdf.PDF_ANNOT_POLYGON, mupdf.PDF_ANNOT_POLY_LINE):
            ap = b"\n".join(ap_tab[:-1]) + b"\n"
            ap_updated = True
            if bfill != b"":
                if type == mupdf.PDF_ANNOT_POLYGON:
                    ap = ap + bfill + b"b"  # close, fill, and stroke
                elif type == mupdf.PDF_ANNOT_POLY_LINE:
                    ap = ap + b"S"  # stroke
            else:
                if type == mupdf.PDF_ANNOT_POLYGON:
                    ap = ap + b"s"  # close and stroke
                elif type == mupdf.PDF_ANNOT_POLY_LINE:
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
        if line_end_le + line_end_ri > 0 and type in (mupdf.PDF_ANNOT_POLYGON, mupdf.PDF_ANNOT_POLY_LINE):

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
            mupdf.PDF_ANNOT_CARET,
            mupdf.PDF_ANNOT_CIRCLE,
            mupdf.PDF_ANNOT_FILE_ATTACHMENT,
            mupdf.PDF_ANNOT_INK,
            mupdf.PDF_ANNOT_LINE,
            mupdf.PDF_ANNOT_POLY_LINE,
            mupdf.PDF_ANNOT_POLYGON,
            mupdf.PDF_ANNOT_SQUARE,
            mupdf.PDF_ANNOT_STAMP,
            mupdf.PDF_ANNOT_TEXT,
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

    def update_file(self, buffer_=None, filename=None, ufilename=None, desc=None):
        """Update attached file."""
        CheckParent(self)

        #return _fitz.Annot_update_file(self, buffer, filename, ufilename, desc)
        annot = self.this
        annot_obj = mupdf.mpdf_annot_obj(annot)
        pdf = mupdf.mpdf_get_bound_document(annot_obj)  # the owning PDF
        type = mupdf.mpdf_annot_type(annot)
        if type != mupdf.PDF_ANNOT_FILE_ATTACHMENT:
            THROWMSG("bad annot type")
        stream = mupdf.mpdf_dict_getl(annot_obj, PDF_NAME('FS'), PDF_NAME('EF'), PDF_NAME('F'))
        # the object for file content
        if not stream.m_internal:
            THROWMSG("bad PDF: no /EF object")

        fs = mupdf.mpdf_dict_get(annot_obj, PDF_NAME('FS'))

        # file content given
        res = JM_BufferFromBytes(buffer_)
        if buffer_ and not res.m_internal:
            THROWMSG("bad type: 'buffer'")
        if res:
            JM_update_stream(pdf, stream, res, 1)
            # adjust /DL and /Size parameters
            len, _ = mupdf.mfz_buffer_storage(res, NULL)
            l = mupdf.mpdf_new_int(len)
            mupdf.mpdf_dict_put(stream, PDF_NAME('DL'), l)
            mupdf.mpdf_dict_putl(stream, l, PDF_NAME('Params'), PDF_NAME('Size'))

        if filename:
            mupdf.mpdf_dict_put_text_string(stream, PDF_NAME('F'), filename)
            mupdf.mpdf_dict_put_text_string(fs, PDF_NAME('F'), filename)
            mupdf.mpdf_dict_put_text_string(stream, PDF_NAME('UF'), filename)
            mupdf.mpdf_dict_put_text_string(fs, PDF_NAME('UF'), filename)
            mupdf.mpdf_dict_put_text_string(annot_obj, PDF_NAME('Contents'), filename)

        if ufilename:
            mupdf.mpdf_dict_put_text_string(stream, PDF_NAME('UF'), ufilename)
            mupdf.mpdf_dict_put_text_string(fs, PDF_NAME('UF'), ufilename)

        if desc:
            mupdf.mpdf_dict_put_text_string(stream, PDF_NAME('Desc'), desc)
            mupdf.mpdf_dict_put_text_string(fs, PDF_NAME('Desc'), desc)

    @property
    def vertices(self):
        """annotation vertex points"""
        CheckParent(self)
        annot = self.this
        assert isinstance(annot, mupdf.PdfAnnot)
        #fz_point point;  # point object to work with
        page_ctm = mupdf.Matrix()   # page transformation matrix
        dummy = mupdf.Rect(0)   # Will have .m_internal=NULL.
        mupdf.mpdf_page_transform(annot.annot_page(), dummy, page_ctm);
        derot = JM_derotate_page_matrix(annot.annot_page())
        page_ctm = mupdf.mfz_concat(page_ctm, derot)

        #----------------------------------------------------------------
        # The following objects occur in different annotation types.
        # So we are sure that (!o) occurs at most once.
        # Every pair of floats is one point, that needs to be separately
        # transformed with the page transformation matrix.
        #----------------------------------------------------------------
        o = mupdf.mpdf_dict_get(annot.annot_obj(), PDF_NAME('Vertices'))
        if not o.m_internal:    o = mupdf.mpdf_dict_get(annot.annot_obj(), PDF_NAME('L'))
        if not o.m_internal:    o = mupdf.mpdf_dict_get(annot.annot_obj(), PDF_NAME('QuadPoints'))
        if not o.m_internal:    o = mupdf.mpdf_dict_gets(annot.annot_obj(), "CL")

        if o.m_internal:
            # handle lists with 1-level depth --------------------------------
            #weiter:;
            res = []
            for i in range(0, mupdf.mpdf_array_len(o), 2):
                x = mupdf.mpdf_to_real(mupdf.mpdf_array_get(o, i))
                y = mupdf.mpdf_to_real(mupdf.mpdf_array_get(o, i+1))
                point = mupdf.Point(x, y)
                point = mupdf.mfz_transform_point(point, page_ctm)
                res.append( (point.x, point.y))
            return res

        else:
            # InkList has 2-level lists --------------------------------------
            #inklist:;
            res = []
            for i in range(mupdf.mpdf_array_len(o)):
                res1 = []
                o1 = mupdf.mpdf_array_get(o, i)
                for j in range(0, mupdf.mpdf_array_len(o1), 2):
                    x = mupdf.mpdf_to_real(mupdf.mpdf_array_get(o1, j))
                    y = mupdf.mpdf_to_real(mupdf.mpdf_array_get(o1, j+1))
                    point = mupdf.Point(x, y)
                    point = mupdf.mfz_transform_point(point, page_ctm)
                    res1.append( (point.x, point.y))
                res.append(res1)
            return res;

    @property
    def xref(self):
        """annotation xref"""
        CheckParent(self)
        #return _fitz.Annot_xref(self)
        annot = self.this
        return mupdf.mpdf_to_num(annot.annot_obj())

    blendmode = property(blendMode, doc="annotation BlendMode")
    fileGet = get_file
    fileUpd = update_file
    file_info = property(fileInfo, doc="Attached file information")
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
    setOC = set_oc
    setOpacity = set_opacity
    setRect = set_rect
    setRotation = set_rotation
    soundGet = get_sound


class Colorspace:
    def __init__(self, type_):
        """Supported are GRAY, RGB and CMYK."""
        #this = _fitz.new_Colorspace(type)
        this = mupdf.Colorspace(type_)
        self.this = this

    def __repr__(self):
        x = ("", "GRAY", "", "RGB", "CMYK")[self.n]
        return "Colorspace(CS_%s) - %s" % (x, self.name)

    def _name(self):
        #return _fitz.Colorspace__name(self)
        return self.this.colorspace_name()

    @property
    def n(self):
        """Size of one pixel."""
        #return _fitz.Colorspace_n(self)
        return self.this.colorspace_n()

    @property
    def name(self):
        """Name of the Colorspace."""
        return self._name()


class Device:
    def __init__(self, *args):
        assert 0, '_fitz.new_Device() not found?'
        this = _fitz.new_Device(*args)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this


class DisplayList:
    def __init__(self, *args):
        if len(args) == 1 and isinstance(args[0], mupdf.Rect):
            self.this = mupdf.DisplayList(args[0])
        elif len(args) == 1 and isinstance(args[0], mupdf.DisplayList):
            self.this = args[0]
        else:
            assert 0, f'Unrecognised args={args}'

    @property
    def rect(self):
        #val = _fitz.DisplayList_rect(self)
        val = JM_py_from_rect(mupdf.mfz_bound_display_list(self.this))
        val = Rect(val)
        return val

    def run(self, dw, m, area):
        #return _fitz.DisplayList_run(self, dw, m, area)
        mupdf.mfz_run_display_list(
                self.this,
                dw.device,
                JM_matrix_from_py(m),
                JM_rect_from_py(area),
                mupdf.Cookie(),
                )

    def get_pixmap(self, matrix=None, colorspace=None, alpha=0, clip=None):
        #val = _fitz.DisplayList_getPixmap(self, matrix, colorspace, alpha, clip)
        if not colorspace:
            colorspace = mupdf.Colorspace(mupdf.Colorspace.Fixed_RGB)
        val = JM_pixmap_from_display_list(self.this, matrix, colorspace, alpha, clip, None);
        val.thisown = True
        return val

    def getTextPage(self, flags=3):
        #val = _fitz.DisplayList_getTextPage(self, flags)
        assert 0, '_fitz.DisplayList_getTextPage not found'
        val.thisown = True

        return val

    def __del__(self):
        if not type(self) is DisplayList: return
        self.thisown = False


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
        #jlib.log('*** Document.__init__(). backtrace is:')
        #jlib.log(jlib.exception_info())
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
        #char *c = NULL;
        #fz_stream *data = NULL;
        w = width
        h = height
        r = JM_rect_from_py(rect)
        if not mupdf.mfz_is_infinite_rect(r):
            w = r.x1 - r.x0
            h = r.y1 - r.y0
        if stream:  # stream given, **MUST** be bytes!
            assert isinstance(stream, bytes)
            c = stream
            #len = (size_t) PyBytes_Size(stream);

            # Pass raw bytes data to mupdf.mfz_open_memory(). This assumes
            # that the bytes string will not be modified; i think the original
            # PyMuPDF code makes the same assumption. Presumably setting
            # self.stream above ensures that the bytes will not be garbage
            # collected?
            data = mupdf.mfz_open_memory(mupdf.python_bytes_data(c), len(c))
            magic = filename
            if not magic:
                magic = filetype
            doc = mupdf.mfz_open_document_with_stream(magic, data)
        else:
            if filename:
                if not filetype:
                    doc = mupdf.mfz_open_document(filename)
                else:
                    handler = mupdf.mfz_recognize_document(filetype);
                    if handler and handler.open:
                        doc = handler.open(filename)
                    else:
                        THROWMSG("unrecognized file type")
            else:
                #jlib.log('*** calling mupdf.mpdf_create_document()')
                #pdf = mupdf.mpdf_create_document()
                #jlib.log('*** calling mupdf.PdfDocument()')
                pdf = mupdf.PdfDocument()
                #jlib.log('*** return from mupdf.mpdf_create_document()')
                #jlib.log('*** returning from mupdf.PdfDocument()')
                #jlib.log('{=mupdf.mpdf_xref_len(pdf)}')
                doc = mupdf.Document(pdf)
                #jlib.log('{=mupdf.mpdf_xref_len(pdf)}')
        #jlib.log('{doc=}')
        if w > 0 and h > 0:
            mupdf.mfz_layout_document(doc, w, h, fontsize)
        elif mupdf.mfz_is_document_reflowable(doc):
           mupdf.mfz_layout_document(doc, 400, 600, 11)
        this = doc

        self.this = this

        # fixme: not sure where self.thisown gets initialised in PyMuPDF.
        #
        self.thisown = True

        if self.thisown:
            self._graft_id = TOOLS.gen_id()
            if self.needsPass is True:
                self.isEncrypted = True
            else: # we won't init until doc is decrypted
                self.initData()
        #jlib.log('__init__() returning. {=mupdf.mpdf_xref_len(pdf)}')

    def _deleteObject(self, xref):
        """Delete object."""
        if self.isClosed:
            raise ValueError("document closed")
        #return _fitz.Document__deleteObject(self, xref)
        doc = self.this
        pdf = mupdf.mpdf_specifics(doc)
        ASSERT_PDF(pdf)
        if not INRANGE(xref, 1, mupdf.mpdf_xref_len(pdf)-1):
            THROWMSG("bad xref")
        mupdf.pdf_delete_object(pdf, xref)

    def _deletePage(self, pno):
        assert 0, '_fitz.Document__deletePage not found.'
        return _fitz.Document__deletePage(self, pno)

    def _delToC(self):
        """Delete the TOC."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        #val = _fitz.Document__delToC(self)
        xrefs = []  # create Python list
        pdf = mupdf.mpdf_specifics(self.this)
        if not pdf.m_internal:
            return xrefs    # not a pdf

        #pdf_obj *root, *olroot, *first;
        #int xref_count, olroot_xref, i, xref;

        # get the main root
        root = mupdf.mpdf_dict_get(mupdf.mpdf_trailer(pdf), PDF_NAME('Root'))
        # get the outline root
        olroot = mupdf.mpdf_dict_get(root, PDF_NAME('Outlines'))
        if not olroot.m_internal:
            return xrefs    # no outlines or some problem

        first = mupdf.mpdf_dict_get(olroot, PDF_NAME('First'))  # first outline

        xrefs = JM_outline_xrefs(first, xrefs)
        xref_count = len(xrefs)

        olroot_xref = mupdf.mpdf_to_num(olroot) # delete OL root
        mupdf.mpdf_delete_object(pdf, olroot_xref)  # delete OL root
        mupdf.mpdf_dict_del(root, PDF_NAME('Outlines')) # delete OL root

        for i in range(xref_count):
            _, xref = JM_INT_ITEM(xrefs, i)
            mupdf.mpdf_delete_object(pdf, xref) # delete outline item
        xrefs.append(olroot_xref)
        val = xrefs
        self.initData()
        return val

    def _do_links(self, doc2, from_page=-1, to_page=-1, start_at=-1):
        return utils.do_links(self, doc2, from_page, to_page, start_at)

    def _dropOutline(self, ol):
        assert 0, 'Unnecessary'
        return _fitz.Document__dropOutline(self, ol)

    def _embeddedFileAdd(self, name, buffer_, filename=None, ufilename=None, desc=None):
        assert 0, 'deprecated'
        return _fitz.Document__embeddedFileAdd(self, name, buffer_, filename, ufilename, desc)

    def _embeddedFileDel(self, idx):
        assert 0, 'deprecated'
        return _fitz.Document__embeddedFileDel(self, idx)

    def _embeddedFileGet(self, idx):
        #return _fitz.Document__embeddedFileGet(self, idx)
        doc = self.this
        #jlib.log('*** calling mupdf.mpdf_document_from_fz_document(doc)')
        pdf = mupdf.mpdf_document_from_fz_document(doc)
        names = mupdf.mpdf_dict_getl(
                mupdf.mpdf_trailer(pdf),
                PDF_NAME('Root'),
                PDF_NAME('Names'),
                PDF_NAME('EmbeddedFiles'),
                PDF_NAME('Names'),
                )
        entry = mupdf.mpdf_array_get(names, 2*idx+1)
        filespec = mupdf.mpdf_dict_getl(entry, PDF_NAME('EF'), PDF_NAME('F'))
        buf = mupdf.mpdf_load_stream(filespec)
        cont = JM_BinFromBuffer(buf)
        return cont

    def _embeddedFileIndex(self, item: typing.Union[int, str]) -> int:
        filenames = self.embfile_names()
        msg = "'%s' not in EmbeddedFiles array." % str(item)
        if item in filenames:
            idx = filenames.index(item)
        elif item in range(len(filenames)):
            idx = item
        else:
            raise ValueError(msg)
        return idx

    def _embeddedFileInfo(self, idx, infodict):
        assert 0, 'no Document__embeddedFileInfo'
        return _fitz.Document__embeddedFileInfo(self, idx, infodict)

    def _embeddedFileNames(self, namelist):
        """Get list of embedded file names."""
        assert 0, 'no Document__embeddedFileNames'
        if self.isClosed:
            raise ValueError("document closed")
        return _fitz.Document__embeddedFileNames(self, namelist)

    def _embeddedFileUpd(self, idx, buffer_=None, filename=None, ufilename=None, desc=None):
        assert 0, 'no Document__embeddedFileUpd'
        return _fitz.Document__embeddedFileUpd(self, idx, buffer_, filename, ufilename, desc)

    def _embfile_del(self, idx):
        #return _fitz.Document__embfile_del(self, idx)
        pdf = self._pdf_document()
        names = mupdf.mpdf_dict_getl(
                mupdf.mpdf_trailer(pdf),
                PDF_NAME('Root'),
                PDF_NAME('Names'),
                PDF_NAME('EmbeddedFiles'),
                PDF_NAME('Names'),
                )
        mupdf.mpdf_array_delete(names, idx + 1)
        mupdf.mpdf_array_delete(names, idx)

    @staticmethod
    def show_dict(o):
        assert 0
        o_dict_len = mupdf.mpdf_dict_len(o)
        #jlib.log( '{o_dict_len=}:')
        for i in range(o_dict_len):
            key = mupdf.mpdf_dict_get_key(o, i)
            value = mupdf.mpdf_dict_get(o, key)
            #jlib.log( '    {key=}: {value=}')


    def _embfile_info(self, idx, infodict):
        #return _fitz.Document__embfile_info(self, idx, infodict)
        pdf = self._pdf_document()
        xref = 0
        ci_xref=0

        trailer = mupdf.mpdf_trailer(pdf);
        #jlib.log( 'trailer:')
        #mupdf.mpdf_debug_obj(trailer)

        names = mupdf.mpdf_dict_getl(
                trailer,
                PDF_NAME('Root'),
                PDF_NAME('Names'),
                PDF_NAME('EmbeddedFiles'),
                PDF_NAME('Names'),
                )

        #jlib.log( 'names:')
        #mupdf.mpdf_debug_obj(names)

        #jlib.log( '{pdfobj_string(names)=}')

        o = mupdf.mpdf_array_get(names, 2*idx+1)
        #jlib.log( '{pdfobj_string(o)=}')

        if 0:
            o_dict_len = mupdf.mpdf_dict_len(o)
            #jlib.log( '{o_dict_len=}:')
            for i in range(o_dict_len):
                key = mupdf.mpdf_dict_get_key(o, i)
                value = mupdf.mpdf_dict_get(o, key)
                #jlib.log( '    {key=}: {value=}')

        ci = mupdf.mpdf_dict_get(o, PDF_NAME('CI'))
        if ci.m_internal:
            ci_xref = mupdf.mpdf_to_num(ci)
            #jlib.log('{ci_xref=}')
        infodict["collection"] = ci_xref
        name = mupdf.mpdf_to_text_string(mupdf.mpdf_dict_get(o, PDF_NAME('F')))
        #jlib.log('{name=}')
        infodict[dictkey_filename] = JM_EscapeStrFromStr(name)

        name = mupdf.mpdf_to_text_string(mupdf.mpdf_dict_get(o, PDF_NAME('UF')))
        infodict[dictkey_ufilename] = JM_EscapeStrFromStr(name)

        name = mupdf.mpdf_to_text_string(mupdf.mpdf_dict_get(o, PDF_NAME('Desc')))
        infodict[dictkey_desc] = JM_UnicodeFromStr(name)

        len = -1
        DL = -1
        fileentry = mupdf.mpdf_dict_getl(o, PDF_NAME('EF'), PDF_NAME('F'))
        xref = mupdf.mpdf_to_num(fileentry)
        o = mupdf.mpdf_dict_get(fileentry, PDF_NAME('Length'))
        if o.m_internal:
            len = mupdf.mpdf_to_int(o)

        o = mupdf.mpdf_dict_get(fileentry, PDF_NAME('DL'))
        if o.m_internal:
            DL = mupdf.mpdf_to_int(o)
        else:
            o = mupdf.mpdf_dict_getl(fileentry, PDF_NAME('Params'), PDF_NAME('Size'))
            if o.m_internal:
                DL = mupdf.mpdf_to_int(o)
        infodict[dictkey_size] = DL
        infodict[dictkey_length] = len
        return xref

    def _embfile_upd(self, idx, buffer_=None, filename=None, ufilename=None, desc=None):
        #return _fitz.Document__embfile_upd(self, idx, buffer, filename, ufilename, desc)
        pdf = self._pdf_document()
        #fz_buffer *res = NULL;
        #fz_var(res);
        xref = 0
        names = mupdf.mpdf_dict_getl(
                mupdf.mpdf_trailer(pdf),
                PDF_NAME('Root'),
                PDF_NAME('Names'),
                PDF_NAME('EmbeddedFiles'),
                PDF_NAME('Names'),
                )

        entry = mupdf.mpdf_array_get(names, 2*idx+1)

        filespec = mupdf.mpdf_dict_getl(entry, PDF_NAME('EF'), PDF_NAME('F'))
        if not filespec.m_internal:
            THROWMSG("bad PDF: /EF object not found")
        res = JM_BufferFromBytes(buffer_)
        if buffer_ and buffer_.m_internal and not res.m_internal:
            THROWMSG("bad type: 'buffer'")
        if res.m_internal and buffer_ and buffer_.m_internal:
            JM_update_stream(pdf, filespec, res, 1)
            # adjust /DL and /Size parameters
            len, _ = mupdf.mfz_buffer_storage(res)
            l = mupdf.mpdf_new_int(len)
            mupdf.mpdf_dict_put(filespec, PDF_NAME('DL'), l)
            mupdf.mpdf_dict_putl(filespec, l, PDF_NAME('Params'), PDF_NAME('Size'))
        xref = mupdf.mpdf_to_num(filespec)
        if filename:
            mupdf.mpdf_dict_put_text_string(entry, PDF_NAME('F'), filename)

        if ufilename:
            mupdf.mpdf_dict_put_text_string(entry, PDF_NAME('UF'), ufilename)

        if desc:
            mupdf.mpdf_dict_put_text_string(entry, PDF_NAME('Desc'), desc)
        return xref

    def _embeddedFileGet(self, idx):
        #return _fitz.Document__embeddedFileGet(self, idx)
        #fz_document *doc = (fz_document *) self;
        #PyObject *cont = NULL;
        pdf = self._pdf_document()
        #fz_buffer *buf = NULL;
        #fz_var(buf);
        #fz_try(gctx) {
        names = mupdf.mpdf_dict_getl(
                mupdf.mpdf_trailer(pdf),
                PDF_NAME('Root'),
                PDF_NAME('Names'),
                PDF_NAME('EmbeddedFiles'),
                PDF_NAME('Names'),
                )

        entry = mupdf.mpdf_array_get(names, 2*idx+1)
        filespec = mupdf.mpdf_dict_getl(entry, PDF_NAME('EF'), PDF_NAME('F'));
        buf = mupdf.mpdf_load_stream(filespec);
        cont = JM_BinFromBuffer(buf)
        return cont

    def _embfile_add(self, name, buffer_, filename=None, ufilename=None, desc=None):
        #return _fitz.Document__embfile_add(self, name, buffer, filename, ufilename, desc)
        doc = self.this
        pdf = self._pdf_document()
        ASSERT_PDF(pdf);
        data = JM_BufferFromBytes(buffer_)
        if not data.m_internal:
            THROWMSG("bad type: 'buffer'")
        size, buffdata = data.buffer_storage_raw()

        names = mupdf.mpdf_dict_getl(
                mupdf.mpdf_trailer(pdf),
                PDF_NAME('Root'),
                PDF_NAME('Names'),
                PDF_NAME('EmbeddedFiles'),
                PDF_NAME('Names'),
                )
        if not mupdf.mpdf_is_array(names):
            root = mupdf.mpdf_dict_get(mupdf.mpdf_trailer(pdf), PDF_NAME('Root'))
            names = mupdf.mpdf_new_array(pdf, 6)    # an even number!
            mupdf.mpdf_dict_putl(
                    root,
                    names,
                    PDF_NAME('Names'),
                    PDF_NAME('EmbeddedFiles'),
                    PDF_NAME('Names'),
                    )

        fileentry = JM_embed_file(pdf, data, filename, ufilename, desc, 1)
        xref = mupdf.mpdf_to_num(
                mupdf.mpdf_dict_getl(fileentry, PDF_NAME('EF'), PDF_NAME('F'))
                )
        mupdf.mpdf_array_push(names, mupdf.mpdf_new_text_string(name))
        mupdf.mpdf_array_push(names, fileentry)
        return xref

    def _embfile_names(self, namelist):
        """Get list of embedded file names."""
        if self.is_closed:
            raise ValueError("document closed")

        #return _fitz.Document__embfile_names(self, namelist)
        doc = self.this
        pdf = self._pdf_document()
        ASSERT_PDF(pdf);
        names = mupdf.mpdf_dict_getl(
                mupdf.mpdf_trailer(pdf),
                PDF_NAME('Root'),
                PDF_NAME('Names'),
                PDF_NAME('EmbeddedFiles'),
                PDF_NAME('Names'),
                )
        if mupdf.mpdf_is_array(names):
            n = mupdf.mpdf_array_len(names)
            for i in range(0, n, 2):
                val = JM_EscapeStrFromStr(
                        mupdf.mpdf_to_text_string(
                            mupdf.mpdf_array_get(names, i)
                            )
                        )
                namelist.append(val)

    def _extend_toc_items(self, items):
        """Add color info to all items of an extended TOC list."""
        if self.isClosed:
            raise ValueError("document closed")
        #return _fitz.Document__extend_toc_items(self, items)
        pdf = mupdf.mpdf_specifics(self.this)
        #PyObject *item=NULL, *itemdict=NULL, *xrefs, *bold, *italic, *collapse, *zoom;
        zoom = "zoom"
        bold = "bold"
        italic = "italic"
        collapse = "collapse"

        root = mupdf.mpdf_dict_get(mupdf.mpdf_trailer(pdf), PDF_NAME('Root'))
        if not root.m_internal:
            return
        olroot = mupdf.mpdf_dict_get(root, PDF_NAME('Outlines'))
        if not olroot.m_internal:
            return
        first = mupdf.mpdf_dict_get(olroot, PDF_NAME('First'))
        if not first.m_internal:
            return
        xrefs = []
        xrefs = JM_outline_xrefs(first, xrefs)
        n = len(xrefs)
        if not n:
            return

        # update all TOC item dictionaries
        for i in range(n):
            xref = int(xrefs[i])
            item = items[i]
            itemdict = item[3]
            if not isinstance(itemdict, dict):
                THROWMSG("need non-simple TOC format")
            itemdict[dictkey_xref] = xrefs[i]
            bm = mupdf.mpdf_load_object(pdf, xref)
            flags = mupdf.mpdf_to_int( mupdf.mpdf_dict_get(bm, PDF_NAME('F')))
            if flags == 1:
                itemdict[italic] = True
            elif flags == 2:
                itemdict[bold] = True
            elif flags == 3:
                itemdict[italic] = True
                itemdict[bold] = True
            count = mupdf.mpdf_to_int( mupdf.mpdf_dict_get(bm, PDF_NAME('Count')))
            if count < 0:
                itemdict[collapse] = True
            elif count > 0:
                itemdict[collapse] = False
            col = mupdf.mpdf_dict_get(bm, PDF_NAME('C'))
            if mupdf.mpdf_is_array(col) and mupdf.mpdf_array_len(col) == 3:
                color = (
                        mupdf.mpdf_to_real(mupdf.mpdf_array_get(col, 0)),
                        mupdf.mpdf_to_real(mupdf.mpdf_array_get(col, 1)),
                        mupdf.mpdf_to_real(mupdf.mpdf_array_get(col, 2)),
                        )
                itemdict[dictkey_color] = color
            z=0
            obj = mupdf.mpdf_dict_get(bm, PDF_NAME('Dest'))
            if not obj.m_internal or not mupdf.mpdf_is_array(obj):
                obj = mupdf.mpdf_dict_getl(bm, PDF_NAME('A'), PDF_NAME('D'))
            if mupdf.mpdf_is_array(obj) and mupdf.mpdf_array_len(obj) == 5:
                z = mupdf.mpdf_to_real(mupdf.mpdf_array_get(obj, 4))
            itemdict[zoom] = float(z)
            item[3] = itemdict
            items[i] = item

    def _get_char_widths(self, xref: int, bfname: str, ext: str, ordering: int, limit: int, idx: int = 0):
        pdf = self._pdf_document()
        mylimit = limit;
        if mylimit < 256:
            mylimit = 256

        ASSERT_PDF(pdf), f'pdf={pdf}'
        if ordering >= 0:
            data, size, index = mupdf.mfz_lookup_cjk_font(ordering);
            font = mupdf.mfz_new_font_from_memory(None, data, size, index, 0);
        else:
            data, size = mupdf.mfz_lookup_base14_font(bfname)
            if data:
                font = mupdf.mfz_new_font_from_memory(bfname, data, size, 0, 0)
            else:
                buf = JM_get_fontbuffer(pdf, xref)
                if not buffer.m_internal:
                    raise Exception("font at xref %d is not supported" % xref)

                font = mupdf.mfz_new_font_from_buffer(NULL, buf, idx, 0)
        wlist = []
        for i in range(mylimit):
            glyph = mupdf.mfz_encode_character(font, i)
            adv = mupdf.mfz_advance_glyph(font, glyph, 0)
            if ordering >= 0:
                glyph = i
            if glyph > 0:
                wlist.append( (glyph, adv))
            else:
                wlist.append( (glyph, 0.0))
        return wlist

    def _getCharWidths(self, xref, bfname, ext, ordering, limit, idx=0):
        """Return list of glyphs and glyph widths of a font."""
        assert 0, 'no Document__getCharWidths'
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        return _fitz.Document__getCharWidths(self, xref, bfname, ext, ordering, limit, idx)

    def _getMetadata(self, key):
        """Get metadata."""
        if self.isClosed:
            raise ValueError("document closed")
        # return self.this _fitz.Document__getMetadata(self, key)
        return self.this.lookup_metadata(key)

    def _getPageInfo(self, pno, what):
        """List fonts, images, XObjects used on a page."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        #return _fitz.Document__getPageInfo(self, pno, what)
        doc = self.this
        pdf = self._pdf_document()
        pageCount = doc.count_pages()
        n = pno;  # pno < 0 is allowed
        while n < 0:
            n += pageCount  # make it non-negative
        if n >= pageCount:
            raise Exception("bad page number(s)")
        pageref = pdf.lookup_page_obj(n)
        rsrc = pageref.dict_get_inheritable(mupdf.PDF_ENUM_NAME_Resources)
        liste = []
        tracer = []
        if rsrc.m_internal:
            JM_scan_resources(pdf, rsrc, liste, what, 0, tracer)
        return liste

    def _getPDFfileid(self):
        """Get PDF file id."""
        if self.isClosed:
            raise ValueError("document closed")
        #return _fitz.Document__getPDFfileid(self)
        doc = self.this
        pdf = mupdf.mpdf_specifics(doc)
        if not pdf.m_internal:
            return
        idlist = []
        identity = mupdf.mpdf_dict_get(mupdf.mpdf_trailer(pdf), PDF_NAME('ID'));
        if identity.m_internal:
            n = mupdf.mpdf_array_len(identity)
            for i in range(n):
                o = mupdf.mpdf_array_get(identity, i)
                text = mupdf.mpdf_to_text_string(o)
                hex_ = binascii.hexlify(text)
                idlist.append(hex_)
        return idlist

    @property
    def _hasXrefOldStyle(self):
        """Check if xref table is old style."""
        assert 0, 'no Document__hasXrefOldStyle'
        if self.isClosed:
            raise ValueError("document closed")
        return _fitz.Document__hasXrefOldStyle(self)

    @property
    def _hasXrefStream(self):
        """Check if xref table is a stream."""
        assert 0, 'no Document__hasXrefStream'
        if self.isClosed:
            raise ValueError("document closed")
        return _fitz.Document__hasXrefStream(self)

    def _loadOutline(self):
        """Load first outline."""
        if self.isClosed:
            raise ValueError("document closed")
        return Outline(self.this.load_outline())

    def _newPage(self, pno=-1, width=595, height=842):
        """Make a new PDF page."""
        assert 0, 'deprecated'
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        val = _fitz.Document__newPage(self, pno, width, height)
        self._reset_page_refs()
        return val

    def _remove_links_to(self, first, last):
        assert 0, 'no Document__remove_links_to'
        return _fitz.Document__remove_links_to(self, first, last)

    def authenticate(self, password):
        """Decrypt document."""
        if self.isClosed:
            raise ValueError("document closed")
        #val = _fitz.Document_authenticate(self, password)
        val = mupdf.mfz_authenticate_password(self.this, password)
        if val:  # the doc is decrypted successfully and we init the outline
            self.isEncrypted = False
            self.initData()
            self.thisown = True
        return val

    def can_save_incrementally(self):
        """Check whether incremental saves are possible."""
        if self.isClosed:
            raise ValueError("document closed")

        #jlib.log('*** calling self.this.document_from_fz_document()')
        pdf = self.this.document_from_fz_document()
        if not pdf.m_internal:
            return False
        return pdf.can_be_saved_incrementally()

    @property
    def chapter_count(self):
        """Number of chapters."""
        if self.isClosed:
            raise ValueError("document closed")
        #return _fitz.Document_chapter_count(self)
        return mupdf.mfz_count_chapters( self.this)

    def chapterPageCount(self, chapter):
        """Page count of chapter."""
        assert 0, 'no Document_chapterPageCount'
        if self.isClosed:
            raise ValueError("document closed")
        return _fitz.Document_chapterPageCount(self, chapter)

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

    def convert_to_pdf(self, from_page=0, to_page=-1, rotate=0):
        """Convert document to a PDF, selecting page range and optional rotation. Output bytes object."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        #return _fitz.Document_convert_to_pdf(self, from_page, to_page, rotate)
        fz_doc = self.this
        fp = from_page
        tp = to_page
        srcCount = mupdf.mfz_count_pages(fz_doc)
        if fp < 0:
            fp = 0
        if fp > srcCount - 1:
            fp = srcCount - 1
        if tp < 0:
            tp = srcCount - 1
        if tp > srcCount - 1:
            tp = srcCount - 1
        doc = JM_convert_to_pdf(fz_doc, fp, tp, rotate)
        return doc

    def embfile_add(self, name: str, buffer_: typing.ByteString,
                              filename: OptStr =None,
                              ufilename: OptStr =None,
                              desc: OptStr =None,) -> None:
        """Add an item to the EmbeddedFiles array.

        Args:
            name: name of the new item, must not already exist.
            buffer_: (binary data) the file content.
            filename: (str) the file name, default: the name
            ufilename: (unicode) the file name, default: filename
            desc: (str) the description.
        """
        filenames = self.embfile_names()
        msg = "Name '%s' already exists." % str(name)
        if name in filenames:
            raise ValueError(msg)

        if filename is None:
            filename = name
        if ufilename is None:
            ufilename = unicode(filename, "utf8") if str is bytes else filename
        if desc is None:
            desc = name
        xref = self._embfile_add(name, buffer_=buffer_,
                                     filename=filename,
                                     ufilename=ufilename,
                                     desc=desc)
        date = get_pdf_now()
        self.xref_set_key(xref, "Type", "/EmbeddedFile")
        self.xref_set_key(xref, "Params/CreationDate", get_pdf_str(date))
        self.xref_set_key(xref, "Params/ModDate", get_pdf_str(date))
        return xref

    def embfile_count(self) -> int:
        """Get number of EmbeddedFiles."""
        return len(self.embfile_names())

    def embfile_del(self, item: typing.Union[int, str]):
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
        return self._embfile_del(idx)

    def embfile_get(self, item: typing.Union[int, str]) -> bytes:
        """Get the content of an item in the EmbeddedFiles array.

        Args:
            item: number or name of item.
        Returns:
            (bytes) The file content.
        """
        idx = self._embeddedFileIndex(item)
        return self._embeddedFileGet(idx)

    def embfile_info(self, item: typing.Union[int, str]) -> dict:
        """Get information of an item in the EmbeddedFiles array.

        Args:
            item: number or name of item.
        Returns:
            Information dictionary.
        """
        idx = self._embeddedFileIndex(item)
        infodict = {"name": self.embfile_names()[idx]}
        #jlib.log('{idx=} {infodict=}')
        xref = self._embfile_info(idx, infodict)
        #jlib.log('{idx=} {xref=} {infodict=}')
        t, date = self.xref_get_key(xref, "Params/CreationDate")
        if t != "null":
            infodict["creationDate"] = date
        t, date = self.xref_get_key(xref, "Params/ModDate")
        if t != "null":
            infodict["modDate"] = date
        t, md5 = self.xref_get_key(xref, "Params/CheckSum")
        if t != "null":
            infodict["checksum"] = binascii.hexlify(md5.encode()).decode()
        return infodict

    def embfile_names(self) -> list:
        """Get list of names of EmbeddedFiles."""
        filenames = []
        self._embfile_names(filenames)
        return filenames

    def embfile_upd(self, item: typing.Union[int, str],
                             buffer_: OptBytes =None,
                             filename: OptStr =None,
                             ufilename: OptStr =None,
                             desc: OptStr =None,) -> None:
        """Change an item of the EmbeddedFiles array.

        Notes:
            Only provided parameters are changed. If all are omitted,
            the method is a no-op.
        Args:
            item: number or name of item.
            buffer_: (binary data) the new file content.
            filename: (str) the new file name.
            ufilename: (unicode) the new filen ame.
            desc: (str) the new description.
        """
        idx = self._embeddedFileIndex(item)
        xref = self._embfile_upd(idx, buffer_=buffer_,
                                     filename=filename,
                                     ufilename=ufilename,
                                     desc=desc)
        date = get_pdf_now()
        self.xref_set_key(xref, "Params/ModDate", get_pdf_str(date))
        return xref

    def get_char_widths(doc, xref: int, limit: int = 256, idx: int = 0, fontdict: OptDict = None
            ) -> list:
        """Get list of glyph information of a font.

        Notes:
            Must be provided by its XREF number. If we already dealt with the
            font, it will be recorded in doc.FontInfos. Otherwise we insert an
            entry there.
            Finally we return the glyphs for the font. This is a list of
            (glyph, width) where glyph is an integer controlling the char
            appearance, and width is a float controlling the char's spacing:
            width * fontsize is the actual space.
            For 'simple' fonts, glyph == ord(char) will usually be true.
            Exceptions are 'Symbol' and 'ZapfDingbats'. We are providing data for these directly here.
        """
        fontinfo = CheckFontInfo(doc, xref)
        if fontinfo is None:  # not recorded yet: create it
            if fontdict is None:
                name, ext, stype, asc, dsc = _get_font_properties(doc, xref)
                fontdict = {
                    "name": name,
                    "type": stype,
                    "ext": ext,
                    "ascender": asc,
                    "descender": dsc,
                }
            else:
                #jlib.log('{type(fontdict)=}')
                name = fontdict["name"]
                ext = fontdict["ext"]
                stype = fontdict["type"]
                ordering = fontdict["ordering"]
                simple = fontdict["simple"]

            if ext == "":
                raise ValueError("xref is not a font")

            # check for 'simple' fonts
            if stype in ("Type1", "MMType1", "TrueType"):
                simple = True
            else:
                simple = False

            # check for CJK fonts
            if name in ("Fangti", "Ming"):
                ordering = 0
            elif name in ("Heiti", "Song"):
                ordering = 1
            elif name in ("Gothic", "Mincho"):
                ordering = 2
            elif name in ("Dotum", "Batang"):
                ordering = 3
            else:
                ordering = -1

            fontdict["simple"] = simple

            if name == "ZapfDingbats":
                glyphs = zapf_glyphs
            elif name == "Symbol":
                glyphs = symbol_glyphs
            else:
                glyphs = None

            fontdict["glyphs"] = glyphs
            fontdict["ordering"] = ordering
            fontinfo = [xref, fontdict]
            doc.FontInfos.append(fontinfo)
        else:
            fontdict = fontinfo[1]
            glyphs = fontdict["glyphs"]
            simple = fontdict["simple"]
            ordering = fontdict["ordering"]

        if glyphs is None:
            oldlimit = 0
        else:
            oldlimit = len(glyphs)

        mylimit = max(256, limit)

        if mylimit <= oldlimit:
            return glyphs

        if ordering < 0:  # not a CJK font
            glyphs = doc._get_char_widths(
                xref, fontdict["name"], fontdict["ext"], fontdict["ordering"], mylimit, idx
            )
        else:  # CJK fonts use char codes and width = 1
            glyphs = None

        fontdict["glyphs"] = glyphs
        fontinfo[1] = fontdict
        UpdateFontInfo(doc, fontinfo)

        return glyphs

    def get_outline_xrefs(self):
        """Get list of outline xref numbers."""
        if self.isClosed:
            raise ValueError("document closed")
        #return _fitz.Document_get_outline_xrefs(self)
            xrefs = []
            pdf = mupdf.mpdf_specifics(self.this)
            if not pdf.m_internal:
                return xrefs
            root = mupdf.mpdf_dict_get(mupdf.mpdf_trailer(pdf), PDF_NAME('Root'))
            if not root.m_internal:
                return xrefs
            olroot = mupdf.mpdf_dict_get(root, PDF_NAME('Outlines'))
            if not olroot.m_internal:
                return xrefs
            first = mupdf.mpdf_dict_get(olroot, PDF_NAME('First'))
            if not first.m_internal:
                return xrefs
            xrefs = JM_outline_xrefs(first, xrefs)
            return xrefs

    def get_page_labels(doc):
        return utils.get_page_labels(doc)

    def get_page_numbers(doc, label, only_one=False):
        return utils.get_page_numbers(doc, label, only_one)

    @property
    def is_closed(self):
        return self.isClosed

    @property
    def is_encrypted(self):
        return self.isEncrypted

    @property
    def is_pdf(self):
        return self.isPDF

    @property
    def isReflowable(self):
        """Check if document is layoutable."""
        assert 0, 'no Document_isReflowable'
        if self.isClosed:
            raise ValueError("document closed")
        return _fitz.Document_isReflowable(self)

    def loadPage(self, page_id):
        """Load a page.

        'page_id' is either a 0-based page number or a tuple (chapter, pno),
        with chapter number and page number within that chapter.
        """
        #jlib.log('@@@ loadPage()')
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        if page_id is None:
            page_id = 0
        if page_id not in self:
            raise ValueError("page not in document")
        if type(page_id) is int and page_id < 0:
            #jlib.log('handling -ve {page_id=}')
            np = self.page_count
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

    def new_page_(
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
        #jlib.log('{=mupdf.mpdf_xref_len(pdf)}')
        mediabox = mupdf.Rect(mupdf.Rect.Fixed_UNIT)
        mediabox.x1 = width
        mediabox.y1 = height
        contents = mupdf.Buffer()
        #jlib.log('{=contents} {contents.buffer_storage_raw()=}')
        if pno < -1:
            raise Exception("bad page number(s)")
        #jlib.log('{=mupdf.mpdf_xref_len(pdf)}')
        # create /Resources and /Contents objects
        #resources = pdf.add_object(pdf.new_dict(1))
        resources = mupdf.mpdf_add_new_dict(pdf, 1)
        #jlib.log('after pdf_add_new_dict(): {=mupdf.mpdf_xref_len(pdf)}')
        page_obj = pdf.add_page(mediabox, 0, resources, contents)
        #jlib.log('after add_page(): {=mupdf.mpdf_xref_len(pdf)}')
        pdf.insert_page(pno, page_obj)
        #jlib.log('after insert_page(): {=mupdf.mpdf_xref_len(pdf)}')
        # fixme: pdf->dirty = 1;

        self._reset_page_refs()
        #jlib.log('after _reset_page_refs(): {=mupdf.mpdf_xref_len(pdf)}')
        return self[pno]

    def new_page(
            self,
            pno: int = -1,
            width: float = 595,
            height: float = 842,
            ):# -> Page:
        ret = self.new_page_(pno, width, height)
        self._reset_page_refs()
        return ret

    def embeddedFileAdd(
            self,
            name: str,
            buffer_: typing.ByteString,
            filename: OptStr =None,
            ufilename: OptStr =None,
            desc: OptStr =None,
            ) -> None:
        """Add an item to the EmbeddedFiles array.

        Args:
            name: name of the new item, must not already exist.
            buffer_: (binary data) the file content.
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
        return self._embeddedFileAdd(
                name,
                buffer_=buffer_,
                filename=filename,
                ufilename=ufilename,
                desc=desc,
                )

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

    def embeddedFileGet(self, item: typing.Union[int, str]) -> bytes:
        """Get the content of an item in the EmbeddedFiles array.

        Args:
            item: number or name of item.
        Returns:
            (bytes) The file content.
        """
        idx = self._embeddedFileIndex(item)
        return self._embeddedFileGet(idx)

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

    def embeddedFileNames(self) -> list:
        """Get list of names of EmbeddedFiles."""
        filenames = []
        self._embeddedFileNames(filenames)
        return filenames

    def embeddedFileUpd(
            self,
            item: typing.Union[int, str],
            buffer_: OptBytes =None,
            filename: OptStr =None,
            ufilename: OptStr =None,
            desc: OptStr =None,
            ) -> None:
        """Change an item of the EmbeddedFiles array.

        Notes:
            Only provided parameters are changed. If all are omitted,
            the method is a no-op.
        Args:
            item: number or name of item.
            buffer_: (binary data) the new file content.
            filename: (str) the new file name.
            ufilename: (unicode) the new filen ame.
            desc: (str) the new description.
        """
        idx = self._embeddedFileIndex(item)
        return self._embeddedFileUpd(
                idx,
                buffer_=buffer_,
                filename=filename,
                ufilename=ufilename,
                desc=desc,
                )

    def extractFont(self, xref=0, info_only=0):
        """Get a font by xref."""
        assert 0, 'no Document_extractFont'
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        #return _fitz.Document_extractFont(self, xref, info_only)

    def extract_image(self, xref):
        """Get image by xref. Returns a dictionary."""
        if self.is_closed or self.is_encrypted:
            raise ValueError("document closed or encrypted")

        #return _fitz.Document_extract_image(self, xref)
        #pdf_document *pdf = pdf_specifics(gctx, (fz_document *) self);
        pdf = self._pdf_document()
        #pdf_obj *obj = NULL;
        #fz_buffer *res = NULL;
        #fz_image *img = NULL;
        #PyObject *rc = NULL;
        #const char *ext = NULL;
        #const char *cs_name = NULL;
        img_type = 0
        smask = 0
        #fz_compressed_buffer *cbuf = NULL;
        #fz_var(img);
        #fz_var(res);
        #fz_var(obj);

        ASSERT_PDF(pdf);
        if not INRANGE(xref, 1, mupdf.mpdf_xref_len(pdf)-1):
            THROWMSG("bad xref")

        obj = mupdf.mpdf_new_indirect(pdf, xref, 0)
        subtype = mupdf.mpdf_dict_get(obj, PDF_NAME('Subtype'))

        if not mupdf.mpdf_name_eq(subtype, PDF_NAME('Image')):
            THROWMSG("not an image")

        o = mupdf.mpdf_dict_geta(obj, PDF_NAME('SMask'), PDF_NAME('Mask'))
        if o.m_internal:
            smask = mupdf.mpdf_to_num(o)

        if mupdf.mpdf_is_jpx_image(obj):
            img_type = mupdf.FZ_IMAGE_JPX
            ext = "jpx"
        if JM_is_jbig2_image(obj):
            img_type = mupdf.FZ_IMAGE_JBIG2
            ext = "jb2"
        res = mupdf.mpdf_load_raw_stream(obj)
        if img_type == mupdf.FZ_IMAGE_UNKNOWN:
            _, c = res.buffer_storage_raw()
            img_type = mupdf.mfz_recognize_image_format(c)
            ext = JM_image_extension(img_type)
        if img_type == mupdf.FZ_IMAGE_UNKNOWN:
            #fz_drop_buffer(gctx, res);
            res = None
            img = mupdf.mpdf_load_image(pdf, obj)
            res = mupdf.mfz_new_buffer_from_image_as_png(img, mupdf.ColorParams())
            ext = "png"
        #} else /*if (smask == 0)*/ {
        else:
            img = mupdf.mfz_new_image_from_buffer(res)
        xres, yres = mupdf.mfz_image_resolution(img)
        width = img.w()
        height = img.h()
        colorspace = img.n()
        bpc = img.bpc()
        cs_name = mupdf.mfz_colorspace_name(img.colorspace())

        rc = dict()
        rc[ dictkey_ext] = ext
        rc[ dictkey_smask] = smask
        rc[ dictkey_width] = width
        rc[ dictkey_height] = height
        rc[ dictkey_colorspace] = colorspace
        rc[ dictkey_bpc] = bpc
        rc[ dictkey_xres] = xres
        rc[ dictkey_yres] = yres
        rc[ dictkey_cs_name] = cs_name
        rc[ dictkey_image] =JM_BinFromBuffer(res)
        return rc


    def get_toc(
            doc,#: Document,
            simple: bool = True,
            ) -> list:
        """Create a table of contents.

        Args:
            simple: a bool to control output. Returns a list, where each entry consists of outline level, title, page number and link destination (if simple = False). For details see PyMuPDF's documentation.
        """
        return utils.get_toc(doc, simple)

    def init_doc(self):
        if self.is_encrypted:
            raise ValueError("cannot initialize - document still encrypted")
        self._outline = self._loadOutline()
        self.metadata = dict(
                    [
                    (k,self._getMetadata(v)) for k,v in {
                        'format':'format',
                        'title':'info:Title',
                        'author':'info:Author',
                        'subject':'info:Subject',
                        'keywords':'info:Keywords',
                        'creator':'info:Creator',
                        'producer':'info:Producer',
                        'creationDate':'info:CreationDate',
                        'modDate':'info:ModDate',
                        'trapped':'info:Trapped'
                        }.items()
                    ]
                )
        self.metadata['encryption'] = None if self._getMetadata('encryption')=='None' else self._getMetadata('encryption')

    outline = property(lambda self: self._outline)


    def get_page_fonts(self, pno: int, full: bool =False) -> list:
        """Retrieve a list of fonts used on a page.
        """
        if self.is_closed or self.is_encrypted:
            raise ValueError("document closed or encrypted")
        if not self.is_pdf:
            return ()
        if type(pno) is not int:
            try:
                pno = pno.number
            except:
                raise ValueError("need a Page or page number")
        val = self._getPageInfo(pno, 1)
        if full is False:
            return [v[:-1] for v in val]
        return val

    @property
    def needsPass(self):
        """Indicate password required."""
        if self.isClosed:
            raise ValueError("document closed")
        return self.this.needs_password()

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
        #return _fitz.Document_nextLocation(self, page_id)
        assert 0, 'no Document_nextLocation'

    def insert_pdf(
            self,
            docsrc,
            from_page=-1,
            to_page=-1,
            start_at=-1,
            rotate=-1,
            links=1,
            annots=1,
            show_progress=0,
            final=1,
            _gmap=None,
            ):
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

        # Insert pages from a source PDF into this PDF.
        # For reconstructing the links (_do_links method), we must save the
        # insertion point (start_at) if it was specified as -1.
        if self.is_closed or self.is_encrypted:
            raise ValueError("document closed or encrypted")
        if self._graft_id == docsrc._graft_id:
            raise ValueError("source and target cannot be same object")
        sa = start_at
        if sa < 0:
            sa = self.page_count
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

        doc = self.this
        pdfout = mupdf.mpdf_specifics(doc)
        pdfsrc = mupdf.mpdf_specifics(docsrc.this)
        outCount = mupdf.mfz_count_pages(doc)
        srcCount = mupdf.mfz_count_pages(docsrc.this)

        # local copies of page numbers
        fp = from_page
        tp = to_page
        sa = start_at

        # normalize page numbers
        fp = max(fp, 0) # -1 = first page
        fp = min(fp, srcCount - 1)  # but do not exceed last page

        if tp < 0:
            tp = srcCount - 1   # -1 = last page
        tp = min(tp, srcCount - 1)  # but do not exceed last page

        if sa < 0:
            sa = outCount   # -1 = behind last page
        sa = min(sa, outCount)  # but that is also the limit

        if not pdfout.m_internal or not pdfsrc.m_internal:
            THROWMSG("source or target not a PDF")
        ENSURE_OPERATION(pdfout)
        JM_merge_range(pdfout, pdfsrc, fp, tp, sa, rotate, links, annots, show_progress, _gmap)

        self._reset_page_refs()
        if links:
            self._do_links(docsrc, from_page = from_page, to_page = to_page, start_at = sa)
        if final == 1:
            self.Graftmaps[isrt] = None

    @property
    def isDirty(self):
        """True if PDF has unsaved changes."""
        if self.isClosed:
            raise ValueError("document closed")
        #return _fitz.Document_isDirty(self)
        assert 0, 'no Document_isDirty'

    @property
    def language(self):
        """Document language."""
        if self.isClosed:
            raise ValueError("document closed")
        #return _fitz.Document_language(self)
        pdf = mupdf.mpdf_specifics(self.this)
        if not pdf.m_internal:
            return
        lang = mupdf.mpdf_document_language(pdf)
        if lang == mupdf.FZ_LANG_UNSET:
            return
        assert 0, 'not implemented yet'
        #char buf[8];
        #return PyUnicode_FromString(fz_string_from_text_language(buf, lang));

    def findBookmark(self, bm):
        """Find new location after layouting a document."""
        assert 0, 'no Document_findBookmark'
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        return _fitz.Document_findBookmark(self, bm)

    def get_oc(self, xref):
        """Get xref of optional content object."""
        assert 0, 'no Document_get_oc'
        if self.isClosed:
            raise ValueError("document closed")
        return _fitz.Document_get_oc(self, xref)

    @property
    def isPDF(self):
        """Check for PDF."""
        #jlib.log('.isPDF')
        if isinstance(self.this, mupdf.PdfDocument):
            return True
        # Avoid calling self.this.specifics() because it will end up creating
        # a new PdfDocument which will call pdf_create_document(), which is ok
        # but a little unnecessary.
        #
        #jlib.log('isPDF(): {jlib.exception_info()}')
        if mupdf.ppdf_specifics(self.this.m_internal):
            ret = True
        else:
            ret = False
        #jlib.log('Returning {ret=}')
        return ret
        return True if self.this.specifics().m_internal else False
        #if self.isClosed:
        #    raise ValueError("document closed")
        #if isinstance(self.this, mupdf.PdfDocument):
        #    return True
        #jlib.log('calling self.this.specifics()')
        #p = self.this.specifics()
        #pp = self.this.specifics()
        #return True if pp.m_internal else False

    @property
    def lastLocation(self):
        """Id (chapter, page) of last page."""
        assert 0, 'no Document_lastLocation'
        if self.isClosed:
            raise ValueError("document closed")
        return _fitz.Document_lastLocation(self)

    def layout(self, rect=None, width=0, height=0, fontsize=11):
        """Re-layout a reflowable document."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        val = _fitz.Document_layout(self, rect, width, height, fontsize)
        doc = self.this
        if not mupdf.mfz_is_document_reflowable(doc):
            return
        w = width
        h = height
        r = JM_rect_from_py(rect)
        if not mupdf.mfz_is_infinite_rect(r):
            w = r.x1 - r.x0
            h = r.y1 - r.y0
        if w <= 0.0 or h <= 0.0:
            THROWMSG("invalid page size")
        mupdf.mfz_layout_document(doc, w, h, fontsize)
        self._reset_page_refs()
        self.initData()
        return

    def location_from_page_number(self, pno):
        """Convert pno to (chapter, page)."""
        if self.isClosed:
            raise ValueError("document closed")
        #return _fitz.Document_location_from_page_number(self, pno)
        this_doc = self.this
        loc = mupdf.mfz_make_location(-1, -1)
        ipage_count = mupdf.mfz_count_pages(this_doc)
        while pno < 0:
            pno += page_count
        if pno >= page_count:
            THROWMSG("bad page number(s)")
        loc = mupdf.mfz_location_from_page_number(this_doc, pno)
        return loc.chapter, loc.page

    def make_bookmark(self, loc):
        """Make a page pointer before layouting document."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        #return _fitz.Document_make_bookmark(self, loc)
        #jlib.log('{loc=}')
        loc = mupdf.Location(*loc)
        mark = mupdf.mfz_make_bookmark( self.this, loc)
        return mark

    @property
    def page_count(self):
        """Number of pages."""
        if self.isClosed:
            raise ValueError("document closed")
        ret = self.this.count_pages()
        return ret

    def page_number_from_location(self, page_id):
        """Convert (chapter, pno) to page number."""
        if type(page_id) is int:
            np = self.page_count
            while page_id < 0:
                page_id += np
            page_id = (0, page_id)
        if page_id not in self:
            raise ValueError("page id not in document")
        #return _fitz.Document_page_number_from_location(self, page_id)
        this_doc = self.this
        page_n = -1
        loc = mupdf.mfz_make_location(page_id[0], page_id[1])
        page_n = mupdf.mfz_page_number_from_location(this_doc, loc)
        return page_n

    def page_xref(self, pno):
        """Get xref of page number."""
        if self.isClosed:
            raise ValueError("document closed")
        #return _fitz.Document_page_xref(self, pno)
        this_doc = self.this
        page_count = mupdf.mfz_count_pages(this_doc)
        n = pno;
        while n < 0:
            n += page_count
        pdf = mupdf.mpdf_specifics(this_doc)
        xref = 0
        if n >= page_count:
            THROWMSG("bad page number(s)")
        ASSERT_PDF(pdf)
        xref = mupdf.mpdf_to_num(mupdf.mpdf_lookup_page_obj(pdf, n))
        return xref

    def pageCropBox(self, pno):
        """Get CropBox of page number (without loading page)."""
        assert 0, 'no Document_pageCropBox'
        if self.isClosed:
            raise ValueError("document closed")
        val = _fitz.Document_pageCropBox(self, pno)
        val = Rect(val)
        return val

    def pdf_catalog(self):
        """Get xref of PDF catalog."""
        if self.isClosed:
            raise ValueError("document closed")
        #return _fitz.Document_pdf_catalog(self)
        doc = self.this
        pdf = mupdf.mpdf_specifics(doc)
        xref = 0
        if not pdf.m_internal:
            return xref
        root = mupdf.mpdf_dict_get(mupdf.mpdf_trailer(pdf), PDF_NAME('Root'))
        xref = mupdf.mpdf_to_num(root)
        return xref

    @property
    def permissions(self):
        """Document permissions."""
        if self.isEncrypted:
            return 0
        #return _fitz.Document_permissions(self)
        doc =self.this
        jlib.log('*** calling mupdf.mpdf_document_from_fz_document(doc)')
        pdf = mupdf.mpdf_document_from_fz_document(doc)

        # for PDF return result of standard function
        if pdf.m_internal:
            return mupdf.mpdf_document_permissions(pdf)

        # otherwise simulate the PDF return value
        perm = 0xFFFFFFFC   # all permissions granted
        # now switch off where needed
        if not mupdf.mfz_has_permission(doc, mupdf.FZ_PERMISSION_PRINT):
            perm = perm ^ mupdf.PDF_PERM_PRINT
        if not mupdf.mfz_has_permission(doc, mupdf.FZ_PERMISSION_EDIT):
            perm = perm ^ mupdf.PDF_PERM_MODIFY;
        if not mupdf.mfz_has_permission(doc, mupdf.FZ_PERMISSION_COPY):
            perm = perm ^ mupdf.PDF_PERM_COPY
        if not mupdf.mfz_has_permission(doc, mupdf.FZ_PERMISSION_ANNOTATE):
            perm = perm ^ mupdf.PDF_PERM_ANNOTATE;
        return perm

    def previousLocation(self, page_id):
        """Get (chapter, page) of previous page."""
        assert 0, 'no Document_previousLocation'
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        if type(page_id) is int:
            page_id = (0, page_id)
        if page_id not in self:
            raise ValueError("page id not in document")
        if page_id  == (0, 0):
            return ()
        return _fitz.Document_previousLocation(self, page_id)

    def resolveLink(self, uri=None, chapters=0):
        """Calculate internal link destination.

        Args:
            uri: (str) some Link.uri
            chapters: (bool) whether to use (chapter, page) format
        Returns:
            (page_id, x, y) where x, y are point coordinates on the page.
            page_id is either page number (if chapters=0), or (chapter, pno).
        """
        assert 0, 'no Document_resolveLink'
        return _fitz.Document_resolveLink(self, uri, chapters)

    def save(
            self,
            filename,
            garbage=0,
            clean=0,
            deflate=0,
            deflate_images=0,
            deflate_fonts=0,
            incremental=0,
            ascii=0,
            expand=0,
            linear=0,
            no_new_id=0,
            appearance=0,
            pretty=0,
            encryption=1,
            permissions=4095,
            owner_pw=None,
            user_pw=None,
            ):
        # From %pythonprepend save
        #
        """Save PDF to file, pathlib.Path or file pointer."""
        #jlib.log('{filename=}')
        if self.is_closed or self.is_encrypted:
            raise ValueError("document closed or encrypted")
        if type(filename) == str:
            pass
        elif hasattr(filename, "open"):  # assume: pathlib.Path
            filename = str(filename)
        elif hasattr(filename, "name"):  # assume: file object
            filename = filename.name
        elif not hasattr(filename, "seek"):  # assume file object
            raise ValueError("filename must be str, Path or file object")
        if filename == self.name and not incremental:
            raise ValueError("save to original must be incremental")
        if self.page_count < 1:
            raise ValueError("cannot save with zero pages")
        if incremental:
            if self.name != filename or self.stream:
                raise ValueError("incremental needs original file")
        #return _fitz.Document_save(
        #        self,
        #        filename,
        #        garbage,
        #        clean,
        #        deflate,
        #        deflate_images,
        #        deflate_fonts,
        #        incremental,
        #        ascii,
        #        expand,
        #        linear,
        #        pretty,
        #        encryption,
        #        permissions,
        #        owner_pw,
        #        user_pw,
        #        )
        opts = mupdf.PdfWriteOptions()
        opts.do_incremental     = incremental
        opts.do_ascii           = ascii
        opts.do_compress        = deflate
        opts.do_compress_images = deflate_images
        opts.do_compress_fonts  = deflate_fonts
        opts.do_decompress      = expand
        opts.do_garbage         = garbage
        opts.do_pretty          = pretty
        opts.do_linear          = linear
        opts.do_clean           = clean
        opts.do_sanitize        = clean
        opts.dont_regenerate_id = no_new_id
        opts.do_appearance      = appearance
        opts.do_encrypt         = encryption
        opts.permissions        = permissions
        if owner_pw is not None:
            opts.opwd_utf8_set_value(owner_pw)
            #memcpy(&opts.opwd_utf8, owner_pw, strlen(owner_pw)+1);
        elif user_pw is not None:
            #memcpy(&opts.opwd_utf8, user_pw, strlen(user_pw)+1);
            opts.opwd_utf8_set_value(user_pw)
        if user_pw is not None:
            #memcpy(&opts.upwd_utf8, user_pw, strlen(user_pw)+1);
            opts.upwd_utf8_set_value(user_pw)

        pdf = self._pdf_document()
        out = None
        ASSERT_PDF(pdf)
        JM_embedded_clean(pdf)
        if no_new_id == 0:
            JM_ensure_identity(pdf)
        if isinstance(filename, str):
            #jlib.log('{filename=}')
            mupdf.mpdf_save_document(pdf, filename, opts)
        else:
            out = JM_new_output_fileptr(filename)
            mupdf.mpdf_write_document(pdf, out, opts)


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

        #val = _fitz.Document_select(self, pyliste)
        # preparatory stuff:
        # (1) get underlying pdf document,
        # (2) transform Python list into integer array

        pdf = mupdf.mpdf_specifics(self.this)
        # call retainpages (code copy of fz_clean_file.c)
        retainpages(pdf, pyliste);
        if pdf.m_internal.rev_page_map:
            mupdf.mpdf_drop_page_tree(pdf)

        Py_RETURN_NONE;
        self._reset_page_refs()
        return val

    def set_oc(self, xref, oc):
        """Attach optional content object to image or form xobject."""
        if self.isClosed:
            raise ValueError("document closed")

        return _fitz.Document_set_oc(self, xref, oc)

    def set_page_labels(doc, labels):
        """Add / replace page label definitions in PDF document.

        Args:
            doc: PDF document (resp. 'self').
            labels: list of label dictionaries like:
            {'startpage': int, 'prefix': str, 'style': str, 'firstpagenum': int},
            as returned by get_page_labels().
        """
        # William Chapman, 2021-01-06

        def create_label_str(label):
            """Convert Python label dict to correspnding PDF rule string.

            Args:
                label: (dict) build rule for the label.
            Returns:
                PDF label rule string wrapped in "<<", ">>".
            """
            s = "%i<<" % label["startpage"]
            if label.get("prefix", "") != "":
                s += "/P(%s)" % label["prefix"]
            if label.get("style", "") != "":
                s += "/S/%s" % label["style"]
            if label.get("firstpagenum", 1) > 1:
                s += "/St %i" % label["firstpagenum"]
            s += ">>"
            return s

        def create_nums(labels):
            """Return concatenated string of all labels rules.

            Args:
                labels: (list) dictionaries as created by function 'rule_dict'.
            Returns:
                PDF compatible string for page label definitions, ready to be
                enclosed in PDF array 'Nums[...]'.
            """
            labels.sort(key=lambda x: x["startpage"])
            s = "".join([create_label_str(label) for label in labels])
            return s

        doc._set_page_labels(create_nums(labels))

    def set_toc(
            doc,#: Document,
            toc: list,
            collapse: int = 1,
            ) -> int:
        """Create new outline tree (table of contents, TOC).

        Args:
            toc: (list, tuple) each entry must contain level, title, page and
                optionally top margin on the page. None or '()' remove the TOC.
            collapse: (int) collapses entries beyond this level. Zero or None
                shows all entries unfolded.
        Returns:
            the number of inserted items, or the number of removed items respectively.
        """
        if doc.is_closed or doc.is_encrypted:
            raise ValueError("document closed or encrypted")
        if not doc.is_pdf:
            raise ValueError("not a PDF")
        if not toc:  # remove all entries
            r = doc._delToC()
            ret = len(r)
            return ret

        # validity checks --------------------------------------------------------
        if type(toc) not in (list, tuple):
            raise ValueError("'toc' must be list or tuple")
        toclen = len(toc)
        page_count = doc.page_count
        t0 = toc[0]
        if type(t0) not in (list, tuple):
            raise ValueError("items must be sequences of 3 or 4 items")
        if t0[0] != 1:
            raise ValueError("hierarchy level of item 0 must be 1")
        for i in list(range(toclen - 1)):
            t1 = toc[i]
            t2 = toc[i + 1]
            if not -1 <= t1[2] <= page_count:
                raise ValueError("row %i: page number out of range" % i)
            if (type(t2) not in (list, tuple)) or len(t2) not in (3, 4):
                raise ValueError("bad row %i" % (i + 1))
            if (type(t2[0]) is not int) or t2[0] < 1:
                raise ValueError("bad hierarchy level in row %i" % (i + 1))
            if t2[0] > t1[0] + 1:
                raise ValueError("bad hierarchy level in row %i" % (i + 1))
        # no formal errors in toc --------------------------------------------------

        # --------------------------------------------------------------------------
        # make a list of xref numbers, which we can use for our TOC entries
        # --------------------------------------------------------------------------
        old_xrefs = doc._delToC()  # del old outlines, get their xref numbers

        # prepare table of xrefs for new bookmarks
        old_xrefs = []
        xref = [0] + old_xrefs
        xref[0] = doc._getOLRootNumber()  # entry zero is outline root xref number
        if toclen > len(old_xrefs):  # too few old xrefs?
            for i in range((toclen - len(old_xrefs))):
                xref.append(doc.get_new_xref())  # acquire new ones

        lvltab = {0: 0}  # to store last entry per hierarchy level

        # ------------------------------------------------------------------------------
        # contains new outline objects as strings - first one is the outline root
        # ------------------------------------------------------------------------------
        olitems = [{"count": 0, "first": -1, "last": -1, "xref": xref[0]}]
        # ------------------------------------------------------------------------------
        # build olitems as a list of PDF-like connnected dictionaries
        # ------------------------------------------------------------------------------
        for i in range(toclen):
            o = toc[i]
            lvl = o[0]  # level
            title = get_pdf_str(o[1])  # title
            pno = min(doc.page_count - 1, max(0, o[2] - 1))  # page number
            page_xref = doc.page_xref(pno)
            page_height = doc.page_cropbox(pno).height
            top = Point(72, page_height - 36)
            dest_dict = {"to": top, "kind": LINK_GOTO}  # fall back target
            if o[2] < 0:
                dest_dict["kind"] = LINK_NONE
            if len(o) > 3:  # some target is specified
                if type(o[3]) in (int, float):  # convert a number to a point
                    dest_dict["to"] = Point(72, page_height - o[3])
                else:  # if something else, make sure we have a dict
                    dest_dict = o[3] if type(o[3]) is dict else dest_dict
                    if "to" not in dest_dict:  # target point not in dict?
                        dest_dict["to"] = top  # put default in
                    else:  # transform target to PDF coordinates
                        point = +dest_dict["to"]
                        point.y = page_height - point.y
                        dest_dict["to"] = point
            d = {}
            d["first"] = -1
            d["count"] = 0
            d["last"] = -1
            d["prev"] = -1
            d["next"] = -1
            d["dest"] = getDestStr(page_xref, dest_dict)
            d["top"] = dest_dict["to"]
            d["title"] = title
            d["parent"] = lvltab[lvl - 1]
            d["xref"] = xref[i + 1]
            d["color"] = dest_dict.get("color")
            d["flags"] = dest_dict.get("italic", 0) + 2 * dest_dict.get("bold", 0)
            lvltab[lvl] = i + 1
            parent = olitems[lvltab[lvl - 1]]  # the parent entry

            if (
                dest_dict.get("collapse") or collapse and lvl > collapse
            ):  # suppress expansion
                parent["count"] -= 1  # make /Count negative
            else:
                parent["count"] += 1  # positive /Count

            if parent["first"] == -1:
                parent["first"] = i + 1
                parent["last"] = i + 1
            else:
                d["prev"] = parent["last"]
                prev = olitems[parent["last"]]
                prev["next"] = i + 1
                parent["last"] = i + 1
            olitems.append(d)

        # ------------------------------------------------------------------------------
        # now create each outline item as a string and insert it in the PDF
        # ------------------------------------------------------------------------------
        for i, ol in enumerate(olitems):
            txt = "<<"
            if ol["count"] != 0:
                txt += "/Count %i" % ol["count"]
            try:
                txt += ol["dest"]
            except:
                pass
            try:
                if ol["first"] > -1:
                    txt += "/First %i 0 R" % xref[ol["first"]]
            except:
                pass
            try:
                if ol["last"] > -1:
                    txt += "/Last %i 0 R" % xref[ol["last"]]
            except:
                pass
            try:
                if ol["next"] > -1:
                    txt += "/Next %i 0 R" % xref[ol["next"]]
            except:
                pass
            try:
                if ol["parent"] > -1:
                    txt += "/Parent %i 0 R" % xref[ol["parent"]]
            except:
                pass
            try:
                if ol["prev"] > -1:
                    txt += "/Prev %i 0 R" % xref[ol["prev"]]
            except:
                pass
            try:
                txt += "/Title" + ol["title"]
            except:
                pass

            if ol.get("color") and len(ol["color"]) == 3:
                txt += "/C[ %g %g %g]" % tuple(ol["color"])
            if ol.get("flags", 0) > 0:
                txt += "/F %i" % ol["flags"]

            if i == 0:  # special: this is the outline root
                txt += "/Type/Outlines"  # so add the /Type entry
            txt += ">>"
            doc.update_object(xref[i], txt)  # insert the PDF object

        doc.init_doc()
        return toclen


    def setLanguage(self, language=None):
        return _fitz.Document_setLanguage(self, language)

    def write(
            self,
            garbage=False,
            clean=False,
            deflate=False,
            deflate_images=False,
            deflate_fonts=False,
            incremental=False,
            ascii=False,
            expand=False,
            linear=False,
            no_new_id=False,
            appearance=False,
            pretty=False,
            encryption=1,
            permissions=4095,
            owner_pw=None,
            user_pw=None
            ):
        from io import BytesIO
        bio = BytesIO()
        self.save(
                bio,
                garbage=garbage,
                clean=clean,
                no_new_id=no_new_id,
                appearance=appearance,
                deflate=deflate,
                deflate_images=deflate_images,
                deflate_fonts=deflate_fonts,
                incremental=incremental,
                ascii=ascii,
                expand=expand,
                linear=linear,
                pretty=pretty,
                encryption=encryption,
                permissions=permissions,
                owner_pw=owner_pw,
                user_pw=user_pw,
                )
        return bio.getvalue()

    tobytes = write

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

    @property
    def xref(self):
        """PDF xref number of page."""
        CheckParent(self)
        return self.parent.page_xref(self.number)

    def xref_get_key(self, xref, key):
        """Get PDF dict key value of object at 'xref'."""
        if self.is_closed:
            raise ValueError("document closed")

        #return _fitz.Document_xref_get_key(self, xref, key)
        pdf = self._pdf_document()
        ASSERT_PDF(pdf);
        xreflen = mupdf.mpdf_xref_len(pdf)
        if not INRANGE(xref, 1, xreflen-1) and xref != -1:
            THROWMSG(f"bad xref={xref} xreflen={xreflen}")
        if xref > 0:
            obj = mupdf.mpdf_load_object(pdf, xref)
        else:
            obj = mupdf.mpdf_trailer(pdf)
        if not obj.m_internal:
            return ("null", "null")
        subobj = mupdf.mpdf_dict_getp(obj, key)
        if not subobj.m_internal:
            return ("null", "null")
        text = None
        if mupdf.mpdf_is_indirect(subobj):
            type = "xref"
            text = "%i 0 R" % mupdf.mpdf_to_num(subobj)
        elif mupdf.mpdf_is_array(subobj):
            type = "array"
        elif mupdf.mpdf_is_dict(subobj):
            type = "dict"
        elif mupdf.mpdf_is_int(subobj):
            type = "int"
            text = "%i" % mupdf.mpdf_to_int(subobj)
        elif mupdf.mpdf_is_real(subobj):
            type = "float"
        elif mupdf.mpdf_is_null(subobj):
            type = "null"
            text = "null"
        elif mupdf.mpdf_is_bool(subobj):
            type = "bool"
            if mupdf.mpdf_to_bool(subobj):
                text = "true"
            else:
                text = "false"
        elif mupdf.mpdf_is_name(subobj):
            type = "name"
            text = "/%s" % mupdf.mpdf_to_name(subobj)
        elif mupdf.mpdf_is_string(subobj):
            type = "string"
            text = JM_UnicodeFromStr(mupdf.mpdf_to_text_string(subobj))
        else:
            type = "unknown";
        if text is None:
            res = JM_object_to_buffer(subobj, 1, 0)
            text = JM_UnicodeFromBuffer(res)
        #rc = Py_BuildValue("sO", type, text);
        return (type, text)

    def xref_length(self):
        """Get length of xref table."""
        if self.isClosed:
            raise ValueError("document closed")
        #return _fitz.Document_xref_length(self)
        xreflen = 0
        #pdf_document *pdf = pdf_specifics(gctx, (fz_document *) self);
        pdf = self._pdf_document()
        if pdf:
            xreflen = mupdf.mpdf_xref_len(pdf)
        return xreflen


    def xref_object(self, xref, compressed=0, ascii=0):
        """Get xref object source as a string."""
        if self.isClosed:
            raise ValueError("document closed")
        #return _fitz.Document_xref_object(self, xref, compressed, ascii)
        if 1:
            pdf = self._pdf_document()
            #pdf_obj *obj = NULL;
            #PyObject *text = NULL;
            #fz_buffer *res=NULL;
            ASSERT_PDF(pdf);
            xreflen = mupdf.mpdf_xref_len(pdf)
            if not INRANGE(xref, 1, xreflen-1) and xref != -1:
                THROWMSG("bad xref")
            if xref > 0:
                obj = mupdf.mpdf_load_object(pdf, xref)
            else:
                obj = mupdf.mpdf_trailer(pdf)
            res = JM_object_to_buffer(mupdf.mpdf_resolve_indirect(obj), compressed, ascii)
            text = JM_EscapeStrFromBuffer(res)
            return text

    def xref_set_key(self, xref, key, value):
        """Set the value of a PDF dictionary key."""
        if self.is_closed:
            raise ValueError("document closed")
        #return _fitz.Document_xref_set_key(self, xref, key, value)
        pdf = self._pdf_document()
        ASSERT_PDF(pdf)
        if not key:
            THROWMSG("bad 'key'")
        if not value:
            THROWMSG("bad 'value'")
        xreflen = mupdf.mpdf_xref_len(pdf)
        if not INRANGE(xref, 1, xreflen-1) and xref != -1:
            THROWMSG("bad xref")
        if len(value) == 0:
            THROWMSG("bad 'value'")
        if len(key) == 0:
            THROWMSG("bad 'key'")
        if xref != -1:
            obj = mupdf.mpdf_load_object(pdf, xref)
        else:
            obj = mupdf.mpdf_trailer(pdf)
        new_obj = JM_set_object_value(obj, key, value)
        if not new_obj.m_internal:
            return  # did not work: skip update
        if xref != -1:
            #pdf_drop_obj(gctx, obj);
            #obj = NULL;
            mupdf.mpdf_update_object(pdf, xref, new_obj)
        else:
            n = mupdf.mpdf_dict_len(new_obj)
            for i in range(n):
                mupdf.mpdf_dict_put(
                        obj,
                        mupdf.mpdf_dict_get_key(new_obj, i),
                        mupdf.mpdf_dict_get_val(new_obj, i),
                        )

    def xref_stream(self, xref):
        """Get decompressed xref stream."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        return _fitz.Document_xref_stream(self, xref)

    def xref_stream_raw(self, xref):
        """Get xref stream without decompression."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")

        return _fitz.Document_xref_stream_raw(self, xref)

    def xref_xml_metadata(self):
        """Get xref of document XML metadata."""
        if self.isClosed:
            raise ValueError("document closed")
        return _fitz.Document_xref_xml_metadata(self)

    def getXmlMetadata(self):
        """Get document XML metadata."""
        if self.isClosed:
            raise ValueError("document closed")
        return _fitz.Document_getXmlMetadata(self)

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

    def pdf_trailer(self, compressed=0, ascii=0):
        """Get PDF trailer as a string."""
        if self.isClosed:
            raise ValueError("document closed")
        return _fitz.Document_pdf_trailer(self, compressed, ascii)

    def update_object(self, xref, text, page=None):
        """Replace object definition source."""
        if self.is_closed or self.is_encrypted:
            raise ValueError("document closed or encrypted")
        #return _fitz.Document_update_stream(self, xref, stream, new)
        pdf = self._pdf_document()
        ASSERT_PDF(pdf)
        xreflen = mupdf.mpdf_xref_len(pdf)
        if not INRANGE(xref, 1, xreflen-1):
            THROWMSG("bad xref")
        ENSURE_OPERATION(pdf)
        # create new object with passed-in string
        new_obj = JM_pdf_obj_from_str(pdf, text)
        mupdf.mpdf_update_object(pdf, xref, new_obj)
        if page:
            JM_refresh_page( mupdf.mpdf_page_from_fz_page(page.super()))

    def _pdf_document(self):
        '''
        Returns self.this as a mupdf.PdfDocument using pdf_specifics() as
        requireds.
        '''
        if isinstance(self.this, mupdf.PdfDocument):
            return self.this
        #return self.this.specifics()
        return mupdf.PdfDocument(self.this)

    def update_stream(self, xref=0, stream=None, new=0):
        """Replace xref stream part."""
        if self.isClosed or self.isEncrypted:
            raise ValueError("document closed or encrypted")
        #return _fitz.Document_update_stream(self, xref, stream, new)
        pdf = self._pdf_document()
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
        #return _fitz.Document__get_page_labels(self)
        pdf = self._pdf_document()

        ASSERT_PDF(pdf);
        rc = []
        pagelabels = mupdf.mpdf_new_name("PageLabels")
        obj = mupdf.mpdf_dict_getl( mupdf.mpdf_trailer(pdf), PDF_NAME('Root'), pagelabels)
        if not obj.m_internal:
            return rc
        # simple case: direct /Nums object
        nums = mupdf.mpdf_resolve_indirect( mupdf.mpdf_dict_get( obj, PDF_NAME('Nums')))
        if nums.m_internal:
            JM_get_page_labels(rc, nums)
            return rc
        # case: /Kids/Nums
        nums = mupdf.mpdf_resolve_indirect( mupdf.mpdf_dict_getl(obj, PDF_NAME('Kids'), PDF_NAME('Nums')))
        if nums.m_internal:
            JM_get_page_labels(rc, nums)
            return rc
        # case: /Kids is an array of multiple /Nums
        kids = mupdf.mpdf_resolve_indirect( mupdf.mpdf_dict_get( obj, PDF_NAME('Kids')))
        if not kids.m_internal or not mupdf.mpdf_is_array(kids):
            return rc
        n = mupdf.mpdf_array_len(kids)
        for i in range(n):
            nums = mupdf.mpdf_resolve_indirect(
                    mupdf.mpdf_dict_get( mupdf.mpdf_array_get(kids, i)),
                    PDF_NAME('Nums'),
                    )
            JM_get_page_labels(rc, nums)
        return rc

    def _set_page_labels(self, labels):
        #val = _fitz.Document__set_page_labels(self, labels)
        #pdf_document *pdf = pdf_specifics(gctx, (fz_document *) self);
        pdf = self._pdf_document()
        ASSERT_PDF(pdf)
        pagelabels = mupdf.mpdf_new_name("PageLabels")
        root = mupdf.mpdf_dict_get(mupdf.mpdf_trailer(pdf), PDF_NAME('Root'))
        mupdf.mpdf_dict_del(root, pagelabels)
        mupdf.mpdf_dict_putl(root, mupdf.mpdf_new_array(pdf, 0), pagelabels, PDF_NAME('Nums'))

        xref = self.pdf_catalog()
        text = self.xref_object(xref, compressed=True)
        text = text.replace("/Nums[]", "/Nums[%s]" % labels)
        self.update_object(xref, text)

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
        #return _fitz.Document_add_ocg(self, name, config, on, intent, usage)
        xref = 0
        #pdf_obj *obj = NULL, *cfg = NULL;
        #pdf_obj *indocg = NULL;
        pdf = self._pdf_document();
        ASSERT_PDF(pdf);

        # make the OCG
        ocg = mupdf.mpdf_add_new_dict(pdf, 3)
        mupdf.mpdf_dict_put(ocg, PDF_NAME('Type'), PDF_NAME('OCG'))
        mupdf.mpdf_dict_put_text_string(ocg, PDF_NAME('Name'), name)
        intents = mupdf.mpdf_dict_put_array(ocg, PDF_NAME('Intent'), 2)
        if not intent:
            mupdf.mpdf_array_push(intents, PDF_NAME('View'))
        elif not isinstance(intent, str):
            assert 0, f'fixme: intent is not a str. type(intent)={type(intent)} type={type!r}'
            #n = len(intent)
            #for i in range(n):
            #    item = intent[i]
            #    c = JM_StrAsChar(item);
            #    if (c) {
            #        pdf_array_push(gctx, intents, pdf_new_name(gctx, c));
            #    }
            #    Py_DECREF(item);
            #}
        else:
            mupdf.mpdf_array_push(intents, mupdf.mpdf_new_name(intent))
        use_for = mupdf.mpdf_dict_put_dict(ocg, PDF_NAME('Usage'), 3)
        ci_name = mupdf.mpdf_new_name("CreatorInfo")
        cre_info = mupdf.mpdf_dict_put_dict(use_for, ci_name, 2)
        mupdf.mpdf_dict_put_text_string(cre_info, PDF_NAME('Creator'), "PyMuPDF")
        if usage:
            mupdf.mpdf_dict_put_name(cre_info, PDF_NAME('Subtype'), usage)
        else:
            mupdf.mpdf_dict_put_name(cre_info, PDF_NAME('Subtype'), "Artwork")
        indocg = mupdf.mpdf_add_object(pdf, ocg)

        # Insert OCG in the right config
        ocp = JM_ensure_ocproperties(pdf)
        obj = mupdf.mpdf_dict_get(ocp, PDF_NAME('OCGs'))
        mupdf.mpdf_array_push(obj, indocg)

        if config > -1:
            obj = mupdf.mpdf_dict_get(ocp, PDF_NAME('Configs'))
            if not mupdf.mpdf_is_array(obj):
                THROWMSG("bad config number")
            cfg = mupdf.mpdf_array_get(obj, config)
            if not cfg.m_internal:
                THROWMSG("bad config number")
        else:
            cfg = mupdf.mpdf_dict_get(ocp, PDF_NAME('D'))

        obj = mupdf.mpdf_dict_get(cfg, PDF_NAME('Order'))
        if not obj.m_internal:
            obj = mupdf.mpdf_dict_put_array(cfg, PDF_NAME('Order'), 1)
        mupdf.mpdf_array_push(obj, indocg)
        if on:
            obj = mupdf.mpdf_dict_get(cfg, PDF_NAME('ON'))
            if not obj.m_internal:
                obj = mupdf.mpdf_dict_put_array(cfg, PDF_NAME('ON'), 1)
        else:
            obj =mupdf.mpdf_dict_get(cfg, PDF_NAME('OFF'))
            if not obj.m_internal:
                obj =mupdf.mpdf_dict_put_array(cfg, PDF_NAME('OFF'), 1)
        mupdf.mpdf_array_push(obj, indocg)

        # let MuPDF take note: re-read OCProperties
        mupdf.mpdf_read_ocg(pdf)

        xref = mupdf.mpdf_to_num(indocg)
        return xref

    addOCG = add_ocg

    def initData(self):
        if self.isEncrypted:
            raise ValueError("cannot initData - document still encrypted")
        self._outline = self._loadOutline()
        self.metadata = dict(
                [(k, self._getMetadata(v))
                    for k,v in {
                        'format':'format',
                        'title':'info:Title',
                        'author':'info:Author',
                        'subject':'info:Subject',
                        'keywords':'info:Keywords',
                        'creator':'info:Creator',
                        'producer':'info:Producer',
                        'creationDate':'info:CreationDate',
                        'modDate':'info:ModDate',
                        'trapped':'info:Trapped',
                        }.items()
                    ]
                )
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

        pageCount = self.page_count
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

        pageCount = self.page_count  # page count of document
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
            #annot.parent = page_proxy  # refresh parent to new page
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
        if type(loc) is int:
            if loc < self.page_count:
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
            start += self.page_count
        if start not in range(self.page_count):
            raise ValueError("bad start page number")

        # set the stop value
        stop = stop if stop is not None and stop <= self.page_count else self.page_count

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
        return self.page_count

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

    '''
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
    '''
    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.close()

    def page_annot_xrefs(self, n):
        page_count = self.this.count_pages()
        #jlib.log('*** {page_count=}')
        while n < 0:
            n += page_count
        #jlib.log('self.this is: {self.this}')
        #jlib.log('{self.this.m_internal=}')
        if 0:
            #jlib.log('dir(self.this):')
            for i in dir(self.this):
                pass
                #jlib.log('    {i}')
        #jlib.log('self.this.specifics is: {self.this.specifics}')
        if isinstance(self.this, mupdf.PdfDocument):
            #jlib.log('self.this is a mupdf.PdfDocument')
            pdf_document = self.this
        else:
            #jlib.log('self.this is not a mupdf.PdfDocument: {self.this}')
            #jlib.log('{self.this.m_internal=}')
            pdf_document = self.this.specifics()
            #jlib.log('self.this.specifics() => {pdf_document=}')
        page_obj = pdf_document.lookup_page_obj(n)
        #jlib.log('page_obj: {dir_str(page_obj)}')
        #jlib.log('page_obj.m_internal: {dir_str(page_obj.m_internal)}')
        annots = JM_get_annot_xref_list(page_obj)
        #jlib.log('{self.this.count_pages()=} {self.page_count=}')
        return annots

    def has_links(self):
        """Check whether there are links on any page."""
        #jlib.log('has_links()')
        if self.isClosed:
            raise ValueError("document closed")
        if not self.isPDF:
            raise ValueError("not a PDF")
        #jlib.log('{self.page_count=}')
        #jlib.log('calling self.page_count')
        for i in range(self.page_count):
            #jlib.log('{i=}')
            for item in self.page_annot_xrefs(i):
                if item[1] == mupdf.PDF_ANNOT_LINK:
                    jlib.log('Returning true')
                    return True
        #jlib.log('Returning false')
        return False

    def has_annots(self):
        """Check whether there are annotations on any page."""
        #jlib.log('has_annots()')
        if self.isClosed:
            raise ValueError("document closed")
        if not self.isPDF:
            raise ValueError("not a PDF")
        for i in range(self.page_count):
            for item in self.page_annot_xrefs(i):
                if not (item[1] == mupdf.PDF_ANNOT_LINK or item[1] == mupdf.PDF_ANNOT_WIDGET):
                    return True
        return False

    @property
    def is_form_pdf(self):
        """Either False or PDF field count."""
        #jlib.log('{self.this.count_pages()=}')
        pdf = self.this.specifics()
        if not pdf.m_internal:
            return False
        #jlib.log('{self.this.count_pages()=}')
        count = -1;
        try:
            #jlib.log('{self.this.count_pages()=}')
            fields = mupdf.mpdf_dict_getl(
                    pdf.trailer(),
                    mupdf.PDF_ENUM_NAME_Root,
                    mupdf.PDF_ENUM_NAME_AcroForm,
                    mupdf.PDF_ENUM_NAME_Fields,
                    )
            #jlib.log('{fields=} {fields.is_array()=} {self.this.count_pages()=}')
            if fields.is_array():
                count = fields.array_len()
            #jlib.log('{self.this.count_pages()=}')
        except Exception:
            return False
        #jlib.log('{count=}')
        #jlib.log('{self.this.count_pages()=}')
        if count >= 0:
            return count
        return False

    @property
    def is_repaired(self):
        """Check whether PDF was repaired."""
        #jlib.log('{self.this.count_pages()=}')
        #jlib.log('*** calling self.this.document_from_fz_document()')
        pdf = self.this.document_from_fz_document()
        if not pdf.m_internal:
            return False
        r = pdf.was_repaired()
        #jlib.log('{r=}')
        if r:
            #jlib.log('returning true')
            return True
        #jlib.log('returning false')
        return False

    @property
    def is_dirty(self):
        #jlib.log('{self.this.count_pages()=}')
        pdf = self.this.specifics()
        #jlib.log('{pdf.m_internal=}')
        if not pdf.m_internal:
            return False
        r = pdf.has_unsaved_changes()
        #jlib.log('{r=}')
        return True if r else False

open = Document


class Font:
    #__swig_setmethods__ = {}
    #__setattr__ = lambda self, name, value: _swig_setattr(self, Font, name, value)
    #__swig_getmethods__ = {}
    #__getattr__ = lambda self, name: _swig_getattr(self, Font, name)

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


        #this = _fitz.new_Font(fontname, fontfile, fontbuffer, script, language, ordering, is_bold, is_italic, is_serif)
        lang = mupdf.mfz_text_language_from_string(language)
        font = JM_get_font(fontname, fontfile,
                   fontbuffer, script, lang, ordering,
                   is_bold, is_italic, is_serif)

        self.this = font
        #try:
        #    self.this.append(this)
        #except __builtin__.Exception:
        #    self.this = this

    def char_lengths(self, text, fontsize=11, language=None, script=0, wmode=0, small_caps=0):
        """Return tuple of char lengths of unicode 'text' under a fontsize."""
        #return _fitz.Font_char_lengths(self, text, fontsize, language, script, wmode, small_caps)
        #fz_font *font, *thisfont = (fz_font *) self;
        lang = mupdf.mfz_text_language_from_string(language)
        #if not PyUnicode_Check(text) or PyUnicode_READY(text) != 0:
        #    THROWMSG("bad type: text")
        #len_ = PyUnicode_GET_LENGTH(text)
        #kind = PyUnicode_KIND(text)
        #data = PyUnicode_DATA(text)
        rc = []
        for ch in text:
            c = ord(ch)
            if small_caps:
                gid = mupdf.mfz_encode_character_sc(thisfont, c)
                if gid >= 0:
                    font = self.this
            else:
                font = mupdf.Font(0)
                gid = mupdf.mfz_encode_character_with_fallback(self.this, c, script, lang, font)
            rc.append(fontsize * mupdf.mfz_advance_glyph(font, gid, wmode))
        return rc

    def glyph_advance(self, chr_, language=None, script=0, wmode=0, small_caps=0):
        """Return the glyph width of a unicode (font size 1)."""

        #return _fitz.Font_glyph_advance(self, chr, language, script, wmode)
        #fz_font *font, *thisfont = (fz_font *) self;
        #int gid;
        lang = mupdf.mfz_text_language_from_string(language)
        if small_caps:
            gid = mupdf.mfz_encode_character_sc(thisfont, chr_)
            if gid >= 0:
                font = self.this
        else:
            font = mupdf.Font(0)
            gid = mupdf.mfz_encode_character_with_fallback(self.this, chr_, script, lang, font)
        return mupdf.mfz_advance_glyph(font, gid, wmode)


    def glyph_bbox(self, chr, language=None, script=0):
        """Return the glyph bbox of a unicode (font size 1)."""

        val = _fitz.Font_glyph_bbox(self, chr, language, script)
        val = Rect(val)

        return val


    def has_glyph(self, chr, language=None, script=0, fallback=0):
        """Check whether font has a glyph for this unicode."""

        return _fitz.Font_has_glyph(self, chr, language, script, fallback)


    @property
    def is_writable(self):
        #return _fitz.Font_is_writable(self)
        font = self.this
        if ( mupdf.mfz_font_t3_procs(font)
                or mupdf.mfz_font_flags(font).m_internal.ft_substitute
                or not mupdf.mpdf_font_writing_supported(font)
                ):
            return False
        return True


    def valid_codepoints(self):
        '''
        list of valid unicodes of a fz_font
        '''
        # fixme: not currently implemented. Only use cases within PyMuPDF
        # are in PyMuPDF/tests/test_font.py:test_font1() and _fitz.py:repair_mono_font().
        # The latter can be implemented using fz_glyph_count().
        return []
    '''
    def valid_codepoints(self):
        from array import array
        gc = self.glyph_count
        cp = array("l", (0,) * gc)
        arr = cp.buffer_info()
        self._valid_unicodes(arr)
        return array("l", sorted(set(cp))[1:])


    def _valid_unicodes(self, arr):
        #return _fitz.Font__valid_unicodes(self, arr)
        temp = arr[0]
        void *ptr = PyLong_AsVoidPtr(temp);
        JM_valid_chars(gctx, font, ptr);
        Py_DECREF(temp);
    '''

    @property
    def flags(self):
        #return _fitz.Font_flags(self)
        f = mupdf.mfz_font_flags(self.this)
        if not f.m_internal:
            return
        return {
                "mono": f.m_internal.is_mono,
                "serif": f.m_internal.is_serif,
                "bold": f.m_internal.is_bold,
                "italic": f.m_internal.is_italic,
                "substitute": f.m_internal.ft_substitute,
                "stretch": f.m_internal.ft_stretch,
                "fake-bold": f.m_internal.fake_bold,
                "fake-italic": f.m_internal.fake_italic,
                "opentype": f.m_internal.has_opentype,
                "invalid-bbox": f.m_internal.invalid_bbox,
                }

    @property
    def isWritable(self):
        return _fitz.Font_isWritable(self)

    @property
    def name(self):
        #return _fitz.Font_name(self)
        return mupdf.mfz_font_name(self.this)

    @property
    def glyph_count(self):
        #return _fitz.Font_glyph_count(self)
        return self.this.m_internal.glyph_count

    @property

    def buffer(self):
        #return _fitz.Font_buffer(self)
        buffer_ = mupdf.Buffer( mupdf.keep_buffer( self.this.m_internal.buffer))
        size, data = buffer_.buffer_extract_raw()
        return mupdf.raw_to_python_bytes( data, size)

    @property
    def bbox(self):
        val = _fitz.Font_bbox(self)
        val = Rect(val)

        return val

    @property
    def ascender(self):
        """Return the glyph ascender value."""
        #return _fitz.Font_ascender(self)
        return mupdf.mfz_font_ascender(self.this)

    @property
    def descender(self):
        """Return the glyph descender value."""
        #return _fitz.Font_descender(self)
        return mupdf.mfz_font_descender(self.this)

    def glyph_name_to_unicode(self, name):
        """Return the unicode for a glyph name."""
        return glyph_name_to_unicode(name)

    def unicode_to_glyph_name(self, ch):
        """Return the glyph name for a unicode."""
        return unicode_to_glyph_name(ch)

    def text_length(self, text, fontsize=11, language=None, script=0, wmode=0, small_caps=0):
        """Return length of unicode 'text' under a fontsize."""
        #return _fitz.Font_text_length(self, text, fontsize, language, script, wmode, small_caps)
        #fz_font *font=NULL, *thisfont = (fz_font *) self;
        thisfont = self.this
        lang = mupdf.mfz_text_language_from_string(language)
        rc = 0
        if not isinstance(text, str):
            THROWMSG("bad type: text");
        len_ = len(text)
        #kind = PyUnicode_KIND(text);
        #void *data = PyUnicode_DATA(text);
        #for i in range(len_):
        for ch in text:
            c = ord(ch)
            if small_caps:
                gid = mupdf.mfz_encode_character_sc(thisfont, c)
                if gid >= 0:
                    font = thisfont
            else:
                font = mupdf.Font(0)
                gid = mupdf.mfz_encode_character_with_fallback(thisfont, c, script, lang, font)
            rc += mupdf.mfz_advance_glyph(font, gid, wmode)
        rc *= fontsize
        return rc

    def __repr__(self):
        return "Font('%s')" % self.name

    def __del__(self):
        if type(self) is not Font:
            return None


class Graftmap:
    #__swig_setmethods__ = {}
    #__setattr__ = lambda self, name, value: _swig_setattr(self, Graftmap, name, value)
    #__swig_getmethods__ = {}
    #__getattr__ = lambda self, name: _swig_getattr(self, Graftmap, name)

    def __init__(self, doc):
        #this = _fitz.new_Graftmap(doc)
        dst = mupdf.mpdf_specifics(doc)
        ASSERT_PDF(dst)
        map_ = mupdf.mpdf_new_graft_map(dst)
        self.this = map_
        #return (struct Graftmap *) pdf_keep_graft_map(gctx, map_);
        self.thisown = True



    def __del__(self):
        if not type(self) is Graftmap:
            return
        self.thisown = False



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
                link_xrefs = [x[0] for x in self.parent.annot_xrefs() if x[1] == mupdf.PDF_ANNOT_LINK]
                link_ids = [x[2] for x in self.parent.annot_xrefs() if x[1] == mupdf.PDF_ANNOT_LINK]
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


class Matrix(object):
    def __init__(self, *args):
        """
        Matrix() - all zeros
        Matrix(a, b, c, d, e, f)
        Matrix(zoom-x, zoom-y) - zoom
        Matrix(shear-x, shear-y, 1) - shear
        Matrix(degree) - rotate
        Matrix(Matrix) - new copy
        Matrix(sequence) - from 'sequence'
        """
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

    def pretranslate(self, tx, ty):
        """Calculate pre translation and replace current matrix."""
        tx = float(tx)
        ty = float(ty)
        self.e += tx * self.a + ty * self.c
        self.f += tx * self.b + ty * self.d
        return self

    def prescale(self, sx, sy):
        """Calculate pre scaling and replace current matrix."""
        sx = float(sx)
        sy = float(sy)
        self.a *= sx
        self.b *= sx
        self.c *= sy
        self.d *= sy
        return self

    def preshear(self, h, v):
        """Calculate pre shearing and replace current matrix."""
        h = float(h)
        v = float(v)
        a, b = self.a, self.b
        self.a += v * self.c
        self.b += v * self.d
        self.c += h * a
        self.d += h * b
        return self

    def prerotate(self, theta):
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
    def is_rectilinear(self):
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

class linkDest(object):
    """link or outline destination details"""

    def __init__(self, obj, rlink):
        isExt = obj.is_external
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
        if obj.is_external:
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
        if obj.is_external:
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


class Widget(object):
    '''
    Class describing a PDF form field ("widget")
    '''
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
        #jlib.log('have created Widget, {self=} {type(self)=} {self.script=}')


    def _validate(self):
        """Validate the class entries.
        """
        if (self.rect.isInfinite
            or self.rect.is_empty
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
        doc = self.parent.this.doc()
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
        #return "'%s' widget on %s" % (self.field_type_string, str(self.parent))
        # No self.parent.
        return f'Widget:(field_type={self.field_type_string} script={self.script})'
        return "'%s' widget" % (self.field_type_string)

    @property
    def next(self):
        return self._annot.next


class Outline:
    #__swig_setmethods__ = {}
    #__setattr__ = lambda self, name, value: _swig_setattr(self, Outline, name, value)
    #__swig_getmethods__ = {}
    #__getattr__ = lambda self, name: _swig_getattr(self, Outline, name)

    def __init__(self, ol):
        self.this = ol

    @property
    def uri(self):
        #return _fitz.Outline_uri(self)
        ol = self.this
        if not ol.m_internal:
            return None
        return JM_UnicodeFromStr(ol.uri())

    @property
    def next(self):
        #return _fitz.Outline_next(self)
        ol = self.this
        next_ol = ol.next();
        if not next_ol.m_internal:
            return
        next_ol = mupdf.mfz_keep_outline(next_ol)
        return Outline(next_ol)

    @property
    def down(self):
        #return _fitz.Outline_down(self)
        ol = self.this
        down_ol = ol.down()
        if not down_ol.m_internal:
            return
        down_ol = mupdf.mfz_keep_outline(down_ol)
        return Outline (down_ol)

    @property
    def is_external(self):
        #return _fitz.Outline_isExternal(self)
        ol = self.this
        if not ol.m_internal:
            return False
        uri = ol.uri()
        if uri is None:
            return False
        return mupdf.mfz_is_external_link(uri)

    @property
    def page(self):
        #return _fitz.Outline_page(self)
        return self.this.page().page;

    @property
    def x(self):
        #return _fitz.Outline_x(self)
        return self.this.x();

    @property
    def y(self):
        #return _fitz.Outline_x(self)
        return self.this.y();

    @property
    def title(self):
        #return _fitz.Outline_title(self)
        return self.this.title()

    @property
    def is_open(self):
        #return _fitz.Outline_is_open(self)
        return self.this.is_open()

    @property
    def dest(self):
        '''outline destination details'''
        return linkDest(self, None)


class Page:

    def __init__(self, page):
        assert isinstance(page, (mupdf.Page, mupdf.PdfPage)), f'page is: {page}'
        self.this = page
        self.thisown = True
        self.lastPoint = None
        self.draw_cont = ''
        self._annot_refs = dict()

    def __str__(self):
        #CheckParent(self)
        parent = getattr(self, 'parent', None)
        #jlib.log('{self.this.m_internal=}')
        if isinstance(self.this.m_internal, mupdf.pdf_page):
            #jlib.log('{self.this.m_internal.super=}')
            number = self.this.m_internal.super.number
        else:
            j#lib.log('{self.this.m_internal=}')
            number = self.this.m_internal.number
        ret = f'page {number}'
        if parent:
            x = self.parent.name
            if self.parent.stream is not None:
                x = "<memory, doc# %i>" % (self.parent._graft_id,)
            if x == "":
                x = "<new PDF, doc# %i>" % self.parent._graft_id
            ret += f' of {x}'
        return ret

    def __repr__(self):
        return self.__str__()
        CheckParent(self)
        x = self.parent.name
        if self.parent.stream is not None:
            x = "<memory, doc# %i>" % (self.parent._graft_id,)
        if x == "":
            x = "<new PDF, doc# %i>" % self.parent._graft_id
        return "page %s of %s" % (self.number, x)

    def _add_caret_annot(self, point):
        #return _fitz.Page__add_caret_annot(self, point)
        page = self._pdf_page()
        annot = page.create_annot(mupdf.PDF_ANNOT_CARET)
        if point:
            p = JM_point_from_py(point)
            r = annot.annot_rect()
            r = mupdf.Rect(p.x, p.y, p.x + r.x1 - r.x0, p.y + r.y1 - r.y0)
            annot.set_annot_rect(r)
        JM_add_annot_id(annot, "A")
        annot.update_annot()
        return annot;


    def _add_file_annot(self, point, buffer_, filename, ufilename=None, desc=None, icon=None):
        #return _fitz.Page__add_file_annot(self, point, buffer, filename, ufilename, desc, icon)
        page = self._pdf_page()
        uf = ufilename if ufilename else filename
        d = desc if desc else filename
        #fz_buffer *filebuf = NULL;
        #fz_rect r;
        p = JM_point_from_py(point)
        try:
            ASSERT_PDF(page);
            filebuf = JM_BufferFromBytes(buffer_)
            if not filebuf.m_internal:
                THROWMSG("bad type: 'buffer'")
            annot = mupdf.mpdf_create_annot(page, mupdf.PDF_ANNOT_FILE_ATTACHMENT)
            r = mupdf.mpdf_annot_rect(annot)
            r = mupdf.mfz_make_rect(p.x, p.y, p.x + r.x1 - r.x0, p.y + r.y1 - r.y0)
            mupdf.mpdf_set_annot_rect(annot, r)
            flags = mupdf.PDF_ANNOT_IS_PRINT;
            mupdf.mpdf_set_annot_flags(annot, flags)

            if icon:
                mupdf.mpdf_set_annot_icon_name(annot, icon)

            val = JM_embed_file(page.doc(), filebuf, filename, uf, d, 1)
            mupdf.mpdf_dict_put(annot.annot_obj(), PDF_NAME('FS'), val)
            mupdf.mpdf_dict_put_text_string(annot.annot_obj(), PDF_NAME('Contents'), filename)
            JM_add_annot_id(annot, "A")
            mupdf.mpdf_update_annot(annot)
            mupdf.mpdf_set_annot_rect(annot, r)
            mupdf.mpdf_set_annot_flags(annot, flags)
        except Exception as e:
            jlib.log('{e=}')
            raise
            return
        return Annot(annot)

    def _add_freetext_annot(self, rect, text, fontsize=11, fontname=None, text_color=None, fill_color=None, align=0, rotate=0):
        #return _fitz.Page__add_freetext_annot(self, rect, text, fontsize, fontname, text_color, fill_color, align, rotate)
        page = self._pdf_page()
        fcol = [1, 1, 1, 1] # fill color: white
        nfcol = JM_color_FromSequence(fill_color, fcol)
        tcol = [0, 0, 0, 0]  # std. text color: black
        ntcol = JM_color_FromSequence(text_color, tcol)
        r = JM_rect_from_py(rect)
        if r.is_infinite_rect() or r.is_empty_rect():
            jlib.log('{rect=} {r=}')
            raise Exception("rect must be finite and not empty")
        annot = page.create_annot(mupdf.PDF_ANNOT_FREE_TEXT)
        annot.set_annot_contents(text)
        annot.set_annot_rect(r)
        annot.annot_obj().dict_put_int(mupdf.PDF_ENUM_NAME_Rotate, rotate)
        annot.annot_obj().dict_put_int(mupdf.PDF_ENUM_NAME_Q, align)

        if fill_color:
            annot.set_annot_color(fcol[:nfcol])

        # insert the default appearance string
        JM_make_annot_DA(annot, ntcol, tcol, fontname, fontsize)
        JM_add_annot_id(annot, "A")
        annot.update_annot()
        return Annot(annot)

    def _add_ink_annot(self, list):
        #return _fitz.Page__add_ink_annot(self, list)
        page = mupdf.mpdf_page_from_fz_page(self.this)
        #pdf_annot *annot = NULL;
        #PyObject *p = NULL, *sublist = NULL;
        #pdf_obj *inklist = NULL, *stroke = NULL;
        #fz_matrix ctm, inv_ctm;
        #fz_point point;
        #fz_var(annot);
        ASSERT_PDF(page);
        if not PySequence_Check(list):
            THROWMSG("arg must be a sequence")
        mupdf.mpdf_page_transform(page, mupdf.Rect(0), ctm)
        inv_ctm = mupdf.mfz_invert_matrix(ctm)
        annot = mupdf.mpdf_create_annot(page, mupdf.PDF_ANNOT_INK)
        annot_obj = mupdf.mpdf_annot_obj(annot)
        n0 = len(list)
        inklist = mupdf.mpdf_new_array(page.doc(), n0)

        for j in range(n0):
            sublist = list[j]
            n1 = len(sublist)
            stroke = mupdf.mpdf_new_array(page.doc(), 2 * n1)

            for i in range(n1):
                p = sublist[i]
                if not PySequence_Check(p) or PySequence_Size(p) != 2:
                    THROWMSG("3rd level entries must be pairs of floats")
                point = mupdf.mfz_transform_point(JM_point_from_py(p), inv_ctm)
                mupdf.mpdf_array_push_real(stroke, point.x)
                mupdf.mpdf_array_push_real(stroke, point.y)

            mupdf.mpdf_array_push(inklist, stroke)

        mupdf.mpdf_dict_put(annot_obj, PDF_NAME('InkList'), inklist)
        mupdf.mpdf_dirty_annot(annot)
        JM_add_annot_id(annot, "A")
        mupdf.mpdf_update_annot(annot)
        return Annot(annot)

    def _add_line_annot(self, p1, p2):
        #return _fitz.Page__add_line_annot(self, p1, p2)
        page = self._pdf_page()
        try:
            ASSERT_PDF(page);
            annot = mupdf.mpdf_create_annot(page, mupdf.PDF_ANNOT_LINE)
            a = JM_point_from_py(p1)
            b = JM_point_from_py(p2)
            mupdf.mpdf_set_annot_line(annot, a, b)
            JM_add_annot_id(annot, "A")
            mupdf.mpdf_update_annot(annot)
        except Exception as e:
            jlib.log('{e=}')
            return
        assert annot.m_internal
        return Annot(annot)

    def _add_multiline(self, points, annot_type):
        #return _fitz.Page__add_multiline(self, points, annot_type)
        page = self._pdf_page()
        try:
            if len(points) < 2:
                THROWMSG("bad list of points")
            annot = mupdf.mpdf_create_annot(page, annot_type)
            for p in points:
                if (PySequence_Size(p) != 2):
                    THROWMSG("bad list of points")
                point = JM_point_from_py(p)
                mupdf.mpdf_add_annot_vertex(annot, point)

            JM_add_annot_id(annot, "A")
            mupdf.mpdf_update_annot(annot)
        except Exception as e:
            jlib.log('{e=}')
            return;
        return Annot(annot)

    def _add_redact_annot(self, quad, text=None, da_str=None, align=0, fill=None, text_color=None):
        #return _fitz.Page__add_redact_annot(self, quad, text, da_str, align, fill, text_color)
        page = self._pdf_page()
        fcol = [ 1, 1, 1, 0]
        nfcol = 0
        try:
            annot = mupdf.mpdf_create_annot(page, mupdf.PDF_ANNOT_REDACT)
            q = JM_quad_from_py(quad)
            r = mupdf.mfz_rect_from_quad(q)

            # TODO calculate de-rotated rect
            mupdf.mpdf_set_annot_rect(annot, r)
            if fill:
                nfcol = JM_color_FromSequence(fill, fcol)
                arr = mupdf.mpdf_new_array(page.doc(), nfcol)
                for i in range(nfcol):
                    mupdf.mpdf_array_push_real(arr, fcol[i])
                mupdf.mpdf_dict_put(annot.annot_obj(), PDF_NAME('IC'), arr)
            if text:
                mupdf.mpdf_dict_puts(
                        annot.annot_obj(),
                        "OverlayText",
                        mupdf.mpdf_new_text_string(text),
                        )
                mupdf.mpdf_dict_put_text_string(annot.annot_obj(), PDF_NAME('DA'), da_str)
                mupdf.mpdf_dict_put_int(annot.annot_obj(), PDF_NAME('Q'), align)
            JM_add_annot_id(annot, "A")
            mupdf.mpdf_update_annot(annot)
        except Exception as e:
            jlib.log('{e=}')
            return
        annot = mupdf.mpdf_keep_annot(annot)
        return Annot(annot)

    def _addAnnot_FromString(self, linklist):
        """Add links from list of object sources."""
        CheckParent(self)

        return _fitz.Page__addAnnot_FromString(self, linklist)

    def _addWidget(self, field_type, field_name):
        #return _fitz.Page__addWidget(self, field_type, field_name)
        #jlib.log('{self.this=} {type(self.this)=}')
        page = self._pdf_page()
        pdf = page.doc()
        annot = JM_create_widget(pdf, page, field_type, field_name)
        if not annot.m_internal:
            THROWMSG("could not create widget")
        JM_add_annot_id(annot, "W")
        return Annot(annot)

    def _apply_redactions(self, images):
        #return _fitz.Page__apply_redactions(self, *args)
        page = self._pdf_page()
        opts = mupdf.PdfRedactOptions()
        opts.black_boxes = 0  # no black boxes
        opts.image_method = images  # how to treat images
        ASSERT_PDF(page)
        success = mupdf.mpdf_redact_page(page.doc(), page, opts)
        return success

    def _erase(self):
        self._reset_annot_refs()
        try:
            self.parent._forget_page(self)
        except:
            pass
        self.parent = None
        self.thisown = False
        self.number = None

    def _forget_annot(self, annot):
        """Remove an annot from reference dictionary."""
        aid = id(annot)
        if aid in self._annot_refs:
            self._annot_refs[aid] = None

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

    def _get_resource_properties(self):
        #return _fitz.Page__get_resource_properties(self)
        if 1:
            page = self._pdf_page()
            ASSERT_PDF(page);
            rc = JM_get_resource_properties(page.obj())
            return rc

    def _getDrawings(self):
        return _fitz.Page__getDrawings(self)

    def _add_square_or_circle(self, rect, annot_type):
        #return _fitz.Page__add_square_or_circle(self, rect, annot_type)
        page = self._pdf_page()
        try:
            r = JM_rect_from_py(rect)
            if mupdf.mfz_is_infinite_rect(r) or mupdf.mfz_is_empty_rect(r):
                THROWMSG("rect must be finite and not empty")
            annot = mupdf.mpdf_create_annot(page, annot_type)
            mupdf.mpdf_set_annot_rect(annot, r)
            JM_add_annot_id(annot, "A")
            mupdf.mpdf_update_annot(annot)
        except Exception as e:
            jlib.log('{e=}')
            return
        assert annot.m_internal
        return Annot(annot)

    def _add_stamp_annot(self, rect, stamp=0):
        #return _fitz.Page__add_stamp_annot(self, rect, stamp)
        page = self._pdf_page()
        #pdf_annot *annot = NULL;
        stamp_id = [
                PDF_NAME('Approved'),
                PDF_NAME('AsIs'),
                PDF_NAME('Confidential'),
                PDF_NAME('Departmental'),
                PDF_NAME('Experimental'),
                PDF_NAME('Expired'),
                PDF_NAME('Final'),
                PDF_NAME('ForComment'),
                PDF_NAME('ForPublicRelease'),
                PDF_NAME('NotApproved'),
                PDF_NAME('NotForPublicRelease'),
                PDF_NAME('Sold'),
                PDF_NAME('TopSecret'),
                PDF_NAME('Draft'),
                ]
        n = len(stamp_id)
        name = stamp_id[0]
        try:
            ASSERT_PDF(page)
            r = JM_rect_from_py(rect)
            if mupdf.mfz_is_infinite_rect(r) or mupdf.mfz_is_empty_rect(r):
                THROWMSG("rect must be finite and not empty")
            if INRANGE(stamp, 0, n-1):
                name = stamp_id[stamp]
            annot = mupdf.mpdf_create_annot(page, mupdf.PDF_ANNOT_STAMP)
            mupdf.mpdf_set_annot_rect(annot, r)
            mupdf.mpdf_dict_put(annot.annot_obj(), PDF_NAME('Name'), name)
            mupdf.mpdf_set_annot_contents(
                    annot,
                    mupdf.mpdf_dict_get_name(annot.annot_obj(), PDF_NAME('Name')),
                    )
            JM_add_annot_id(annot, "A")
            mupdf.mpdf_update_annot(annot)
        except Exception as e:
            jlib.log('{e=} {jlib.exception_info()=}')
            return
        return Annot(annot)

    def _add_text_annot(self, point, text, icon=None):
        #return _fitz.Page__add_text_annot(self, point, text, icon)
        page = self._pdf_page()
        p = point
        try:
            ASSERT_PDF(page)
            annot = mupdf.mpdf_create_annot(page, mupdf.PDF_ANNOT_TEXT)
            r = mupdf.mpdf_annot_rect(annot)
            r = mupdf.mfz_make_rect(p.x, p.y, p.x + r.x1 - r.x0, p.y + r.y1 - r.y0)
            mupdf.mpdf_set_annot_rect(annot, r)
            flags = mupdf.PDF_ANNOT_IS_PRINT
            mupdf.mpdf_set_annot_flags(annot, flags)
            mupdf.mpdf_set_annot_contents(annot, text)
            if icon:
                mupdf.mpdf_set_annot_icon_name(annot, icon)
            JM_add_annot_id(annot, "A")
            mupdf.mpdf_update_annot(annot)
            mupdf.mpdf_set_annot_rect(annot, r)
            mupdf.mpdf_set_annot_flags(annot, flags)
        except Exception as e:
            jlib.log('{e=}: {jlib.exception_info()=}')
            return
        return Annot(annot)

    def _add_text_marker(self, quads, annot_type):

        CheckParent(self)
        if not self.parent.isPDF:
            raise ValueError("not a PDF")

        val = Page__add_text_marker(self, quads, annot_type)
        if not val:
            return None
        #val.parent = weakref.proxy(self)
        self._annot_refs[id(val)] = val

        return val

    def _get_text_page(self, clip=None, flags=0):
        val = _fitz.Page__get_text_page(self, clip, flags)
        val.thisown = True
        return val

    def _get_textpage(self, clip=None, flags=0, matrix=None):
        #return _fitz.Page__get_textpage(self, clip, flags, matrix)
        page = self.this
        options = mupdf.StextOptions(flags)
        #jlib.log('{=clip flags matrix options}')
        rect = JM_rect_from_py(clip)
        ctm = JM_matrix_from_py(matrix)
        #jlib.log('{=options rect ctm}')
        #jlib.log('{=rect.x0-(-2147483648.0) rect.y0 rect.x1 rect.y1}')
        tpage = mupdf.StextPage(rect)
        dev = mupdf.mfz_new_stext_device(tpage, options)
        #jlib.log('{type(page)=}')
        if isinstance(page, mupdf.Page):
            pass
        elif isinstance(page, mupdf.PdfPage):
            #jlib.log('calling page = page.super()')
            page = page.super()
        else:
            assert 0, f'Unrecognised type(page)={type(page)}'
        mupdf.mfz_run_page(page, dev, ctm, mupdf.Cookie());
        mupdf.mfz_close_device(dev)
        #jlib.log('{=tpage tpage.m_internal.first_block tpage.m_internal.last_block}')
        #for b in tpage:
        #    jlib.log('{b=}')
        return tpage

    def _insertFont(self, fontname, bfname, fontfile, fontbuffer, set_simple, idx, wmode, serif, encoding, ordering):
        #return _fitz.Page__insertFont(self, fontname, bfname, fontfile, fontbuffer, set_simple, idx, wmode, serif, encoding, ordering)
        page = self._pdf_page()
        ASSERT_PDF(page);
        pdf = page.doc()

        value = JM_insert_font(pdf, bfname, fontfile,fontbuffer, set_simple, idx, wmode, serif, encoding, ordering)
        #jlib.log('{value=}')
        # get the objects /Resources, /Resources/Font
        resources = mupdf.mpdf_dict_get_inheritable( page.obj(), PDF_NAME('Resources'))
        fonts = mupdf.mpdf_dict_get(resources, PDF_NAME('Font'))
        if not fonts.m_internal:    # page has no fonts yet
            fonts = mupdf.mpdf_new_dict(pdf, 5)
            mupdf.mpdf_dict_putl(page.obj(), fonts, PDF_NAME('Resources'), PDF_NAME('Font'))
        # store font in resources and fonts objects will contain named reference to font
        _, xref = JM_INT_ITEM(value, 0)
        if not xref:
            THROWMSG("cannot insert font")
        font_obj = mupdf.mpdf_new_indirect(pdf, xref, 0)
        mupdf.mpdf_dict_puts(fonts, fontname, font_obj)
        return value

    def _insert_image(self,
            filename=None, pixmap=None, stream=None, imask=None, clip=None,
            overlay=1, rotate=0, keep_proportion=1, oc=0, width=0, height=0,
            xref=0, alpha=-1, _imgname=None, digests=None
            ):

        #jlib.log('{=filename pixmap stream imask clip overlay rotate keep_proportion oc width height xref alpha imgname digests}')
        #return _fitz.Page__insert_image(self, filename, pixmap, stream, imask,
        #        clip, overlay, rotate, keep_proportion, oc, width, height,
        #        xref, alpha, _imgname, digests)

        #jlib.log('calling self._pdf_page()')
        page = self._pdf_page()
        #jlib.log('{=page.m_internal.super.chapter page.m_internal.super.number}')
        # This will create an empty PdfDocument with a call to
        # pdf_new_document() then assign page.doc()'s return value to it (which
        # drop the original empty pdf_document).
        #jlib.log('calling page.doc() (will make call to pdf_new_document()).')
        pdf = page.doc()
        #jlib.log('{=mupdf.mpdf_xref_len(pdf)}')
        w = width
        h = height
        #fz_pixmap *pm = NULL;
        #fz_pixmap *pix = NULL;
        #fz_image *mask = NULL, *zimg = NULL, *image = NULL, *freethis = NULL;
        #pdf_obj *resources, *xobject, *ref;
        #fz_buffer *nres = NULL,  *imgbuf = NULL, *maskbuf = NULL;
        #fz_compressed_buffer *cbuf1 = NULL;
        #int xres, yres, bpc,
        img_xref = xref
        rc_digest = 0
        #unsigned char digest[16];
        #PyObject *md5_py = NULL, *temp;
        template = "\nq\n%g %g %g %g %g %g cm\n/%s Do\nQ\n"

        #jlib.log('{=filename pixmap stream imask clip overlay'
        #        ' rotate keep_proportion oc width height'
        #        ' xref alpha _imgname digests}')

        do_process_pixmap = 1
        do_process_stream = 1
        do_have_imask = 1
        do_have_image = 1
        do_have_xref = 1

        #jlib.log('{xref=}')

        if xref > 0:
            #jlib.log('xref > 0')
            ref = mupdf.mpdf_new_indirect(pdf, xref, 0)
            w = mupdf.mpdf_to_int( mupdf.mpdf_dict_geta(ref, PDF_NAME('Width'), PDF_NAME('W')))
            h = mupdf.mpdf_to_int( mupdf.mpdf_dict_geta(gctx, ref, PDF_NAME('Height'), PDF_NAME('H')))
            if w + h == 0:
                THROWMSG("xref is no image");
            #goto have_xref()
            do_process_pixmap = 0
            do_process_stream = 0
            do_have_imask = 0
            do_have_image = 0

        else:
            #jlib.log('{stream=}')
            if stream:
                #jlib.log('calling JM_BufferFromBytes()')
                imgbuf = JM_BufferFromBytes(stream)
                #jlib.log(' ')
                do_process_pixmap = 0
            else:
                #jlib.log('{filename=}')
                if filename:
                    #jlib.log('calling fz_read_file()')
                    imgbuf = mupdf.mfz_read_file(filename)
                    #jlib.log('fz_read_file() => {imgbuf=}')
                    #goto have_stream()
                    do_process_pixmap = 0

        if do_process_pixmap:
            # process pixmap ---------------------------------
            #jlib.log('do_process_pixmap {pixmap.this=}')
            arg_pix = pixmap.this
            w = arg_pix.w
            h = arg_pix.h
            digest = mupdf.mfz_md5_pixmap(arg_pix)
            md5_py = digest
            temp = digests.get(md5_py, None)
            #jlib.log('{temp=}')
            if temp is not None:
                img_xref = temp
                ref = mupdf.mpdf_new_indirect(page.doc(), img_xref, 0)
                #jlib.log('pdf_new_indirect() => {ref=}')
                #goto have_xref()
                do_process_stream = 0
                do_have_imask = 0
                do_have_image = 0
            else:
                #jlib.log('{arg_pix.alpha()=}')
                if arg_pix.alpha() == 0:
                    #jlib.log('calling fz_new_image_from_pixmap')
                    image = mupdf.mfz_new_image_from_pixmap(arg_pix, mupdf.Image(0))
                    #jlib.log('fz_new_image_from_pixmap() => {image=}')
                else:
                    #jlib.log('calling fz_convert_pixmap')
                    pm = mupdf.mfz_convert_pixmap(
                            arg_pix,
                            mupdf.Colorspace(0),
                            mupdf.Colorspace(0),
                            mupdf.DefaultColorspaces(0),
                            mupdf.ColorParams(),
                            1,
                            )
                    pm.alpha = 0;
                    pm.colorspace = NULL;
                    mask = mupdf.mfz_new_image_from_pixmap(pm, mupdf.Image(0))
                    image = mupdf.mfz_new_image_from_pixmap(arg_pix, mask)
                #jlib.log(' ')
                #goto have_image()
                do_process_stream = 0
                do_have_imask = 0

        if do_process_stream:
            # process stream ---------------------------------
            #jlib.log('do_process_stream')
            state = mupdf.Md5()
            mupdf.mfz_md5_update(state, imgbuf.m_internal.data, imgbuf.m_internal.len)
            if imask:
                #jlib.log('calling JM_BufferFromBytes()')
                maskbuf = JM_BufferFromBytes(imask)
                fz_md5_update(state, maskbuf.m_internal.data, maskbuf.m_internal.len)
            digest = state.md5_final2()
            md5_py = bytes(digest)
            #jlib.log('{md5_py=}')
            temp = digests.get(md5_py, None)
            #jlib.log('{temp=}')
            if temp is not None:
                img_xref = temp
                #jlib.log('calling pdf_new_indirect()')
                ref = mupdf.mpdf_new_indirect(page.doc(), img_xref, 0)
                w = mupdf.mpdf_to_int( mupdf.mpdf_dict_geta( ref, PDF_NAME('Width'), PDF_NAME('W')))
                h = mupdf.mpdf_to_int( mupdf.mpdf_dict_geta( ref, PDF_NAME('Height'), PDF_NAME('H')))
                #goto have_xref()
                do_have_imask = 0
                do_have_image = 0
            else:
                #jlib.log('calling fz_new_image_from_buffer()')
                image = mupdf.mfz_new_image_from_buffer(imgbuf)
                #jlib.log('fz_new_image_from_buffer() => {image=} {image.w()=} {image.h()=}')
                w = image.w()
                h = image.h()
                if imask:
                    pass
                    #jlib.log('imask is true; goto have_imask()')
                    #goto have_imask()
                else:
                    #jlib.log('{alpha=}')
                    if alpha==0:
                        #jlib.log(' return have_image()')
                        #goto have_image()
                        do_have_imask = 0
                    else:
                        #jlib.log('calling fz_get_pixmap_from_image()')
                        pix, w, h = mupdf.mfz_get_pixmap_from_image(
                                image,
                                mupdf.Irect(FZ_MIN_INF_RECT, FZ_MIN_INF_RECT, FZ_MAX_INF_RECT, FZ_MAX_INF_RECT),
                                mupdf.Matrix(image.w(), 0, 0, image.h(), 0, 0),
                                )
                        if not pix.alpha():
                            #jlib.log(' return have_image()')
                            #goto have_image()
                            do_have_imask = 0
                        else:
                            #jlib.log('calling fz_get_pixmap_from_image()')
                            pix, _, _ = mupdf.mfz_get_pixmap_from_image(
                                    image,
                                    mupdf.Irect(FZ_MIN_INF_RECT, FZ_MIN_INF_RECT, FZ_MAX_INF_RECT, FZ_MAX_INF_RECT),
                                    mupdf.Matrix(image.w(), 0, 0, image.h(), 0, 0),
                                    )
                            #jlib.log('calling fz_convert_pixmap()')
                            pm = mupdf.mfz_convert_pixmap(
                                    pix,
                                    mupdf.Colorspace(0),
                                    mupdf.Colorspace(0),
                                    mupdf.DefaultColorspaces(),
                                    mupdf.ColorParams(),
                                    1,
                                    );
                            pm.m_internal.alpha = 0
                            pm.m_internal.colorspace = None
                            mask = mupdf.mfz_new_image_from_pixmap(pm, mupdf.Image())
                            zimg = mupdf.mfz_new_image_from_pixmap(pix, mask)
                            image = zimg
                            #goto have_image()
                            do_have_imask = 0

        if do_have_imask:
            #jlib.log('do_have_imask')
            #jlib.log('calling fz_compressed_image_buffer()')
            cbuf1 = mupdf.mfz_compressed_image_buffer(image)
            if not cbuf1.m_internal:
                THROWMSG("cannot mask uncompressed image")
            bpc = image.bpc()
            colorspace = image.colorspace()
            xres, yres = mupdf.mfz_image_resolution(image)
            mask = mupdf.mfz_new_image_from_buffer(maskbuf)
            zimg = mupdf.mfz_new_image_from_compressed_buffer(
                    w, h,
                    bpc, colorspace, xres, yres, 1, 0, NULL,
                    NULL, cbuf1, mask
                    )
            freethis = image
            image = zimg
            #goto have_image()

        if do_have_image:
            #jlib.log('do_have_image {image=}')
            #jlib.log('calling pdf_add_image()')
            ref =  mupdf.mpdf_add_image(pdf, image)
            #jlib.log(' {oc=}')
            if oc:
                JM_add_oc_object(pdf, ref, oc)
            img_xref = mupdf.mpdf_to_num(ref)
            #jlib.log(' {img_xref=} {md5_py=}')
            digests[md5_py] = img_xref
            rc_digest = 1
            #jlib.log(' return have_xref()')

        if do_have_xref:
            #jlib.log('do_have_xref')
            #jlib.log('calling pdf_dict_get_inheritabl()')
            resources = mupdf.mpdf_dict_get_inheritable(page.obj(), PDF_NAME('Resources'))
            if not resources.m_internal:
                #jlib.log('not resources.m_internal calling pdf_dict_put_dict()')
                resources = mupdf.mpdf_dict_put_dict(page.obj(), PDF_NAME('Resources'), 2)
            #jlib.log('calling mpdf_dict_get() XObject')
            xobject = mupdf.mpdf_dict_get(resources, PDF_NAME('XObject'))
            if not xobject.m_internal:
                #jlib.log('not xobject.m_internal calling pdf_dict_put_dict()')
                xobject = mupdf.mpdf_dict_put_dict(resources, PDF_NAME('XObject'), 2)
            #jlib.log('calling calc_image_matrix() {=w h clip rotate keep_proportion}')
            mat = calc_image_matrix(w, h, clip, rotate, keep_proportion)
            #jlib.log('{mat=}')
            #jlib.log('calling mpdf_dict_puts {=_imgname ref}')
            mupdf.mpdf_dict_puts(xobject, _imgname, ref);
            nres = mupdf.mfz_new_buffer(50)
            #mupdf.mfz_append_printf(nres, template, mat.a, mat.b, mat.c, mat.d, mat.e, mat.f, _imgname)
            # fixme: this does not use fz_append_printf()'s special handling of %g etc.
            s = template % (mat.a, mat.b, mat.c, mat.d, mat.e, mat.f, _imgname)
            #s = s.replace('\n', '\r\n')
            #jlib.log('calling fz_append_pdf_string() with {=len(s) s!r}')
            #jlib.log(s)
            mupdf.mfz_append_string(nres, s)
            JM_insert_contents(pdf, page.obj(), nres, overlay)

        if rc_digest:
            #jlib.log('returning ({img_xref=}, {digests})')
            return img_xref, digests
        else:
            #jlib.log('returning ({img_xref=}, {None})')
            return img_xref, None


    def _load_annot(self, name, xref):
        #return _fitz.Page__load_annot(self, name, xref)
        page = self._pdf_page()
        try:
            ASSERT_PDF(page)
            if xref == 0:
                annot = JM_get_annot_by_name(page, name)
            else:
                annot = JM_get_annot_by_xref(page, xref)
        except Exception as e:
            jlib.log('{e=} {jlib.exception_info()=}')
            return
        return Annot(annot) if annot else None

    def _makePixmap(self, doc, ctm, cs, alpha=0, annots=1, clip=None):
        return _fitz.Page__makePixmap(self, doc, ctm, cs, alpha, annots, clip)

    def _pdf_page(self):
        '''
        Returns self.this as a mupdf.PdfPage using page_from_fz_page() if
        required.
        '''
        if isinstance(self.this, mupdf.PdfPage):
            return self.this
        return self.this.page_from_fz_page()

    def _reset_annot_refs(self):
        """Invalidate / delete all annots of this page."""
        self._annot_refs.clear()

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

        #return _fitz.Page__set_opacity(self, gstate, CA, ca)
        if not gstate:
            return
        page = mupdf.mpdf_page_from_fz_page(self.this)
        ASSERT_PDF(page)
        resources = mupdf.mpdf_dict_get(page.obj(), PDF_NAME('Resources'))
        if not resources.m_internal:
            resources = mupdf.mpdf_dict_put_dict(page.obj(), PDF_NAME('Resources'), 2)
        extg = mupdf.mpdf_dict_get(resources, PDF_NAME('ExtGState'))
        if not extg.m_internal:
            extg = mupdf.mpdf_dict_put_dict(resources, PDF_NAME('ExtGState'), 2)
        n = mupdf.mpdf_dict_len(extg)
        for i in range(m):
            o1 = mupdf.mpdf_dict_get_key(extg, i)
            name = mupdf.mpdf_to_name(o1)
            if name == gstate:
                return gstate
        opa = mupdf.mpdf_new_dict(page.doc(), 3)
        mupdf.mpdf_dict_put_real(opa, PDF_NAME('CA'), CA)
        mupdf.mpdf_dict_put_real(opa, PDF_NAME('ca'), ca)
        mupdf.mpdf_dict_puts(extg, gstate, opa)
        return gstate

    def _set_resource_property(self, name, xref):
        #return _fitz.Page__set_resource_property(self, name, xref)
        page = self._pdf_page()
        ASSERT_PDF(page);
        JM_set_resource_property(page.obj(), name, xref)

    def _show_pdf_page(self, fz_srcpage, overlay=1, matrix=None, xref=0, oc=0, clip=None, graftmap=None, _imgname=None):
        #return _fitz.Page__show_pdf_page(self, fz_srcpage, overlay, matrix, xref, oc, clip, graftmap, _imgname)
        #fz_buffer *res=NULL, *nres=NULL;
        cropbox = JM_rect_from_py(clip)
        mat = JM_matrix_from_py(matrix)
        rc_xref = xref
        tpage = mupdf.mpdf_page_from_fz_page(self.this)
        tpageref = tpage.obj()
        pdfout = tpage.doc()    # target PDF
        ENSURE_OPERATION(pdfout)
        #-------------------------------------------------------------
        # convert the source page to a Form XObject
        #-------------------------------------------------------------
        xobj1 = JM_xobject_from_page(pdfout, fz_srcpage, xref, graftmap.this)
        if not rc_xref:
            rc_xref = mupdf.mpdf_to_num(xobj1)

        #-------------------------------------------------------------
        # create referencing XObject (controls display on target page)
        #-------------------------------------------------------------
        # fill reference to xobj1 into the /Resources
        #-------------------------------------------------------------
        subres1 = mupdf.mpdf_new_dict(pdfout, 5)
        mupdf.mpdf_dict_puts(subres1, "fullpage", xobj1)
        subres = mupdf.mpdf_new_dict(pdfout, 5)
        mupdf.mpdf_dict_put(subres, PDF_NAME('XObject'), subres1)

        res = mupdf.mfz_new_buffer(20)
        mupdf.mfz_append_string(res, "/fullpage Do")

        xobj2 = mupdf.mpdf_new_xobject(pdfout, cropbox, mat, subres, res)
        if oc > 0:
            JM_add_oc_object(pdfout, mupdf.mpdf_resolve_indirect(xobj2), oc)

        #-------------------------------------------------------------
        # update target page with xobj2:
        #-------------------------------------------------------------
        # 1. insert Xobject in Resources
        #-------------------------------------------------------------
        resources = mupdf.mpdf_dict_get_inheritable(tpageref, PDF_NAME('Resources'))
        subres = mupdf.mpdf_dict_get(resources, PDF_NAME('XObject'))
        if not subres.m_internal:
            subres = mupdf.mpdf_dict_put_dict(resources, PDF_NAME('XObject'), 5)

        mupdf.mpdf_dict_puts(subres, _imgname, xobj2)

        #-------------------------------------------------------------
        # 2. make and insert new Contents object
        #-------------------------------------------------------------
        nres = mupdf.mfz_new_buffer(50) # buffer for Do-command
        mupdf.mfz_append_string(nres, " q /")   # Do-command
        mupdf.mfz_append_string(nres, _imgname)
        mupdf.mfz_append_string(nres, " Do Q ")

        JM_insert_contents(pdfout, tpageref, nres, overlay)
        return rc_xref

    @property
    def CropBox(self):
        """The CropBox."""
        CheckParent(self)

        #val = _fitz.Page_CropBox(self)
        page = self._pdf_page()
        if not page.m_internal:
            return JM_py_from_rect(mupdf.mfz_bound_page(self.this))
        val = JM_py_from_rect(JM_cropbox(page.obj()))
        val = Rect(val)

        return val

    @property
    def CropBoxPosition(self):
        return self.CropBox.tl

    @property
    def MediaBox(self):
        """The MediaBox."""
        CheckParent(self)

        #val = _fitz.Page_MediaBox(self)
        page = self._pdf_page()
        if not page.m_internal:
            return JM_py_from_rect(mupdf.mfz_bound_page(self.this))
        val = JM_py_from_rect(JM_mediabox(page.obj()))
        val = Rect(val)

        return val

    def add_caret_annot(self, point: point_like) -> "struct Annot *":
        """Add a 'Caret' annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_caret_annot(point)
        finally:
            if old_rotation != 0:
                self.set_rotation(old_rotation)
        annot_postprocess(self, annot)
        return Annot(annot)

    def add_circle_annot(self, rect: rect_like) -> "struct Annot *":
        """Add a 'Circle' (ellipse, oval) annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_square_or_circle(rect, mupdf.PDF_ANNOT_CIRCLE)
        finally:
            if old_rotation != 0:
                self.set_rotation(old_rotation)
        annot_postprocess(self, annot)
        return annot

    def add_file_annot(
            self,
            point: point_like,
            buffer_: typing.ByteString,
            filename: str,
            ufilename: OptStr =None,
            desc: OptStr =None,
            icon: OptStr =None
            ) -> "struct Annot *":
        """Add a 'FileAttachment' annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_file_annot(point,
                    buffer_,
                    filename,
                    ufilename=ufilename,
                    desc=desc,
                    icon=icon,
                    )
        finally:
            if old_rotation != 0:
                self.set_rotation(old_rotation)
        #jlib.log('{type(annot)=} {annot=}')
        annot_postprocess(self, annot)
        return annot

    def add_freetext_annot(self, rect: rect_like, text: str, fontsize: float =11,
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
                self.set_rotation(old_rotation)
        annot_postprocess(self, annot)
        return annot

    def add_highlight_annot(self, quads=None, start=None,
                          stop=None, clip=None) -> "struct Annot *":
        """Add a 'Highlight' annotation."""
        if quads is None:
            q = get_highlight_selection(self, start=start, stop=stop, clip=clip)
        else:
            q = CheckMarkerArg(quads)
        ret = self._add_text_marker(q, mupdf.PDF_ANNOT_HIGHLIGHT)
        return ret

    def add_ink_annot(self, handwriting: list) -> "struct Annot *":
        """Add a 'Ink' ('handwriting') annotation.

        The argument must be a list of lists of point_likes.
        """
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_ink_annot(handwriting)
        finally:
            if old_rotation != 0:
                self.set_rotation(old_rotation)
        annot_postprocess(self, annot)
        return annot

    def add_line_annot(self, p1: point_like, p2: point_like) -> "struct Annot *":
        """Add a 'Line' annotation."""
        try:
            old_rotation = annot_preprocess(self)
            try:
                annot = self._add_line_annot(p1, p2)
            finally:
                if old_rotation != 0:
                    self.set_rotation(old_rotation)
            annot_postprocess(self, annot)
            return annot
        except Exception as e:
            jlib.log('{jlib.exception_info()}')
            assert 0

    def add_polygon_annot(self, points: list) -> "struct Annot *":
        """Add a 'Polygon' annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_multiline(points, mupdf.PDF_ANNOT_POLYGON)
        finally:
            if old_rotation != 0:
                self.set_rotation(old_rotation)
        annot_postprocess(self, annot)
        return annot

    def add_polyline_annot(self, points: list) -> "struct Annot *":
        """Add a 'PolyLine' annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_multiline(points, mupdf.PDF_ANNOT_POLY_LINE)
        finally:
            if old_rotation != 0:
                self.set_rotation(old_rotation)
        annot_postprocess(self, annot)
        return annot

    def add_rect_annot(self, rect: rect_like) -> "struct Annot *":
        """Add a 'Square' (rectangle) annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_square_or_circle(rect, mupdf.PDF_ANNOT_SQUARE)
        finally:
            if old_rotation != 0:
                self.set_rotation(old_rotation)
        annot_postprocess(self, annot)
        return annot

    def add_redact_annot(
            self,
            quad,
            text: OptStr =None,
            fontname: OptStr =None,
            fontsize: float =11,
            align: int =0,
            fill: OptSeq =None,
            text_color: OptSeq =None,
            cross_out: bool =True,
            ) -> "struct Annot *":
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
                self.set_rotation(old_rotation)
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

    def add_squiggly_annot(self, quads=None, start=None,
                         stop=None, clip=None) -> "struct Annot *":
        """Add a 'Squiggly' annotation."""
        if quads is None:
            q = get_highlight_selection(self, start=start, stop=stop, clip=clip)
        else:
            q = CheckMarkerArg(quads)
        return self._add_text_marker(q, mupdf.PDF_ANNOT_SQUIGGLY)

    def add_stamp_annot(self, rect: rect_like, stamp: int =0) -> "struct Annot *":
        """Add a ('rubber') 'Stamp' annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_stamp_annot(rect, stamp)
        finally:
            if old_rotation != 0:
                self.set_rotation(old_rotation)
        annot_postprocess(self, annot)
        return annot

    def add_strikeout_annot(self, quads=None, start=None, stop=None, clip=None) -> "struct Annot *":
        """Add a 'StrikeOut' annotation."""
        if quads is None:
            q = get_highlight_selection(self, start=start, stop=stop, clip=clip)
        else:
            q = CheckMarkerArg(quads)
        return self._add_text_marker(q, mupdf.PDF_ANNOT_STRIKE_OUT)

    def add_text_annot(self, point: point_like, text: str, icon: str ="Note") -> "struct Annot *":
        """Add a 'Text' (sticky note) annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_text_annot(point, text, icon=icon)
        finally:
            if old_rotation != 0:
                self.set_rotation(old_rotation)
        annot_postprocess(self, annot)
        return annot

    def add_underline_annot(self, quads=None, start=None, stop=None, clip=None) -> "struct Annot *":
        """Add a 'Underline' annotation."""
        if quads is None:
            q = get_highlight_selection(self, start=start, stop=stop, clip=clip)
        else:
            q = CheckMarkerArg(quads)
        return self._add_text_marker(q, mupdf.PDF_ANNOT_UNDERLINE)

    def add_widget(self, widget: Widget) -> "struct Annot *":
        """Add a 'Widget' (form field)."""
        CheckParent(self)
        doc = self.parent
        if not doc.is_pdf:
            raise ValueError("not a PDF")
        widget._validate()
        annot = self._addWidget(widget.field_type, widget.field_name)
        if not annot:
            return None
        annot.thisown = True
        #annot.parent = weakref.proxy(self) # owning page object
        self._annot_refs[id(annot)] = annot
        widget.parent = annot.parent
        widget._annot = annot
        widget.update()
        return annot

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

    def addCircleAnnot(self, rect: rect_like) -> "struct Annot *":
        """Add a 'Circle' (ellipse, oval) annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_square_or_circle(rect, mupdf.PDF_ANNOT_CIRCLE)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot

    def addFileAnnot(self, point: point_like,
        buffer_: typing.ByteString,
        filename: str,
        ufilename: OptStr =None,
        desc: OptStr =None,
        icon: OptStr =None) -> "struct Annot *":
        """Add a 'FileAttachment' annotation."""

        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_file_annot(point,
                        buffer_,
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

    def addHighlightAnnot(self, quads=None, start=None,
                          stop=None, clip=None) -> "struct Annot *":
        """Add a 'Highlight' annotation."""
        if quads is None:
            q = get_highlight_selection(self, start=start, stop=stop, clip=clip)
        else:
            q = CheckMarkerArg(quads)
        return self._add_text_marker(q, mupdf.PDF_ANNOT_HIGHLIGHT)

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

    def addPolygonAnnot(self, points: list) -> "struct Annot *":
        """Add a 'Polygon' annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_multiline(points, mupdf.PDF_ANNOT_POLYGON)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot

    def addPolylineAnnot(self, points: list) -> "struct Annot *":
        """Add a 'PolyLine' annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_multiline(points, mupdf.PDF_ANNOT_POLY_LINE)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot

    def addRectAnnot(self, rect: rect_like) -> "struct Annot *":
        """Add a 'Square' (rectangle) annotation."""
        old_rotation = annot_preprocess(self)
        try:
            annot = self._add_square_or_circle(rect, mupdf.PDF_ANNOT_SQUARE)
        finally:
            if old_rotation != 0:
                self.setRotation(old_rotation)
        annot_postprocess(self, annot)
        return annot

    def addSquigglyAnnot(self, quads=None, start=None,
                         stop=None, clip=None) -> "struct Annot *":
        """Add a 'Squiggly' annotation."""
        if quads is None:
            q = get_highlight_selection(self, start=start, stop=stop, clip=clip)
        else:
            q = CheckMarkerArg(quads)
        return self._add_text_marker(q, mupdf.PDF_ANNOT_SQUIGGLY)

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

    def addStrikeoutAnnot(self, quads=None, start=None, stop=None, clip=None) -> "struct Annot *":
        """Add a 'StrikeOut' annotation."""
        if quads is None:
            q = get_highlight_selection(self, start=start, stop=stop, clip=clip)
        else:
            q = CheckMarkerArg(quads)
        return self._add_text_marker(q, mupdf.PDF_ANNOT_STRIKE_OUT)

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

    def addUnderlineAnnot(self, quads=None, start=None, stop=None, clip=None) -> "struct Annot *":
        """Add a 'Underline' annotation."""
        if quads is None:
            q = get_highlight_selection(self, start=start, stop=stop, clip=clip)
        else:
            q = CheckMarkerArg(quads)
        return self._add_text_marker(q, mupdf.PDF_ANNOT_UNDERLINE)

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

    def annot_names(self):
        """List of names of annotations, fields and links."""
        CheckParent(self)

        #return _fitz.Page_annot_names(self)
        page = self._pdf_page()
        if not page.m_internal:
            return
        return JM_get_annot_id_list(page)

    def annot_xrefs(self):
        """List of xref numbers of annotations, fields and links."""
        CheckParent(self)

        #return _fitz.Page_annot_xrefs(self)
        page = self._pdf_page()
        if not page.m_internal:
            return
        return JM_get_annot_xref_list(page);

    def apply_redactions(self, images: int = 2) -> bool:
        """Apply the redaction annotations of the page.

        Args:
            page: the PDF page.
            images: 0 - ignore images, 1 - remove complete overlapping image,
                    2 - blank out overlapping image parts.
        """
        page = self
        def center_rect(annot_rect, text, font, fsize):
            """Calculate minimal sub-rectangle for the overlay text.

            Notes:
                Because 'insert_textbox' supports no vertical text centering,
                we calculate an approximate number of lines here and return a
                sub-rect with smaller height, which should still be sufficient.
            Args:
                annot_rect: the annotation rectangle
                text: the text to insert.
                font: the fontname. Must be one of the CJK or Base-14 set, else
                    the rectangle is returned unchanged.
                fsize: the fontsize
            Returns:
                A rectangle to use instead of the annot rectangle.
            """
            if not text:
                return annot_rect
            try:
                text_width = get_text_length(text, font, fsize)
            except ValueError:  # unsupported font
                return annot_rect
            line_height = fsize * 1.2
            limit = annot_rect.width
            h = math.ceil(text_width / limit) * line_height  # estimate rect height
            if h >= annot_rect.height:
                return annot_rect
            r = annot_rect
            y = (annot_rect.tl.y + annot_rect.bl.y - h) * 0.5
            r.y0 = y
            return r

        CheckParent(page)
        doc = page.parent
        if doc.is_encrypted or doc.is_closed:
            raise ValueError("document closed or encrypted")
        if not doc.is_pdf:
            raise ValueError("not a PDF")

        redact_annots = []  # storage of annot values
        for annot in page.annots(types=(mupdf.PDF_ANNOT_REDACT,)):  # loop redactions
            redact_annots.append(annot._get_redact_values())  # save annot values

        if redact_annots == []:  # any redactions on this page?
            return False  # no redactions

        rc = page._apply_redactions(images)  # call MuPDF redaction process step
        if not rc:  # should not happen really
            raise ValueError("Error applying redactions.")

        # now write replacement text in old redact rectangles
        shape = page.new_shape()
        for redact in redact_annots:
            annot_rect = redact["rect"]
            fill = redact["fill"]
            if fill:
                shape.draw_rect(annot_rect)  # colorize the rect background
                shape.finish(fill=fill, color=fill)
            if "text" in redact.keys():  # if we also have text
                trect = center_rect(  # try finding vertical centered sub-rect
                    annot_rect, redact["text"], redact["fontname"], redact["fontsize"]
                )
                fsize = redact["fontsize"]  # start with stored fontsize
                rc = -1
                while rc < 0 and fsize >= 4:  # while not enough room
                    rc = shape.insert_textbox(  # (re-) try insertion
                        trect,
                        redact["text"],
                        fontname=redact["fontname"],
                        fontsize=fsize,
                        color=redact["text_color"],
                        align=redact.get("align", TEXT_ALIGN_LEFT),
                    )
                    fsize -= 0.5  # reduce font if unsuccessful
        shape.commit()  # append new contents object
        return True

    def bound(self):
        """Get page rectangle."""
        CheckParent(self)
        val = self.this.bound_page()
        val = Rect(val)

        return val

    rect = property(bound, doc="page rectangle")

    def clean_contents(self, sanitize=1):
        """Clean page /Contents into one object."""
        CheckParent(self)
        if not sanitize and not self.is_wrapped:
            self.wrap_contents()

        return Page_clean_contents(self, sanitize)

    @property
    def cropbox(self):
        """The CropBox."""
        CheckParent(self)

        #val = _fitz.Page_cropbox(self)
        page = self._pdf_page()
        if not page.m_internal:
            val = mupdf.mfz_bound_page(self.this)
        else:
            val = JM_cropbox(page.obj())
        val = Rect(val)

        return val

    @property
    def cropbox_position(self):
        return self.CropBoxPosition

    def delete_annot(self, annot):
        """Delete annot and return next one."""
        CheckParent(self)
        CheckParent(annot)

        page = self._pdf_page()
        while 1:
            # first loop through all /IRT annots and remove them
            irt_annot = JM_find_annot_irt(annot.this)
            if not irt_annot:    # no more there
                break
            JM_delete_annot(page, irt_annot)
        nextannot = mupdf.mpdf_next_annot(annot.this)   # store next
        JM_delete_annot(page, annot.this)
        #fixme: page->doc->dirty = 1;
        val = Annot(nextannot)

        if val:
            val.thisown = True
            #val.parent = weakref.proxy(self) # owning page object
            val.parent._annot_refs[id(val)] = val
        annot._erase()
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

    @property
    def derotation_matrix(self) -> Matrix:
        """Reflects page de-rotation."""
        #return Matrix(TOOLS._derotate_matrix(self))
        pdfpage = self._pdf_page()
        if not pdfpage.m_internal:
            #return JM_py_from_matrix(fz_identity);
            return JM_py_from_matrix(mupdf.Rect(mupdf.Rect.UNIT))
        return JM_py_from_matrix(JM_derotate_page_matrix(pdfpage))

    derotationMatrix = derotation_matrix

    def draw_bezier(
            page: Page,
            p1: point_like,
            p2: point_like,
            p3: point_like,
            p4: point_like,
            color: OptSeq = None,
            fill: OptSeq = None,
            dashes: OptStr = None,
            width: float = 1,
            morph: OptStr = None,
            closePath: bool = False,
            lineCap: int = 0,
            lineJoin: int = 0,
            overlay: bool = True,
            stroke_opacity: float = 1,
            fill_opacity: float = 1,
            oc: int = 0,
            ) -> Point:
        """Draw a general cubic Bezier curve from p1 to p4 using control points p2 and p3."""
        img = page.new_shape()
        Q = img.draw_bezier(Point(p1), Point(p2), Point(p3), Point(p4))
        img.finish(
            color=color,
            fill=fill,
            dashes=dashes,
            width=width,
            lineCap=lineCap,
            lineJoin=lineJoin,
            morph=morph,
            closePath=closePath,
            stroke_opacity=stroke_opacity,
            fill_opacity=fill_opacity,
            oc=oc,
        )
        img.commit(overlay)

        return Q

    def draw_circle(
            page: Page,
            center: point_like,
            radius: float,
            color: OptSeq = None,
            fill: OptSeq = None,
            morph: OptSeq = None,
            dashes: OptStr = None,
            width: float = 1,
            lineCap: int = 0,
            lineJoin: int = 0,
            overlay: bool = True,
            stroke_opacity: float = 1,
            fill_opacity: float = 1,
            oc: int = 0,
            ) -> Point:
        """Draw a circle given its center and radius."""
        img = page.new_shape()
        Q = img.draw_circle(Point(center), radius)
        img.finish(
            color=color,
            fill=fill,
            dashes=dashes,
            width=width,
            lineCap=lineCap,
            lineJoin=lineJoin,
            morph=morph,
            stroke_opacity=stroke_opacity,
            fill_opacity=fill_opacity,
            oc=oc,
        )
        img.commit(overlay)
        return Q

    def draw_curve(
            page: Page,
            p1: point_like,
            p2: point_like,
            p3: point_like,
            color: OptSeq = None,
            fill: OptSeq = None,
            dashes: OptStr = None,
            width: float = 1,
            morph: OptSeq = None,
            closePath: bool = False,
            lineCap: int = 0,
            lineJoin: int = 0,
            overlay: bool = True,
            stroke_opacity: float = 1,
            fill_opacity: float = 1,
            oc: int = 0,
            ) -> Point:
        """Draw a special Bezier curve from p1 to p3, generating control points on lines p1 to p2 and p2 to p3."""
        img = page.new_shape()
        Q = img.draw_curve(Point(p1), Point(p2), Point(p3))
        img.finish(
            color=color,
            fill=fill,
            dashes=dashes,
            width=width,
            lineCap=lineCap,
            lineJoin=lineJoin,
            morph=morph,
            closePath=closePath,
            stroke_opacity=stroke_opacity,
            fill_opacity=fill_opacity,
            oc=oc,
        )
        img.commit(overlay)

        return Q

    def draw_line(
            page: Page,
            p1: point_like,
            p2: point_like,
            color: OptSeq = None,
            dashes: OptStr = None,
            width: float = 1,
            lineCap: int = 0,
            lineJoin: int = 0,
            overlay: bool = True,
            morph: OptSeq = None,
            stroke_opacity: float = 1,
            fill_opacity: float = 1,
            oc=0,
            ) -> Point:
        """Draw a line from point p1 to point p2."""
        img = page.new_shape()
        p = img.draw_line(Point(p1), Point(p2))
        img.finish(
            color=color,
            dashes=dashes,
            width=width,
            closePath=False,
            lineCap=lineCap,
            lineJoin=lineJoin,
            morph=morph,
            stroke_opacity=stroke_opacity,
            fill_opacity=fill_opacity,
            oc=oc,
        )
        img.commit(overlay)

        return p

    def draw_oval(
            page: Page,
            rect: typing.Union[rect_like, quad_like],
            color: OptSeq = None,
            fill: OptSeq = None,
            dashes: OptStr = None,
            morph: OptSeq = None,
            width: float = 1,
            lineCap: int = 0,
            lineJoin: int = 0,
            overlay: bool = True,
            stroke_opacity: float = 1,
            fill_opacity: float = 1,
            oc: int = 0,
            ) -> Point:
        """Draw an oval given its containing rectangle or quad."""
        img = page.new_shape()
        Q = img.draw_oval(rect)
        img.finish(
            color=color,
            fill=fill,
            dashes=dashes,
            width=width,
            lineCap=lineCap,
            lineJoin=lineJoin,
            morph=morph,
            stroke_opacity=stroke_opacity,
            fill_opacity=fill_opacity,
            oc=oc,
        )
        img.commit(overlay)

        return Q

    def draw_polyline(
            page: Page,
            points: list,
            color: OptSeq = None,
            fill: OptSeq = None,
            dashes: OptStr = None,
            width: float = 1,
            morph: OptSeq = None,
            lineCap: int = 0,
            lineJoin: int = 0,
            overlay: bool = True,
            closePath: bool = False,
            stroke_opacity: float = 1,
            fill_opacity: float = 1,
            oc: int = 0,
            ) -> Point:
        """Draw multiple connected line segments."""
        img = page.new_shape()
        Q = img.draw_polyline(points)
        img.finish(
            color=color,
            fill=fill,
            dashes=dashes,
            width=width,
            lineCap=lineCap,
            lineJoin=lineJoin,
            morph=morph,
            closePath=closePath,
            stroke_opacity=stroke_opacity,
            fill_opacity=fill_opacity,
            oc=oc,
        )
        img.commit(overlay)
        return Q

    def draw_quad(
            page: Page,
            quad: quad_like,
            color: OptSeq = None,
            fill: OptSeq = None,
            dashes: OptStr = None,
            width: float = 1,
            lineCap: int = 0,
            lineJoin: int = 0,
            morph: OptSeq = None,
            overlay: bool = True,
            stroke_opacity: float = 1,
            fill_opacity: float = 1,
            oc: int = 0,
            ) -> Point:
        """Draw a quadrilateral."""
        img = page.new_shape()
        Q = img.draw_quad(Quad(quad))
        img.finish(
            color=color,
            fill=fill,
            dashes=dashes,
            width=width,
            lineCap=lineCap,
            lineJoin=lineJoin,
            morph=morph,
            stroke_opacity=stroke_opacity,
            fill_opacity=fill_opacity,
            oc=oc,
        )
        img.commit(overlay)
        return Q

    def draw_rect(
            page: Page,
            rect: rect_like,
            color: OptSeq = None,
            fill: OptSeq = None,
            dashes: OptStr = None,
            width: float = 1,
            lineCap: int = 0,
            lineJoin: int = 0,
            morph: OptSeq = None,
            overlay: bool = True,
            stroke_opacity: float = 1,
            fill_opacity: float = 1,
            oc: int = 0,
            ) -> Point:
        """Draw a rectangle."""
        img = page.new_shape()
        Q = img.draw_rect(Rect(rect))
        img.finish(
            color=color,
            fill=fill,
            dashes=dashes,
            width=width,
            lineCap=lineCap,
            lineJoin=lineJoin,
            morph=morph,
            stroke_opacity=stroke_opacity,
            fill_opacity=fill_opacity,
            oc=oc,
        )
        img.commit(overlay)

        return Q

    def draw_sector(
        page: Page,
        center: point_like,
        point: point_like,
        beta: float,
        color: OptSeq = None,
        fill: OptSeq = None,
        dashes: OptStr = None,
        fullSector: bool = True,
        morph: OptSeq = None,
        width: float = 1,
        closePath: bool = False,
        lineCap: int = 0,
        lineJoin: int = 0,
        overlay: bool = True,
        stroke_opacity: float = 1,
        fill_opacity: float = 1,
        oc: int = 0,
    ) -> Point:
        """Draw a circle sector given circle center, one arc end point and the angle of the arc.

        Parameters:
            center -- center of circle
            point -- arc end point
            beta -- angle of arc (degrees)
            fullSector -- connect arc ends with center
        """
        img = page.new_shape()
        Q = img.draw_sector(Point(center), Point(point), beta, fullSector=fullSector)
        img.finish(
            color=color,
            fill=fill,
            dashes=dashes,
            width=width,
            lineCap=lineCap,
            lineJoin=lineJoin,
            morph=morph,
            closePath=closePath,
            stroke_opacity=stroke_opacity,
            fill_opacity=fill_opacity,
            oc=oc,
        )
        img.commit(overlay)

        return Q

    def draw_squiggle(
            page: Page,
            p1: point_like,
            p2: point_like,
            breadth: float = 2,
            color: OptSeq = None,
            dashes: OptStr = None,
            width: float = 1,
            lineCap: int = 0,
            lineJoin: int = 0,
            overlay: bool = True,
            morph: OptSeq = None,
            stroke_opacity: float = 1,
            fill_opacity: float = 1,
            oc: int = 0,
            ) -> Point:
        """Draw a squiggly line from point p1 to point p2."""
        img = page.new_shape()
        p = img.draw_squiggle(Point(p1), Point(p2), breadth=breadth)
        img.finish(
            color=color,
            dashes=dashes,
            width=width,
            closePath=False,
            lineCap=lineCap,
            lineJoin=lineJoin,
            morph=morph,
            stroke_opacity=stroke_opacity,
            fill_opacity=fill_opacity,
            oc=oc,
        )
        img.commit(overlay)

        return p

    def draw_zigzag(
            page: Page,
            p1: point_like,
            p2: point_like,
            breadth: float = 2,
            color: OptSeq = None,
            dashes: OptStr = None,
            width: float = 1,
            lineCap: int = 0,
            lineJoin: int = 0,
            overlay: bool = True,
            morph: OptSeq = None,
            stroke_opacity: float = 1,
            fill_opacity: float = 1,
            oc: int = 0,
            ) -> Point:
        """Draw a zigzag line from point p1 to point p2."""
        img = page.new_shape()
        p = img.draw_zigzag(Point(p1), Point(p2), breadth=breadth)
        img.finish(
            color=color,
            dashes=dashes,
            width=width,
            closePath=False,
            lineCap=lineCap,
            lineJoin=lineJoin,
            morph=morph,
            stroke_opacity=stroke_opacity,
            fill_opacity=fill_opacity,
            oc=oc,
        )
        img.commit(overlay)

        return p

    @property
    def firstAnnot(self):
        """First annotation."""
        CheckParent(self)
        #val = _fitz.Page_firstAnnot(self)
        page = self._pdf_page()
        if page:
            annot = mupdf.mpdf_first_annot(page)
        val = Annot(annot) if annot else None
        if val:
            val.thisown = True
            #val.parent = weakref.proxy(self) # owning page object
            self._annot_refs[id(val)] = val
        return val

    @property
    def first_link(self):
        '''
        First link on page
        '''

    @property
    def first_widget(self):
        """First widget/field."""
        CheckParent(self)
        #val = _fitz.Page_first_widget(self)
        #pdf_annot *annot = NULL;
        annot = 0
        page = self._pdf_page()
        if page:
            annot = mupdf.mpdf_first_widget(page)
        val = Annot(annot)

        if val.this.m_internal:
            val.thisown = True
            #val.parent = weakref.proxy(self) # owning page object
            #self._annot_refs[id(val)] = val
            widget = Widget()
            TOOLS._fill_widget(val, widget)
            val = widget
        return val

    @property
    def firstWidget(self):
        """First widget/field."""
        CheckParent(self)

        #val = _fitz.Page_first_widget(self)
        annot = None
        #jlib.log('{self.this=}')
        page = self._pdf_page()# mupdf.mpdf_page_from_fz_page(self.this)
        if page.m_internal:
            annot = mupdf.mpdf_first_widget(page)
            if annot.m_internal:
                 mupdf.mpdf_keep_annot(annot)
        val = Annot(annot)

        jlib.log('{val=}')
        jlib.log('{type(val)=}')
        if val:
            val.thisown = True
            #val.parent = weakref.proxy(self) # owning page object
            self._annot_refs[id(val)] = val
            widget = Widget()
            TOOLS._fill_widget(val, widget)
            val = widget


        return val

    def get_cdrawings(self):
        """Extract drawing paths from the page."""
        CheckParent(self)
        old_rotation = self.rotation
        if old_rotation != 0:
            self.set_rotation(0)
        #val = _fitz.Page_get_cdrawings(self)
        page = self.this
        if isinstance(page, mupdf.PdfPage):
            # Downcast pdf_page to fz_page.
            page = mupdf.Page(page)
        assert isinstance(page, mupdf.Page), f'self.this={self.this}'
        rc = []
        trace_device_Linewidth = 0
        prect = mupdf.mfz_bound_page(page)
        trace_device_ptm = mupdf.mfz_make_matrix(1, 0, 0, -1, 0, prect.y1)
        dev = JM_new_tracedraw_device(rc)
        mupdf.mfz_run_page(page, dev, fz_identity, mupdf.Cookie())
        mupdf.mfz_close_device(dev)
        val = rc

        if old_rotation != 0:
            self.set_rotation(old_rotation)
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

    def get_displaylist(self, annots=1):
        '''
        Make a DisplayList from the page for Pixmap generation.

        Include (default) or exclude annotations.
        '''
        CheckParent(self)
        if annots:
            dl = mupdf.mfz_new_display_list_from_page(self.this)
        else:
            dl = mupdf.mfz_new_display_list_from_page_contents(self.this)
        return DisplayList(dl)

    def get_fonts(self, full=False):
        """List of fonts defined in the page object."""
        CheckParent(self)
        return self.parent.get_page_fonts(self.number, full=full)

    def get_image_bbox(self, name, transform=0):
        """Get rectangle occupied by image 'name'.

        'name' is either an item of the image list, or the referencing
        name string - elem[7] of the resp. item.
        Option 'transform' also returns the image transformation matrix.
        """
        CheckParent(self)
        doc = self.parent
        if doc.is_closed or doc.is_encrypted:
            raise ValueError("document closed or encrypted")

        inf_rect = Rect(1, 1, -1, -1)
        null_mat = Matrix()
        if transform:
            rc = (inf_rect, null_mat)
        else:
            rc = inf_rect

        if type(name) in (list, tuple):
            if not type(name[-1]) is int:
                raise ValueError("need item of full page image list")
            item = name
        else:
            imglist = [i for i in doc.get_page_images(self.number, True) if name == i[7]]
            if len(imglist) == 1:
                item = imglist[0]
            elif imglist == []:
                raise ValueError("bad image name")
            else:
                raise ValueError("found multiple images named '%s'." % name)
        xref = item[-1]
        if xref != 0 or transform == True:
            try:
                return self.get_image_rects(item, transform=transform)[0]
            except:
                return inf_rect


        #val = _fitz.Page_get_image_bbox(self, name, transform)
        pdf_page = self._pdf_page()
        val = JM_image_reporter(pdf_page)

        if not bool(val):
            return rc

        for v in val:
            if v[0] != item[-3]:
                continue
            q = Quad(v[1])
            bbox = q.rect
            if transform == 0:
                rc = bbox
                break

            hm = Matrix(TOOLS._hor_matrix(q.ll, q.lr))
            h = abs(q.ll - q.ul)
            w = abs(q.ur - q.ul)
            m0 = Matrix(1 / w, 0, 0, 1 / h, 0, 0)
            m = ~(hm * m0)
            rc = (bbox, m)
            break
        val = rc

        return val

    def get_image_info(self: Page, hashes: bool = False, xrefs: bool = False) -> list:
        ret = utils.get_image_info(self, hashes, xrefs)
        #jlib.log('@@@ returning: {len(ret)=}: {ret}')
        return ret


    def get_images(self, full=False):
        """List of images defined in the page object."""
        CheckParent(self)
        return self.parent.get_page_images(self.number, full=full)

    def get_links(self):
        return utils.get_links(self)

    def get_label(page):
        return utils.get_label(page)

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

    def get_pixmap(page, **kw): # -> Pixmap:
        """Create pixmap of page.

        Args:
            matrix: Matrix for transformation (default: Identity).
            colorspace: (str/Colorspace) cmyk, rgb, gray - case ignored, default csRGB.
            clip: (irect-like) restrict rendering to this area.
            alpha: (bool) whether to include alpha channel
            annots: (bool) whether to also render annotations
        """
        CheckParent(page)
        matrix = kw.get("matrix", Identity)
        colorspace = kw.get("colorspace", csRGB)
        clip = kw.get("clip")
        alpha = bool(kw.get("alpha", False))
        annots = bool(kw.get("annots", True))

        if type(colorspace) is str:
            if colorspace.upper() == "GRAY":
                colorspace = csGRAY
            elif colorspace.upper() == "CMYK":
                colorspace = csCMYK
            else:
                colorspace = csRGB
        if colorspace.n not in (1, 3, 4):
            raise ValueError("unsupported colorspace")

        dl = page.get_displaylist(annots=annots)
        pix = dl.get_pixmap(matrix=matrix, colorspace=colorspace, alpha=alpha, clip=clip)
        dl = None
        return Pixmap(pix)

    def get_svg_image(self, matrix=None, text_as_path=1):
        """Make SVG image from page."""
        CheckParent(self)
        #return _fitz.Page_get_svg_image(self, matrix, text_as_path)
        mediabox = mupdf.mfz_bound_page(self.this)
        #fz_device *dev = NULL;
        #fz_buffer *res = NULL;
        #PyObject *text = NULL;
        ctm = JM_matrix_from_py(matrix)
        #fz_output *out = NULL;
        #fz_separations *seps = NULL;
        tbounds = mediabox
        text_option = mupdf.FZ_SVG_TEXT_AS_PATH if text_as_path == 1 else muodf.FZ_SVG_TEXT_AS_TEXT
        tbounds = mupdf.mfz_transform_rect(tbounds, ctm)

        res = mupdf.mfz_new_buffer(1024)
        out = mupdf.mfz_new_output_with_buffer(res)
        dev = mupdf.mfz_new_svg_device(
                out,
                tbounds.x1-tbounds.x0,  # width
                tbounds.y1-tbounds.y0,  # height
                text_option,
                1,
                )
        mupdf.mfz_run_page(self.this, dev, ctm, mupdf.Cookie())
        mupdf.mfz_close_device(dev)
        text = JM_EscapeStrFromBuffer(res)
        return text

    def get_text(
            page: Page,
            option: str = "text",
            clip: rect_like = None,
            flags: OptInt = None,
            textpage = None,#: TextPage = None,
            sort: bool = False,
            ):
        """Extract text from a page or an annotation.

        This is a unifying wrapper for various methods of the TextPage class.

        Args:
            option: (str) text, words, blocks, html, dict, json, rawdict, xhtml or xml.
            clip: (rect-like) restrict output to this area.
            flags: bit switches to e.g. exclude images or decompose ligatures.
            textpage: reuse this TextPage and make no new one. If specified,
                'flags' and 'clip' are ignored.

        Returns:
            the output of methods get_text_words / get_text_blocks or TextPage
            methods extractText, extractHTML, extractDICT, extractJSON, extractRAWDICT,
            extractXHTML or etractXML respectively.
            Default and misspelling choice is "text".
        """
        formats = {
            "text": 0,
            "html": 1,
            "json": 1,
            "rawjson": 1,
            "xml": 0,
            "xhtml": 1,
            "dict": 1,
            "rawdict": 1,
            "words": 0,
            "blocks": 1,
        }
        option = option.lower()
        if option not in formats:
            option = "text"
        if flags is None:
            flags = TEXT_PRESERVE_WHITESPACE | TEXT_PRESERVE_LIGATURES
            if formats[option] == 1:
                flags |= TEXT_PRESERVE_IMAGES

        if option == "words":
            return get_text_words(
                page, clip=clip, flags=flags, textpage=textpage, sort=sort
            )
        if option == "blocks":
            return get_text_blocks(
                page, clip=clip, flags=flags, textpage=textpage, sort=sort
            )
        CheckParent(page)
        cb = None
        if option in ("html", "xml", "xhtml"):  # no clipping for MuPDF functions
            clip = page.cropbox
        if clip != None:
            clip = Rect(clip)
            cb = None
        elif type(page) is Page:
            cb = page.cropbox
        # TextPage with or without images
        tp = textpage
        if tp is None:
            tp = page.get_textpage(clip=clip, flags=flags)
        elif getattr(tp, "parent") != page:
            raise ValueError("not a textpage of this page")

        if option == "json":
            t = tp.extractJSON(cb=cb, sort=sort)
        elif option == "rawjson":
            t = tp.extractRAWJSON(cb=cb, sort=sort)
        elif option == "dict":
            t = tp.extractDICT(cb=cb, sort=sort)
        elif option == "rawdict":
            t = tp.extractRAWDICT(cb=cb, sort=sort)
        elif option == "html":
            t = tp.extractHTML()
        elif option == "xml":
            t = tp.extractXML()
        elif option == "xhtml":
            t = tp.extractXHTML()
        else:
            t = tp.extractText(sort=sort)

        if textpage is None:
            del tp
        return t


    def get_textbox(
            page: Page,
            rect: rect_like,
            textpage=None,  #: TextPage = None,
            ) -> str:
        tp = textpage
        if tp is None:
            tp = page.get_textpage()
        elif getattr(tp, "parent") != page:
            raise ValueError("not a textpage of this page")
        rc = tp.extractTextbox(rect)
        if textpage is None:
            del tp
        return rc

    def get_textpage(self, clip: rect_like = None, flags: int = 0, matrix=None) -> "TextPage":
        CheckParent(self)
        if matrix is None:
            matrix = Matrix(1, 1)
        old_rotation = self.rotation
        if old_rotation != 0:
            self.set_rotation(0)
        try:
            textpage = self._get_textpage(clip, flags=flags, matrix=matrix)
        finally:
            if old_rotation != 0:
                self.set_rotation(old_rotation)
        #textpage.parent = weakref.proxy(self)
        #jlib.log('{textpage=}')
        textpage = TextPage(textpage)
        #jlib.log('{textpage=}')
        return textpage

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

    def insert_font(self, fontname="helv", fontfile=None, fontbuffer=None,
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
            # need to build the doc FontInfo entry - done via get_char_widths
            doc.get_char_widths(xref)
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
        #jlib.log('{fontname=} {bfname=} {fontfile=} {fontbuffer=} {set_simple=} {idx=} {wmode=} {serif=} {encoding=} {CJK_number=}')
        val = self._insertFont(fontname, bfname, fontfile, fontbuffer, set_simple, idx,
                               wmode, serif, encoding, CJK_number)

        if not val:                   # did not work, error return
            return val

        xref = val[0]                 # xref of installed font
        fontdict = val[1]
        #jlib.log('{type(fontdict)=}')

        if CheckFontInfo(doc, xref):  # check again: document already has this font
            return xref               # we are done

        # need to create document font info
        doc.get_char_widths(xref, fontdict=fontdict)
        return xref

    def insert_text(
            page,
            point: point_like,
            text: typing.Union[str, list],
            fontsize: float = 11,
            lineheight: OptFloat = None,
            fontname: str = "helv",
            fontfile: OptStr = None,
            set_simple: int = 0,
            encoding: int = 0,
            color: OptSeq = None,
            fill: OptSeq = None,
            border_width: float = 1,
            render_mode: int = 0,
            rotate: int = 0,
            morph: OptSeq = None,
            overlay: bool = True,
            stroke_opacity: float = 1,
            fill_opacity: float = 1,
            oc: int = 0,
            ):
        img = page.new_shape()
        rc = img.insert_text(
                point,
                text,
                fontsize=fontsize,
                lineheight=lineheight,
                fontname=fontname,
                fontfile=fontfile,
                set_simple=set_simple,
                encoding=encoding,
                color=color,
                fill=fill,
                border_width=border_width,
                render_mode=render_mode,
                rotate=rotate,
                morph=morph,
                stroke_opacity=stroke_opacity,
                fill_opacity=fill_opacity,
                oc=oc,
                )
        if rc >= 0:
            img.commit(overlay)
        return rc

    def insert_image(page, rect, **kwargs):
        return utils.insert_image(page, rect, **kwargs)

    def insert_textbox(*args, **kwargs):
        return utils.insert_textbox(*args, **kwargs)

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

    @property
    def language(self):
        """Page language."""
        #return _fitz.Page_language(self)
        pdfpage = mupdf.mpdf_page_from_fz_page(self.this)
        if not pdfpage.m_internal:
            return
        lang = mupdf.mpdf_dict_get_inheritable(pdfpage.obj(), PDF_NAME('Lang'))
        if not lang.m_internal:
            return
        return mupdf.mpdf_to_str_buf(lang)

    def load_links(self):
        return sel.loadLinks()

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
        #val.parent = weakref.proxy(self)
        self._annot_refs[id(val)] = val
        return val

    load_annot = loadAnnot

    def loadLinks(self):
        """Get first Link."""
        CheckParent(self)

        val = _fitz.Page_loadLinks(self)

        if val:
            val.thisown = True
            val.parent = weakref.proxy(self) # owning page object
            self._annot_refs[id(val)] = val
            if self.parent.isPDF:
                link_id = [x for x in self.annot_xrefs() if x[1] == mupdf.PDF_ANNOT_LINK][0]
                val.xref = link_id[0]
                val.id = link_id[2]
            else:
                val.xref = 0
                val.id = ""
        return val

    @property
    def mediabox(self):
        """The MediaBox."""
        CheckParent(self)
        #val = _fitz.Page_mediabox(self)
        page = self._pdf_page()
        if not page:
            val = JM_py_from_rect(mupdf.mfz_bound_page(self.this))
        else:
            val = JM_py_from_rect(JM_mediabox(page.page_obj))
        val = Rect(val)
        return val

    def new_shape(self):
        return Shape(self)

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

    @property
    def rotationMatrix(self) -> Matrix:
        """Reflects page rotation."""
        return Matrix(TOOLS._rotate_matrix(self))

    def run(self, dw, m):
        """Run page through a device.
        dw: DeviceWrapper
        """
        CheckParent(self)
        #return _fitz.Page_run(self, dw, m)
        mupdf.mfz_run_page(self.this, dw.device, JM_matrix_from_py(m), mupdf.Cookie());


    def search_for(*args, **kwargs) -> list:
        """Search for a string on a page.

        Args:
            text: string to be searched for
            clip: restrict search to this rectangle
            quads: (bool) return quads instead of rectangles
            flags: bit switches, default: join hyphened words
            textpage: a pre-created TextPage
        Returns:
            a list of rectangles or quads, each containing one occurrence.
        """
        if len(args) != 2:
            raise ValueError("bad number of positional parameters")
        page, text = args
        quads = kwargs.get("quads", 0)
        clip = kwargs.get("clip")
        #jlib.log('{clip=}')
        textpage = kwargs.get("textpage")
        if clip != None:
            clip = Rect(clip)
        flags = kwargs.get(
            "flags",
            TEXT_DEHYPHENATE | TEXT_PRESERVE_WHITESPACE | TEXT_PRESERVE_LIGATURES,
            )
        #jlib.log('{flags=}')

        CheckParent(page)
        tp = textpage
        if tp is None:
            tp = page.get_textpage(clip=clip, flags=flags)  # create TextPage
        elif getattr(tp, "parent") != page:
            raise ValueError("not a textpage of this page")
        #jlib.log('{type(tp)=}')
        rlist = tp.search(text, quads=quads)
        #jlib.log('returning {len(rlist)=} {rlist=}')
        return rlist

    def set_contents(self, xref):
        """Set an xref as the (only) /Contents object."""
        CheckParent(self)

        return Page_set_contents(self, xref)

    def set_language(self, language=None):
        """Set PDF page default language."""
        CheckParent(self)
        #return _fitz.Page_set_language(self, language)
        pdfpage = mupdf.mpdf_page_from_fz_page(self.this)
        ASSERT_PDF(pdfpage)
        if not language:
            pdf_dict_del(pdfpage.obj(), PDF_NAME('Lang'))
        else:
            lang = mupdf.mfz_text_language_from_string(language)
            mupdf.mpdf_dict_put_text_string(
                    pdfpage.obj,
                    PDF_NAME(Lang),
                    mupdf.mfz_string_from_text_language(buf, lang)  # fixme: needs wrapper to handle char buf[8].
                    )

    def setCropBox(self, rect):
        """Set the CropBox."""
        CheckParent(self)
        return _fitz.Page_setCropBox(self, rect)

    def setMediaBox(self, rect):
        """Set the MediaBox."""
        CheckParent(self)
        return _fitz.Page_setMediaBox(self, rect)

    firstLink = property(loadLinks, doc="First link on page")

    def setRotation(self, rotation):
        """Set page rotation."""
        CheckParent(self)

        return _fitz.Page_setRotation(self, rotation)

    def show_pdf_page(*args, **kwargs) -> int:
        """Show page number 'pno' of PDF 'src' in rectangle 'rect'.

        Args:
            rect: (rect-like) where to place the source image
            src: (document) source PDF
            pno: (int) source page number
            overlay: (bool) put in foreground
            keep_proportion: (bool) do not change width-height-ratio
            rotate: (int) degrees (multiple of 90)
            clip: (rect-like) part of source page rectangle
        Returns:
            xref of inserted object (for reuse)
        """
        if len(args) not in (3, 4):
            raise ValueError("bad number of positional parameters")
        pno = None
        if len(args) == 3:
            page, rect, src = args
        else:
            page, rect, src, pno = args
        if pno == None:
            pno = int(kwargs.get("pno", 0))
        overlay = bool(kwargs.get("overlay", True))
        keep_proportion = bool(kwargs.get("keep_proportion", True))
        rotate = float(kwargs.get("rotate", 0))
        oc = int(kwargs.get("oc", 0))
        reuse_xref = int(kwargs.get("reuse_xref", 0))
        clip = kwargs.get("clip")

        def calc_matrix(sr, tr, keep=True, rotate=0):
            """Calculate transformation matrix from source to target rect.

            Notes:
                The product of four matrices in this sequence: (1) translate correct
                source corner to origin, (2) rotate, (3) scale, (4) translate to
                target's top-left corner.
            Args:
                sr: source rect in PDF (!) coordinate system
                tr: target rect in PDF coordinate system
                keep: whether to keep source ratio of width to height
                rotate: rotation angle in degrees
            Returns:
                Transformation matrix.
            """
            # calc center point of source rect
            smp = (sr.tl + sr.br) / 2.0
            # calc center point of target rect
            tmp = (tr.tl + tr.br) / 2.0

            # m moves to (0, 0), then rotates
            m = Matrix(1, 0, 0, 1, -smp.x, -smp.y) * Matrix(rotate)

            sr1 = sr * m  # resulting source rect to calculate scale factors

            fw = tr.width / sr1.width  # scale the width
            fh = tr.height / sr1.height  # scale the height
            if keep:
                fw = fh = min(fw, fh)  # take min if keeping aspect ratio

            m *= Matrix(fw, fh)  # concat scale matrix
            m *= Matrix(1, 0, 0, 1, tmp.x, tmp.y)  # concat move to target center
            return JM_TUPLE(m)

        CheckParent(page)
        doc = page.parent

        if not doc.is_pdf or not src.is_pdf:
            raise ValueError("not a PDF")

        rect = page.rect & rect  # intersect with page rectangle
        if rect.is_empty or rect.is_infinite:
            raise ValueError("rect must be finite and not empty")

        if reuse_xref > 0:
            warnings.warn("ignoring 'reuse_xref'", DeprecationWarning)

        while pno < 0:  # support negative page numbers
            pno += src.page_count
        src_page = src[pno]  # load source page
        if src_page.get_contents() == []:
            raise ValueError("nothing to show - source page empty")

        tar_rect = rect * ~page.transformation_matrix  # target rect in PDF coordinates

        src_rect = src_page.rect if not clip else src_page.rect & clip  # source rect
        if src_rect.is_empty or src_rect.is_infinite:
            raise ValueError("clip must be finite and not empty")
        src_rect = src_rect * ~src_page.transformation_matrix  # ... in PDF coord

        matrix = calc_matrix(src_rect, tar_rect, keep=keep_proportion, rotate=rotate)

        # list of existing /Form /XObjects
        ilst = [i[1] for i in doc.get_page_xobjects(page.number)]
        ilst += [i[7] for i in doc.get_page_images(page.number)]
        ilst += [i[4] for i in doc.get_page_fonts(page.number)]

        # create a name not in that list
        n = "fzFrm"
        i = 0
        _imgname = n + "0"
        while _imgname in ilst:
            i += 1
            _imgname = n + str(i)

        isrc = src._graft_id  # used as key for graftmaps
        if doc._graft_id == isrc:
            raise ValueError("source document must not equal target")

        # retrieve / make Graftmap for source PDF
        gmap = doc.Graftmaps.get(isrc, None)
        if gmap is None:
            gmap = Graftmap(doc)
            doc.Graftmaps[isrc] = gmap

        # take note of generated xref for automatic reuse
        pno_id = (isrc, pno)  # id of src[pno]
        xref = doc.ShownPages.get(pno_id, 0)

        xref = page._show_pdf_page(
            src_page,
            overlay=overlay,
            matrix=matrix,
            xref=xref,
            oc=oc,
            clip=src_rect,
            graftmap=gmap,
            _imgname=_imgname,
        )
        doc.ShownPages[pno_id] = xref

        return xref
    def refresh(self):
        """Refresh page after link/annot/widget updates."""
        CheckParent(self)
        return _fitz.Page_refresh(self)

    @property
    def transformationMatrix(self):
        """Page transformation matrix."""
        CheckParent(self)

        #val = _fitz.Page_transformationMatrix(self)
        ctm = mupdf.Matrix()
        page = self._pdf_page()
        if not page.m_internal:
            return JM_py_from_matrix(ctm)
        mediabox = mupdf.Rect(mupdf.Rect.Fixed_UNIT)    # fixme: original code passed mediabox=NULL.
        page.page_transform(mediabox, ctm)
        val = JM_py_from_matrix(ctm)

        if self.rotation % 360 == 0:
            val = Matrix(val)
        else:
            val = Matrix(1, 0, 0, -1, 0, self.CropBox.height)
        return val

    @property
    def transformation_matrix(self):
        return self.transformationMatrix

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

    getFontList = get_fonts

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

    @property
    def mediabox_size(self):
        return self.MediaBoxSize

    cleanContents = clean_contents
    getContents = get_contents

    def write_text(self, **kwargs):
        #jlib.log('{kwargs=}')
        return utils.write_text(self, **kwargs)


class Pixmap:

    def __init__(self, *args):
        """
        Pixmap(colorspace, irect, alpha) - empty pixmap.
        Pixmap(colorspace, src) - copy changing colorspace.
        Pixmap(src, width, height,[clip]) - scaled copy, float dimensions.
        Pixmap(src, alpha=1) - copy and add or drop alpha channel.
        Pixmap(filename) - from an image in a file.
        Pixmap(image) - from an image in memory (bytes).
        Pixmap(colorspace, width, height, samples, alpha) - from samples data.
        Pixmap(PDFdoc, xref) - from an image at xref in a PDF document.
        """
        # From PyMuPDF/fitz/fitz.i:struct Pixmap {...}.
        #
        if 0:
            pass

        elif args_match(args, mupdf.Colorspace, mupdf.Rect, int):
            # create empty pixmap with colorspace and IRect
            pm = mupdf.mfz_new_pixmap_with_bbox(args[0], JM_irect_from_py(args[1]), mupdf.Separations(0), args[2])
            self.this = pm

        elif args_match(args, mupdf.Colorspace, mupdf.Pixmap):
            # copy pixmap, converting colorspace
            if not mupdf.mfz_pixmap_colorspace(args[1]):
                THROWMSG("cannot copy pixmap without colorspace");
            pm = mupdf.mfz_convert_pixmap(
                    args[1],
                    args[0],
                    mupdf.Colorspace(0),
                    mupdf.ColorParams(),
                    1,
                    )
            self.this = pm

        elif args_match(args, mupdf.Pixmap, mupdf.Pixmap):
            # add mask to a non-transparent pixmap
            color, mask = args
            w = color.w
            h = color.h
            n = color.n
            if color.alpha:
                THROWMSG("color pixmap must not have an alpha channel")
            if mask.n != 1:
                THROWMSG("mask pixmap must have exactly one channel")
            if mask.w != color.w or mask.h != color.h:
                THROWMSG("color and mask pixmaps must be the same size")

            dst = mupdf.mfz_new_pixmap_with_bbox(color.colorspace(), mupdf.mfz_pixmap_bbox(color), mupdf.Separations(0), 1)
            for y in range(h):
                cs = y * color.m_internal.stride
                ms = y * mask.m_internal.stride
                ds = y * dst.m_internal.stride
                for x in range(w):
                    a = mask.samples_get(ms)
                    ms += 1
                    for k in range(n):
                        dst.samples_set(ds, mupdf.mfz_mul255(color.samples.get(cs), a))
                        ds += 1
                        cs += 1
                    dst.samples.set(ds, a)
                    ds += 1
            self.this = dst

        elif args_match(args, (Pixmap, mupdf.Pixmap), (float, int), (float, int), None):
            # create pixmap as scaled copy of another one
            assert 0, f'Cannot handle args={args} because fz_scale_pixmap() and fz_scale_pixmap_cached() are not declared in MuPDF headers'
            spix, w, h, clip = args
            src_pix = spix.this if isinstance(spix, Pixmap) else spix
            bbox = JM_irect_from_py(clip)
            if not mupdf.mfz_is_infinite_irect(bbox):
                pm = mupdf.mfz_scale_pixmap_cached(src_pix, src_pix.x, src_pix.y, w, h, bbox)
            else:
                pm = mupdf.mfz_scale_pixmap(src_pix, src_pix.x, src_pix.y, w, h, NULL);
            self.this = pm

        elif args_match(args, (Pixmap, mupdf.Pixmap), (int, None)):
            # copy pixmap & add / drop the alpha channel
            #jlib.log('{args=}')
            spix = args[0]
            alpha = args[1] if len(args) == 2 else 1
            src_pix = spix.this if isinstance(spix, Pixmap) else spix
            if not INRANGE(alpha, 0, 1):
                THROWMSG("bad alpha value")
            cs = mupdf.mfz_pixmap_colorspace(src_pix)
            if not cs.m_internal and not alpha:
                THROWMSG("cannot drop alpha for 'NULL' colorspace")
            seps = mupdf.Separations()
            n = mupdf.mfz_pixmap_colorants(src_pix)
            w = mupdf.mfz_pixmap_width(src_pix)
            h = mupdf.mfz_pixmap_height(src_pix)
            pm = mupdf.mfz_new_pixmap(cs, w, h, seps, alpha)
            #jlib.log('{pm=} {src_pix=}')
            pm.m_internal.x = src_pix.m_internal.x
            pm.m_internal.y = src_pix.m_internal.y
            pm.m_internal.xres = src_pix.m_internal.xres
            pm.m_internal.yres = src_pix.m_internal.yres

            # copy samples data ------------------------------------------
            if src_pix.alpha() == pm.alpha():   # identical samples
                #memcpy(tptr, sptr, w * h * (n + alpha));
                # fixme: inefficient.
                for i in range(w * h * (n + alpha)):
                    pm.samples_set(i, src_pix.samples_get(i))
            else:
                tptr = 0
                sptr = 0
                for i in range(w * h):
                    #memcpy(tptr, sptr, n);
                    # fixme: inefficient.
                    for j in range(n):
                        pm.samples_set(tptr + j, src_pix.samples_get(sptr + j))
                    tptr += n
                    if pm.alpha():
                        pm.samples_set(tptr, 255)
                        tptr += 1
                    sptr += n + src_pix.alpha()
            self.this = pm

        elif args_match(args, mupdf.Colorspace, int, int, None, int):
            # create pixmap from samples data
            cs, w, h, samples, alpha = args
            n = mupdf.mfz_colorspace_n(cs)
            stride = (n + alpha)*w
            size = 0;
            res = JM_BufferFromBytes(samples);
            if not res.m_internal:
                THROWMSG("bad samples data")
            size, c = mupdf.mfz_buffer_storage_raw(res)
            if stride * h != size:
                THROWMSG("bad samples length")
            pm = mupdf.mfz_new_pixmap(cs, w, h, seps, alpha)
            assert 0, 'cannot memcpy from buffer to pixmap samples.'
            # do we need one of these?:
            #   fz_pixmap *fz_new_pixmap_from_buffer(fz_buffer* buffer,
            #           fz_colorspace *cs, int w, int h, fz_separations *seps, int alpha
            #           );
            #   fz_pixmap *fz_new_pixmap_from_samples(const unsigned char* samples,
            #           fz_colorspace *cs, int w, int h, fz_separations *seps, int alpha
            #           );
            #

            #memcpy(pm->samples, c, size);
            self.this = pm

        elif args_match(args, None):
            # create pixmap from filename, file object, pathlib.Path or memory
            #jlib.log('{args=}')
            imagedata, = args
            name = 'name'
            if hasattr(imagedata, "resolve"):
                fname = imagedata.__str__()
                if fname:
                    img = mupdf.mfz_new_image_from_file(fname)
            elif hasattr(imagedata, name):
                fname = imagedata.name
                if fname:
                    img = mupdf.mfz_new_image_from_file(fname)
            elif isinstance(imagedata, str):
                img = mupdf.mfz_new_image_from_file(imagedata)
            else:
                res = JM_BufferFromBytes(imagedata)
                if not res.m_internal:
                    THROWMSG("bad image data")
                size, data = res.buffer_storage_raw()
                if not size:
                    THROWMSG("bad image data")
                img = mupdf.mfz_new_image_from_buffer(res)

            pm, w, h = mupdf.mfz_get_pixmap_from_image(
                    img,
                    mupdf.Irect(FZ_MIN_INF_RECT, FZ_MIN_INF_RECT, FZ_MAX_INF_RECT, FZ_MAX_INF_RECT),
                    mupdf.Matrix(),
                    )
            xres, yres = img.image_resolution()
            pm.xres = xres
            pm.yres = yres
            self.this = pm

        elif args_match(args, (Document, mupdf.Document), int):
            # Create pixmap from PDF image identified by XREF number
            #jlib.log('Create pixmap from ((Document, mupdf.Document), int)')
            doc, xref = args
            #jlib.log('{doc=} {xref=}')
            if isinstance(doc, Document):
                doc = doc.this
            pdf = mupdf.mpdf_specifics(doc)
            ASSERT_PDF(pdf)
            xreflen = mupdf.mpdf_xref_len(pdf)
            #jlib.log('{xreflen=}')
            if not INRANGE(xref, 1, xreflen-1):
                THROWMSG("bad xref")
            ref = mupdf.mpdf_new_indirect(pdf, xref, 0)
            type_ = mupdf.mpdf_dict_get(ref, PDF_NAME('Subtype'))
            if not mupdf.mpdf_name_eq(type_, PDF_NAME('Image')):
                THROWMSG("not an image");
            img = mupdf.mpdf_load_image(pdf, ref)
            #jlib.log('img: {img.w()=} {img.h()=} {img.n()=} {img.bpc()=} {img.xres()=} {img.yres()=}')
            # Original code passed null for subarea and ctm, but that's not
            # possible with MuPDF's python bindings, so instead we pass an
            # infinite rect and identify matrix.
            pix, w, h = mupdf.mfz_get_pixmap_from_image(
                    img,
                    mupdf.Irect(FZ_MIN_INF_RECT, FZ_MIN_INF_RECT, FZ_MAX_INF_RECT, FZ_MAX_INF_RECT),
                    mupdf.Matrix(img.w(), 0, 0, img.h(), 0, 0),
                    )
            self.this = pix

        else:
            raise Exception(f'Unrecognised args for constructing Pixmap: {args}')

        self.samples_ptr = self._samples_ptr()
        self.samples_mv = self._samples_mv()

    def __len__(self):
        return self.size

    def __repr__(self):
        if not type(self) is Pixmap: return
        if self.colorspace:
            return "Pixmap(%s, %s, %s)" % (self.colorspace.this.m_internal.name, self.irect, self.alpha)
        else:
            return "Pixmap(%s, %s, %s)" % ('None', self.irect, self.alpha)

    def _getImageData(self, format):
        return _fitz.Pixmap__getImageData(self, format)

    def _samples_mv(self):
        #return _fitz.Pixmap__samples_mv(self)
        raw_data = self.this.samples()
        raw_len = self.this.stride() * self.this.h()
        return raw_data, raw_len

    def _samples_ptr(self):
        #return _fitz.Pixmap__samples_ptr(self)
        raw_data = self.this.samples()
        raw_len = self.this.stride() * self.this.h()
        return raw_data, raw_len

    def _tobytes(self, format_):
        #return _fitz.Pixmap__tobytes(self, format)
        #fz_output *out = NULL;
        #fz_buffer *res = NULL;
        #PyObject *barray = NULL;
        pm = self.this
        size = mupdf.mfz_pixmap_stride(pm) * pm.h();
        res = mupdf.mfz_new_buffer(size)
        out = mupdf.Output(res)
        #jlib.log('{type(out)=}')
        if   format_ == 1:  mupdf.mfz_write_pixmap_as_png(out, pm)
        elif format_ == 2:  mupdf.mfz_write_pixmap_as_pnm(out, pm)
        elif format_ == 3:  mupdf.mfz_write_pixmap_as_pam(out, pm)
        elif format_ == 5:  mupdf.mfz_write_pixmap_as_psd(out, pm)
        elif format_ == 6:  mupdf.mfz_write_pixmap_as_ps(out, pm)
        else:               mupdf.mfz_write_pixmap_as_png(out, pm)

        barray = JM_BinFromBuffer(res)
        return barray

    def _writeIMG(self, filename, format_):
        #return _fitz.Pixmap__writeIMG(self, filename, format)
        pm = self.this
        if   format_ == 1:  mupdf.mfz_save_pixmap_as_png(pm, filename)
        elif format_ == 2:  mupdf.mfz_save_pixmap_as_pnm(pm, filename)
        elif format_ == 3:  mupdf.mfz_save_pixmap_as_pam(pm, filename)
        elif format_ == 5:  mupdf.mfz_save_pixmap_as_psd(pm, filename)
        elif format_ == 6:  mupdf.mfz_save_pixmap_as_ps(pm, filename)
        else:               mupdf.mfz_save_pixmap_as_png(pm, filename)

    @property
    def alpha(self):
        """Indicates presence of alpha channel."""
        #return _fitz.Pixmap_alpha(self)
        return mupdf.mfz_pixmap_alpha(self.this)

    def clearWith(self, *args):
        """Fill all color components with same value."""

        return _fitz.Pixmap_clearWith(self, *args)

    @property
    def colorspace(self):
        """Pixmap Colorspace."""
        #return _fitz.Pixmap_colorspace(self)
        return Colorspace(mupdf.mfz_pixmap_colorspace(self.this))

    def copyPixmap(self, src, bbox):
        """Copy bbox from another Pixmap."""

        return _fitz.Pixmap_copyPixmap(self, src, bbox)

    @property
    def digest(self):
        """MD5 digest of pixmap (bytes)."""
        #return _fitz.Pixmap_digest(self)
        return self.this.md5_pixmap()

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

    def getPNGData(self):
        """Wrapper for Pixmap.getImageData("png")."""
        barray = self._getImageData(1)
        return barray

    @property
    def h(self):
        """The height."""
        #return _fitz.Pixmap_h(self)
        return mupdf.mfz_pixmap_height(self.this)

    def invertIRect(self, bbox=None):
        """Invert the colors inside a bbox."""

        return _fitz.Pixmap_invertIRect(self, bbox)

    @property
    def irect(self):
        """Pixmap bbox - an IRect object."""
        #val = _fitz.Pixmap_irect(self)
        val = mupdf.mfz_pixmap_bbox(self.this)
        val = IRect(val)
        return val

    @property
    def n(self):
        """The size of one pixel."""
        #return _fitz.Pixmap_n(self)
        return mupdf.mfz_pixmap_components(self.this)

    def pillowData(self, *args, **kwargs):
        """Convert to binary image stream using pillow.

        Arguments are passed to Pillow's Image.save() method.
        Use it instead of writeImage when other output formats are needed.
        """
        from io import BytesIO
        bytes_out = BytesIO()
        self.pillowWrite(bytes_out, *args, **kwargs)
        return bytes_out.getvalue()

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

    def pixel(self, x, y):
        """Get color tuple of pixel (x, y).
        Last item is the alpha if Pixmap.alpha is true."""

        return _fitz.Pixmap_pixel(self, x, y)

    @property
    def samples(self)->bytes:
        raw_data, raw_len = self.samples_mv
        return mupdf.raw_to_python_bytes( raw_data, raw_len)

    def save(self, filename, output=None):
        """Output as image in format determined by filename extension.

        Args:
            output: (str) only use to overrule filename extension. Default is PNG.
                    Others are PNM, PGM, PPM, PBM, PAM, PSD, PS.
        """
        valid_formats = {"png": 1, "pnm": 2, "pgm": 2, "ppm": 2, "pbm": 2,
                         "pam": 3, "tga": 4, "tpic": 4,
                         "psd": 5, "ps": 6}
        if type(filename) is str:
            pass
        elif hasattr(filename, "absolute"):
            filename = str(filename)
        elif hasattr(filename, "name"):
            filename = filename.name
        if output is None:
            _, ext = os.path.splitext(filename)
            output = ext[1:]

        idx = valid_formats.get(output.lower(), 1)

        if self.alpha and idx in (2, 6):
            raise ValueError("'%s' cannot have alpha" % output)
        if self.colorspace and self.colorspace.this.m_internal.n > 3 and idx in (1, 2, 4):
            raise ValueError("unsupported colorspace for '%s'" % output)

        return self._writeIMG(filename, idx)

    def set_alpha(self, alphavalues=None, premultiply=1, opaque=None):
        """Set alpha channel to values contained in a byte array.
        If omitted, set alphas to 255.

        Args:
            alphavalues: (bytes) with length (width * height) values in range(255).
            premultiply: (bool, True) premultiply colors with alpha values.
            opaque: (tuple) length colorspace.n, color value to set to opacity 0.
        """
        #return _fitz.Pixmap_set_alpha(self, alphavalues, premultiply, opaque)
        #fz_buffer *res = NULL;
        pix = self.this
        divisor = 255
        if pix.alpha() == 0:
            THROWMSG("pixmap has no alpha")
        n = mupdf.mfz_pixmap_colorants(pix)
        w = mupdf.mfz_pixmap_width(pix)
        h = mupdf.mfz_pixmap_height(pix)
        balen = w * h * (n+1)
        colors = [0, 0, 0, 0]
        zero_out = 0
        if opaque and isinstance(opaque, (list, tuple)) and len(opaque) == n:
            for i in range(n):
                colors[i] = opaque[i]
            zero_out = 1
        #unsigned char *data = NULL;
        data_len = 0;

        data_len = None
        if alphavalues:
            #res = JM_BufferFromBytes(alphavalues)
            #data_len, data = mupdf.mfz_buffer_storage(res)
            #if data_len < w * h:
            #    THROWMSG("bad alpha values")
            # fixme: don't seem to need to create an fz_buffer - can
            # use <alphavalues> directly?
            if isinstance(alphavalues, (bytes, bytearray)):
                data = alphavalues
                data_len = len(alphavalues)
            else:
                assert 0, f'unexpected type for alphavalues: {type(alphavalues)}'
            if data_len < w * h:
                THROWMSG("bad alpha values")
        i = k = j = 0
        data_fix = 255
        while i < balen:
            if zero_out:
                for j in range(i, i+n):
                    if pix.samples_get(j) != colors[j - i]:
                        data_fix = 255
                        break
                    else:
                        data_fix = 0
            if data_len:
                if data_fix == 0:
                    pix.samples_set(i+n, 0)
                else:
                    pix.samples_set(i+n, data[k])
                if premultiply == 1:
                    denom = int(data[k])
                    for j in range(i, i+n):
                        pix.samples_set(j, pix.samples_get(j) * denom // divisor)
            else:
                pixsamples_set(i+n, data_fix)
            i += n+1
            k += 1


    def setAlpha(self, alphavalues=None, premultiply=1):
        """Set alphas to values contained in a byte array.
        If omitted, set alphas to 255."""

        return _fitz.Pixmap_setAlpha(self, alphavalues, premultiply)

    def setOrigin(self, x, y):
        """Set top-left coordinates."""
        return _fitz.Pixmap_setOrigin(self, x, y)

    def setPixel(self, x, y, color):
        """Set color of pixel (x, y)."""
        return _fitz.Pixmap_setPixel(self, x, y, color)

    def setRect(self, bbox, color):
        """Set color of all pixels in bbox."""
        return _fitz.Pixmap_setRect(self, bbox, color)

    def setResolution(self, xres, yres):
        """Set resolution in both dimensions.
        Use pillowWrite to reflect this in output image."""
        return _fitz.Pixmap_setResolution(self, xres, yres)

    def shrink(self, factor):
        """Divide width and height by 2**factor.
        E.g. factor=1 shrinks to 25% of original size (in place)."""

        return _fitz.Pixmap_shrink(self, factor)

    def size(self):
        """Pixmap size."""
        return _fitz.Pixmap_size(self)

    @property
    def stride(self):
        """Length of one image line (width * n)."""
        #return _fitz.Pixmap_stride(self)
        return self.this.stride()

    def tintWith(self, black, white):
        return _fitz.Pixmap_tintWith(self, black, white)

    def tobytes(self, output="png"):
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
        if self.colorspace and self.colorspace.this.m_internal.n > 3 and idx in (1, 2, 4):
            raise ValueError("unsupported colorspace for '%s'" % output)
        barray = self._tobytes(idx)
        return barray

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

    @property
    def w(self):
        """The width."""
        #return _fitz.Pixmap_w(self)
        return mupdf.mfz_pixmap_width(self.this)

    @property
    def x(self):
        """x component of Pixmap origin."""
        #return _fitz.Pixmap_x(self)
        return mupdf.mfz_pixmap_x(self.this)

    @property
    def xres(self):
        """Resolution in x direction."""
        #return _fitz.Pixmap_xres(self)
        return self.this.xres()

    @property
    def y(self):
        """y component of Pixmap origin."""
        #return _fitz.Pixmap_y(self)
        return mupdf.mfz_pixmap_y(self.this)

    @property
    def yres(self):
        """Resolution in y direction."""
        #return _fitz.Pixmap_yres(self)
        return mupdf.mfz_pixmap_width(self.this)

    width  = w
    height = h


class Point(object):
    """Point() - all zeros
    Point(x, y)
    Point(Point) - new copy
    Point(sequence) - from 'sequence'
    """
    def __abs__(self):
        return math.sqrt(self.x * self.x + self.y * self.y)

    def __add__(self, p):
        if hasattr(p, "__float__"):
            return Point(self.x + p, self.y + p)
        if len(p) != 2:
            raise ValueError("bad Point: sequ. length")
        return Point(self.x + p[0], self.y + p[1])

    def __bool__(self):
        return not (max(self) == min(self) == 0)

    def __eq__(self, p):
        if not hasattr(p, "__len__"):
            return False
        return len(p) == 2 and bool(self - p) is False

    def __getitem__(self, i):
        return (self.x, self.y)[i]

    def __hash__(self):
        return hash(tuple(self))

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

    def __len__(self):
        return 2

    def __mul__(self, m):
        if hasattr(m, "__float__"):
            return Point(self.x * m, self.y * m)
        p = Point(self)
        return p.transform(m)

    def __neg__(self):
        return Point(-self.x, -self.y)

    def __nonzero__(self):
        return not (max(self) == min(self) == 0)

    def __pos__(self):
        return Point(self)

    def __repr__(self):
        return "Point" + str(tuple(self))

    def __setitem__(self, i, v):
        v = float(v)
        if   i == 0: self.x = v
        elif i == 1: self.y = v
        else:
            raise IndexError("index out of range")
        return None

    def __sub__(self, p):
        if hasattr(p, "__float__"):
            return Point(self.x - p, self.y - p)
        if len(p) != 2:
            raise ValueError("bad Point: sequ. length")
        return Point(self.x - p[0], self.y - p[1])

    def __truediv__(self, m):
        if hasattr(m, "__float__"):
            return Point(self.x * 1./m, self.y * 1./m)
        m1 = TOOLS._invert_matrix(m)[1]
        if not m1:
            raise ZeroDivisionError("matrix not invertible")
        p = Point(self)
        return p.transform(m1)

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

    __div__ = __truediv__
    norm = __abs__


class Quad(object):
    """Quad() - all zero points\nQuad(ul, ur, ll, lr)\nQuad(quad) - new copy\nQuad(sequence) - from 'sequence'"""

    def __abs__(self):
        if self.is_empty:
            return 0.0
        return abs(self.ul - self.ur) * abs(self.ul - self.ll)

    def __bool__(self):
        return not self.is_empty

    def __eq__(self, quad):
        if not hasattr(quad, "__len__"):
            return False
        return len(quad) == 4 and (
            self.ul == quad[0] and
            self.ur == quad[1] and
            self.ll == quad[2] and
            self.lr == quad[3]
        )

    def __getitem__(self, i):
        return (self.ul, self.ur, self.ll, self.lr)[i]

    def __hash__(self):
        return hash(tuple(self))

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
            if isinstance(l, mupdf.Quad):
                self.this = l
                self.ul, self.ur, self.ll, self.lr = l.ul, l.ur, l.ll, l.lr
                return
            if hasattr(l, "__getitem__") is False:
                raise ValueError("bad Quad constructor")
            if len(l) != 4:
                raise ValueError("bad Quad: sequ. length")
            self.ul, self.ur, self.ll, self.lr = map(Point, l)
            return None
        raise ValueError("bad Quad constructor")

    def __len__(self):
        return 4

    def __mul__(self, m):
        r = Quad(self)
        r = r.transform(m)
        return r

    def __neg__(self):
        return Quad(-self.ul, -self.ur, -self.ll, -self.lr)

    def __nonzero__(self):
        return not self.is_empty

    def __pos__(self):
        return Quad(self)

    def __repr__(self):
        return "Quad" + str(tuple(self))

    def __setitem__(self, i, v):
        if   i == 0: self.ul = Point(v)
        elif i == 1: self.ur = Point(v)
        elif i == 2: self.ll = Point(v)
        elif i == 3: self.lr = Point(v)
        else:
            raise IndexError("index out of range")
        return None

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

    @property
    def is_convex(self):
        """Check if quad is convex and not degenerate.

        Notes:
            Check that for the two diagonals, the other two corners are not
            on the same side of the diagonal.
        Returns:
            True or False.
        """
        m = planish_line(self.ul, self.lr)  # puts this diagonal on x-axis
        p1 = self.ll * m  # transform the
        p2 = self.ur * m  # other two points
        if p1.y * p2.y > 0:
            return False
        m = planish_line(self.ll, self.ur)  # puts other diagonal on x-axis
        p1 = self.lr * m  # tranform the
        p2 = self.ul * m  # remaining points
        if p1.y * p2.y > 0:
            return False
        return True

    @property
    def is_empty(self):
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

    @property
    def is_rectangular(self):
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

    def morph(self, p, m):
        """Morph the quad with matrix-like 'm' and point-like 'p'.

        Return a new quad."""

        delta = Matrix(1, 1).preTranslate(p.x, p.y)
        q = self * ~delta * m * delta
        return q

    @property
    def rect(self):
        r = Rect()
        r.x0 = min(self.ul.x, self.ur.x, self.lr.x, self.ll.x)
        r.y0 = min(self.ul.y, self.ur.y, self.lr.y, self.ll.y)
        r.x1 = max(self.ul.x, self.ur.x, self.lr.x, self.ll.x)
        r.y1 = max(self.ul.y, self.ur.y, self.lr.y, self.ll.y)
        return r

    def transform(self, m):
        """Replace quad by its transformation with matrix m."""
        if len(m) != 6:
            raise ValueError("bad Matrix: sequ. length")
        self.ul *= m
        self.ur *= m
        self.ll *= m
        self.lr *= m
        return self

    __div__ = __truediv__
    width  = property(lambda self: max(abs(self.ul - self.ur), abs(self.ll - self.lr)))
    height = property(lambda self: max(abs(self.ul - self.ll), abs(self.ur - self.lr)))


class Rect(object):
    """Rect() - all zeros
    Rect(x0, y0, x1, y1)
    Rect(top-left, x1, y1)
    Rect(x0, y0, bottom-right)
    Rect(top-left, bottom-right)
    Rect(Rect or IRect) - new copy
    Rect(sequence) - from 'sequence'
    """
    def __abs__(self):
        if self.is_empty or self.isInfinite:
            return 0.0
        return (self.x1 - self.x0) * (self.y1 - self.y0)

    def __add__(self, p):
        if hasattr(p, "__float__"):
            r = Rect(self.x0 + p, self.y0 + p, self.x1 + p, self.y1 + p)
        else:
            if len(p) != 4:
                raise ValueError("bad Rect: sequ. length")
            r = Rect(self.x0 + p[0], self.y0 + p[1], self.x1 + p[2], self.y1 + p[3])
        return r

    def __and__(self, x):
        if not hasattr(x, "__len__"):
            raise ValueError("bad operand 2")

        r1 = Rect(x)
        r = Rect(self)
        return r.intersect(r1)

    def __bool__(self):
        return not (max(self) == min(self) == 0)

    def __contains__(self, x):
        if hasattr(x, "__float__"):
            return x in tuple(self)
        l = len(x)
        r = Rect(self).normalize()
        if l == 4:
            if r.is_empty: return False
            xr = Rect(x).normalize()
            if xr.is_empty: return True
            if r.x0 <= xr.x0 and r.y0 <= xr.y0 and r.x1 >= xr.x1 and r.y1 >= xr.y1:
               return True
            return False
        if l == 2:
            if r.x0 <= x[0] < r.x1 and r.y0 <= x[1] < r.y1:
               return True
            return False
        msg = "bad type or sequence: '%s'" % repr(x)
        raise ValueError("bad type or sequence: '%s'" % repr(x))

    def __eq__(self, rect):
        if not hasattr(rect, "__len__"):
            return False
        return len(rect) == 4 and bool(self - rect) is False

    def __init__(self, *args):
        #jlib.log('{args=}')
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
            #jlib.log('{l=} {type(l)=}')
            if isinstance(l, (mupdf.Rect, mupdf.Irect)):
                self.x0 = l.x0
                self.y0 = l.y0
                self.x1 = l.x1
                self.y1 = l.y1
                return
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

    def __getitem__(self, i):
        return (self.x0, self.y0, self.x1, self.y1)[i]

    def __hash__(self):
        return hash(tuple(self))

    def __len__(self):
        return 4

    def __mul__(self, m):
        if hasattr(m, "__float__"):
            return Rect(self.x0 * m, self.y0 * m, self.x1 * m, self.y1 * m)
        r = Rect(self)
        r = r.transform(m)
        return r

    def __neg__(self):
        return Rect(-self.x0, -self.y0, -self.x1, -self.y1)

    def __nonzero__(self):
        return not (max(self) == min(self) == 0)

    def __or__(self, x):
        if not hasattr(x, "__len__"):
            raise ValueError("bad operand 2")

        r = Rect(self)
        if len(x) == 2:
            return r.includePoint(x)
        if len(x) == 4:
            return r.include_rect(x)
        raise ValueError("bad operand 2")

    def __pos__(self):
        return Rect(self)

    def __repr__(self):
        return "Rect" + str(tuple(self))

    def __setitem__(self, i, v):
        v = float(v)
        if   i == 0: self.x0 = v
        elif i == 1: self.y0 = v
        elif i == 2: self.x1 = v
        elif i == 3: self.y1 = v
        else:
            raise IndexError("index out of range")
        return None

    def __sub__(self, p):
        if hasattr(p, "__float__"):
            return Rect(self.x0 - p, self.y0 - p, self.x1 - p, self.y1 - p)
        if len(p) != 4:
            raise ValueError("bad Rect: sequ. length")
        return Rect(self.x0 - p[0], self.y0 - p[1], self.x1 - p[2], self.y1 - p[3])

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

    @property
    def is_empty(self):
        """True if rectangle area is empty."""
        return self.x0 >= self.x1 or self.y0 >= self.y1

    @property
    def is_infinite(self):
        """True if rectangle is infinite."""
        return self.x0 > self.x1 or self.y0 > self.y1
    isInfinite = is_infinite

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

    def contains(self, x):
        """Check if containing point-like or rect-like x."""
        return self.__contains__(x)

    def include_point(self, p):
        """Extend to include point-like p."""
        if len(p) != 2:
            raise ValueError("bad Point: sequ. length")
        self.x0, self.y0, self.x1, self.y1 = TOOLS._include_point_in_rect(self, p)
        return self

    includePoint = include_point

    def include_rect(self, r):
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

    def intersects(self, x):
        """Check if intersection with rectangle x is not empty."""
        r1 = Rect(x)
        if self.is_empty or self.isInfinite or r1.is_empty or r1.isInfinite:
            return False
        r = Rect(self)
        if r.intersect(r1).is_empty:
            return False
        return True

    def morph(self, p, m):
        """Morph with matrix-like m and point-like p.

        Returns a new quad."""
        return self.quad.morph(p, m)

    def norm(self):
        return math.sqrt(sum([c*c for c in self]))

    def normalize(self):
        """Replace rectangle with its finite version."""
        if self.x1 < self.x0:
            self.x0, self.x1 = self.x1, self.x0
        if self.y1 < self.y0:
            self.y0, self.y1 = self.y1, self.y0
        return self

    @property
    def quad(self):
        """Return Quad version of rectangle."""
        return Quad(self.tl, self.tr, self.bl, self.br)

    def round(self):
        """Return the IRect."""
        return IRect(min(self.x0, self.x1), min(self.y0, self.y1),
                     max(self.x0, self.x1), max(self.y0, self.y1))

    def transform(self, m):
        """Replace with the transformation by matrix-like m."""
        if not len(m) == 6:
            raise ValueError("bad Matrix: sequ. length")
        self.x0, self.y0, self.x1, self.y1 = TOOLS._transform_rect(self, m)
        return self

    bl = bottom_left
    br = bottom_right
    height = property(lambda self: abs(self.y1 - self.y0))
    irect = property(round)
    tl = top_left
    tr = top_right
    width  = property(lambda self: abs(self.x1 - self.x0))


class Shape(object):
    """Create a new shape."""

    def __init__(self, page: Page):
        CheckParent(page)
        self.page = page
        self.doc = page.parent
        if not self.doc.is_pdf:
            raise ValueError("not a PDF")
        self.height = page.mediabox_size.y
        self.width = page.mediabox_size.x
        self.x = page.cropbox_position.x
        self.y = page.cropbox_position.y

        self.pctm = page.transformation_matrix  # page transf. matrix
        self.ipctm = ~self.pctm  # inverted transf. matrix

        self.draw_cont = ""
        self.text_cont = ""
        self.totalcont = ""
        self.lastPoint = None
        self.rect = None

    def commit(self, overlay: bool = True) -> None:
        """Update the page's /Contents object with Shape data. The argument controls whether data appear in foreground (default) or background."""
        CheckParent(self.page)  # doc may have died meanwhile
        self.totalcont += self.text_cont

        self.totalcont = self.totalcont.encode()

        if self.totalcont != b"":
            # make /Contents object with dummy stream
            xref = TOOLS._insert_contents(self.page, b" ", overlay)
            # update it with potential compression
            self.doc.update_stream(xref, self.totalcont)

        self.lastPoint = None  # clean up ...
        self.rect = None  #
        self.draw_cont = ""  # for potential ...
        self.text_cont = ""  # ...
        self.totalcont = ""  # re-use
        return

    def draw_line(self, p1: point_like, p2: point_like):# -> Point:
        """Draw a line between two points."""
        p1 = Point(p1)
        p2 = Point(p2)
        if not (self.lastPoint == p1):
            self.draw_cont += "%g %g m\n" % JM_TUPLE(p1 * self.ipctm)
            self.lastPoint = p1
            self.updateRect(p1)

        self.draw_cont += "%g %g l\n" % JM_TUPLE(p2 * self.ipctm)
        self.updateRect(p2)
        self.lastPoint = p2
        return self.lastPoint

    def draw_polyline(self, points: list):# -> Point:
        """Draw several connected line segments."""
        for i, p in enumerate(points):
            if i == 0:
                if not (self.lastPoint == Point(p)):
                    self.draw_cont += "%g %g m\n" % JM_TUPLE(Point(p) * self.ipctm)
                    self.lastPoint = Point(p)
            else:
                self.draw_cont += "%g %g l\n" % JM_TUPLE(Point(p) * self.ipctm)
            self.updateRect(p)

        self.lastPoint = Point(points[-1])
        return self.lastPoint

    def draw_bezier(
            self,
            p1: point_like,
            p2: point_like,
            p3: point_like,
            p4: point_like,
            ):# -> Point:
        """Draw a standard cubic Bezier curve."""
        p1 = Point(p1)
        p2 = Point(p2)
        p3 = Point(p3)
        p4 = Point(p4)
        if not (self.lastPoint == p1):
            self.draw_cont += "%g %g m\n" % JM_TUPLE(p1 * self.ipctm)
        self.draw_cont += "%g %g %g %g %g %g c\n" % JM_TUPLE(
            list(p2 * self.ipctm) + list(p3 * self.ipctm) + list(p4 * self.ipctm)
        )
        self.updateRect(p1)
        self.updateRect(p2)
        self.updateRect(p3)
        self.updateRect(p4)
        self.lastPoint = p4
        return self.lastPoint

    def draw_oval(self, tetra: typing.Union[quad_like, rect_like]):# -> Point:
        """Draw an ellipse inside a tetrapod."""
        if len(tetra) != 4:
            raise ValueError("invalid arg length")
        if hasattr(tetra[0], "__float__"):
            q = Rect(tetra).quad
        else:
            q = Quad(tetra)

        mt = q.ul + (q.ur - q.ul) * 0.5
        mr = q.ur + (q.lr - q.ur) * 0.5
        mb = q.ll + (q.lr - q.ll) * 0.5
        ml = q.ul + (q.ll - q.ul) * 0.5
        if not (self.lastPoint == ml):
            self.draw_cont += "%g %g m\n" % JM_TUPLE(ml * self.ipctm)
            self.lastPoint = ml
        self.draw_curve(ml, q.ll, mb)
        self.draw_curve(mb, q.lr, mr)
        self.draw_curve(mr, q.ur, mt)
        self.draw_curve(mt, q.ul, ml)
        self.updateRect(q.rect)
        self.lastPoint = ml
        return self.lastPoint

    def draw_circle(self, center: point_like, radius: float):# -> Point:
        """Draw a circle given its center and radius."""
        if not radius > EPSILON:
            raise ValueError("radius must be postive")
        center = Point(center)
        p1 = center - (radius, 0)
        return self.draw_sector(center, p1, 360, fullSector=False)

    def draw_curve(
            self,
            p1: point_like,
            p2: point_like,
            p3: point_like,
            ):# -> Point:
        """Draw a curve between points using one control point."""
        kappa = 0.55228474983
        p1 = Point(p1)
        p2 = Point(p2)
        p3 = Point(p3)
        k1 = p1 + (p2 - p1) * kappa
        k2 = p3 + (p2 - p3) * kappa
        return self.draw_bezier(p1, k1, k2, p3)

    def draw_quad(self, quad: quad_like):# -> Point:
        """Draw a Quad."""
        q = Quad(quad)
        return self.draw_polyline([q.ul, q.ll, q.lr, q.ur, q.ul])

    def draw_sector(
            self,
            center: point_like,
            point: point_like,
            beta: float,
            fullSector: bool = True,
            ):# -> Point:
        """Draw a circle sector."""
        center = Point(center)
        point = Point(point)
        l3 = "%g %g m\n"
        l4 = "%g %g %g %g %g %g c\n"
        l5 = "%g %g l\n"
        betar = math.radians(-beta)
        w360 = math.radians(math.copysign(360, betar)) * (-1)
        w90 = math.radians(math.copysign(90, betar))
        w45 = w90 / 2
        while abs(betar) > 2 * math.pi:
            betar += w360  # bring angle below 360 degrees
        if not (self.lastPoint == point):
            self.draw_cont += l3 % JM_TUPLE(point * self.ipctm)
            self.lastPoint = point
        Q = Point(0, 0)  # just make sure it exists
        C = center
        P = point
        S = P - C  # vector 'center' -> 'point'
        rad = abs(S)  # circle radius

        if not rad > EPSILON:
            raise ValueError("radius must be positive")

        alfa = self.horizontal_angle(center, point)
        while abs(betar) > abs(w90):  # draw 90 degree arcs
            q1 = C.x + math.cos(alfa + w90) * rad
            q2 = C.y + math.sin(alfa + w90) * rad
            Q = Point(q1, q2)  # the arc's end point
            r1 = C.x + math.cos(alfa + w45) * rad / math.cos(w45)
            r2 = C.y + math.sin(alfa + w45) * rad / math.cos(w45)
            R = Point(r1, r2)  # crossing point of tangents
            kappah = (1 - math.cos(w45)) * 4 / 3 / abs(R - Q)
            kappa = kappah * abs(P - Q)
            cp1 = P + (R - P) * kappa  # control point 1
            cp2 = Q + (R - Q) * kappa  # control point 2
            self.draw_cont += l4 % JM_TUPLE(
                list(cp1 * self.ipctm) + list(cp2 * self.ipctm) + list(Q * self.ipctm)
            )

            betar -= w90  # reduce parm angle by 90 deg
            alfa += w90  # advance start angle by 90 deg
            P = Q  # advance to arc end point
        # draw (remaining) arc
        if abs(betar) > 1e-3:  # significant degrees left?
            beta2 = betar / 2
            q1 = C.x + math.cos(alfa + betar) * rad
            q2 = C.y + math.sin(alfa + betar) * rad
            Q = Point(q1, q2)  # the arc's end point
            r1 = C.x + math.cos(alfa + beta2) * rad / math.cos(beta2)
            r2 = C.y + math.sin(alfa + beta2) * rad / math.cos(beta2)
            R = Point(r1, r2)  # crossing point of tangents
            # kappa height is 4/3 of segment height
            kappah = (1 - math.cos(beta2)) * 4 / 3 / abs(R - Q)  # kappa height
            kappa = kappah * abs(P - Q) / (1 - math.cos(betar))
            cp1 = P + (R - P) * kappa  # control point 1
            cp2 = Q + (R - Q) * kappa  # control point 2
            self.draw_cont += l4 % JM_TUPLE(
                list(cp1 * self.ipctm) + list(cp2 * self.ipctm) + list(Q * self.ipctm)
            )
        if fullSector:
            self.draw_cont += l3 % JM_TUPLE(point * self.ipctm)
            self.draw_cont += l5 % JM_TUPLE(center * self.ipctm)
            self.draw_cont += l5 % JM_TUPLE(Q * self.ipctm)
        self.lastPoint = Q
        return self.lastPoint

    def draw_rect(self, rect: rect_like):# -> Point:
        """Draw a rectangle."""
        r = Rect(rect)
        self.draw_cont += "%g %g %g %g re\n" % JM_TUPLE(
            list(r.bl * self.ipctm) + [r.width, r.height]
        )
        self.updateRect(r)
        self.lastPoint = r.tl
        return self.lastPoint

    def draw_squiggle(
            self,
            p1: point_like,
            p2: point_like,
            breadth=2,
            ):# -> Point:
        """Draw a squiggly line from p1 to p2."""
        p1 = Point(p1)
        p2 = Point(p2)
        S = p2 - p1  # vector start - end
        rad = abs(S)  # distance of points
        cnt = 4 * int(round(rad / (4 * breadth), 0))  # always take full phases
        if cnt < 4:
            raise ValueError("points too close")
        mb = rad / cnt  # revised breadth
        matrix = Matrix(TOOLS._hor_matrix(p1, p2))  # normalize line to x-axis
        i_mat = ~matrix  # get original position
        k = 2.4142135623765633  # y of draw_curve helper point

        points = []  # stores edges
        for i in range(1, cnt):
            if i % 4 == 1:  # point "above" connection
                p = Point(i, -k) * mb
            elif i % 4 == 3:  # point "below" connection
                p = Point(i, k) * mb
            else:  # else on connection line
                p = Point(i, 0) * mb
            points.append(p * i_mat)

        points = [p1] + points + [p2]
        cnt = len(points)
        i = 0
        while i + 2 < cnt:
            self.draw_curve(points[i], points[i + 1], points[i + 2])
            i += 2
        return p2

    def draw_zigzag(
            self,
            p1: point_like,
            p2: point_like,
            breadth: float = 2,
            ):# -> Point:
        """Draw a zig-zagged line from p1 to p2."""
        p1 = Point(p1)
        p2 = Point(p2)
        S = p2 - p1  # vector start - end
        rad = abs(S)  # distance of points
        cnt = 4 * int(round(rad / (4 * breadth), 0))  # always take full phases
        if cnt < 4:
            raise ValueError("points too close")
        mb = rad / cnt  # revised breadth
        matrix = Matrix(TOOLS._hor_matrix(p1, p2))  # normalize line to x-axis
        i_mat = ~matrix  # get original position
        points = []  # stores edges
        for i in range(1, cnt):
            if i % 4 == 1:  # point "above" connection
                p = Point(i, -1) * mb
            elif i % 4 == 3:  # point "below" connection
                p = Point(i, 1) * mb
            else:  # ignore others
                continue
            points.append(p * i_mat)
        self.draw_polyline([p1] + points + [p2])  # add start and end points
        return p2

    def finish(
            self,
            width: float = 1,
            color: OptSeq = None,
            fill: OptSeq = None,
            lineCap: int = 0,
            lineJoin: int = 0,
            dashes: OptStr = None,
            even_odd: bool = False,
            morph: OptSeq = None,
            closePath: bool = True,
            fill_opacity: float = 1,
            stroke_opacity: float = 1,
            oc: int = 0,
            ) -> None:
        """Finish the current drawing segment.

        Notes:
            Apply colors, opacity, dashes, line style and width, or
            morphing. Also whether to close the path
            by connecting last to first point.
        """
        if self.draw_cont == "":  # treat empty contents as no-op
            return

        if width == 0:  # border color makes no sense then
            color = None
        elif color is None:  # vice versa
            width = 0
        color_str = ColorCode(color, "c")  # ensure proper color string
        fill_str = ColorCode(fill, "f")  # ensure proper fill string

        optcont = self.page._get_optional_content(oc)
        if optcont is not None:
            self.draw_cont = "/OC /%s BDC\n" % optcont + self.draw_cont
            emc = "EMC\n"
        else:
            emc = ""

        alpha = self.page._set_opacity(CA=stroke_opacity, ca=fill_opacity)
        if alpha != None:
            self.draw_cont = "/%s gs\n" % alpha + self.draw_cont

        if width != 1:
            self.draw_cont += "%g w\n" % width

        if lineCap != 0:
            self.draw_cont = "%i J\n" % lineCap + self.draw_cont
        if lineJoin != 0:
            self.draw_cont = "%i j\n" % lineJoin + self.draw_cont

        if dashes not in (None, "", "[] 0"):
            self.draw_cont = "%s d\n" % dashes + self.draw_cont

        if closePath:
            self.draw_cont += "h\n"
            self.lastPoint = None

        if color is not None:
            self.draw_cont += color_str

        if fill is not None:
            self.draw_cont += fill_str
            if color is not None:
                if not even_odd:
                    self.draw_cont += "B\n"
                else:
                    self.draw_cont += "B*\n"
            else:
                if not even_odd:
                    self.draw_cont += "f\n"
                else:
                    self.draw_cont += "f*\n"
        else:
            self.draw_cont += "S\n"

        self.draw_cont += emc
        if CheckMorph(morph):
            m1 = Matrix(
                1, 0, 0, 1, morph[0].x + self.x, self.height - morph[0].y - self.y
            )
            mat = ~m1 * morph[1] * m1
            self.draw_cont = "%g %g %g %g %g %g cm\n" % JM_TUPLE(mat) + self.draw_cont

        self.totalcont += "\nq\n" + self.draw_cont + "Q\n"
        self.draw_cont = ""
        self.lastPoint = None
        return

    @staticmethod
    def horizontal_angle(C, P):
        """Return the angle to the horizontal for the connection from C to P.
        This uses the arcus sine function and resolves its inherent ambiguity by
        looking up in which quadrant vector S = P - C is located.
        """
        S = Point(P - C).unit  # unit vector 'C' -> 'P'
        alfa = math.asin(abs(S.y))  # absolute angle from horizontal
        if S.x < 0:  # make arcsin result unique
            if S.y <= 0:  # bottom-left
                alfa = -(math.pi - alfa)
            else:  # top-left
                alfa = math.pi - alfa
        else:
            if S.y >= 0:  # top-right
                pass
            else:  # bottom-right
                alfa = -alfa
        return alfa

    def insert_text(
        self,
        point: point_like,
        buffer_: typing.Union[str, list],
        fontsize: float = 11,
        lineheight: OptFloat = None,
        fontname: str = "helv",
        fontfile: OptStr = None,
        set_simple: bool = 0,
        encoding: int = 0,
        color: OptSeq = None,
        fill: OptSeq = None,
        render_mode: int = 0,
        border_width: float = 1,
        rotate: int = 0,
        morph: OptSeq = None,
        stroke_opacity: float = 1,
        fill_opacity: float = 1,
        oc: int = 0,
    ) -> int:

        #jlib.log('{self=} {point=} {buffer_=} {fontsize=} {lineheight=} {fontname=} {fontfile=} {set_simple=} {encoding=} {color=} {fill=} {render_mode=} {border_width=} {rotate=} {morph=} {stroke_opacity=} {fill_opacity=} {oc=}')
        # ensure 'text' is a list of strings, worth dealing with
        if not bool(buffer_):
            return 0

        if type(buffer_) not in (list, tuple):
            text = buffer_.splitlines()
        else:
            text = buffer_

        if not len(text) > 0:
            return 0

        point = Point(point)
        try:
            maxcode = max([ord(c) for c in " ".join(text)])
        except:
            return 0

        # ensure valid 'fontname'
        fname = fontname
        if fname.startswith("/"):
            fname = fname[1:]

        #jlib.log('{fname=} {fontfile=} {encoding=} {set_simple=}')
        xref = self.page.insert_font(
            fontname=fname, fontfile=fontfile, encoding=encoding, set_simple=set_simple
        )
        #jlib.log('self.page.insert_font() => {xref=}')
        fontinfo = CheckFontInfo(self.doc, xref)
        #jlib.log('{fontinfo=}')

        fontdict = fontinfo[1]
        ordering = fontdict["ordering"]
        simple = fontdict["simple"]
        bfname = fontdict["name"]
        ascender = fontdict["ascender"]
        descender = fontdict["descender"]
        if lineheight:
            lheight = fontsize * lineheight
        elif ascender - descender <= 1:
            lheight = fontsize * 1.2
        else:
            lheight = fontsize * (ascender - descender)

        if maxcode > 255:
            glyphs = self.doc.get_char_widths(xref, maxcode + 1)
        else:
            glyphs = fontdict["glyphs"]

        tab = []
        for t in text:
            if simple and bfname not in ("Symbol", "ZapfDingbats"):
                g = None
            else:
                g = glyphs
            tab.append(getTJstr(t, g, simple, ordering))
        text = tab

        color_str = ColorCode(color, "c")
        fill_str = ColorCode(fill, "f")
        if not fill and render_mode == 0:  # ensure fill color when 0 Tr
            fill = color
            fill_str = ColorCode(color, "f")

        morphing = CheckMorph(morph)
        rot = rotate
        if rot % 90 != 0:
            raise ValueError("bad rotate value")

        while rot < 0:
            rot += 360
        rot = rot % 360  # text rotate = 0, 90, 270, 180

        templ1 = "\nq\n%s%sBT\n%s1 0 0 1 %g %g Tm\n/%s %g Tf "
        templ2 = "TJ\n0 -%g TD\n"
        cmp90 = "0 1 -1 0 0 0 cm\n"  # rotates 90 deg counter-clockwise
        cmm90 = "0 -1 1 0 0 0 cm\n"  # rotates 90 deg clockwise
        cm180 = "-1 0 0 -1 0 0 cm\n"  # rotates by 180 deg.
        height = self.height
        width = self.width

        # setting up for standard rotation directions
        # case rotate = 0
        if morphing:
            m1 = Matrix(1, 0, 0, 1, morph[0].x + self.x, height - morph[0].y - self.y)
            mat = ~m1 * morph[1] * m1
            cm = "%g %g %g %g %g %g cm\n" % JM_TUPLE(mat)
        else:
            cm = ""
        top = height - point.y - self.y  # start of 1st char
        left = point.x + self.x  # start of 1. char
        space = top  # space available
        headroom = point.y + self.y  # distance to page border
        if rot == 90:
            left = height - point.y - self.y
            top = -point.x - self.x
            cm += cmp90
            space = width - abs(top)
            headroom = point.x + self.x

        elif rot == 270:
            left = -height + point.y + self.y
            top = point.x + self.x
            cm += cmm90
            space = abs(top)
            headroom = width - point.x - self.x

        elif rot == 180:
            left = -point.x - self.x
            top = -height + point.y + self.y
            cm += cm180
            space = abs(point.y + self.y)
            headroom = height - point.y - self.y

        optcont = self.page._get_optional_content(oc)
        if optcont != None:
            bdc = "/OC /%s BDC\n" % optcont
            emc = "EMC\n"
        else:
            bdc = emc = ""

        alpha = self.page._set_opacity(CA=stroke_opacity, ca=fill_opacity)
        if alpha == None:
            alpha = ""
        else:
            alpha = "/%s gs\n" % alpha
        nres = templ1 % (bdc, alpha, cm, left, top, fname, fontsize)
        if render_mode > 0:
            nres += "%i Tr " % render_mode
        if border_width != 1:
            nres += "%g w " % border_width
        if color is not None:
            nres += color_str
        if fill is not None:
            nres += fill_str

        # =========================================================================
        #   start text insertion
        # =========================================================================
        nres += text[0]
        nlines = 1  # set output line counter
        if len(text) > 1:
            nres += templ2 % lheight  # line 1
        else:
            nres += templ2[:2]
        for i in range(1, len(text)):
            if space < lheight:
                break  # no space left on page
            if i > 1:
                nres += "\nT* "
            nres += text[i] + templ2[:2]
            space -= lheight
            nlines += 1

        nres += "\nET\n%sQ\n" % emc

        # =========================================================================
        #   end of text insertion
        # =========================================================================
        # update the /Contents object
        self.text_cont += nres
        return nlines

    # ==============================================================================
    # Shape.insert_textbox
    # ==============================================================================
    def insert_textbox(
        self,
        rect: rect_like,
        buffer_: typing.Union[str, list],
        fontname: OptStr = "helv",
        fontfile: OptStr = None,
        fontsize: float = 11,
        lineheight: OptFloat = None,
        set_simple: bool = 0,
        encoding: int = 0,
        color: OptSeq = None,
        fill: OptSeq = None,
        expandtabs: int = 1,
        border_width: float = 1,
        align: int = 0,
        render_mode: int = 0,
        rotate: int = 0,
        morph: OptSeq = None,
        stroke_opacity: float = 1,
        fill_opacity: float = 1,
        oc: int = 0,
    ) -> float:
        """Insert text into a given rectangle.

        Args:
            rect -- the textbox to fill
            buffer_ -- text to be inserted
            fontname -- a Base-14 font, font name or '/name'
            fontfile -- name of a font file
            fontsize -- font size
            lineheight -- overwrite the font property
            color -- RGB stroke color triple
            fill -- RGB fill color triple
            render_mode -- text rendering control
            border_width -- thickness of glyph borders
            expandtabs -- handles tabulators with string function
            align -- left, center, right, justified
            rotate -- 0, 90, 180, or 270 degrees
            morph -- morph box with a matrix and a fixpoint
        Returns:
            unused or deficit rectangle area (float)
        """
        rect = Rect(rect)
        if rect.is_empty or rect.is_infinite:
            raise ValueError("text box must be finite and not empty")

        color_str = ColorCode(color, "c")
        fill_str = ColorCode(fill, "f")
        if fill is None and render_mode == 0:  # ensure fill color for 0 Tr
            fill = color
            fill_str = ColorCode(color, "f")

        optcont = self.page._get_optional_content(oc)
        if optcont != None:
            bdc = "/OC /%s BDC\n" % optcont
            emc = "EMC\n"
        else:
            bdc = emc = ""

        # determine opacity / transparency
        alpha = self.page._set_opacity(CA=stroke_opacity, ca=fill_opacity)
        if alpha == None:
            alpha = ""
        else:
            alpha = "/%s gs\n" % alpha

        if rotate % 90 != 0:
            raise ValueError("rotate must be multiple of 90")

        rot = rotate
        while rot < 0:
            rot += 360
        rot = rot % 360

        # is buffer_ worth of dealing with?
        if not bool(buffer_):
            return rect.height if rot in (0, 180) else rect.width

        cmp90 = "0 1 -1 0 0 0 cm\n"  # rotates counter-clockwise
        cmm90 = "0 -1 1 0 0 0 cm\n"  # rotates clockwise
        cm180 = "-1 0 0 -1 0 0 cm\n"  # rotates by 180 deg.
        height = self.height

        fname = fontname
        if fname.startswith("/"):
            fname = fname[1:]

        xref = self.page.insert_font(
            fontname=fname, fontfile=fontfile, encoding=encoding, set_simple=set_simple
        )
        fontinfo = CheckFontInfo(self.doc, xref)

        fontdict = fontinfo[1]
        ordering = fontdict["ordering"]
        simple = fontdict["simple"]
        glyphs = fontdict["glyphs"]
        bfname = fontdict["name"]
        ascender = fontdict["ascender"]
        descender = fontdict["descender"]

        if lineheight:
            lheight_factor = lineheight
        elif ascender - descender <= 1:
            lheight_factor = 1.2
        else:
            lheight_factor = ascender - descender
        lheight = fontsize * lheight_factor

        # create a list from buffer_, split into its lines
        if type(buffer_) in (list, tuple):
            t0 = "\n".join(buffer_)
        else:
            t0 = buffer_

        maxcode = max([ord(c) for c in t0])
        # replace invalid char codes for simple fonts
        if simple and maxcode > 255:
            t0 = "".join([c if ord(c) < 256 else "?" for c in t0])

        t0 = t0.splitlines()

        glyphs = self.doc.get_char_widths(xref, maxcode + 1)
        if simple and bfname not in ("Symbol", "ZapfDingbats"):
            tj_glyphs = None
        else:
            tj_glyphs = glyphs

        # ----------------------------------------------------------------------
        # calculate pixel length of a string
        # ----------------------------------------------------------------------
        def pixlen(x):
            """Calculate pixel length of x."""
            if ordering < 0:
                return sum([glyphs[ord(c)][1] for c in x]) * fontsize
            else:
                return len(x) * fontsize

        # ----------------------------------------------------------------------

        if ordering < 0:
            blen = glyphs[32][1] * fontsize  # pixel size of space character
        else:
            blen = fontsize

        text = ""  # output buffer

        if CheckMorph(morph):
            m1 = Matrix(
                1, 0, 0, 1, morph[0].x + self.x, self.height - morph[0].y - self.y
            )
            mat = ~m1 * morph[1] * m1
            cm = "%g %g %g %g %g %g cm\n" % JM_TUPLE(mat)
        else:
            cm = ""

        # ---------------------------------------------------------------------------
        # adjust for text orientation / rotation
        # ---------------------------------------------------------------------------
        progr = 1  # direction of line progress
        c_pnt = Point(0, fontsize * ascender)  # used for line progress
        if rot == 0:  # normal orientation
            point = rect.tl + c_pnt  # line 1 is 'lheight' below top
            pos = point.y + self.y  # y of first line
            maxwidth = rect.width  # pixels available in one line
            maxpos = rect.y1 + self.y  # lines must not be below this

        elif rot == 90:  # rotate counter clockwise
            c_pnt = Point(fontsize * ascender, 0)  # progress in x-direction
            point = rect.bl + c_pnt  # line 1 'lheight' away from left
            pos = point.x + self.x  # position of first line
            maxwidth = rect.height  # pixels available in one line
            maxpos = rect.x1 + self.x  # lines must not be right of this
            cm += cmp90

        elif rot == 180:  # text upside down
            # progress upwards in y direction
            c_pnt = -Point(0, fontsize * ascender)
            point = rect.br + c_pnt  # line 1 'lheight' above bottom
            pos = point.y + self.y  # position of first line
            maxwidth = rect.width  # pixels available in one line
            progr = -1  # subtract lheight for next line
            maxpos = rect.y0 + self.y  # lines must not be above this
            cm += cm180

        else:  # rotate clockwise (270 or -90)
            # progress from right to left
            c_pnt = -Point(fontsize * ascender, 0)
            point = rect.tr + c_pnt  # line 1 'lheight' left of right
            pos = point.x + self.x  # position of first line
            maxwidth = rect.height  # pixels available in one line
            progr = -1  # subtract lheight for next line
            maxpos = rect.x0 + self.x  # lines must not left of this
            cm += cmm90

        # =======================================================================
        # line loop
        # =======================================================================
        just_tab = []  # 'justify' indicators per line

        for i, line in enumerate(t0):
            line_t = line.expandtabs(expandtabs).split(" ")  # split into words
            lbuff = ""  # init line buffer
            rest = maxwidth  # available line pixels
            # ===================================================================
            # word loop
            # ===================================================================
            for word in line_t:
                pl_w = pixlen(word)  # pixel len of word
                if rest >= pl_w:  # will it fit on the line?
                    lbuff += word + " "  # yes, and append word
                    rest -= pl_w + blen  # update available line space
                    continue
                # word won't fit - output line (if not empty)
                if len(lbuff) > 0:
                    lbuff = lbuff.rstrip() + "\n"  # line full, append line break
                    text += lbuff  # append to total text
                    pos += lheight * progr  # increase line position
                    just_tab.append(True)  # line is justify candidate
                    lbuff = ""  # re-init line buffer
                rest = maxwidth  # re-init avail. space
                if pl_w <= maxwidth:  # word shorter than 1 line?
                    lbuff = word + " "  # start the line with it
                    rest = maxwidth - pl_w - blen  # update free space
                    continue
                # long word: split across multiple lines - char by char ...
                if len(just_tab) > 0:
                    just_tab[-1] = False  # reset justify indicator
                for c in word:
                    if pixlen(lbuff) <= maxwidth - pixlen(c):
                        lbuff += c
                    else:  # line full
                        lbuff += "\n"  # close line
                        text += lbuff  # append to text
                        pos += lheight * progr  # increase line position
                        just_tab.append(False)  # do not justify line
                        lbuff = c  # start new line with this char
                lbuff += " "  # finish long word
                rest = maxwidth - pixlen(lbuff)  # long word stored

            if lbuff != "":  # unprocessed line content?
                text += lbuff.rstrip()  # append to text
                just_tab.append(False)  # do not justify line
            if i < len(t0) - 1:  # not the last line?
                text += "\n"  # insert line break
                pos += lheight * progr  # increase line position

        more = (pos - maxpos) * progr  # difference to rect size limit

        if more > EPSILON:  # landed too much outside rect
            return (-1) * more  # return deficit, don't output

        more = abs(more)
        if more < EPSILON:
            more = 0  # don't bother with epsilons
        nres = "\nq\n%s%sBT\n" % (bdc, alpha) + cm  # initialize output buffer
        templ = "1 0 0 1 %g %g Tm /%s %g Tf "
        # center, right, justify: output each line with its own specifics
        text_t = text.splitlines()  # split text in lines again
        just_tab[-1] = False  # never justify last line
        for i, t in enumerate(text_t):
            pl = maxwidth - pixlen(t)  # length of empty line part
            pnt = point + c_pnt * (i * lheight_factor)  # text start of line
            if align == 1:  # center: right shift by half width
                if rot in (0, 180):
                    pnt = pnt + Point(pl / 2, 0) * progr
                else:
                    pnt = pnt - Point(0, pl / 2) * progr
            elif align == 2:  # right: right shift by full width
                if rot in (0, 180):
                    pnt = pnt + Point(pl, 0) * progr
                else:
                    pnt = pnt - Point(0, pl) * progr
            elif align == 3:  # justify
                spaces = t.count(" ")  # number of spaces in line
                if spaces > 0 and just_tab[i]:  # if any, and we may justify
                    spacing = pl / spaces  # make every space this much larger
                else:
                    spacing = 0  # keep normal space length
            top = height - pnt.y - self.y
            left = pnt.x + self.x
            if rot == 90:
                left = height - pnt.y - self.y
                top = -pnt.x - self.x
            elif rot == 270:
                left = -height + pnt.y + self.y
                top = pnt.x + self.x
            elif rot == 180:
                left = -pnt.x - self.x
                top = -height + pnt.y + self.y

            nres += templ % (left, top, fname, fontsize)
            if render_mode > 0:
                nres += "%i Tr " % render_mode
            if align == 3:
                nres += "%g Tw " % spacing

            if color is not None:
                nres += color_str
            if fill is not None:
                nres += fill_str
            if border_width != 1:
                nres += "%g w " % border_width
            nres += "%sTJ\n" % getTJstr(t, tj_glyphs, simple, ordering)

        nres += "ET\n%sQ\n" % emc

        self.text_cont += nres
        self.updateRect(rect)
        return more

    def update_rect(self, x):
        if self.rect is None:
            if len(x) == 2:
                self.rect = Rect(x, x)
            else:
                self.rect = Rect(x)

        else:
            if len(x) == 2:
                x = Point(x)
                self.rect.x0 = min(self.rect.x0, x.x)
                self.rect.y0 = min(self.rect.y0, x.y)
                self.rect.x1 = max(self.rect.x1, x.x)
                self.rect.y1 = max(self.rect.y1, x.y)
            else:
                x = Rect(x)
                self.rect.x0 = min(self.rect.x0, x.x0)
                self.rect.y0 = min(self.rect.y0, x.y0)
                self.rect.x1 = max(self.rect.x1, x.x1)
                self.rect.y1 = max(self.rect.y1, x.y1)

    updateRect = update_rect

    # define deprecated aliases ------------------------------------------
    drawBezier = draw_bezier
    drawCircle = draw_circle
    drawCurve = draw_curve
    drawLine = draw_line
    drawOval = draw_oval
    drawPolyline = draw_polyline
    drawQuad = draw_quad
    drawRect = draw_rect
    drawSector = draw_sector
    drawSquiggle = draw_squiggle
    drawZigzag = draw_zigzag
    insertText = insert_text
    insertTextbox = insert_textbox


class TextPage:

    def __init__(self, *args):
        if args_match(args, mupdf.Rect):
            #self.this = _fitz.new_TextPage(args[0])
            self.this = mupdf.StextPage(args[0])
        elif args_match(args, mupdf.StextPage):
            self.this = args[0]
        else:
            raise Exception(f'Unrecognised args: {args}')
        self.thisown = True

    def _extractText(self, format_):
        #return _fitz.TextPage__extractText(self, format)
        this_tpage = self.this
        res = mupdf.mfz_new_buffer(1024)
        out = mupdf.mfz_new_output_with_buffer(res)
        # fixme: mupdfwrap.py thinks fz_output is not copyable, possibly
        # because there is no .refs member visible and no fz_keep_output() fn,
        # although there is an fz_drop_output(). So mupdf.mfz_new_output_with_buffer()
        # doesn't convert the returnd fz_output* into a mupdf.Output.
        out = mupdf.Output(out)
        if format_ == 1:
            mupdf.mfz_print_stext_page_as_html(out, this_tpage, 0)
        elif format_ == 3:
            mupdf.mfz_print_stext_page_as_xml(out, this_tpage, 0)
        elif format_ == 4:
            mupdf.mfz_print_stext_page_as_xhtml(out, this_tpage, 0)
        else:
            JM_print_stext_page_as_text(out, this_tpage)
        text = JM_UnicodeFromBuffer(res)
        return text

    def _getNewBlockList(self, page_dict, raw):
        #return _fitz.TextPage__getNewBlockList(self, page_dict, raw)
        JM_make_textpage_dict(self.this, page_dict, raw)

    def _textpage_dict(self, raw=False):
        page_dict = {"width": self.rect.width, "height": self.rect.height}
        self._getNewBlockList(page_dict, raw)
        return page_dict

    def extractBLOCKS(self):
        """Return a list with text block information."""
        block_n = -1
        #jlib.log('{type(self)=}')
        this_tpage = self.this
        #jlib.log('{type(self)=} {type(self.this)=}')
        tp_rect = mupdf.Rect(this_tpage.m_internal.mediabox)
        res = mupdf.mfz_new_buffer(1024);
        lines = []
        for block in this_tpage:
            block_n += 1
            blockrect = mupdf.Rect(mupdf.Rect.Fixed_EMPTY)
            if block.m_internal.type == mupdf.FZ_STEXT_BLOCK_TEXT:
                mupdf.mfz_clear_buffer(res) # set text buffer to empty
                line_n = -1
                last_y0 = 0.0
                last_char = 0
                for line in block:
                    line_n += 1
                    linerect = mupdf.Rect(mupdf.Rect.Fixed_EMPTY)
                    for ch in line:
                        cbbox = JM_char_bbox(line, ch)
                        if (not mupdf.mfz_contains_rect(tp_rect, cbbox)
                                and not mupdf.mfz_is_infinite_rect(tp_rect)
                                ):
                            continue
                        JM_append_rune(res, ch.m_internal.c)
                        last_char = ch.m_internal.c
                        linerect = mupdf.mfz_union_rect(linerect, cbbox)
                    if last_char != 10 and not mupdf.mfz_is_empty_rect(linerect):
                        mupdf.mfz_append_byte(res, 10)
                    blockrect = mupdf.mfz_union_rect(blockrect, linerect)
                text = JM_EscapeStrFromBuffer(res)
            elif (mupdf.mfz_contains_rect(tp_rect, block.bbox)
                    or mupdf.mfz_is_infinite_rect(tp_rect)
                    ):
                img = block.i_image()
                cs = img.colorspace()
                text = "<image: %s, width: %d, height: %d, bpc: %d>" % (
                        mupdf.mfz_colorspace_name(cs),
                        img.w(), img.h(), img.bpc
                        )
                blockrect = mupdf.mfz_union_rect(blockrect, block.bbox)
            if not mupdf.mfz_is_empty_rect(blockrect):
                litem = (
                        blockrect.x0,
                        blockrect.y0,
                        blockrect.x1,
                        blockrect.y1,
                        text,
                        block_n,
                        block.m_internal.type,
                        )
                lines.append(litem)
        return lines

    def extractDICT(self, cb=None, sort=False) -> dict:
        """Return page content as a Python dict of images and text spans."""
        #val = self._textpage_dict(raw=False)
        raw=False
        page_dict = {"width": self.rect.width, "height": self.rect.height}
        self._getNewBlockList(page_dict, raw)
        val = page_dict
        if cb is not None:
            val["width"] = cb.width
            val["height"] = cb.height
        if sort is True:
            blocks = val["blocks"]
            blocks.sort(key=lambda b: (b["bbox"][3], b["bbox"][0]))
            val["blocks"] = blocks
        return val

    def extractIMGINFO(self, hashes=0):
        """Return a list with image meta information."""

        #return _fitz.TextPage_extractIMGINFO(self, hashes)
        block_n = -1
        this_tpage = self.this
        #PyObject *rc = NULL, *block_dict = NULL;
        #fz_pixmap *pix = NULL;
        rc = []
        #jlib.log('iterating blocks...')
        for block in this_tpage:
            block_n += 1
            #jlib.log('{block_n=} {block.m_internal.type}')
            if block.m_internal.type == mupdf.FZ_STEXT_BLOCK_TEXT:
                continue
            img = block.i_image()
            #jlib.log('{hashes=}')
            if hashes:
                r = mupdf.Irect(FZ_MIN_INF_RECT, FZ_MIN_INF_RECT, FZ_MAX_INF_RECT, FZ_MAX_INF_RECT)
                assert r.is_infinite_irect()
                m = mupdf.Matrix()
                #jlib.log('calling mupdf.mfz_get_pixmap_from_image()')
                pix, _, _ = mupdf.mfz_get_pixmap_from_image(img, r, m)
                digest = bytes(pix.md5_pixmap())
            cs = mupdf.Colorspace(mupdf.keep_colorspace(img.m_internal.colorspace))
            block_dict = dict()
            block_dict[ dictkey_number] = block_n
            block_dict[ dictkey_bbox] = JM_py_from_rect(block.m_internal.bbox)
            block_dict[ dictkey_matrix] = JM_py_from_matrix(block.i_transform())
            block_dict[ dictkey_width] = img.w()
            block_dict[ dictkey_height] = img.h()
            block_dict[ dictkey_colorspace] = mupdf.mfz_colorspace_n(cs)
            block_dict[ dictkey_cs_name] = mupdf.mfz_colorspace_name(cs)
            block_dict[ dictkey_xres] = img.xres()
            block_dict[ dictkey_yres] = img.xres()  # fixme: shouldn't this be img.yres()?
            block_dict[ dictkey_bpc] = img.bpc()
            block_dict[ dictkey_size] = mupdf.mfz_image_size(img)
            if hashes:
                block_dict[ "digest"] = digest
            rc.append(block_dict)
        #jlib.log('returning {rc=}')
        return rc

    def extractRAWDICT(self, cb=None, sort=False) -> dict:
        """Return page content as a Python dict of images and text characters."""
        val =  self._textpage_dict(raw=True)
        if cb is not None:
            val["width"] = cb.width
            val["height"] = cb.height
        if sort is True:
            blocks = val["blocks"]
            blocks.sort(key=lambda b: (b["bbox"][3], b["bbox"][0]))
            val["blocks"] = blocks
        return val

    def extractSelection(self, pointa, pointb):
        return _fitz.TextPage_extractSelection(self, pointa, pointb)

    def extractText(self, sort=False) -> str:
        """Return simple, bare text on the page."""
        if sort is False:
            return self._extractText(0)
        blocks = self.extractBLOCKS()[:]
        blocks.sort(key=lambda b: (b[3], b[0]))
        return "".join([b[4] for b in blocks])

    def extractTextbox(self, rect):
        #return _fitz.TextPage_extractTextbox(self, rect)
        #fz_stext_page *this_tpage = (fz_stext_page *) self;
        this_tpage = self.this
        assert isinstance(this_tpage, mupdf.StextPage)
        area = JM_rect_from_py(rect)
        #PyObject *rc = NULL;
        #char *found = NULL;
        found = JM_copy_rectangle(this_tpage, area);
        if (found):
            rc = JM_UnicodeFromStr(found)
        else:
            rc = ''
        return rc

    extractTEXT = extractText

    def extractWORDS(self):
        """Return a list with text word information."""
        #return _fitz.TextPage_extractWORDS(self)
        buflen = 0
        block_n = -1
        wbbox = mupdf.Rect(mupdf.Rect.Fixed_EMPTY)  # word bbox
        this_tpage = self.this
        tp_rect = mupdf.Rect(this_tpage.m_internal.mediabox)

        lines = None
        buff = mupdf.mfz_new_buffer(64)
        lines = []
        for block in this_tpage:
            block_n += 1
            if block.m_internal.type != mupdf.FZ_STEXT_BLOCK_TEXT:
                continue
            line_n = 0
            for line in block:
                word_n = 0                        # word counter per line
                mupdf.mfz_clear_buffer(buff)      # reset word buffer
                buflen = 0                        # reset char counter
                for ch in line:
                    cbbox = JM_char_bbox(line, ch)
                    if mupdf.mfz_is_empty_rect(cbbox):
                        continue
                    #jlib.log('{type(tp_rect)=} {type(cbbox)=}')
                    if (not mupdf.mfz_contains_rect(tp_rect, cbbox)
                            and not mupdf.mfz_is_infinite_rect(tp_rect)
                            ):
                        continue
                    if ch.m_internal.c == 32 and buflen == 0:
                        continue    # skip spaces at line start
                    if ch.m_internal.c == 32:
                        if not mupdf.mfz_is_empty_rect(wbbox):
                            word_n, wbbox = JM_append_word(lines, buff, wbbox, block_n, line_n, word_n)
                        mupdf.mfz_clear_buffer(buff)
                        buflen = 0  # reset char counter
                        continue
                    # append one unicode character to the word
                    JM_append_rune(buff, ch.m_internal.c)
                    buflen += 1
                    # enlarge word bbox
                    wbbox = mupdf.mfz_union_rect(wbbox, JM_char_bbox(line, ch))
                if buflen:
                    word_n, wbbox = JM_append_word(lines, buff, wbbox, block_n, line_n, word_n)
                    mupdf.mfz_clear_buffer(buff)
                buflen = 0
                line_n += 1
        return lines

    @property
    def rect(self):
        """Page rectangle."""

        #val = _fitz.TextPage_rect(self)
        this_tpage = self.this
        mediabox = this_tpage.m_internal.mediabox
        val = JM_py_from_rect(mediabox)
        val = Rect(val)

        return val

    def extractHTML(self) -> str:
        """Return page content as a HTML string."""
        return self._extractText(1)

    def extractJSON(self, cb=None, sort=False) -> str:
        """Return 'extractDICT' converted to JSON format."""
        import base64, json
        val = self._textpage_dict(raw=False)

        class b64encode(json.JSONEncoder):
            def default(self, s):
                if type(s) in (bytes, bytearray):
                    return base64.b64encode(s).decode()

        if cb is not None:
            val["width"] = cb.width
            val["height"] = cb.height
        if sort is True:
            blocks = val["blocks"]
            blocks.sort(key=lambda b: (b["bbox"][3], b["bbox"][0]))
            val["blocks"] = blocks
        val = json.dumps(val, separators=(",", ":"), cls=b64encode, indent=1)
        return val

    def extractRAWJSON(self, cb=None, sort=False) -> str:
        """Return 'extractRAWDICT' converted to JSON format."""
        import base64, json
        val = self._textpage_dict(raw=True)

        class b64encode(json.JSONEncoder):
            def default(self,s):
                if type(s) in (bytes, bytearray):
                    return base64.b64encode(s).decode()

        if cb is not None:
            val["width"] = cb.width
            val["height"] = cb.height
        if sort is True:
            blocks = val["blocks"]
            blocks.sort(key=lambda b: (b["bbox"][3], b["bbox"][0]))
            val["blocks"] = blocks
        val = json.dumps(val, separators=(",", ":"), cls=b64encode, indent=1)
        return val

    def extractXML(self) -> str:
        """Return page content as a XML string."""
        return self._extractText(3)

    def extractXHTML(self) -> str:
        """Return page content as a XHTML string."""
        return self._extractText(4)

    def search(self, needle, hit_max=0, quads=1):
        """Locate 'needle' returning rects or quads."""
        #val = _fitz.TextPage_search(self, needle, hit_max, quads)
        val = JM_search_stext_page(self.this, needle)
        nl = '\n'
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
            if v1.y1 != v2.y1 or (v1 & v2).is_empty:
                i += 1
                continue  # no overlap on same line
            val[i] = v1 | v2  # join rectangles
            del val[i + 1]  # remove v2
            items -= 1  # reduce item count
        return val


class TextWriter:

    def __init__(self, page_rect, opacity=1, color=None):
        """Stores text spans for later output on compatible PDF pages."""

        #this = _fitz.new_TextWriter(page_rect, opacity, color)
        #try:
        #    self.this.append(this)
        #except __builtin__.Exception:
        #    self.this = this
        self.this = mupdf.mfz_new_text()

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

    @property
    def _bbox(self):
        #val = _fitz.TextWriter__bbox(self)
        val = JM_py_from_rect( mupdf.mfz_bound_text( self.this, mupdf.StrokeState(0), mupdf.Matrix()))
        val = Rect(val)
        return val

    def append(self, pos, text, font=None, fontsize=11, language=None, right_to_left=0, small_caps=0):
        """Store 'text' at point 'pos' using 'font' and 'fontsize'."""
        pos = Point(pos) * self.ictm
        if font is None:
            font = Font("helv")
        if not font.is_writable:
            raise ValueError("Unsupported font '%s'." % font.name)
        if right_to_left:
            text = self.clean_rtl(text)
            text = "".join(reversed(text))
            right_to_left = 0

        #val = _fitz.TextWriter_append(self, pos, text, font, fontsize, language, right_to_left, small_caps)
        lang = mupdf.mfz_text_language_from_string(language)
        p = JM_point_from_py(pos)
        trm = mupdf.mfz_make_matrix(fontsize, 0, 0, fontsize, p.x, p.y)
        markup_dir = 0
        wmode = 0
        if small_caps == 0:
            trm = mupdf.mfz_show_string( self.this, font.this, trm, text, wmode, right_to_left, markup_dir, lang)
        else:
            trm = JM_show_string_cs( self.this, font.this, trm, text, wmode, right_to_left, markup_dir, lang)
        val = JM_py_from_matrix(trm)

        self.last_point = Point(val[-2:]) * self.ctm
        self.text_rect = self._bbox * self.ctm
        val = self.text_rect, self.last_point
        #jlib.log('{type(font)=} {font=} {font.flags=}')
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

    def clean_rtl(self, text):
        """Revert the sequence of Latin text parts.

        Text with right-to-left writing direction (Arabic, Hebrew) often
        contains Latin parts, which are written in left-to-right: numbers, names,
        etc. For output as PDF text we need *everything* in right-to-left.
        E.g. an input like "<arabic> ABCDE FG HIJ <arabic> KL <arabic>" will be
        converted to "<arabic> JIH GF EDCBA <arabic> LK <arabic>". The Arabic
        parts remain untouched.

        Args:
            text: str
        Returns:
            Massaged string.
        """
        if not text:
            return text
        # split into words at space boundaries
        words = text.split(" ")
        idx = []
        for i in range(len(words)):
            w = words[i]
        # revert character sequence for Latin only words
            if not (len(w) < 2 or max([ord(c) for c in w]) > 255):
                words[i] = "".join(reversed(w))
                idx.append(i)  # stored index of Latin word

        # adjacent Latin words must revert their sequence, too
        idx2 = []  # store indices of adjacent Latin words
        for i in range(len(idx)):
            if idx2 == []:  # empty yet?
                idx2.append(idx[i]) # store Latin word number

            elif idx[i] > idx2[-1] + 1:  # large gap to last?
                if len(idx2) > 1:  # at least two consecutives?
                    words[idx2[0] : idx2[-1] + 1] = reversed(
                        words[idx2[0] : idx2[-1] + 1]
                    )  # revert their sequence
                idx2 = [idx[i]]  # re-initialize

            elif idx[i] == idx2[-1] + 1:  # new adjacent Latin word
                idx2.append(idx[i])

        text = " ".join(words)
        return text

    def fill_textbox(*args, **kwargs):
        return utils.fill_textbox(*args, **kwargs)

    def write_text(self, page, color=None, opacity=-1, overlay=1, morph=None, matrix=None, render_mode=0, oc=0):
        """Write the text to a PDF page having the TextWriter's page size.

        Args:
            page: a PDF page having same size.
            color: override text color.
            opacity: override transparency.
            overlay: put in foreground or background.
            morph: tuple(Point, Matrix), apply a matrix with a fixpoint.
            matrix: Matrix to be used instead of 'morph' argument.
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
        if matrix != None and morph != None:
            raise ValueError("only one of matrix, morph is allowed")
        if getattr(opacity, "__float__", None) is None or opacity == -1:
            opacity = self.opacity
        if color is None:
            color = self.color

        #val = _fitz.TextWriter_write_text(self, page, color, opacity, overlay, morph, matrix, render_mode, oc)
        if 1:
            pdfpage = page._pdf_page()
            #pdf_obj *resources = NULL;
            #fz_buffer *contents = NULL;
            #fz_device *dev = NULL;
            #PyObject *result = NULL, *max_nums, *cont_string;
            alpha = 1
            if opacity >= 0 and opacity < 1:
                alpha = opacity
            #fz_colorspace *colorspace;
            ncol = 1
            dev_color = [0, 0, 0, 0]
            if color:
                ncol = JM_color_FromSequence(color, dev_color)
            if ncol == 3:
                colorspace = mupdf.mfz_device_rgb()
            elif ncol == 4:
                colorspace = mupdf.mfz_device_cmyk()
            else:
                colorspace = mupdf.mfz_device_gray()

            ASSERT_PDF(pdfpage)
            resources = mupdf.mpdf_new_dict(pdfpage.doc(), 5)
            contents = mupdf.mfz_new_buffer(1024)
            dev = mupdf.mpdf_new_pdf_device( pdfpage.doc(), mupdf.Matrix(), resources, contents)
            #jlib.log('{dev_color=}')
            mupdf.mfz_fill_text(
                    dev,
                    self.this,
                    mupdf.Matrix(),
                    colorspace,
                    dev_color,
                    alpha,
                    mupdf.ColorParams(mupdf.fz_default_color_params),
                    )
            mupdf.mfz_close_device( dev)

            # copy generated resources into the one of the page
            max_nums = JM_merge_resources( pdfpage, resources)
            cont_string = JM_EscapeStrFromBuffer( contents)
            result = (max_nums, cont_string)
            val = result

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

        cb = page.cropbox_position
        if bool(cb):
            new_cont_lines.append("1 0 0 1 %g %g cm" % (cb.x, cb.y))

        if morph:
            p = morph[0] * self.ictm
            delta = Matrix(1, 1).pretranslate(p.x, p.y)
            matrix = ~delta * morph[1] * delta
        if morph or matrix:
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
                fsize = float(temp[1])
                if render_mode != 0:
                    w = fsize * 0.05
                else:
                    w = 1
                new_cont_lines.append("%g w" % w)
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


class IRect(Rect):
    """IRect() - all zeros\nIRect(x0, y0, x1, y1)\nIRect(Rect or IRect) - new copy\nIRect(sequence) - from 'sequence'"""
    def __add__(self, p):
        return Rect.__add__(self, p).round()

    def __and__(self, x):
        return Rect.__and__(self, x).round()

    def __init__(self, *args):
        Rect.__init__(self, *args)
        self.x0 = math.floor(self.x0 + 0.001)
        self.y0 = math.floor(self.y0 + 0.001)
        self.x1 = math.ceil(self.x1 - 0.001)
        self.y1 = math.ceil(self.y1 - 0.001)
        return None

    def __mul__(self, m):
        return Rect.__mul__(self, m).round()

    def __neg__(self):
        return IRect(-self.x0, -self.y0, -self.x1, -self.y1)

    def __or__(self, x):
        return Rect.__or__(self, x).round()

    def __pos__(self):
        return IRect(self)

    def __repr__(self):
        return "IRect" + str(tuple(self))

    def __setitem__(self, i, v):
        v = int(v)
        if   i == 0: self.x0 = v
        elif i == 1: self.y0 = v
        elif i == 2: self.x1 = v
        elif i == 3: self.y1 = v
        else:
            raise IndexError("index out of range")
        return None

    def __sub__(self, p):
        return Rect.__sub__(self, p).round()

    def __truediv__(self, m):
        return Rect.__truediv__(self, m).round()

    def includePoint(self, p):
        """Extend rectangle to include point p."""
        return Rect.includePoint(self, p).round()

    def includeRect(self, r):
        """Extend rectangle to include rectangle r."""
        return Rect.includeRect(self, r).round()

    def intersect(self, r):
        """Restrict rectangle to intersection with rectangle r."""
        return Rect.intersect(self, r).round()

    irect = round

    @property
    def rect(self):
        return Rect(self)

    @property
    def round(self):
        pass

    def transform(self, m):
        return Rect.transform(self, m).round()


# Data
#

if 1:   # Import some mupdf constants
    self = sys.modules[__name__]
    for name, value in inspect.getmembers(mupdf):
        if name.startswith('PDF_'):
            assert not inspect.isroutine(value)
            setattr(self, name, value)
    assert PDF_TX_FIELD_IS_MULTILINE == mupdf.PDF_TX_FIELD_IS_MULTILINE
    del self

_adobe_glyphs = {}
_adobe_unicodes = {}

AnyType = typing.Any

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

CS_GRAY = mupdf.Colorspace.Fixed_GRAY
CS_RGB = mupdf.Colorspace.Fixed_RGB
CS_BGR = mupdf.Colorspace.Fixed_BGR
CS_CMYK = mupdf.Colorspace.Fixed_CMYK
CS_LAB = mupdf.Colorspace.Fixed_LAB

EPSILON = 1e-5
FLT_EPSILON = 1e-5

# largest 32bit integers surviving C float conversion roundtrips
# used by MuPDF to define infinite rectangles
FZ_MIN_INF_RECT = -0x80000000
FZ_MAX_INF_RECT = 0x7fffff80

JM_annot_id_stem = "fitz"
JM_mupdf_warnings_store = []
JM_mupdf_show_errors = 1
JM_mupdf_show_warnings = 0


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

TEXT_ALIGN_LEFT = 0
TEXT_ALIGN_CENTER = 1
TEXT_ALIGN_RIGHT = 2
TEXT_ALIGN_JUSTIFY = 3

TEXT_FONT_SUPERSCRIPT = 1
TEXT_FONT_ITALIC = 2
TEXT_FONT_SERIFED = 4
TEXT_FONT_MONOSPACED = 8
TEXT_FONT_BOLD = 16

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

TEXT_ENCODING_LATIN = 0
TEXT_ENCODING_GREEK = 1
TEXT_ENCODING_CYRILLIC = 2

TOOLS_JM_UNIQUE_ID = 0

annot_skel = {
    "goto1": "<</A<</S/GoTo/D[%i 0 R/XYZ %g %g 0]>>/Rect[%s]/BS<</W 0>>/Subtype/Link>>",
    "goto2": "<</A<</S/GoTo/D%s>>/Rect[%s]/BS<</W 0>>/Subtype/Link>>",
    "gotor1": "<</A<</S/GoToR/D[%i /XYZ %g %g 0]/F<</F(%s)/UF(%s)/Type/Filespec>>>>/Rect[%s]/BS<</W 0>>/Subtype/Link>>",
    "gotor2": "<</A<</S/GoToR/D%s/F(%s)>>/Rect[%s]/BS<</W 0>>/Subtype/Link>>",
    "launch": "<</A<</S/Launch/F<</F(%s)/UF(%s)/Type/Filespec>>>>/Rect[%s]/BS<</W 0>>/Subtype/Link>>",
    "uri": "<</A<</S/URI/URI(%s)>>/Rect[%s]/BS<</W 0>>/Subtype/Link>>",
    "named": "<</A<</S/Named/N/%s/Type/Action>>/Rect[%s]/BS<</W 0>>/Subtype/Link>>",
    }

csRGB = Colorspace(CS_RGB)
csGRAY = Colorspace(CS_GRAY)
csCMYK = Colorspace(CS_CMYK)

dictkey_align = "align"
dictkey_align = "ascender"
dictkey_bbox = "bbox"
dictkey_blocks = "blocks"
dictkey_bpc = "bpc"
dictkey_c = "c"
dictkey_chars = "chars"
dictkey_color = "color"
dictkey_colorspace = "colorspace"
dictkey_content = "content"
dictkey_creationDate = "creationDate"
dictkey_cs_name = "cs-name"
dictkey_da = "da"
dictkey_dashes = "dashes"
dictkey_desc = "desc"
dictkey_desc = "descender"
dictkey_dir = "dir"
dictkey_effect = "effect"
dictkey_ext = "ext"
dictkey_filename = "filename"
dictkey_fill = "fill"
dictkey_flags = "flags"
dictkey_font = "font"
dictkey_height = "height"
dictkey_id = "id"
dictkey_image = "image"
dictkey_items = "items"
dictkey_length = "length"
dictkey_lines = "lines"
dictkey_matrix = "transform"
dictkey_modDate = "modDate"
dictkey_name = "name"
dictkey_number = "number"
dictkey_origin = "origin"
dictkey_rect = "rect"
dictkey_size = "size"
dictkey_smask = "smask"
dictkey_spans = "spans"
dictkey_stroke = "stroke"
dictkey_style = "style"
dictkey_subject = "subject"
dictkey_text = "text"
dictkey_title = "title"
dictkey_type = "type"
dictkey_ufilename = "ufilename"
dictkey_width = "width"
dictkey_wmode = "wmode"
dictkey_xref = "xref"
dictkey_xres = "xres"
dictkey_yres = "yres"

fitz_fontdescriptors = dict()

skip_quad_corrections = 0   # Unset ascender / descender corrections
small_glyph_heights = 0 # Switch for computing glyph of fontsize height
subset_fontnames = 0    # Switch for returning fontnames including subset prefix

symbol_glyphs = (   # Glyph list for the built-in font 'Symbol'
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


zapf_glyphs = ( # Glyph list for the built-in font 'ZapfDingbats'
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


# Functions
#

def ASSERT_PDF(cond):
    assert isinstance(cond, (mupdf.PdfPage, mupdf.PdfDocument)), f'type(cond)={type(cond)} cond={cond}'
    if not cond.m_internal:
        raise Exception('not a PDF')

def DUMMY(*args, **kw):
    return

def ENSURE_OPERATION(pdf):
     if not JM_have_operation(pdf):
        raise Exception("No journalling operation started")

def INRANGE(v, low, high):
    return low <= v and v <= high


def JM_BinFromBuffer(buffer_):
    '''
    Turn fz_buffer into a Python bytes object
    '''
    assert isinstance(buffer_, mupdf.Buffer)
    return buffer_.buffer_extract()


def JM_EscapeStrFromStr(c):
    # fixme: need to make this handle escape sequences.
    return c


def JM_BufferFromBytes(stream):
    '''
    Make fz_buffer from a PyBytes, PyByteArray, io.BytesIO object.
    '''
    if isinstance(stream, bytes):
        return mupdf.Buffer.new_buffer_from_copied_data(stream)
    if isinstance(stream, bytearray):
        return mupdf.Buffer.new_buffer_from_copied_data(stream)
    if hasattr(stream, 'getvalue'):
        data = stream.getvalue()
        if isinstance(data, bytes):
            pass
        elif isinstance(data, str):
            data = data.encode('utf-8')
        else:
            raise Exception(f'.getvalue() returned unexpected type: {type(data)}')
        return mupdf.Buffer.new_buffer_from_copied_data(data)
    #if instance(stream, io.BytesIO):
    #    b = stream.getvalue()
    #    return mupdf.Buffer.new_buffer_from_copied_data(b)
    #if
    #jlib.log('Unrecognised {type(stream)=}')
    return mupdf.Buffer()


def JM_FLOAT_ITEM(obj, idx):
    if idx < 0 or idx >= len(obj):
        return None
    ret = obj[idx]
    assert isinstance(ret, float)
    return ret


def JM_FLOAT_ITEM(obj, idx):
    if not PySequence_Check(obj):
        return None
    return float(obj[idx])

def JM_INT_ITEM(obj, idx):
    if idx < len(obj):
        temp = obj[idx]
        if isinstance(temp, (int, float)):
            return 0, temp
    return 1, None

def JM_StrAsChar(x):
    # fixme: should encode, but swig doesn't pass bytes to C as const char*.
    return x
    #return x.encode('utf8')

def JM_TUPLE(o: typing.Sequence) -> tuple:
    return tuple(map(lambda x: round(x, 5) if abs(x) >= 1e-4 else 0, o))

def JM_UnicodeFromStr(s):
    if s is None:
        return ''
    if isinstance(s, bytes):
        s = s.decode('utf8')
    assert isinstance(s, str), f'type(s)={type(s)} s={s}'
    return s


def JM_add_annot_id(annot, stem):
    assert isinstance(annot, mupdf.PdfAnnot)
    names = JM_get_annot_id_list(annot.annot_page())
    i = 0
    while 1:
        stem_id = f'{JM_annot_id_stem}-{stem}{i}'
        if stem_id not in names:
            break
        i += 1

    response = stem_id
    name = mupdf.PdfObj(response)
    #annot.annot_obj().dict_puts_drop("NM", name)
    annot.annot_obj().dict_puts("NM", name)
    # fixme: pymupdf's JM_add_annot_id() appears be able to compile this code:
    #
    #   pdf_annot *annot;
    #   annot->obj;
    #   annot->page;
    #
    # Even though mumpdf headers only forward-declare pdf_annot.  Full
    # definition of pdf_annot is in mupdf/source/pdf/pdf-annot-imp.h, which is
    # not included by any .h files.


def JM_add_oc_object(pdf, ref, xref):
    '''
    Add OC object reference to a dictionary
    '''
    indobj = mupdf.mpdf_new_indirect(pdf, xref, 0)
    if not mupdf.mpdf_is_dict(indobj):
        THROWMSG("bad 'oc' reference")
    type = mupdf.mpdf_dict_get(indobj, PDF_NAME('Type'))
    if (mupdf.mpdf_objcmp(type, PDF_NAME('OCG')) == 0
            or mupdf.mpdf_objcmp(type, PDF_NAME('OCMD')) == 0
            ):
        mupdf.mpdf_dict_put(ref, PDF_NAME('OC'), indobj)
    else:
        THROWMSG("bad 'oc' type")

def JM_annot_border(annot_obj):
    assert isinstance(annot_obj, mupdf.PdfObj), f'{annot_obj}'

    res = {}
    dash_py   = []
    effect_py = []
    width = -1.0
    effect1 = -1
    effect2 = None
    style = None
    o = annot_obj.dict_get(mupdf.PDF_ENUM_NAME_Border)
    if o.is_array():
        width = pdf_to_real(ctx, o.array_get(2))
        if o.array_len() == 4:
            dash = o.array_get(3)
            for i in range(dash.array_len()):
                val = mupdf.ppdf_to_int(ctx, dash.array_get(i))
                dash_py.append(val)

    bs_o = annot_obj.dict_get(mupdf.PDF_ENUM_NAME_BS)
    if bs_o.m_internal:
        o = bs_o.dict_get(mupdf.PDF_ENUM_NAME_W)
        if o.m_internal:
            width = o.to_real()
        o = bs_o.dict_get(mupdf.PDF_ENUM_NAME_S)
        if o.m_internal:
            style = o.to_name()
        o = bs_o.dict_get(mupdf.PDF_ENUM_NAME_D)
        if o.m_internal:
            for i in range(o.array_len()):
                val = o.array_get(i).to_int()
                dash_py.append(val)

    be_o = annot_obj.dict_gets("BE")
    if be_o.m_internal:
        o = be_o.dict_get(mupdf.PDF_ENUM_NAME_S)
        if o.m_internal:
            effect2 = o.to_name()
        o = be_o.dict_get(mupdf.PDF_ENUM_NAME_I)
        if o.m_internal:
            effect1 = o.to_int()

    effect_py.append(effect1)
    effect_py.append(effect2)
    res[dictkey_width] = width
    res[dictkey_dashes] = dash_py
    res[dictkey_style] = style
    if effect1 > -1:
        res[dictkey_effect] = effect_py
    return res;


def JM_annot_colors(annot_obj):
    res = dict()
    bc = list() # stroke colors
    fc =list()  # fill colors
    o = annot_obj.dict_get(mupdf.PDF_ENUM_NAME_C)
    if o.is_array:
        n = o.array_len()
        for i in range(n):
            col = o.array_get(i).to_real()
            bc.append(col)
    res[dictkey_stroke] = bc

    o = annot_obj.dict_gets("IC")
    if o.is_array():
        n = o.array_len()
        for i in range(n):
            col = o.array_get(i).to_real()
            fc.append(col)

    res[dictkey_fill] = fc
    return res;


def JM_annot_set_border(border, doc, annot_obj):
    assert isinstance(border, dict)

    nwidth = border.get(dictkey_width)  # new width
    ndashes = border.get(dictkey_dashes)# new dashes
    nstyle  = border.get(dictkey_style) # new style

    # first get old border properties
    oborder = JM_annot_border(annot_obj)
    owidth = oborder.get(dictkey_width)     # old width
    odashes = oborder.get(dictkey_dashes)   # old dashes
    ostyle = oborder.get(dictkey_style)     # old style

    # then delete any relevant entries
    annot_obj.dict_del(mupdf.PDF_ENUM_NAME_BS)
    annot_obj.dict_del(mupdf.PDF_ENUM_NAME_BE)
    annot_obj.dict_del(mupdf.PDF_ENUM_NAME_Border)

    # populate new border array
    if nwidth < 0:
        nwidth = owidth # no new width: take current
    if nwidth < 0:
        nwidth = 0.0    # default if no width given
    if ndashes is None:
        ndashes = odashes   # no new dashes: take old
    if nstyle is None:
        nstyle  = ostyle;   # no new style: take old

    if ndashes and isinstance(ndashes, (tuple, list)) and len(ndashes) > 0:
        n = len(ndashes)
        darr = doc.new_array(n);
        for i in range(n):
            d = ndashes[i]
            darr.array_push_int(d)
        annot_obj.dict_putl(darr, mupdf.PDF_ENUM_NAME_BS, mupdf.PDF_ENUM_NAME_D)
        nstyle = "D"

    annot_obj.dict_putl(
            mupdf.mpdf_new_real(float(nwidth)),
            mupdf.PDF_ENUM_NAME_BS,
            mupdf.PDF_ENUM_NAME_W,
            )

    val = JM_get_border_style(nstyle)

    annot_obj.dict_putl(val, mupdf.PDF_ENUM_NAME_BS, mupdf.PDF_ENUM_NAME_S)


def JM_append_rune(buff, ch):
    '''
    APPEND non-ascii runes in unicode escape format to fz_buffer
    '''
    if (ch >= 32 and ch <= 127) or ch == 10:
        mupdf.mfz_append_byte(buff, ch)
    elif ch <= 0xffff:  # 4 hex digits
        mupdf.mfz_append_printf(buff, "\\u%04x", ch)
    else:   # 8 hex digits
        mupdf.mfz_append_printf(buff, "\\U%08x", ch)


def JM_append_word(lines, buff, wbbox, block_n, line_n, word_n):
    '''
    Functions for wordlist output
    '''
    s = JM_EscapeStrFromBuffer(buff)
    litem = (
            wbbox.x0,
            wbbox.y0,
            wbbox.x1,
            wbbox.y1,
            s,
            block_n,
            line_n,
            word_n,
            )
    lines.append(litem)
    return word_n + 1, mupdf.Rect(mupdf.Rect.Fixed_EMPTY)   # word counter


"""
# fz_output for Python file objects

def JM_bytesio_write(opaque, data, size_t len):
    '''
    bio.write(bytes object)
    '''
    bio = opaque, *b, *name, *rc;
    fz_try(ctx){
        b = PyBytes_FromStringAndSize((const char *) data, (Py_ssize_t) len);
        name = PyUnicode_FromString("write");
        rc = PyObject_CallMethodObjArgs(bio, name, b, NULL);
        if (!rc) {
            THROWMSG(ctx, "could not write to Py file obj");
        }
    }
    fz_always(ctx) {
        Py_XDECREF(b);
        Py_XDECREF(name);
        Py_XDECREF(rc);
        PyErr_Clear();
    }
    fz_catch(ctx) {
        fz_rethrow(ctx);
    }
}

static void
JM_bytesio_truncate(fz_context *ctx, void *opaque)
{  // bio.truncate(bio.tell()) !!!
    PyObject *bio = opaque, *trunc = NULL, *tell = NULL, *rctell= NULL, *rc = NULL;
    fz_try(ctx) {
        trunc = PyUnicode_FromString("truncate");
        tell = PyUnicode_FromString("tell");
        rctell = PyObject_CallMethodObjArgs(bio, tell, NULL);
        rc = PyObject_CallMethodObjArgs(bio, trunc, rctell, NULL);
        if (!rc) {
            THROWMSG(ctx, "could not truncate Py file obj");
        }
    }
    fz_always(ctx) {
        Py_XDECREF(tell);
        Py_XDECREF(trunc);
        Py_XDECREF(rc);
        Py_XDECREF(rctell);
        PyErr_Clear();
    }
    fz_catch(ctx) {
        fz_rethrow(ctx);
    }
}

static int64_t
JM_bytesio_tell(fz_context *ctx, void *opaque)
{  // returns bio.tell() -> int
    PyObject *bio = opaque, *rc = NULL, *name = NULL;
    int64_t pos = 0;
    fz_try(ctx) {
        name = PyUnicode_FromString("tell");
        rc = PyObject_CallMethodObjArgs(bio, name, NULL);
        if (!rc) {
            THROWMSG(ctx, "could not tell Py file obj");
        }
        pos = (int64_t) PyLong_AsUnsignedLongLong(rc);
    }
    fz_always(ctx) {
        Py_XDECREF(name);
        Py_XDECREF(rc);
        PyErr_Clear();
    }
    fz_catch(ctx) {
        fz_rethrow(ctx);
    }
    return pos;
}


static void
JM_bytesio_seek(fz_context *ctx, void *opaque, int64_t off, int whence)
{  // bio.seek(off, whence=0)
    PyObject *bio = opaque, *rc = NULL, *name = NULL, *pos = NULL;
    fz_try(ctx) {
        name = PyUnicode_FromString("seek");
        pos = PyLong_FromUnsignedLongLong((unsigned long long) off);
        rc = PyObject_CallMethodObjArgs(bio, name, pos, whence, NULL);
        if (!rc) {
            THROWMSG(ctx, "could not seek Py file obj");
        }
    }
    fz_always(ctx) {
        Py_XDECREF(rc);
        Py_XDECREF(name);
        Py_XDECREF(pos);
        PyErr_Clear();
    }
    fz_catch(ctx) {
        fz_rethrow(ctx);
    }
}
"""

def JM_char_bbox(line, ch, verbose=0):
    '''
    return rect of char quad
    '''
    q = JM_char_quad(line, ch, verbose)
    if verbose:
        jlib.log('q={q}')
    r = mupdf.mfz_rect_from_quad(q)
    if verbose:
        jlib.log('q={q} r={r}')
    if verbose:
        jlib.log('{ch.m_internal.size=} {line.m_internal.wmode=} {r=}')
    if not line.m_internal.wmode:
        return r
    if r.y1 < r.y0 + ch.m_internal.size:
        r.y0 = r.y1 - ch.m_internal.size
    return r


def JM_char_font_flags(font, line, ch):
    flags = detect_super_script(line, ch)
    flags += mupdf.mfz_font_is_italic(font) * TEXT_FONT_ITALIC
    flags += mupdf.mfz_font_is_serif(font) * TEXT_FONT_SERIFED
    flags += mupdf.mfz_font_is_monospaced(font) * TEXT_FONT_MONOSPACED
    flags += mupdf.mfz_font_is_bold(font) * TEXT_FONT_BOLD
    return flags


def JM_char_quad(line, ch, verbose=0):
    '''
    re-compute char quad if ascender/descender values make no sense
    '''
    assert isinstance(line, mupdf.StextLine)
    assert isinstance(ch, mupdf.StextChar)
    if verbose: jlib.log('{skip_quad_corrections=} {small_glyph_heights=}')
    if skip_quad_corrections:   # no special handling
        if verbose: jlib.log('skip_quad_corrections, returning {ch.quad=}')
        return ch.quad
    if line.m_internal.wmode:  # never touch vertical write mode
        if verbose: jlib.log('wmode set, returning ch.quad')
        return ch.quad
    #font = mupdf.Font(ch.m_internal.font)
    font = mupdf.Font(mupdf.keep_font(ch.m_internal.font))
    asc = JM_font_ascender(font)
    dsc = JM_font_descender(font, verbose)
    if verbose: jlib.log('{asc=} {dsc=} {asc-dsc=} {small_glyph_heights=}')
    if asc - dsc >= 1 and small_glyph_heights == 0: # no problem
        if verbose: jlib.log('{asc=} - {dsc=} >= 1: returning ch.m_internal.quad')
        return mupdf.Quad(ch.m_internal.quad)

    # Re-compute quad with adjusted ascender / descender values:
    # Move ch->origin to (0,0) and de-rotate quad, then adjust the corners,
    # re-rotate and move back to ch->origin location.
    fsize = ch.m_internal.size
    #fz_matrix trm1, trm2, xlate1, xlate2;
    #fz_quad quad;
    bbox = mupdf.mfz_font_bbox(font)
    fwidth = bbox.x1 - bbox.x0
    if asc < 1e-3:  # probably Tesseract glyphless font
        dsc = -0.1

    # Re-compute asc, dsc if there are problems.
    # In that case, we also do not trust dsc and try correcting it.
    if asc - dsc < 1:
        if bbox.y0 < dsc:
            dsc = bbox.y0
        asc = 1 + dsc

    c = line.m_internal.dir.x  # cosine
    s = line.m_internal.dir.y  # sine
    trm1 = mupdf.mfz_make_matrix(c, -s, s, c, 0, 0) # derotate
    trm2 = mupdf.mfz_make_matrix(c, s, -s, c, 0, 0) # rotate
    xlate1 = mupdf.mfz_make_matrix(1, 0, 0, 1, -ch.m_internal.origin.x, -ch.m_internal.origin.y)
    xlate2 = mupdf.mfz_make_matrix(1, 0, 0, 1, ch.m_internal.origin.x, ch.m_internal.origin.y)

    quad = mupdf.mfz_transform_quad(mupdf.Quad(ch.m_internal.quad), xlate1)    # move origin to (0,0)
    quad = mupdf.mfz_transform_quad(quad, trm1) # de-rotate corners

    # adjust vertical coordinates if meaningful
    if quad.ll.y - quad.ul.y > fsize:
        quad.ll.y = -fsize * dsc / (asc - dsc)
        quad.ul.y = quad.ll.y - fsize
        quad.lr.y = quad.ll.y
        quad.ur.y = quad.ul.y

    # adjust crazy horizontal coordinates
    if quad.lr.x - quad.ll.x < FLT_EPSILON:
        quad.lr.x = quad.ll.x + fwidth * fsize
        quad.ur.x = quad.lr.x

    quad = mupdf.mfz_transform_quad(quad, trm2) # rotate back
    quad = mupdf.mfz_transform_quad(quad, xlate2)   # translate back
    return quad


def JM_choice_options(annot):
    '''
    return list of choices for list or combo boxes
    '''
    annot_obj = mupdf.mpdf_annot_obj(annot)
    pdf = mupdf.mpdf_get_bound_document(annot_obj)

    # pdf_choice_widget_options() is not usable from python, so we implement it
    # ourselves here.
    #
    #n = mupdf.mpdf_choice_widget_options(annot, 0, 0)
    #jlib.log('{type(annot)=}')
    optarr = mupdf.mpdf_dict_get_inheritable(annot.this.annot_obj(), PDF_NAME('Opt'))
    n = mupdf.mpdf_array_len(optarr)

    if n == 0:
        return  # wrong widget type
    optarr = mupdf.mpdf_dict_get(annot_obj, PDF_NAME('Opt'))
    liste = []
    for i in range(n):
        m = mupdf.mpdf_array_len(mupdf.mpdf_array_get(optarr, i))
        if m == 2:
            val = (
                    mupdf.mpdf_to_text_string(mupdf.mpdf_array_get(mupdf.mpdf_array_get(optarr, i), 0)),
                    mupdf.mpdf_to_text_string(mupdf.mpdf_array_get(mupdf.mpdf_array_get(optarr, i), 1)),
                    )
            liste.append(val)
        else:
            val = JM_UnicodeFromStr(mupdf.mpdf_to_text_string(mupdf.mpdf_array_get(optarr, i)));
            liste.append(val)
    return liste


def JM_color_FromSequence(color, col):
    #assert isinstance( col, list)
    if not color or (not isinstance(color, list) and not isinstance(color, float)):
        return 1
    if isinstance(color, float):    # maybe just a single float
        c = color
        if not INRANGE(c, 0, 1):
            return 1
        col[0] = c
        return 1

    len_ = len(color)
    if not INRANGE(len_, 1, 4) or len_ == 2:
        return 1

    mcol = [0,0,0,0]    # local color storage
    for i in range(len_):
        if i < len(mcol):
            mcol[i] = color[i]
            rc = 0
        else:
            rc = 1
        if not INRANGE(mcol[i], 0, 1) or rc == 1:
            mcol[i] = 1;

    for i in range(len_):
        col[i] = mcol[i]
    return len_


def JM_compress_buffer(inbuffer):
    '''
    compress char* into a new buffer
    '''
    data, compressed_length = mupdf.new_deflated_data_from_buffer(
            inbuffer.m_internal,
            mupdf.FZ_DEFLATE_BEST,
            )
    #jlib.log('{data=} {compressed_length=}')
    if not data or compressed_length == 0:
        return None
    buf = mupdf.Buffer(mupdf.new_buffer_from_data(data, compressed_length))
    buf.resize_buffer(compressed_length)
    return buf;


def JM_copy_rectangle(page, area):
    need_new_line = 0
    buffer_ = mupdf.mfz_new_buffer(1024)
    for block in page:
        if block.m_internal.type != mupdf.FZ_STEXT_BLOCK_TEXT:
            continue
        for line in block:
            line_had_text = 0
            for ch in line:
                r = JM_char_bbox(line, ch)
                if mupdf.mfz_contains_rect(area, r):
                    line_had_text = 1
                    if need_new_line:
                        mupdf.mfz_append_string(buffer_, "\n")
                        need_new_line = 0
                    mupdf.mfz_append_rune(
                            buffer_,
                            FZ_REPLACEMENT_CHARACTER if ch.m_internal.c < 32 else ch.m_internal.c,
                            )
            if line_had_text:
                need_new_line = 1
    mupdf.mfz_terminate_buffer(buffer_)

    s = buffer_.buffer_extract()   # take over the data
    return s


def JM_convert_to_pdf(doc, fp, tp, rotate):
    '''
    Convert any MuPDF document to a PDF
    Returns bytes object containing the PDF, created via 'write' function.
    '''
    #jlib.log('*** calling mupdf.mpdf_create_document()')
    #pdfout = mupdf.mpdf_create_document()   # new PDF document
    #jlib.log('*** calling mupdf.PdfDocument()')
    pdfout = mupdf.PdfDocument()
    incr = 1
    s = fp
    e = tp
    if fp > tp:
        incr = -1   # count backwards
        s = tp      # adjust ...
        e = fp      # ... range
    #fz_rect mediabox;
    rot = JM_norm_rotation(rotate)
    #fz_device *dev = NULL;
    #fz_buffer *contents = NULL;
    #pdf_obj *resources = NULL;
    #fz_page *page;
    #for (i = fp; INRANGE(i, s, e); i += incr) {  # interpret & write document pages as PDF pages
    i = fp
    while 1:    # interpret & write document pages as PDF pages
        if not INRANGE(i, s, e):
            break
        page = mupdf.mfz_load_page(doc, i)
        mediabox = mupdf.mfz_bound_page(page)
        resources = mupdf.PdfObj(0)
        contents = mupdf.Buffer(0)
        dev = mupdf.mpdf_page_write(pdfout, mediabox, resources, contents);
        mupdf.mfz_run_page(page, dev, mupdf.Matrix(), mupdf.Cookie());
        mupdf.mfz_close_device(dev)
        dev = None
        page_obj = mupdf.mpdf_add_page(pdfout, mediabox, rot, resources, contents)
        mupdf.mpdf_insert_page(pdfout, -1, page_obj)
        i += 1
    # PDF created - now write it to Python bytearray
    #PyObject *r = NULL;
    #fz_output *out = NULL;
    #fz_buffer *res = NULL;
    # prepare write options structure
    opts = mupdf.PdfWriteOptions()
    opts.do_garbage         = 4
    opts.do_compress        = 1
    opts.do_compress_images = 1
    opts.do_compress_fonts  = 1
    opts.do_sanitize        = 1
    opts.do_incremental     = 0
    opts.do_ascii           = 0
    opts.do_decompress      = 0
    opts.do_linear          = 0
    opts.do_clean           = 1
    opts.do_pretty          = 0

    res = mupdf.mfz_new_buffer(8192)
    out = mupdf.Output(res)
    #jlib.log('{type(out)=} {out=}')
    mupdf.mpdf_write_document(pdfout, out, opts)
    #unsigned char *c = NULL;
    c = res.buffer_extract()
    assert isinstance(c, bytes)
    return c
    #r = PyBytes_FromStringAndSize((const char *) c, (Py_ssize_t) len);
    #return r;


# Copied from MuPDF v1.14
# Create widget
def JM_create_widget(doc, page, type, fieldname):
    old_sigflags = mupdf.mpdf_to_int(mupdf.mpdf_dict_getp(mupdf.mpdf_trailer(doc), "Root/AcroForm/SigFlags"))
    annot = mupdf.mpdf_create_annot_raw(page, mupdf.PDF_ANNOT_WIDGET)
    annot_obj = mupdf.mpdf_annot_obj(annot)
    try:
        JM_set_field_type(doc, annot_obj, type)
        mupdf.mpdf_dict_put_text_string(annot_obj, PDF_NAME('T'), fieldname)

        if type == mupdf.PDF_WIDGET_TYPE_SIGNATURE:
            sigflags = old_sigflags | (SigFlag_SignaturesExist|SigFlag_AppendOnly)
            mupdf.mpdf_dict_putl(
                    mupdf.mpdf_trailer(doc),
                    mupdf.mpdf_new_nt(sigflags),
                    PDF_NAME('Root'),
                    PDF_NAME('AcroForm'),
                    PDF_NAME('SigFlags'),
                    )
        # pdf_create_annot will have linked the new widget into the page's
        # annot array. We also need it linked into the document's form
        form = mupdf.mpdf_dict_getp(mupdf.mpdf_trailer(doc), "Root/AcroForm/Fields")
        if not form.m_internal:
            form = mupdf.mpdf_new_array(doc, 1)
            mupdf.mpdf_dict_putl(
                    mupdf.mpdf_trailer(doc),
                    form,
                    PDF_NAME('Root'),
                    PDF_NAME('AcroForm'),
                    PDF_NAME('Fields'),
                    )
        mupdf.mpdf_array_push(form, annot_obj)  # Cleanup relies on this statement being last
    except Exception:
        mupdf.mpdf_delete_annot(page, annot)

        if type == mupdf.PDF_WIDGET_TYPE_SIGNATURE:
            mupdf.mpdf_dict_putl(
                    mupdf.mpdf_trailer(doc),
                    mupdf.mpdf_new_int(old_sigflags),
                    PDF_NAME('Root'),
                    PDF_NAME('AcroForm'),
                    PDF_NAME('SigFlags'),
                    )
        raise

    return annot;


def JM_cropbox(page_obj):
    '''
    return a PDF page's CropBox
    '''
    mediabox = JM_mediabox(page_obj)
    cropbox = mupdf.mpdf_to_rect(
                mupdf.mpdf_dict_get_inheritable(page_obj, PDF_NAME('CropBox'))
                )
    if mupdf.mfz_is_infinite_rect(cropbox) or mupdf.mfz_is_empty_rect(cropbox):
        return mediabox
    y0 = mediabox.y1 - cropbox.y1
    y1 = mediabox.y1 - cropbox.y0
    cropbox.y0 = y0
    cropbox.y1 = y1
    return cropbox


def JM_derotate_page_matrix(page):
    '''
    just the inverse of rotation
    '''
    mp = JM_rotate_page_matrix(page)
    return mupdf.mfz_invert_matrix(mp)


def JM_delete_annot(page, annot):
    '''
    delete an annotation using mupdf functions, but first delete the /AP
    dict key in annot->obj.
    '''
    if not annot or not annot.m_internal:
        return
    try:
        # first get any existing popup for the annotation
        popup = mupdf.mpdf_dict_get(annot.annot_obj(), PDF_NAME('Popup'))

        # next delete the /Popup and /AP entries from annot dictionary
        mupdf.mpdf_dict_del(annot.annot_obj(), PDF_NAME('AP'))

        annots = mupdf.mpdf_dict_get(page.obj(), PDF_NAME('Annots'))
        assert annots.m_internal
        n = mupdf.mpdf_array_len(annots)
        for i in range(n - 1, -1, -1):
            o = mupdf.mpdf_array_get(annots, i)
            p = mupdf.mpdf_dict_get(o, PDF_NAME('Parent'))
            if not p.m_internal:
                continue;
            if not mupdf.mpdf_objcmp(p, annot.annot_obj()):
                mupdf.mpdf_array_delete(annots, i)
        assert annot.m_internal
        type_ = mupdf.mpdf_annot_type(annot)
        if type_ != mupdf.PDF_ANNOT_WIDGET:
            mupdf.mpdf_delete_annot(page, annot)
        else:
            #jlib.log('Calling JM_delete_widget()')
            JM_delete_widget(page, annot)
    except Exception as e:
        jlib.log('{e=}')
        #jlib.log('could not delete annotation')
        #jlib.log('{jlib.exception_info()=}')
        # fixme: mupdf.mfz_warn("could not delete annotation")
    return


def JM_embed_file(
        pdf,
        buf,
        filename,
        ufilename,
        desc,
        compress,
        ):
    '''
    embed a new file in a PDF (not only /EmbeddedFiles entries)
    '''
    len_ = 0;
    #pdf_obj *ef, *f, *params, *val = NULL;
    #fz_var(val);
    try:
        val = mupdf.mpdf_new_dict(pdf, 6)
        mupdf.mpdf_dict_put_dict(val, PDF_NAME('CI'), 4)
        ef = mupdf.mpdf_dict_put_dict(val, PDF_NAME('EF'), 4)
        mupdf.mpdf_dict_put_text_string(val, PDF_NAME('F'), filename)
        mupdf.mpdf_dict_put_text_string(val, PDF_NAME('UF'), ufilename)
        mupdf.mpdf_dict_put_text_string(val, PDF_NAME('Desc'), desc)
        mupdf.mpdf_dict_put(val, PDF_NAME('Type'), PDF_NAME('Filespec'))
        bs = b'  '
        f = mupdf.mpdf_add_stream(
                pdf,
                #mupdf.mfz_new_buffer_from_copied_data(bs),
                mupdf.Buffer.new_buffer_from_copied_data(bs),
                mupdf.PdfObj(),
                0,
                )
        mupdf.mpdf_dict_put(ef, PDF_NAME('F'), f)
        JM_update_stream(pdf, f, buf, compress)
        len_, _ = buf.buffer_storage_raw()
        mupdf.mpdf_dict_put_int(f, PDF_NAME('DL'), len_)
        mupdf.mpdf_dict_put_int(f, PDF_NAME('Length'), len_)
        params = mupdf.mpdf_dict_put_dict(f, PDF_NAME('Params'), 4)
        mupdf.mpdf_dict_put_int(params, PDF_NAME('Size'), len_)
    except Exception as e:
        #jlib.log('{e=} {jlib.exception_info()=}')
        raise
    return val


def JM_embedded_clean(pdf):
    '''
    perform some cleaning if we have /EmbeddedFiles:
    (1) remove any /Limits if /Names exists
    (2) remove any empty /Collection
    (3) set /PageMode/UseAttachments
    '''
    root = mupdf.mpdf_dict_get( mupdf.mpdf_trailer( pdf), PDF_NAME('Root'))

    # remove any empty /Collection entry
    coll = mupdf.mpdf_dict_get(root, PDF_NAME('Collection'))
    if coll.m_internal and mupdf.mpdf_dict_len(coll) == 0:
        mupdf.mpdf_dict_del(root, PDF_NAME('Collection'))

    efiles = mupdf.mpdf_dict_getl(
            root,
            PDF_NAME('Names'),
            PDF_NAME('EmbeddedFiles'),
            PDF_NAME('Names'),
            )
    if efiles.m_internal:
        mupdf.mpdf_dict_put_name(root, PDF_NAME('PageMode'), "UseAttachments")


def JM_EscapeStrFromBuffer(buff):
    if not buff.m_internal:
         return ''
    s = buff.buffer_extract()
    val = PyUnicode_DecodeRawUnicodeEscape(s, errors='replace')
    return val;


def JM_ensure_identity(pdf):
    '''
    Store ID in PDF trailer
    '''
    #unsigned char rnd[16];
    id_ = mupdf.mpdf_dict_get( mupdf.mpdf_trailer(pdf), PDF_NAME('ID'))
    if not id_.m_internal:
        rnd0 = mupdf.mfz_memrnd2(16)
        #jlib.log('{type(rnd0)=} {rnd0!r=}')
        # Need to convert raw bytes into a str to send to
        # mupdf.mpdf_new_string(). chr() seems to work for this.
        rnd = ''
        for i in rnd0:
            rnd += chr(i)
        #jlib.log('{type(rnd)=} {rnd!r=}')
        id_ = mupdf.mpdf_dict_put_array( mupdf.mpdf_trailer( pdf), PDF_NAME('ID'), 2)
        mupdf.mpdf_array_push( id_, mupdf.mpdf_new_string( rnd, len(rnd)))
        mupdf.mpdf_array_push( id_, mupdf.mpdf_new_string( rnd, len(rnd)))

def JM_ensure_ocproperties(pdf):
    '''
    Ensure OCProperties, return /OCProperties key
    '''
    ocp = mupdf.mpdf_dict_get(mupdf.mpdf_dict_get(mupdf.mpdf_trailer(pdf), PDF_NAME('Root')), PDF_NAME('OCProperties'))
    if ocp.m_internal:
        return ocp
    root = mupdf.mpdf_dict_get(mupdf.mpdf_trailer(pdf), PDF_NAME('Root'))
    ocp = mupdf.mpdf_dict_put_dict(root, PDF_NAME('OCProperties'), 2)
    mupdf.mpdf_dict_put_array(ocp, PDF_NAME('OCGs'), 0)
    D = mupdf.mpdf_dict_put_dict(ocp, PDF_NAME('D'), 5)
    mupdf.mpdf_dict_put_array(D, PDF_NAME('ON'), 0)
    mupdf.mpdf_dict_put_array(D, PDF_NAME('OFF'), 0)
    mupdf.mpdf_dict_put_array(D, PDF_NAME('Order'), 0)
    mupdf.mpdf_dict_put_array(D, PDF_NAME('RBGroups'), 0)
    return ocp


def JM_expand_fname(name):
    '''
    Make /DA string of annotation
    '''
    if not name:    return "Helv"
    if name.startswith("Co"):   return "Cour"
    if name.startswith("co"):   return "Cour"
    if name.startswith("Ti"):   return "TiRo"
    if name.startswith("ti"):   return "TiRo"
    if name.startswith("Sy"):   return "Symb"
    if name.startswith("sy"):   return "Symb"
    if name.startswith("Za"):   return "ZaDb"
    if name.startswith("za"):   return "ZaDb"
    return "Helv"


def JM_field_type_text(wtype):
    '''
    String from widget type
    '''
    if wtype == PDF_WIDGET_TYPE_BUTTON:
        return "Button"
    if wtype == PDF_WIDGET_TYPE_CHECKBOX:
        return "CheckBox"
    if wtype == PDF_WIDGET_TYPE_RADIOBUTTON:
        return "RadioButton"
    if wtype == PDF_WIDGET_TYPE_TEXT:
        return "Text"
    if wtype == PDF_WIDGET_TYPE_LISTBOX:
        return "ListBox"
    if wtype == PDF_WIDGET_TYPE_COMBOBOX:
        return "ComboBox"
    if wtype == PDF_WIDGET_TYPE_SIGNATURE:
        return "Signature"
    return "unknown"


def JM_find_annot_irt(annot):
    '''
    Return the first annotation whose /IRT key ("In Response To") points to
    annot. Used to remove the response chain of a given annotation.
    '''
    assert isinstance(annot, mupdf.PdfAnnot)
    found = 0;
    try:    # loop thru MuPDF's internal annots array
        page = annot.annot_page()
        annotptr = page.first_annot()
        while 1:
            assert isinstance(annotptr, mupdf.PdfAnnot)
            if not annotptr.m_internal:
                break
            o = mupdf.mpdf_dict_gets(annotptr.annot_obj(), 'IRT')
            if o:
                if not mupdf.mpdf_objcmp(o, annot.annot_obj()):
                    found = 1
                    break
            annotptr = annotptr.next_annot()
    except Exception as e:
        jlib.log('{e=}')
    return irt_annot if found else None


def JM_font_ascender(font):
    '''
    need own versions of ascender / descender
    '''
    assert isinstance(font, mupdf.Font)
    if skip_quad_corrections:
        return 0.8
    return mupdf.mfz_font_ascender(font)


def JM_font_descender(font, verbose=0):
    '''
    need own versions of ascender / descender
    '''
    assert isinstance(font, mupdf.Font)
    if skip_quad_corrections:
        return -0.2
    ret = mupdf.mfz_font_descender(font)
    if verbose and abs(ret) > 200:
        jlib.log('{font.m_internal.t3procs=}')
        if font.m_internal.t3procs:
            jlib.log('{font.m_internal.bbox.y0=}')
        else:
            jlib.log('{font.m_internal.ft_face=}')
    return ret


def JM_font_name(font):
    assert isinstance(font, mupdf.Font)
    name = mupdf.mfz_font_name(font)
    s = name.find('+')
    if subset_fontnames or s == -1 or s-name != 6:
        return name
    return name[s + 1:]


def JM_gather_fonts(pdf, dict_, fontlist, stream_xref):
    rc = 1
    n = dict_.dict_len()
    for i in range(n):

        refname = dict_.dict_get_key(i)
        fontdict = dict_.dict_get_val(i)
        if not fontdict.is_dict():
            #fz_warn(ctx, "'%s' is no font dict (%d 0 R)", pdf_to_name(ctx, refname), pdf_to_num(ctx, fontdict));
            print(f'{refname.to_name()} is no font dict ({fontdict.to_num()} 0 R)')
            continue

        subtype = fontdict.dict_get(mupdf.PDF_ENUM_NAME_Subtype)
        basefont = fontdict.dict_get(mupdf.PDF_ENUM_NAME_BaseFont)
        if not basefont.m_internal or basefont.is_null():
            name = fontdict.dict_get(mupdf.PDF_ENUM_NAME_Name)
        else:
            name = basefont
        encoding = fontdict.dict_get(mupdf.PDF_ENUM_NAME_Encoding)
        if encoding.is_dict():
            encoding = encoding.dict_get(mupdf.PDF_ENUM_NAME_BaseEncoding)
        xref = fontdict.to_num()
        ext = "n/a"
        if xref:
            ext = JM_get_fontextension(pdf, xref)
        entry = (
                xref,
                ext,
                subtype.to_name(),
                JM_EscapeStrFromStr(name.to_name()),
                refname.to_name(),
                encoding.to_name(),
                stream_xref,
                )
        fontlist.append(entry)
    return rc


def JM_gather_forms(doc, dict_: mupdf.PdfObj, imagelist, stream_xref: int):
    '''
    Store info of a /Form xobject in Python list
    '''
    assert isinstance(doc, mupdf.PdfDocument)
    rc = 1
    n = mupdf.mpdf_dict_len(dict_);
    #pdf_obj *o = NULL, *m = NULL;
    for i in range(n):
        #pdf_obj *refname = NULL;

        refname = mupdf.mpdf_dict_get_key( dict_, i)
        imagedict = mupdf.mpdf_dict_get_val(dict_, i)
        if not mupdf.mpdf_is_dict(imagedict):
            mupdf.mfz_warn("'%s' is no form dict (%d 0 R)",
                    mupdf.mpdf_to_name(refname),
                    mupdf.mpdf_to_num(imagedict),
                    )
            continue

        type_ = mupdf.mpdf_dict_get(imagedict, PDF_NAME('Subtype'))
        if not mupdf.mpdf_name_eq(type_, PDF_NAME('Form')):
            continue

        o = mupdf.mpdf_dict_get(imagedict, PDF_NAME('BBox'))
        m = mupdf.mpdf_dict_get(imagedict, PDF_NAME('Matrix'))
        if m.m_internal:
            mat = mupdf.mpdf_to_matrix(m)
        else:
            mat = mupdf.Matrix()
        if o.m_internal:
            bbox = mupdf.mfz_transform_rect( mupdf.mpdf_to_rect(o), mat)
        else:
            bbox = mupdf.Rect(mupdf.Rect.Fixed_INFINITE)
        xref = mupdf.mpdf_to_num(imagedict)

        entry = (
                xref,
                mupdf.mpdf_to_name(ctx, refname),
                stream_xref,
                JM_py_from_rect(bbox),
                entry,
                )
        imagelist.append(entry)
    return rc


def JM_gather_images(doc: mupdf.PdfDocument, dict_: mupdf.PdfObj, imagelist, stream_xref: int):
    '''
    Store info of an image in Python list
    '''
    rc = 1;
    n = mupdf.mpdf_dict_len( dict_)
    for i in range(n):
        refname = mupdf.mpdf_dict_get_key(dict_, i)
        imagedict = mupdf.mpdf_dict_get_val(dict_, i)
        if not mupdf.mpdf_is_dict(imagedict):
            mupdf.mfz_warn("'%s' is no image dict (%d 0 R)",
                    mupdf.mpdf_to_name(refname),
                    mupdf.mpdf_to_num(imagedict),
                    )
            continue

        type_ = mupdf.mpdf_dict_get(imagedict, PDF_NAME('Subtype'))
        if not mupdf.mpdf_name_eq(type_, PDF_NAME('Image')):
            continue

        xref = mupdf.mpdf_to_num(imagedict)
        gen = 0
        smask = mupdf.mpdf_dict_geta(imagedict, PDF_NAME('SMask'), PDF_NAME('Mask'))
        if smask.m_internal:
            gen = mupdf.mpdf_to_num(smask)

        filter_ = mupdf.mpdf_dict_geta(imagedict, PDF_NAME('Filter'), PDF_NAME('F'))
        if mupdf.mpdf_is_array(filter_):
            filter_ = mupdf.mpdf_array_get(filter_, 0)

        altcs = mupdf.PdfObj(0)
        cs = mupdf.mpdf_dict_geta(imagedict, PDF_NAME('ColorSpace'), PDF_NAME('CS'))
        if mupdf.mpdf_is_array(cs):
            cses = cs
            cs = mupdf.mpdf_array_get(cses, 0)
            if (mupdf.mpdf_name_eq(cs, PDF_NAME('DeviceN'))
                    or mupdf.mpdf_name_eq(cs, PDF_NAME('Separation'))
                    ):
                altcs = mupdf.mpdf_array_get(cses, 2)
                if mupdf.mpdf_is_array(altcs):
                    altcs = mupdf.mpdf_array_get(altcs, 0)
        width = mupdf.mpdf_dict_geta(imagedict, PDF_NAME('Width'), PDF_NAME('W'))
        height = mupdf.mpdf_dict_geta(imagedict, PDF_NAME('Height'), PDF_NAME('H'))
        bpc = mupdf.mpdf_dict_geta(imagedict, PDF_NAME('BitsPerComponent'), PDF_NAME('BPC'))

        #jlib.log('{altcs=}')
        entry = (
                xref,
                gen,
                mupdf.mpdf_to_int(width),
                mupdf.mpdf_to_int(height),
                mupdf.mpdf_to_int(bpc),
                JM_EscapeStrFromStr(mupdf.mpdf_to_name(cs)),
                JM_EscapeStrFromStr(mupdf.mpdf_to_name(altcs)),
                JM_EscapeStrFromStr(mupdf.mpdf_to_name(refname)),
                JM_EscapeStrFromStr(mupdf.mpdf_to_name(filter_)),
                stream_xref,
                )
        imagelist.append(entry)
    return rc


def JM_get_annot_by_xref(page, xref):
    '''
    retrieve annot by its xref
    '''
    assert isinstance(page, mupdf.PdfPage)
    found = 0
    try:    # loop thru MuPDF's internal annots array
        annot = page.first_annot()
        while 1:
            if not annot.m_internal:
                break
            if xref == mupdf.mpdf_to_num(annot.annot_obj()):
                found = 1
                break
        if not found:
            raise Exception("xref %d is not an annot of this page" % xref)
    except Exception as e:
        #jlib.log('{e=}')
        raise
    return annot


def JM_get_annot_by_name(page, name):
    '''
    retrieve annot by name (/NM key)
    '''
    assert isinstance(page, mupdf.PdfPage)
    if not name:
        return
    #pdf_annot **annotptr = NULL;
    #pdf_annot *annot = NULL;
    found = 0

    try:    # loop thru MuPDF's internal annots and widget arrays
        annot = page.first_annot()
        while 1:
            if not annot.m_internal:
                break

            response, len_ = mupdf.mpdf_to_string(mupdf.mpdf_dict_gets(annot.annot_obj(), "NM"))
            if name == response:
                found = 1
                break
            annot = annot.next_annot()
        if not found:
            raise Exception("'%s' is not an annot of this page" % name)
    except Exception as e:
        #jlib.log('{e=}')
        raise
    return annot


def JM_get_annot_id_list(page):
    names = []
    annots = page.obj().dict_get( mupdf.PDF_ENUM_NAME_Annots)
    if not annots.m_internal:
        return names
    for i in range( annots.array_len()):
        annot_obj =annots.array_get(i)
        name = annot_obj.dict_gets("NM")
        if name.m_internal:
            names.append(
                name.to_text_string()
                )
    return names

def JM_get_annot_xref_list(page_or_page_obj):
    '''
    Wrapper for PyMuPDF/fitz/helper-annot.i:
        PyObject *JM_get_annot_xref_list(fz_context *ctx, pdf_obj *page_obj)

    Not PyMuPDF/fitz/fitz_wrap.c:
        PyObject *JM_get_annot_xref_list(fz_context *ctx, pdf_page *page)
    '''
    if 0 and isinstance(page_or_page_obj, mupdf.PdfObj):
        # Wrapper for PyMuPDF/fitz/helper-annot.i:
        #   PyObject *JM_get_annot_xref_list(fz_context *ctx, pdf_obj *page_obj)
        page_obj = page_or_page_obj
        names = []
        #jlib.log('{page_obj=}')
        #jlib.log('{mupdf.PDF_ENUM_NAME_Annots=}')
        #jlib.log('calling page_obj.dict_get()')
        annots = page_obj.dict_get(mupdf.PDF_ENUM_NAME_Annots)
        #jlib.log('{annots=}')
        if not annots:
            return names
        n = annots.array_len()
        #jlib.log('{n=}')
        for i in range(n):
            annot_obj = mupdf.ppdf_array_get(annots, i)
            xref = mupdf.ppdf_to_num(annot_obj)
            #jlib.log('{mupdf.PDF_ENUM_NAME_Subtype=}')
            subtype = mupdf.ppdf_dict_get(annot_obj, mupdf.PDF_ENUM_NAME_Subtype)
            type_ = mupdf.PDF_ANNOT_UNKNOWN
            if (subtype):
                name = mupdf.ppdf_to_name(subtype)
                type_ = mupdf.ppdf_annot_type_from_string(name)
            id_ = mupdf.ppdf_dict_gets(annot_obj, "NM")
            names.append( (xref, type_, mupdf.ppdf_to_text_string(id_)) )
        return names
    elif 0 and isinstance(page_or_page_obj, mupdf.PdfPage):
        # Not PyMuPDF/fitz/fitz_wrap.c:
        #   PyObject *JM_get_annot_xref_list(fz_context *ctx, pdf_page *page)
        page = page_or_page_obj
        names = []
        annots = page.obj().dict_get(mupdf.PDF_ENUM_NAME_Annots)
        if not annots.m_internal:
            return names
        n = annots.array_len()
        for i in range(n):
            annot_obj = annots.array_get(i)
            xref = annot_obj.to_num()
            subtype = annot_obj.dict_get(mupdf.PDF_ENUM_NAME_Subtype)
            type_ = mupdf.PDF_ANNOT_UNKNOWN
            if subtype.m_internal:
                name = subtype.to_name()
                type_ = name.annot_type_from_string()
            id_ = annot_obj.dict_gets("NM")
            names.append( (xref, type_, id_.to_text_string()))
        return names

    else:
        if isinstance(page_or_page_obj, mupdf.PdfPage):
            obj = page_or_page_obj.obj()
        elif isinstance(page_or_page_obj, mupdf.PdfObj):
            obj = page_or_page_obj
        else:
            assert 0
        names = []
        annots = obj.dict_get(mupdf.PDF_ENUM_NAME_Annots)
        if not annots.m_internal:
            return names
        n = annots.array_len()
        for i in range(n):
            annot_obj = annots.array_get(i)
            xref = annot_obj.to_num()
            subtype = annot_obj.dict_get(mupdf.PDF_ENUM_NAME_Subtype)
            type_ = mupdf.PDF_ANNOT_UNKNOWN
            if subtype.m_internal:
                name = subtype.to_name()
                type_ = mupdf.ppdf_annot_type_from_string(name)
            id_ = annot_obj.dict_gets("NM")
            names.append( (xref, type_, id_.to_text_string()))
        return names


def JM_get_border_style(style):
    '''
    return pdf_obj "border style" from Python str
    '''
    val = mupdf.PDF_ENUM_NAME_S
    if style is None:
        return val
    s = style
    if   s.startswith("b") or s.startswith("B"):    val = mupdf.PDF_ENUM_NAME_B
    elif s.startswith("d") or s.startswith("D"):    val = mupdf.PDF_ENUM_NAME_D
    elif s.startswith("i") or s.startswith("I"):    val = mupdf.PDF_ENUM_NAME_I
    elif s.startswith("u") or s.startswith("U"):    val = mupdf.PDF_ENUM_NAME_U
    return val


def JM_get_font(
        fontname,
        fontfile,
        fontbuffer,
        script,
        lang,
        ordering,
        is_bold,
        is_italic,
        is_serif,
        ):
    '''
    return a fz_font from a number of parameters
    '''
    index = 0
    font = None
    if fontfile:
        #goto have_file;
        font = mupdf.mfz_new_font_from_file(None, fontfile, index, 0)
        if not font.m_internal:
            THROWMSG(ctx, "could not create font")
        return font

    if fontbuffer:
        #goto have_buffer;
        res = JM_BufferFromBytes(fontbuffer)
        font = mupdf.mfz_new_font_from_buffer(None, res, index, 0)
        if not font.m_internal:
            THROWMSG(ctx, "could not create font");
        return font

    if ordering > -1:
        # goto have_cjk;
        data, size, index = mupdf.mfz_lookup_cjk_font(ordering);
        if data:
            font = mupdf.mfz_new_font_from_memory(None, data, size, index, 0);
        if not font.m_internal:
            THROWMSG(ctx, "could not create font");
        return font

    if fontname:
        # goto have_base14;
        data, size = mupdf.mfz_lookup_base14_font(fontname)
        if data:
            font = mupdf.mfz_new_font_from_memory(fontname, data, size, 0, 0)
        if font.m_internal:
            return font

        data, size = mupdf.mfz_lookup_builtin_font(fontname, is_bold, is_italic)
        if data:
            font = mupdf.mfz_new_font_from_memory(fontname, data, size, 0, 0)
        if not font.m_internal:
            THROWMSG(ctx, "could not create font");
        return font


def JM_get_resource_properties(ref):
    '''
    Return the items of Resources/Properties (used for Marked Content)
    Argument may be e.g. a page object or a Form XObject
    '''
    properties = mupdf.mpdf_dict_getl(ref, PDF_NAME('Resources'), PDF_NAME('Properties'))
    if not properties.m_internal:
        return ()
    else:
        n = mupdf.mpdf_dict_len(properties)
        if n < 1:
            return ()
        rc = []
        for i in range(n):
            key = mupdf.mpdf_dict_get_key(properties, i)
            val = mupdf.mpdf_dict_get_val(properties, i)
            c = mupdf.mpdf_to_name(key)
            xref = mupdf.mpdf_to_num(val)
            rc.append((c, xref))
    return rc


def JM_get_widget_properties(annot, Widget):
    '''
    Populate a Python Widget object with the values from a PDF form field.
    Called by "Page.firstWidget" and "Widget.next".
    '''
    #jlib.log('{Widget=} {type(Widget)=}')
    annot_obj = mupdf.mpdf_annot_obj(annot)
    page = mupdf.mpdf_annot_page(annot)
    pdf = page.doc()
    tw = annot

    def SETATTR(key, value):
        setattr(Widget, key, value)

    def SETATTR_DROP(mod, key, value):
        # Original C code for this function deletes if PyObject* is NULL. We
        # don't have a representation for that in Python - e.g. None is not
        # represented by NULL.
        if 0 and value is None:
            #jlib.log('deleting {mod=} {key=} {value=}')
            try:
                delattr(mod, key)
            except Exception:
                pass
        else:
            setattr(mod, key, value)

    field_type = mupdf.mpdf_widget_type(tw)
    #Widget["field_type"] = field_type
    Widget.field_type = field_type
    if field_type == PDF_WIDGET_TYPE_SIGNATURE:
        if mupdf.mpdf_signature_is_signed(pdf, annot_obj):
            SETATTR("is_signed", True)
        else:
            SETATTR("is_signed",False)
    else:
        SETATTR("is_signed", None)
    SETATTR_DROP(Widget, "border_style", JM_UnicodeFromStr(mupdf.mpdf_field_border_style(annot_obj)))
    SETATTR_DROP(Widget, "field_type_string", JM_UnicodeFromStr(JM_field_type_text(field_type)))

    field_name = mupdf.mpdf_field_name(annot_obj)
    SETATTR_DROP(Widget, "field_name", field_name)

    obj = mupdf.mpdf_dict_get(annot_obj, PDF_NAME('TU'))
    if obj.m_internal:
        label = mupdf.mpdf_to_text_string(obj)
    SETATTR_DROP(Widget, "field_label", label)

    SETATTR_DROP(Widget, "field_value", mupdf.mpdf_field_value(annot_obj))

    SETATTR_DROP(Widget, "field_display", mupdf.mpdf_field_display(annot_obj))

    border_width = mupdf.mpdf_to_real(mupdf.mpdf_dict_getl(annot_obj, PDF_NAME('BS'), PDF_NAME('W')))
    if border_width == 0:
        border_width = 1
    SETATTR_DROP(Widget, "border_width", border_width)

    obj = mupdf.mpdf_dict_getl(annot_obj, PDF_NAME('BS'), PDF_NAME('D'))
    if mupdf.mpdf_is_array(obj):
        n = mupdf.mpdf_array_len(obj)
        d = [0] * n
        for i in range(n):
            d[i] = mupdf.mpdf_to_int(mupdf.mpdf_array_get(obj, i))
        SETATTR_DROP(Widget, "border_dashes", d)

    SETATTR_DROP(Widget, "text_maxlen", mupdf.mpdf_text_widget_max_len(tw))

    SETATTR_DROP(Widget, "text_format", mupdf.mpdf_text_widget_format(tw))

    obj = mupdf.mpdf_dict_getl(annot_obj, PDF_NAME('MK'), PDF_NAME('BG'))
    if mupdf.mpdf_is_array(obj):
        n = mupdf.mpdf_array_len(obj)
        col = [0] * n
        for i in range(n):
            col[i] = mupdf.mpdf_to_real(mupdf.mpdf_array_get(obj, i))
        SETATTR_DROP(Widget, "fill_color", col)

    obj = mupdf.mpdf_dict_getl(annot_obj, PDF_NAME('MK'), PDF_NAME('BC'))
    if mupdf.mpdf_is_array(obj):
        n = mupdf.mpdf_array_len(obj)
        col = [0] * n
        for i in range(n):
            col[i] = mupdf.mpdf_to_real(mupdf.mpdf_array_get(obj, i))
        SETATTR_DROP(Widget, "border_color", col)

    SETATTR_DROP(Widget, "choice_values", JM_choice_options(annot))

    da = mupdf.mpdf_to_text_string(mupdf.mpdf_dict_get_inheritable(annot_obj, PDF_NAME('DA')))
    SETATTR_DROP(Widget, "_text_da", JM_UnicodeFromStr(da))

    obj = mupdf.mpdf_dict_getl(annot_obj, PDF_NAME('MK'), PDF_NAME('CA'))
    if obj.m_internal:
        SETATTR_DROP(Widget, "button_caption", JM_UnicodeFromStr(mupdf.mpdf_to_text_string(obj)))

    SETATTR_DROP(Widget, "field_flags", mupdf.mpdf_field_flags(annot_obj))

    # call Py method to reconstruct text color, font name, size
    #call = CALLATTR("_parse_da", NULL)
    call = Widget._parse_da()

    # extract JavaScript action texts
    s = mupdf.mpdf_dict_get(annot_obj, PDF_NAME('A'))
    ss = JM_get_script(s)
    SETATTR_DROP(Widget, "script", ss)

    SETATTR_DROP(Widget, "script_stroke",
            JM_get_script(mupdf.mpdf_dict_getl(annot_obj, PDF_NAME('AA'), PDF_NAME('K')))
            )

    SETATTR_DROP(Widget, "script_format",
            JM_get_script(mupdf.mpdf_dict_getl(annot_obj, PDF_NAME('AA'), PDF_NAME('F')))
            )

    SETATTR_DROP(Widget, "script_change",
            JM_get_script(mupdf.mpdf_dict_getl(annot_obj, PDF_NAME('AA'), PDF_NAME('V')))
            )

    SETATTR_DROP(Widget, "script_calc",
        JM_get_script(mupdf.mpdf_dict_getl(annot_obj, PDF_NAME('AA'), PDF_NAME('C')))
        )


def JM_get_fontextension(doc, xref):
    '''
    Return the file extension of a font file, identified by xref
    '''
    if xref < 1:
        return "n/a"
    o = mupdf.mpdf_load_object(doc, xref)
    desft = mupdf.mpdf_dict_get(o, PDF_NAME('DescendantFonts'))
    if desft.m_internal:
        obj = mupdf.mpdf_resolve_indirect(mupdf.mpdf_array_get(desft, 0))
        obj = mupdf.mpdf_dict_get(obj, PDF_NAME('FontDescriptor'))
    else:
        obj = mupdf.mpdf_dict_get(o, PDF_NAME('FontDescriptor'))
    if not obj.m_internal:
        return "n/a"    # this is a base-14 font

    o = obj # we have the FontDescriptor

    obj = mupdf.mpdf_dict_get(o, PDF_NAME('FontFile'))
    if obj.m_internal:
        return "pfa"

    obj = mupdf.mpdf_dict_get(o, PDF_NAME('FontFile2'))
    if obj.m_internal:
        return "ttf"

    obj = mupdf.mpdf_dict_get(o, PDF_NAME('FontFile3'))
    if obj.m_internal:
        obj = mupdf.mpdf_dict_get(obj, PDF_NAME('Subtype'))
        if obj.m_internal and not mupdf.mpdf_is_name(obj):
            PySys_WriteStdout("invalid font descriptor subtype")
            return "n/a"
        if mupdf.mpdf_name_eq(obj, PDF_NAME('Type1C')):
            return "cff"
        elif mupdf.mpdf_name_eq(obj, PDF_NAME('CIDFontType0C')):
            return "cid"
        elif mupdf.mpdf_name_eq(obj, PDF_NAME('OpenType')):
            return "otf"
        else:
            PySys_WriteStdout("unhandled font type '%s'", mupdf.mpdf_to_name(obj))

    return "n/a"


def JM_get_page_labels(liste, nums):
    n = mupdf.mpdf_array_len(nums)
    for i in range(0, n, 2):
        key = mupdf.mpdf_resolve_indirect( mupdf.mpdf_array_get(nums, i))
        pno = mupdf.mpdf_to_int(key)
        val = mupdf.mpdf_resolve_indirect( mupdf.mpdf_array_get(nums, i + 1))
        res = JM_object_to_buffer(val, 1, 0)
        c = res.buffer_extract()
        assert isinstance(c, bytes)
        c = c.decode('utf-8')
        liste.append( (pno, c))


def JM_get_script(key):
    '''
    JavaScript extractor
    Returns either the script source or None. Parameter is a PDF action
    dictionary, which must have keys /S and /JS. The value of /S must be
    '/JavaScript'. The value of /JS is returned.
    '''
    if not key.m_internal:
        return

    j = mupdf.mpdf_dict_get(key, PDF_NAME('S'))
    jj = mupdf.mpdf_to_name(j)
    if jj == "JavaScript":
        js = mupdf.mpdf_dict_get(key, PDF_NAME('JS'))
        if not js.m_internal:
            return
    else:
        return

    if mupdf.mpdf_is_string(js):
        script = JM_UnicodeFromStr(mupdf.mpdf_to_text_string(js))
    elif mupdf.mpdf_is_stream(js):
        res = mupdf.mpdf_load_stream(js)
        script = JM_EscapeStrFromBuffer(res)
    else:
        return
    if script:  # do not return an empty script
        return script
    return


def JM_have_operation(pdf):
    '''
    Ensure valid journalling state
    '''
    if pdf.m_internal.journal and not mupdf.mpdf_undoredo_step(pdf, 0):
        return 0
    return 1;

def JM_image_extension(type_):
    '''
    return extension for fitz image type
    '''
    if type_ == mupdf.FZ_IMAGE_RAW:     return "raw"
    if type_ == mupdf.FZ_IMAGE_FLATE:   return "flate"
    if type_ == mupdf.FZ_IMAGE_LZW:     return "lzw"
    if type_ == mupdf.FZ_IMAGE_RLD:     return "rld"
    if type_ == mupdf.FZ_IMAGE_BMP:     return "bmp"
    if type_ == mupdf.FZ_IMAGE_GIF:     return "gif"
    if type_ == mupdf.FZ_IMAGE_JBIG2:   return "jb2"
    if type_ == mupdf.FZ_IMAGE_JPEG:    return "jpeg"
    if type_ == mupdf.FZ_IMAGE_JPX:     return "jpx"
    if type_ == mupdf.FZ_IMAGE_JXR:     return "jxr"
    if type_ == mupdf.FZ_IMAGE_PNG:     return "png"
    if type_ == mupdf.FZ_IMAGE_PNM:     return "pnm"
    if type_ == mupdf.FZ_IMAGE_TIFF:    return "tiff"
    return "n/a"


def JM_image_reporter(page):
    assert 0, 'not implemented - needs swig director support for fn ptrs.'
    doc = page.doc()
    filter_ = mupdf.PdfFilterOptions()
    '''
    filter_.opaque = page
    filter_.text_filter = NULL;
    filter_.image_filter = JM_image_filter;
    filter_.end_page = NULL;
    filter_.recurse = 0;
    filter_.instance_forms = 1;
    filter_.sanitize = 1;
    filter_.ascii = 1;

    pdf_obj *contents, *old_res;
    pdf_obj *struct_parents_obj;
    pdf_obj *new_res;
    fz_buffer *buffer;
    int struct_parents;
    fz_matrix ctm = fz_identity;
    pdf_page_transform(gctx, page, NULL, &ctm);
    struct_parents_obj = pdf_dict_get(ctx, page->obj, PDF_NAME(StructParents));
    struct_parents = -1;
    if (pdf_is_number(ctx, struct_parents_obj))
        struct_parents = pdf_to_int(ctx, struct_parents_obj);

    contents = pdf_page_contents(ctx, page);
    old_res = pdf_page_resources(ctx, page);
    img_info = PyList_New(0);
    JM_filter_content_stream(ctx, doc, contents, old_res, ctm, &filter, struct_parents, &buffer, &new_res);
    fz_drop_buffer(ctx, buffer);
    pdf_drop_obj(ctx, new_res);
    PyObject *rc = PySequence_Tuple(img_info);
    Py_CLEAR(img_info);
    return rc;
    '''


def JM_insert_contents(pdf, pageref, newcont, overlay):
    '''
    Insert a buffer as a new separate /Contents object of a page.
    1. Create a new stream object from buffer 'newcont'
    2. If /Contents already is an array, then just prepend or append this object
    3. Else, create new array and put old content obj and this object into it.
       If the page had no /Contents before, just create a 1-item array.
    '''
    contents = mupdf.mpdf_dict_get(pageref, PDF_NAME('Contents'))
    newconts = mupdf.mpdf_add_stream(pdf, newcont, mupdf.PdfObj(), 0)
    xref = mupdf.mpdf_to_num(newconts)
    if mupdf.mpdf_is_array(contents):
        if overlay:  # append new object
            mupdf.mpdf_array_push(contents, newconts)
        else:   # prepend new object
            mupdf.mpdf_array_insert(contents, newconts, 0)
    else:
        carr = mupdf.mpdf_new_array(pdf, 5)
        if overlay:
            if contents.m_internal:
                mupdf.mpdf_array_push(carr, contents)
            mupdf.mpdf_array_push(carr, newconts)
        else:
            #mupdf.mpdf_array_push_drop(carr, newconts)
            mupdf.mpdf_array_push(carr, newconts)
            if contents.m_internal:
                mupdf.mpdf_array_push(carr, contents)
        mupdf.mpdf_dict_put(pageref, PDF_NAME('Contents'), carr)
    return xref


def JM_insert_font(pdf, bfname, fontfile, fontbuffer, set_simple, idx, wmode, serif, encoding, ordering):
    '''
    Insert a font in a PDF
    '''
    #jlib.log('{bfname=} {fontfile=} {fontbuffer=} {set_simple=} {idx=} {wmode=} {serif=} {encoding=} {ordering=}')
    font = None
    res = None
    data = None
    ixref = 0
    index = 0
    simple = 0
    value=None
    name=None
    subt=None
    exto = None

    ENSURE_OPERATION(pdf);
    # check for CJK font
    if ordering > -1:
        data, size, index = mupdf.mfz_lookup_cjk_font(ordering)
    if data:
        font = mupdf.mfz_new_font_from_memory(None, data, size, index, 0)
        font_obj = mupdf.mpdf_add_cjk_font(pdf, font, ordering, wmode, serif)
        exto = "n/a"
        simple = 0;
        #goto weiter;
    else:

        # check for PDF Base-14 font
        if bfname:
            data, size = mupdf.mfz_lookup_base14_font(bfname)
        if data:
            font = mupdf.mfz_new_font_from_memory(bfname, data, size, 0, 0)
            font_obj = mupdf.mpdf_add_simple_font(pdf, font, encoding)
            exto = "n/a"
            simple = 1
            #goto weiter;

        else:
            if fontfile:
                font = mupdf.mfz_new_font_from_file(None, fontfile, idx, 0)
            else:
                res = JM_BufferFromBytes(fontbuffer)
                if not res.m_internal:
                    THROWMSG("need one of fontfile, fontbuffer")
                font = mupdf.mfz_new_font_from_buffer(None, res, idx, 0)

            if not set_simple:
                font_obj = mupdf.mpdf_add_cid_font(pdf, font)
                simple = 0
            else:
                font_obj = mupdf.mpdf_add_simple_font(pdf, font, encoding)
                simple = 2

    #weiter: ;
    ixref = mupdf.mpdf_to_num(font_obj)
    name = JM_EscapeStrFromStr( mupdf.mpdf_to_name( mupdf.mpdf_dict_get(font_obj, PDF_NAME('BaseFont'))))

    subt = JM_UnicodeFromStr( mupdf.mpdf_to_name( mupdf.mpdf_dict_get( font_obj, PDF_NAME('Subtype'))))

    if not exto:
        exto = JM_UnicodeFromStr(JM_get_fontextension(pdf, ixref))

    asc = mupdf.mfz_font_ascender(font)
    dsc = mupdf.mfz_font_descender(font)
    value = [
            ixref,
            {
                "name": name,        # base font name
                "type": subt,        # subtype
                "ext": exto,         # file extension
                "simple": bool(simple), # simple font?
                "ordering": ordering, # CJK font?
                "ascender": asc,
                "descender": dsc,
            },
            ]
    return value


def JM_is_jbig2_image(dict_):
	return 0
	#filter_ = pdf_dict_get(ctx, dict_, PDF_NAME(Filter));
	#if (pdf_name_eq(ctx, filter_, PDF_NAME(JBIG2Decode)))
	#	return 1;
	#n = pdf_array_len(ctx, filter_);
	#for (i = 0; i < n; i++)
	#	if (pdf_name_eq(ctx, pdf_array_get(ctx, filter_, i), PDF_NAME(JBIG2Decode)))
	#		return 1;
	#return 0;


def JM_make_annot_DA(annot, ncol, col, fontname, fontsize):
    buf = mupdf.Buffer(50)
    if ncol == 1:
        buf.append_string(f'{col[0]} g ');
    elif ncol == 3:
        buf.append_string(f'{col[0]} {col[1]} {col[2]} rg ')
    else:
        buf.append_string(f'{col[0]} {col[1]} {col[2]} {col[3]} k ')

    buf.append_string(f'/{JM_expand_fname(fontname)} {fontsize} Tf')
    len_, da = buf.buffer_storage_raw()
    buf_bytes = mupdf.raw_to_python_bytes(da, len_)
    buf_string = buf_bytes.decode('utf-8')
    annot.annot_obj().dict_put_text_string(mupdf.PDF_ENUM_NAME_DA, buf_string)


def JM_make_spanlist(line_dict, line, raw, buff, tp_rect):
    char_list = None
    span_list = []
    mupdf.mfz_clear_buffer(buff)
    line_rect = mupdf.Rect(mupdf.Rect.Fixed_EMPTY)

    class char_style:
        pass
    old_style = char_style()
    old_style.size = -1
    old_style.flags = -1
    old_style.font = ''
    old_style.color = -1
    old_style.asc = 0
    old_style.desc = 0

    style = char_style()

    for ch in line:
        # start-trace
        r = JM_char_bbox(line, ch)
        if (not mupdf.mfz_contains_rect(tp_rect, r)
                and not mupdf.mfz_is_infinite_rect(tp_rect)
                ):
            continue

        flags = JM_char_font_flags(mupdf.Font(mupdf.keep_font(ch.m_internal.font)), line, ch)
        origin = mupdf.Point(ch.m_internal.origin)
        style.size = ch.m_internal.size
        style.flags = flags
        style.font = JM_font_name(mupdf.Font(mupdf.keep_font(ch.m_internal.font)))
        style.asc = JM_font_ascender(mupdf.Font(mupdf.keep_font(ch.m_internal.font)))
        style.desc = JM_font_descender(mupdf.Font(mupdf.keep_font(ch.m_internal.font)))
        style.color = ch.m_internal.color

        if (style.size != old_style.size
                or style.flags != old_style.flags
                or style.color != old_style.color
                or style.font != old_style.font
                ):
            # style changed -> make new span

            if old_style.size >= 0:
                # not first one, output previous
                if raw:
                    # put character list in the span
                    span[dictkey_chars] = char_list
                    char_list = None
                else:
                    # put text string in the span
                    span[dictkey_text] = JM_EscapeStrFromBuffer( buff)
                    mupdf.mfz_clear_buffer(buff)

                span[dictkey_origin] = JM_py_from_point(span_origin)
                span[dictkey_bbox] = JM_py_from_rect(span_rect)
                line_rect = mupdf.mfz_union_rect(line_rect, span_rect)
                if not mupdf.mfz_is_empty_rect(span_rect):
                    span_list.append(span)
                span = None

            span = dict()
            asc = style.asc
            desc = style.desc
            if style.asc < 1e-3:
                asc = 0.9;
                desc = -0.1

            span[dictkey_size] = style.size
            span[dictkey_flags] = style.flags
            span[dictkey_font] = JM_EscapeStrFromStr(style.font)
            span[dictkey_color] = style.color
            span["ascender"] = asc
            span["descender"] = desc

            old_style = style
            span_rect = r
            span_origin = origin

        span_rect = mupdf.mfz_union_rect(span_rect, r)
        if origin.y > span_origin.y:
            span_origin.y = origin.y

        if raw: # make and append a char dict
            char_dict = dict()

            char_dict[dictkey_origin] = ch.m_internal.origin
            char_dict[dictkey_bbox] = JM_py_from_rect(r)

            char_dict[dictkey_c] = ch.m_internal.c

            if char_list is None:
                char_list = []
            char_list.append(char_dict)
        else:   # add character byte to buffer
            JM_append_rune(buff, ch.m_internal.c)

    # all characters processed, now flush remaining span
    if span:
        if raw:
            span[dictkey_chars] = char_list
            char_list = None
        else:
            span[dictkey_text] = JM_EscapeStrFromBuffer(buff)
            mupdf.mfz_clear_buffer(buff)
        span[dictkey_origin] = JM_py_from_point(span_origin)
        span[dictkey_bbox] = JM_py_from_rect(span_rect)

        if not mupdf.mfz_is_empty_rect(span_rect):
            span_list.append(span)
            line_rect = mupdf.mfz_union_rect(line_rect, span_rect)
        span = None
    if not mupdf.mfz_is_empty_rect(line_rect):
        line_dict[dictkey_spans] = span_list
    else:
        line_dict[dictkey_spans] = span_list
    # stop-trace
    return line_rect


def JM_make_text_block(block, block_dict, raw, buff, tp_rect):
    line_list = []
    block_rect = mupdf.Rect(mupdf.Rect.Fixed_EMPTY)
    for line in block:
        if (mupdf.mfz_is_empty_rect(mupdf.mfz_intersect_rect(tp_rect, mupdf.Rect(line.m_internal.bbox)))
                and not mupdf.mfz_is_infinite_rect(tp_rect)
                ):
            continue
        line_dict = dict()
        line_rect = JM_make_spanlist(line_dict, line, raw, buff, tp_rect)
        block_rect = mupdf.mfz_union_rect(block_rect, line_rect)
        line_dict[dictkey_wmode] = line.m_internal.wmode
        line_dict[dictkey_dir] = JM_py_from_point(line.m_internal.dir)
        line_dict[dictkey_bbox] = JM_py_from_rect(line_rect)
        line_list.append(line_dict)
    block_dict[dictkey_bbox] = block_rect
    block_dict[dictkey_lines] = line_list


def JM_make_textpage_dict(tp, page_dict, raw):
    text_buffer = mupdf.mfz_new_buffer(128)
    block_list = []
    tp_rect = mupdf.Rect(tp.m_internal.mediabox)
    block_n = -1
    for block in tp:
        block_n += 1
        if (not mupdf.mfz_contains_rect(tp_rect, mupdf.Rect(block.m_internal.bbox))
                and not mupdf.mfz_is_infinite_rect(tp_rect)
                and block.m_internal.type == mupdf.FZ_STEXT_BLOCK_IMAGE
                ):
            continue
        if (not mupdf.mfz_is_infinite_rect(tp_rect)
                and mupdf.mfz_is_empty_rect(mupdf.mfz_intersect_rect(tp_rect, mupdf.Rect(block.m_internal.bbox)))
                ):
            continue

        block_dict = dict()
        block_dict[dictkey_number] = block_n
        block_dict[dictkey_type] = block.m_internal.type
        if block.m_internal.type == mupdf.FZ_STEXT_BLOCK_IMAGE:
            block_dict[dictkey_bbox] = JM_py_from_rect(block.m_internal.bbox)
            JM_make_image_block(block, block_dict)
        else:
            JM_make_text_block(block, block_dict, raw, text_buffer, tp_rect)

        block_list.append(block_dict)
    page_dict[dictkey_blocks] = block_list


def JM_matrix_from_py(m):
    a = [0, 0, 0, 0, 0, 0]
    if isinstance(m, mupdf.Matrix):
        return m
    if isinstance(m, Matrix):
        return mupdf.Matrix(m.a, m.b, m.c, m.d, m.e, m.f)
    if not m or not PySequence_Check(m) or PySequence_Size(m) != 6:
        return mupdf.Matrix()
    for i in range(6):
        a[i] = JM_FLOAT_ITEM(m, i)
        if a[i] is None:
            return mupdf.Rect()
    return mupdf.Matrix(a[0], a[1], a[2], a[3], a[4], a[5])


def JM_mediabox(page_obj):
    '''
    return a PDF page's MediaBox
    '''
    page_mediabox = mupdf.Rect(mupdf.Rect.Fixed_UNIT)
    #fz_rect mediabox, page_mediabox;
    mediabox = mupdf.mpdf_to_rect(
            mupdf.mpdf_dict_get_inheritable(page_obj, PDF_NAME('MediaBox'))
            );
    if mupdf.mfz_is_empty_rect(mediabox) or mupdf.mfz_is_infinite_rect(mediabox):
        mediabox.x0 = 0
        mediabox.y0 = 0
        mediabox.x1 = 612
        mediabox.y1 = 792

    page_mediabox = mupdf.Rect(
            mupdf.mfz_min(mediabox.x0, mediabox.x1),
            mupdf.mfz_min(mediabox.y0, mediabox.y1),
            mupdf.mfz_max(mediabox.x0, mediabox.x1),
            mupdf.mfz_max(mediabox.y0, mediabox.y1),
            )

    if (page_mediabox.x1 - page_mediabox.x0 < 1
            or page_mediabox.y1 - page_mediabox.y0 < 1
            ):
        page_mediabox = mupdf.Rect(mupdf.Rect.Fixed_UNIT)

    return page_mediabox

def JM_merge_range(
        doc_des,
        doc_src,
        spage,
        epage,
        apage,
        rotate,
        links,
        annots,
        show_progress,
        graft_map,
        ):
    '''
    Copy a range of pages (spage, epage) from a source PDF to a specified
    location (apage) of the target PDF.
    If spage > epage, the sequence of source pages is reversed.
    '''
    afterpage = apage;
    counter = 0;  # copied pages counter
    total = mupdf.mfz_absi(epage - spage) + 1   # total pages to copy

    if spage < epage:
        page = spage
        while page <= epage:
            page_merge(doc_des, doc_src, page, afterpage, rotate, links, annots, graft_map)
            counter += 1
            if show_progress > 0 and counter % show_progress == 0:
                sys.stdout.write("Inserted %i of %i pages.\n", counter, total)
            page += 1
            afterpage += 1
    else:
        page = spage
        while page >= epage:
            page_merge(doc_des, doc_src, page, afterpage, rotate, links, annots, graft_map)
            counter += 1
            if show_progress > 0 and counter % show_progress == 0:
                sys.stdout.write("Inserted %i of %i pages.\n", counter, total)
            page -= 1
            afterpage += 1


def JM_merge_resources( page, temp_res):
    '''
    Merge the /Resources object created by a text pdf device into the page.
    The device may have created multiple /ExtGState/Alp? and /Font/F? objects.
    These need to be renamed (renumbered) to not overwrite existing page
    objects from previous executions.
    Returns the next available numbers n, m for objects /Alp<n>, /F<m>.
    '''
    # page objects /Resources, /Resources/ExtGState, /Resources/Font
    resources = mupdf.mpdf_dict_get(page.obj(), PDF_NAME('Resources'))
    main_extg = mupdf.mpdf_dict_get(resources, PDF_NAME('ExtGState'))
    main_fonts = mupdf.mpdf_dict_get(resources, PDF_NAME('Font'))

    # text pdf device objects /ExtGState, /Font
    temp_extg = mupdf.mpdf_dict_get(temp_res, PDF_NAME('ExtGState'))
    temp_fonts = mupdf.mpdf_dict_get(temp_res, PDF_NAME('Font'))


    max_alp = -1
    max_fonts = -1
    #char text[20];

    # Handle /Alp objects
    if mupdf.mpdf_is_dict(temp_extg):   # any created at all?
        n = mupdf.mpdf_dict_len(temp_extg)
        if mupdf.mpdf_is_dict(main_extg):   # does page have /ExtGState yet?
            for i in range(mupdf.mpdf_dict_len(main_extg)):
                # get highest number of objects named /Alpxxx
                alp = mupdf.mpdf_to_name( mupdf.mpdf_dict_get_key(main_extg, i))
                if not alp.startswith('Alp'):
                    continue
                j = mupdf.mfz_atoi(alp[3:])
                if j > max_alp:
                    max_alp = j
        else:   # create a /ExtGState for the page
            main_extg = mupdf.mpdf_dict_put_dict(resources, PDF_NAME('ExtGState'), n)

        max_alp += 1
        for i in range(n):  # copy over renumbered /Alp objects
            alp = mupdf.mpdf_to_name( mupdf.mpdf_dict_get_key( temp_extg, i))
            j = mupdf.mfz_atoi(alp[3:]) + max_alp
            text = f'Alp{j}'
            val = mupdf.mpdf_dict_get_val( temp_extg, i)
            mupdf.mpdf_dict_puts(main_extg, text, val)


    if mupdf.mpdf_is_dict(main_fonts):  # has page any fonts yet?
        for i in range(mupdf.mpdf_dict_len(main_fonts)):    # get max font number
            font = mupdf.mpdf_to_name( mupdf.mpdf_dict_get_key( main_fonts, i))
            if not font.startswith("F"):
                continue
            j = mupdf.mfz_atoi(font[1:])
            if j > max_fonts:
                max_fonts = j
    else:   # create a Resources/Font for the page
        main_fonts = mupdf.mpdf_dict_put_dict(resources, PDF_NAME('Font'), 2)

    max_fonts += 1
    for i in range(mupdf.mpdf_dict_len(temp_fonts)):    # copy renumbered fonts
        font = mupdf.mpdf_to_name( mupdf.mpdf_dict_get_key( temp_fonts, i))
        j = mupdf.mfz_atoi(font[1:]) + max_fonts
        text = f'F{j}'
        val = mupdf.mpdf_dict_get_val(temp_fonts, i)
        mupdf.mpdf_dict_puts(main_fonts, text, val)
    return (max_alp, max_fonts) # next available numbers


def JM_mupdf_warning(user, message):
    '''
    redirect MuPDF warnings
    '''
    #jlib.log('{user=} {message=}')
    JM_mupdf_warnings_store.append(message)
    if JM_mupdf_show_warnings:
        sys.stderr.write(f'mupdf: {message}\n')


def JM_new_buffer_from_stext_page(page):
    '''
    make a buffer from an stext_page's text
    '''
    assert isinstance(page, mupdf.StextPage)
    #fz_stext_block *block;
    #fz_stext_line *line;
    #fz_stext_char *ch;
    rect = mupdf.Rect(page.m_internal.mediabox)
    buf = mupdf.mfz_new_buffer(256)
    for block in page:
        if block.m_internal.type == mupdf.FZ_STEXT_BLOCK_TEXT:
            for line in block:
                for ch in line:
                    if (not mupdf.mfz_contains_rect(rect, JM_char_bbox(line, ch))
                            and not mupdf.mfz_is_infinite_rect(rect)
                            ):
                        continue
                    mupdf.mfz_append_rune(buf, ch.m_internal.c)
                mupdf.mfz_append_byte(buf, ord('\n'))
            mupdf.mfz_append_byte(buf, ord('\n'))
    return buf


def JM_new_output_fileptr(bio):
    out = mupdf.mfz_new_output(0, bio, JM_bytesio_write, None, None)
    out.seek = JM_bytesio_seek
    out.tell = JM_bytesio_tell
    out.truncate = JM_bytesio_truncate
    return out


def JM_new_tracedraw_device(out):
    assert 0, 'derived devices not yet supported'
    dev = mupdf.mfz_new_derived_device(jm_tracedraw_device)

    dev.m_internal.fill_path = jm_tracedraw_fill_path
    dev.m_internal.stroke_path = jm_tracedraw_stroke_path
    dev.m_internal.clip_path = 0
    dev.m_internal.clip_stroke_path = 0

    dev.m_internal.fill_text = jm_increase_seqno
    dev.m_internal.stroke_text = jm_increase_seqno
    dev.m_internal.clip_text = 0
    dev.m_internal.clip_stroke_text = 0
    dev.m_internal.ignore_text = jm_increase_seqno

    dev.m_internal.fill_shade = jm_increase_seqno
    dev.m_internal.fill_image = jm_increase_seqno
    dev.m_internal.fill_image_mask = jm_increase_seqno
    dev.m_internal.clip_image_mask = 0

    dev.m_internal.pop_clip = 0

    dev.m_internal.begin_mask = 0
    dev.m_internal.end_mask = 0
    dev.m_internal.begin_group = 0
    dev.m_internal.end_group = 0

    dev.m_internal.begin_tile = 0
    dev.m_internal.end_tile = 0

    dev.m_internal.begin_layer = 0
    dev.m_internal.end_layer = 0

    dev.m_internal.render_flags = 0
    dev.m_internal.set_default_colorspaces = 0

    dev.out = out
    dev.seqno = 0
    return dev

def JM_new_tracetext_device(out):
    assert 0, 'derived devices not yet supported'
    dev = mupdf.mfz_new_derived_device(jm_tracedraw_device)

    dev.m_internal.fill_path = jm_increase_seqno
    dev.m_internal.stroke_path = jm_trace_device_Linewidth
    dev.m_internal.clip_path = 0
    dev.m_internal.clip_stroke_path = 0

    dev.m_internal.fill_text = jm_tracedraw_fill_text
    dev.m_internal.stroke_text = jm_tracedraw_stroke_text
    dev.m_internal.clip_text = 0
    dev.m_internal.clip_stroke_text = 0
    dev.m_internal.ignore_text = jm_tracedraw_ignore_text

    dev.m_internal.fill_shade = jm_increase_seqno
    dev.m_internal.fill_image = jm_increase_seqno
    dev.m_internal.fill_image_mask = jm_increase_seqno
    dev.m_internal.clip_image_mask = 0

    dev.m_internal.pop_clip = 0

    dev.m_internal.begin_mask = 0
    dev.m_internal.end_mask = 0
    dev.m_internal.begin_group = 0
    dev.m_internal.end_group = 0

    dev.m_internal.begin_tile = 0
    dev.m_internal.end_tile = 0

    dev.m_internal.begin_layer = 0
    dev.m_internal.end_layer = 0

    dev.m_internal.render_flags = 0
    dev.m_internal.set_default_colorspaces = 0

    dev.out = out
    dev.seqno = 0
    return dev


def JM_norm_rotation(rotate):
    '''
    # return normalized /Rotate value
    '''
    while rotate < 0:
        rotate += 360
    while rotate >= 360:
        rotate -= 360
    if rotate % 90 != 0:
        return 0
    return rotate

def JM_object_to_buffer(what, compress, ascii):
    res = mupdf.mfz_new_buffer(512)
    out = mupdf.Output(res)
    #jlib.log('{out=}')
    mupdf.mpdf_print_obj(out, what, compress, ascii)
    mupdf.mfz_terminate_buffer(res)
    return res

def JM_outline_xrefs(obj, xrefs):
    '''
    Return list of outline xref numbers. Recursive function. Arguments:
    'obj' first OL item
    'xrefs' empty Python list
    '''
    if not obj.m_internal:
        return xrefs
    thisobj = obj
    while thisobj.m_internal:
        xrefs.append( mupdf.mpdf_to_num(thisobj))
        first = mupdf.mpdf_dict_get(thisobj, PDF_NAME('First')) # try go down
        if first.m_internal:
            xrefs = JM_outline_xrefs(first, xrefs)
        thisobj = mupdf.mpdf_dict_get(thisobj, PDF_NAME('Next'))    # try go next
        parent = mupdf.mpdf_dict_get(thisobj, PDF_NAME('Parent'))   # get parent
        if not thisobj.m_internal:
            thisobj = parent    # goto parent if no next
    return xrefs


def JM_page_rotation(page):
    '''
    return a PDF page's /Rotate value: one of (0, 90, 180, 270)
    '''
    rotate = 0

    obj = page.obj().dict_get_inheritable( mupdf.PDF_ENUM_NAME_Rotate)
    rotate = obj.to_int()
    rotate = JM_norm_rotation(rotate)
    return rotate


def JM_pdf_obj_from_str(doc, src):
    '''
    create PDF object from given string (new in v1.14.0: MuPDF dropped it)
    '''
    # fixme: seems inefficient to convert to bytes instance then make another
    # copy inside fz_new_buffer_from_copied_data(), but no other way?
    #
    buffer_ = mupdf.Buffer.new_buffer_from_copied_data(bytes(src, 'utf8'))
    stream = buffer_.open_buffer()
    lexbuf = mupdf.PdfLexbuf(mupdf.PDF_LEXBUF_SMALL)
    result = mupdf.mpdf_parse_stm_obj(doc, stream, lexbuf)
    return result;

def JM_pixmap_from_display_list(
        list_,
        ctm,
        cs,
        alpha,
        clip,
        seps,
        ):
    '''
    Version of fz_new_pixmap_from_display_list (util.c) to also support
    rendering of only the 'clip' part of the displaylist rectangle
    '''
    assert isinstance(list_, mupdf.DisplayList)
    if seps is None:
        seps = mupdf.Separations()
    assert seps is None or isinstance(seps, mupdf.Separations), f'type={type(seps)}: {seps}'

    rect = mupdf.mfz_bound_display_list(list_)
    matrix = JM_matrix_from_py(ctm)
    #fz_pixmap *pix = NULL;
    #fz_var(pix);
    #fz_device *dev = NULL;
    #fz_var(dev);
    rclip = JM_rect_from_py(clip)
    rect = mupdf.mfz_intersect_rect(rect, rclip)    # no-op if clip is not given

    rect = mupdf.mfz_transform_rect(rect, matrix)
    irect = mupdf.mfz_round_rect(rect)

    pix = mupdf.mfz_new_pixmap_with_bbox(cs, irect, seps, alpha)
    if alpha:
        mupdf.mfz_clear_pixmap(pix)
    else:
        mupdf.mfz_clear_pixmap_with_value(pix, 0xFF)

    if not mupdf.mfz_is_infinite_rect(rclip):
        dev = mupdf.mfz_new_draw_device_with_bbox(matrix, pix, irect)
        mupdf.mfz_run_display_list(list_, dev, fz_identity, rclip, None)
    else:
        dev = mupdf.mfz_new_draw_device(matrix, pix)
        mupdf.mfz_run_display_list(list_, dev, mupdf.Matrix(), mupdf.Rect(mupdf.Rect.Fixed_INFINITE), mupdf.Cookie())

    mupdf.mfz_close_device(dev)
    return Pixmap(pix)


def JM_point_from_py(p):
    '''
    PySequence to fz_point. Default: (FZ_MIN_INF_RECT, FZ_MIN_INF_RECT)
    '''
    #jlib.log('{p=} {type(p)=}')
    if isinstance(p, mupdf.Point):
        return p
    if isinstance(p, Point):
        return mupdf.Point(p.x, p.y)
    p0 = mupdf.Point(0, 0)
    x = JM_FLOAT_ITEM(p, 0)
    y = JM_FLOAT_ITEM(p, 1)
    if x is None or y is None:
        return p0
    return mupdf.Point(x, y)


def JM_print_stext_page_as_text(out, page):
    '''
    Plain text output. An identical copy of fz_print_stext_page_as_text,
    but lines within a block are concatenated by space instead a new-line
    character (which else leads to 2 new-lines).
    '''
    #jlib.log('{type(out)=}')
    assert isinstance(out, mupdf.Output)
    assert isinstance(page, mupdf.StextPage)
    rect = mupdf.Rect(page.m_internal.mediabox)
    last_char = 0

    for block in page:
        if block.m_internal.type == mupdf.FZ_STEXT_BLOCK_TEXT:
            for line in block:
                last_char = 0
                for ch in line:
                    chbbox = JM_char_bbox(line, ch)
                    if mupdf.mfz_is_empty_rect(chbbox):
                        continue
                    if (mupdf.mfz_is_infinite_rect(rect)
                            or mupdf.mfz_contains_rect(rect, chbbox)
                            ):
                        last_char = ch.m_internal.c
                        utf = mupdf.runetochar2(last_char)
                        for c in utf:
                            #jlib.log('{type(c)=} {c!r=}')
                            assert isinstance(c, int)
                            assert 0 <= c < 256, f'utf={utf!r} cc={c}'
                            mupdf.mfz_write_byte(out, c)
                if last_char != 10 and last_char > 0:
                    mupdf.mfz_write_string(out, "\n")


def JM_put_script(annot_obj, key1, key2, value):
    '''
    Create a JavaScript PDF action.
    Usable for all object types which support PDF actions, even if the
    argument name suggests annotations. Up to 2 key values can be specified, so
    JavaScript actions can be stored for '/A' and '/AA/?' keys.
    '''
    #PyObject *script = NULL;
    key1_obj = mupdf.mpdf_dict_get(annot_obj, key1)
    pdf = mupdf.mpdf_get_bound_document(annot_obj)  # owning PDF

    # if no new script given, just delete corresponding key
    if not value:
        if not key2 or not key2.m_internal:
            mupdf.mpdf_dict_del(annot_obj, key1)
        elif key1_obj.m_internal:
            mupdf.mpdf_dict_del(key1_obj, key2)
        return

    # read any existing script as a PyUnicode string
    if not key2.m_internal or not key1_obj.m_internal:
        script = JM_get_script(key1_obj)
    else:
        script = JM_get_script(mupdf.mpdf_dict_get(key1_obj, key2))

    # replace old script, if different from new one
    if value != script:
        newaction = JM_new_javascript(pdf, value)
        if not key2.m_internal:
            mupdf.mpdf_dict_put(annot_obj, key1, newaction)
        else:
            mupdf.mpdf_dict_putl(annot_obj, newaction, key1, key2)


def JM_py_from_matrix(m):
    return m.a, m.b, m.c, m.d, m.e, m.f


def JM_py_from_point(p):
    return p.x, p.y


def JM_py_from_rect(r):
    return r.x0, r.y0, r.x1, r.y1


def JM_quad_from_py(r):
    if isinstance(r, mupdf.Quad):
        return r
    q = mupdf.mfz_make_quad(0, 0, 0, 0, 0, 0, 0, 0)
    p = [0,0,0,0]
    if not r or not isinstance(r, (tuple, list)) or len(r) != 4:
        return q

    if JM_FLOAT_ITEM(r, 0) is None:
        return mupdf.mfz_quad_from_rect(JM_rect_from_py(r))

    for i in range(4):
        obj = r[i]  # next point item
        if not obj.m_internal or not PySequence_Check(obj) or PySequence_Size(obj) != 2:
            return q    # invalid: cancel the rest

        p[i].x = JM_FLOAT_ITEM(obj, 0)
        p[i].y = JM_FLOAT_ITEM(obj, 1)
        if p[i].x is None or p[i].y is None:
            return q
    q.ul = p[0]
    q.ur = p[1]
    q.ll = p[2]
    q.lr = p[3]
    return q


def JM_read_contents(pageref):
    '''
    Read and concatenate a PDF page's /Conents object(s) in a buffer
    '''
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


def JM_rect_from_py(r):
    if isinstance(r, mupdf.Rect):
        return r
    if isinstance(r, Rect):
        return mupdf.mfz_make_rect(r.x0, r.y0, r.x1, r.y1)
    if not r or not PySequence_Check(r) or PySequence_Size(r) != 4:
        return mupdf.Rect(mupdf.Rect.Fixed_INFINITE)
    f = [0, 0, 0, 0]
    for i in range(4):
        f[i] = JM_FLOAT_ITEM(r, i)
        if f[i] is None:
            return mupdf.Rect(mupdf.Rect.Fixed_INFINITE)
    return mupdf.mfz_make_rect(f[0], f[1], f[2], f[3])


def JM_rotate_page_matrix(page):
    '''
    calculate page rotation matrices
    '''
    if not page.m_internal:
        return mupdf.Matrix()  # no valid pdf page given
    rotation = JM_page_rotation(page)
    if rotation == 0:
        return mupdf.Matrix()  # no rotation
    cb_size = JM_cropbox_size(page.obj())
    w = cb_size.x
    h = cb_size.y
    if rotation == 90:
        m = mupdf.mfz_make_matrix(0, 1, -1, 0, h, 0)
    elif rotation == 180:
        m = mupdf.mfz_make_matrix(-1, 0, 0, -1, w, h)
    else:
        m = mupdf.mfz_make_matrix(0, -1, 1, 0, 0, w)
    return m


def JM_search_stext_page(page, needle):
    #struct highlight hits;
    #fz_stext_block *block;
    #fz_stext_line *line;
    #fz_stext_char *ch;
    #fz_buffer *buffer = NULL;
    #const char *haystack, *begin, *end;
    #jlib.log('{page=} {needle=}')
    rect = mupdf.Rect(page.m_internal.mediabox)
    #int c, inside;
    #jlib.log('{rect=}')

    if not needle:
        return
    quads = []
    class Hits:
        def __str__(self):
            return f'Hits(len={self.len} quads={self.quads} hfuzz={self.hfuzz} vfuzz={self.vfuzz}'
    hits = Hits()
    hits.len = 0
    hits.quads = quads
    hits.hfuzz = 0.2    # merge kerns but not large gaps
    hits.vfuzz = 0.1

    buffer_ = JM_new_buffer_from_stext_page(page)
    haystack_string = mupdf.mfz_string_from_buffer(buffer_)
    #jlib.log('{haystack_string=}')
    haystack = 0
    begin, end = find_string(haystack_string[haystack:], needle)
    #jlib.log('{begin=} {end=}')
    if begin is None:
        #goto no_more_matches;
        #jlib.log('returning {quads=}')
        return quads
    #jlib.log('{haystack=} {begin-haystack=}')

    begin += haystack
    end += haystack
    inside = 0
    verbose = 0
    i = 0
    for block in page:
        #jlib.log('block')
        if block.m_internal.type != mupdf.FZ_STEXT_BLOCK_TEXT:
            continue
        for line in block:
            #jlib.log('line')
            for ch in line:
                i += 1
                #verbose = i > 950 and i < 1000
                if (verbose): jlib.log('========================================')
                if (verbose): jlib.log('ch loop start: ch {i=} {inside=} {haystack=}')
                if not mupdf.mfz_is_infinite_rect(rect):
                    r = JM_char_bbox(line, ch, verbose)
                    #jlib.log('{rect=} {r=}')
                    if not mupdf.mfz_contains_rect(rect, r):
                        if (verbose): jlib.log('not fz_contains_rect() [goto next_char]')
                        continue
                while 1:
                    #try_new_match:
                    if (verbose): jlib.log('[try_new_match] {inside=}')
                    if not inside:
                        hs = haystack_string[haystack:]
                        if (verbose): jlib.log('{haystack=} {begin=} {haystack-begin=}')
                        if haystack >= begin:
                            if (verbose): jlib.log('setting inside to 1')
                            inside = 1
                    if (verbose): jlib.log('{inside=}')
                    if inside:
                        if (verbose): jlib.log('{haystack-end=}')
                        if haystack < end:
                            if (verbose): jlib.log('calling on_highlight_char()')
                            on_highlight_char(hits, line, ch)

                            break
                        else:
                            inside = 0
                            if (verbose): jlib.log('calling find_string()')
                            begin, end = find_string(haystack_string[haystack:], needle)
                            if (verbose): jlib.log('{begin=} {end=} {haystack=}')
                            if begin is None:
                                #goto no_more_matches;
                                if (verbose): jlib.log('returning, {len(quods)=}')
                                return quads
                            else:
                                #goto try_new_match;
                                begin += haystack
                                end += haystack
                                if (verbose): jlib.log('[try_new_match] doing continue. {inside=}')
                                continue
                    break
                #else:
                #    if (verbose): jlib.log('skpping char')
                #next_char:;
                if (verbose): jlib.log('[next_char]')
                if (verbose): jlib.log('calling fz_chartorune()')
                rune, _ = mupdf.mfz_chartorune(haystack_string[haystack:])
                haystack += rune
                hs = haystack_string[haystack:]
                if (verbose): jlib.log('{rune=} {haystack=}: {hs}')
                #break
            assert haystack_string[haystack] == '\n'
            haystack += 1
            if (verbose): jlib.log('{i=} incremented to {haystack=}')
        assert haystack_string[haystack] == '\n'
        haystack += 1
    #no_more_matches:;
    return quads


    # fixme: this assumes that fz_search_stext_page is equivalent to pymupdf's
    # JM_search_stext_page().
    #
    # Need to change fz_search_stext_page() to be able to return arbitrary
    # number of quads?
    #
    #return mupdf.mfz_search_stext_page(page, needle)

    # fixme: figure out a way to avoid having to pass in max_quads.
    ret = page.search_stext_page(needle, 10)
    assert isinstance(ret, tuple)
    ret = list(ret)
    nl = '\n'
    #jlib.log('{len(ret)=}:\n{nl.join([str(i) for i in ret])}\n')
    return ret


def JM_scan_resources(pdf, rsrc, liste, what, stream_xref, tracer):
    '''
    Step through /Resources, looking up image, xobject or font information
    '''
    if rsrc.mark_obj():
        # fixme: not implemented yet: fz_warn(ctx, "Circular dependencies! Consider page cleaning.");
        print(f'Circular dependencies! Consider page cleaning.')
        return  # Circular dependencies!
    try:
        xobj = rsrc.dict_get(mupdf.PDF_ENUM_NAME_XObject)

        if what == 1:   # lookup fonts
            font = rsrc.dict_get(mupdf.PDF_ENUM_NAME_Font)
            JM_gather_fonts(pdf, font, liste, stream_xref)
        elif what == 2: # look up images
            JM_gather_images(pdf, xobj, liste, stream_xref)
        elif what == 3: # look up form xobjects
            JM_gather_forms(pdf, xobj, liste, stream_xref)
        else:   # should never happen
            return

        # check if we need to recurse into Form XObjects
        n = xobj.dict_len()
        for i in range(n):
            obj = xobj.dict_get_val(i)
            if obj.is_stream():
                sxref = obj.to_num()
            else:
                sxref = 0
            subrsrc = obj.dict_get(mupdf.PDF_ENUM_NAME_Resources)
            if subrsrc.m_internal:
                sxref_t = sxref
                if sxref_t not in tracer:
                    tracer.append(sxref_t)
                    JM_scan_resources( pdf, subrsrc, liste, what, sxref, tracer)
                else:
                    #fz_warn(ctx, "Circular dependencies! Consider page cleaning.");
                    print(f'Circular dependencies! Consider page cleaning.')
                    return
    finally:
        rsrc.unmark_obj()


def JM_set_choice_options(annot, liste):
    '''
    set ListBox / ComboBox values
    '''
    if not PySequence_Check(liste):
        return
    n = len(liste)
    if not n:
        return
    tuple_ = tuple(liste)
    annot_obj = mupdf.mpdf_annot_obj(annot)
    pdf = mupdf.mpdf_get_bound_document(annot_obj)
    #const char *opt = NULL, *opt1 = NULL, *opt2 = NULL;
    optarr = mupdf.mpdf_new_array(pdf, n)
    #pdf_obj *optarrsub = NULL;
    #PyObject *val = NULL;
    for i in range(n):
        val = tuple_[i]
        opt = val
        if opt:
            mupdf.mpdf_array_push_text_string(optarr, opt)
        else:
            opt1 = PyTuple_GetItem(val, 0)
            opt2 = PyTuple_GetItem(val, 1)
            if not opt1 or not opt2:
                return
            optarrsub = mupdf.mpdf_array_push_array(optarr, 2)
            mupdf.mpdf_array_push_text_string(optarrsub, opt1)
            mupdf.mpdf_array_push_text_string(optarrsub, opt2)
    mupdf.mpdf_dict_put(annot_obj, PDF_NAME('Opt'), optarr)


def JM_set_field_type(doc, obj, type):
    '''
    Set the field type
    '''
    setbits = 0;
    clearbits = 0;
    typename = None
    if type == mupdf.PDF_WIDGET_TYPE_BUTTON:
        typename = PDF_NAME('Btn')
        setbits = PDF_BTN_FIELD_IS_PUSHBUTTON
    elif type == mupdf.PDF_WIDGET_TYPE_RADIOBUTTON:
        typename = PDF_NAME('Btn')
        clearbits = PDF_BTN_FIELD_IS_PUSHBUTTON
        setbits = PDF_BTN_FIELD_IS_RADIO
    elif type == mupdf.PDF_WIDGET_TYPE_CHECKBOX:
        typename = PDF_NAME('Btn')
        clearbits = (PDF_BTN_FIELD_IS_PUSHBUTTON|PDF_BTN_FIELD_IS_RADIO)
    elif type == mupdf.PDF_WIDGET_TYPE_TEXT:
        typename = PDF_NAME('Tx')
    elif type == mupdf.PDF_WIDGET_TYPE_LISTBOX:
        typename = PDF_NAME('Ch')
        clearbits = PDF_CH_FIELD_IS_COMBO
    elif type == mupdf.PDF_WIDGET_TYPE_COMBOBOX:
        typename = PDF_NAME('Ch')
        setbits = PDF_CH_FIELD_IS_COMBO
    elif type == mupdf.PDF_WIDGET_TYPE_SIGNATURE:
        typename = PDF_NAME('Sig')

    if typename:
        mupdf.mpdf_dict_put(obj, PDF_NAME('FT'), typename)

    if setbits != 0 or clearbits != 0:
        bits = mupdf.mpdf_dict_get_int(obj, PDF_NAME('Ff'))
        bits &= ~clearbits
        bits |= setbits
        mupdf.mpdf_dict_put_int(obj, PDF_NAME('Ff'), bits)


def JM_set_object_value(obj, key, value):
    '''
    Set a PDF dict key to some value
    '''
    #fz_buffer *res = NULL;
    #pdf_obj *new_obj = NULL, *testkey = NULL;
    #PyObject *skey = PyUnicode_FromString(key);  // Python version of dict key
    #PyObject *slash = PyUnicode_FromString("/");  // PDF path separator
    #PyObject *list = NULL, *newval=NULL, *newstr=NULL, *nullval=NULL;
    eyecatcher = "fitz: replace me!"
    pdf = mupdf.mpdf_get_bound_document(obj)
    # split PDF key at path seps and take last key part
    list_ = key.split('/')
    len_ = len(list_);
    i = len_ - 1;
    skey = list_[i]

    del list_[i]    # del the last sub-key
    len_ =  len(list_)   # remaining length
    testkey = mupdf.mpdf_dict_getp(obj, key)    # check if key already exists
    if not testkey.m_internal:
        #No, it will be created here. But we cannot allow this happening if
        #indirect objects are referenced. So we check all higher level
        #sub-paths for indirect references.
        while len_ > 0:
            t = '/'.join(list_) # next high level
            if mupdf.mpdf_is_indirect(mupdf.mpdf_dict_getp(obj, JM_StrAsChar(t))):
                raise Exception("path to '%s' has indirects", JM_StrAsChar(skey))
            del list_[len_ - 1]   # del last sub-key
            len_ = len(list_)   # remaining length
    # Insert our eyecatcher. Will create all sub-paths in the chain, or
    # respectively remove old value of key-path.
    mupdf.mpdf_dict_putp(obj, key, mupdf.mpdf_new_text_string(eyecatcher))
    testkey = mupdf.mpdf_dict_getp(obj, key)
    if not mupdf.mpdf_is_string(testkey):
        raise Exception("cannot insert value for '%s'", key)
    temp = mupdf.mpdf_to_text_string(testkey)
    if temp != eyecatcher:
        raise Exception("cannot insert value for '%s'", key)
    # read the result as a string
    res = JM_object_to_buffer(obj, 1, 0)
    objstr = JM_EscapeStrFromBuffer(res)

    # replace 'eyecatcher' by desired 'value'
    nullval = "/%s(%s)" % ( skey, eyecatcher)
    newval = "/%s %s" % (skey, value)
    newstr = objstr.replace(nullval, newval, 1)

    # make PDF object from resulting string
    new_obj = JM_pdf_obj_from_str(pdf, newstr)
    return new_obj;


def JM_set_resource_property(ref, name, xref):
    '''
    Insert an item into Resources/Properties (used for Marked Content)
    Arguments:
    (1) e.g. page object, Form XObject
    (2) marked content name
    (3) xref of the referenced object (insert as indirect reference)
    '''
    pdf = mupdf.mpdf_get_bound_document(ref)
    ind = mupdf.mpdf_new_indirect(pdf, xref, 0)
    if not ind.m_internal:
        THROWMSG("bad xref")
    resources = mupdf.mpdf_dict_get(ref, PDF_NAME('Resources'))
    if not resources.m_internal:
        resources = mupdf.mpdf_dict_put_dict(ref, PDF_NAME('Resources'), 1)
    properties = mupdf.mpdf_dict_get(resources, PDF_NAME('Properties'))
    if not properties.m_internal:
        properties = mupdf.mpdf_dict_put_dict(resources, PDF_NAME('Properties'), 1)
    mupdf.mpdf_dict_put(properties, mupdf.mpdf_new_name(name), ind)


def JM_set_widget_properties(annot, Widget):
    '''
    Update the PDF form field with the properties from a Python Widget object.
    Called by "Page.addWidget" and "Annot.updateWidget".
    '''
    page = mupdf.mpdf_annot_page(annot)
    annot_obj = mupdf.mpdf_annot_obj(annot)
    pdf = page.doc()
    #Py_ssize_t i, n = 0;
    #result = 0;
    def GETATTR(name):
        return getattr(Widget, name, None)

    value = GETATTR("field_type")
    field_type = value

    # rectangle --------------------------------------------------------------
    value = GETATTR("rect");
    rect = JM_rect_from_py(value);
    rot_mat = JM_rotate_page_matrix(page)
    rect = mupdf.mfz_transform_rect(rect, rot_mat)
    mupdf.mpdf_set_annot_rect(annot, rect)

    # fill color -------------------------------------------------------------
    value = GETATTR("fill_color");
    if value and PySequence_Check(value):
        n = len(value)
        fill_col = mupdf.mpdf_new_array(pdf, n);
        col = 0;
        for i in range(n):
            col = value[i]
            mupdf.mpdf_array_push_real(fill_col, col)
        mupdf.mpdf_field_set_fill_color(annot_obj, fill_col)
        #mupdf.mpdf_drop_obj(fill_col)

    # dashes -----------------------------------------------------------------
    value = GETATTR("border_dashes")
    if value and PySequence_Check(value):
        n = len(value)
        dashes = mupdf.mpdf_new_array(pdf, n)
        for i in range(n):
            mupdf.mpdf_array_push_int(dashes, value[i])
        mupdf.mpdf_dict_putl(annot_obj, dashes, PDF_NAME('BS'), PDF_NAME('D'))

    # border color -----------------------------------------------------------
    value = GETATTR("border_color");
    if value and PySequence_Check(value):
        n = len(value)
        border_col = mupdf.mpdf_new_array(pdf, n)
        col = 0;
        for i in range(n):
            col = value[i]
            mupdf.mpdf_array_push_real(border_col, col)
        mupdf.mpdf_dict_putl(annot_obj, border_col, PDF_NAME('MK'), PDF_NAME('BC'))

    # entry ignored - may be used later
    #
    #int text_format = (int) PyInt_AsLong(GETATTR("text_format"));
    #

    # field label -----------------------------------------------------------
    value = GETATTR("field_label");
    if value is not None:
        label = JM_StrAsChar(value)
        mupdf.mpdf_dict_put_text_string(annot_obj, PDF_NAME('TU'), label)

    # field name -------------------------------------------------------------
    value = GETATTR("field_name");
    if value is not None:
        name = JM_StrAsChar(value)
        old_name = mupdf.mpdf_field_name(annot_obj)
        if name != old_name:
            mupdf.mpdf_dict_put_text_string(annot_obj, PDF_NAME('T'), name)

    # max text len -----------------------------------------------------------
    if field_type == PDF_WIDGET_TYPE_TEXT:
        value = GETATTR("text_maxlen")
        text_maxlen = value
        if text_maxlen:
            mupdf.mpdf_dict_put_int(annot_obj, PDF_NAME('MaxLen'), text_maxlen)
    value = GETATTR("field_display")
    d = value
    mupdf.mpdf_field_set_display(annot_obj, d)

    # choice values ----------------------------------------------------------
    if field_type in (PDF_WIDGET_TYPE_LISTBOX, PDF_WIDGET_TYPE_COMBOBOX):
        value = GETATTR("choice_values")
        JM_set_choice_options(annot, value)

    # border style -----------------------------------------------------------
    value = GETATTR("border_style");
    val = JM_get_border_style(value)
    mupdf.mpdf_dict_putl(annot_obj, val, PDF_NAME('BS'), PDF_NAME('S'))

    # border width -----------------------------------------------------------
    value = GETATTR("border_width");
    border_width = value
    mupdf.mpdf_dict_putl(
            annot_obj,
            mupdf.mpdf_new_real(border_width),
            PDF_NAME('BS'),
            PDF_NAME('W'),
            )

    # /DA string -------------------------------------------------------------
    value = GETATTR("_text_da");
    da = JM_StrAsChar(value)
    mupdf.mpdf_dict_put_text_string(annot_obj, PDF_NAME('DA'), da)
    mupdf.mpdf_dict_del(annot_obj, PDF_NAME('DS'))  # not supported by MuPDF
    mupdf.mpdf_dict_del(annot_obj, PDF_NAME('RC'))  # not supported by MuPDF

    # field flags ------------------------------------------------------------
    field_flags = 0
    Ff = 0
    if field_type not in (
            PDF_WIDGET_TYPE_CHECKBOX,
            PDF_WIDGET_TYPE_BUTTON,
            PDF_WIDGET_TYPE_RADIOBUTTON,
            ):
        value = GETATTR("field_flags") or 0
        field_flags = value
        Ff = mupdf.mpdf_field_flags(annot_obj)
        Ff |= field_flags
    mupdf.mpdf_dict_put_int(annot_obj, PDF_NAME('Ff'), Ff)

    # button caption ---------------------------------------------------------
    value = GETATTR("button_caption")
    ca = JM_StrAsChar(value)
    if ca:
        mupdf.mpdf_field_set_button_caption(annot_obj, ca)

    # script (/A) -------------------------------------------------------
    value = GETATTR("script")
    JM_put_script(annot_obj, PDF_NAME('A'), None, value)

    # script (/AA/K) -------------------------------------------------------
    value = GETATTR("script_stroke")
    JM_put_script(annot_obj, PDF_NAME('AA'), PDF_NAME('K'), value)

    # script (/AA/F) -------------------------------------------------------
    value = GETATTR("script_format")
    JM_put_script(annot_obj, PDF_NAME('AA'), PDF_NAME('F'), value)

    # script (/AA/V) -------------------------------------------------------
    value = GETATTR("script_change")
    JM_put_script(annot_obj, PDF_NAME('AA'), PDF_NAME('V'), value)

    # script (/AA/C) -------------------------------------------------------
    value = GETATTR("script_calc")
    JM_put_script(annot_obj, PDF_NAME('AA'), PDF_NAME('C'), value)

    # field value ------------------------------------------------------------
    value = GETATTR("field_value");
    if field_type in (PDF_WIDGET_TYPE_CHECKBOX, PDF_WIDGET_TYPE_RADIOBUTTON):
        #if PyObject_RichCompareBool(value, Py_True, Py_EQ):
        if value == True:
            onstate = mupdf.mpdf_button_field_on_state(annot_obj)
            on = mupdf.mpdf_to_name(onstate)
            result = mupdf.mpdf_set_field_value(pdf, annot_obj, on, 1)
            mupdf.mpdf_dict_put_name(annot_obj, PDF_NAME('V'), on)
        else:
            result = mupdf.mpdf_set_field_value(pdf, annot_obj, "Off", 1)
            mupdf.mpdf_dict_put(annot_obj, PDF_NAME('V'), PDF_NAME('Off'))
    else:
        text = JM_StrAsChar(value)
        if text:
            result = mupdf.mpdf_set_field_value(pdf, annot_obj, text, 1)
            if field_type in (PDF_WIDGET_TYPE_COMBOBOX, PDF_WIDGET_TYPE_LISTBOX):
                mupdf.mpdf_dict_del(annot_obj, PDF_NAME('I'))
    mupdf.mpdf_dirty_annot(annot)
    mupdf.mpdf_set_annot_hot(annot, 1)
    mupdf.mpdf_set_annot_active(annot, 1)
    mupdf.mpdf_update_annot(annot)


def JM_UnicodeFromBuffer(buff):
    buff_bytes = buff.buffer_extract()
    val = buff_bytes.decode(errors='replace')
    return val


def JM_update_stream(doc, obj, buffer_, compress):
    '''
    update a stream object
    compress stream when beneficial
    '''
    #jlib.log('{dir(buffer_)}')
    len_, _ = buffer_.buffer_storage_raw()
    nlen = len_

    if len_ > 30:   # ignore small stuff
        nres = JM_compress_buffer(buffer_)
        assert isinstance(nres, mupdf.Buffer)
        nlen, _ = nres.buffer_storage_raw()

    if nlen < len_ and nres and compress==1:   # was it worth the effort?
        obj.dict_put(
                mupdf.PDF_ENUM_NAME_Filter,
                mupdf.PDF_ENUM_NAME_FlateDecode,
                )
        doc.update_stream(obj, nres, 1)
    else:
        doc.update_stream(obj, buffer_, 0);























def JM_xobject_from_page(pdfout, fsrcpage, xref, gmap):
    '''
    Make an XObject from a PDF page
    For a positive xref assume that its object can be used instead
    '''
    #pdf_obj *xobj1, *resources = NULL, *o, *spageref;
    assert isinstance(gmap, mupdf.PdfGraftMap), f'type(gmap)={type(gmap)}'
    if xref > 0:
        xobj1 = mupdf.mpdf_new_indirect(pdfout, xref, 0)
    else:
        #fz_buffer *res = NULL;
        #fz_rect mediabox;
        srcpage = mupdf.mpdf_page_from_fz_page(fsrcpage)
        spageref = srcpage.obj()
        mediabox = mupdf.mpdf_to_rect(mupdf.mpdf_dict_get_inheritable(spageref, PDF_NAME('MediaBox')))
        # Deep-copy resources object of source page
        o = mupdf.mpdf_dict_get_inheritable(spageref, PDF_NAME('Resources'))
        if gmap.m_internal:
            # use graftmap when possible
            resources = mupdf.mpdf_graft_mapped_object(gmap, o)
        else:
            resources = mupdf.mpdf_graft_object(pdfout, o)

        # get spgage contents source
        res = JM_read_contents(spageref)

        #-------------------------------------------------------------
        # create XObject representing the source page
        #-------------------------------------------------------------
        xobj1 = mupdf.mpdf_new_xobject(pdfout, mediabox, mupdf.Matrix(), mupdf.PdfObj(0), res)
        # store spage contents
        JM_update_stream(pdfout, xobj1, res, 1)

        # store spage resources
        mupdf.mpdf_dict_put(xobj1, PDF_NAME('Resources'), resources)
    return xobj1


def PySequence_Check(s):
    return isinstance(s, (tuple, list))


def PySequence_Size(s):
    return len(s)


def THROWMSG(msg):
    raise Exception(msg)

def PyUnicode_DecodeRawUnicodeEscape(s, errors='strict'):
    # fixme: handle escape sequencies
    return s.decode('utf8')

def CheckColor(c: OptSeq):
    if c:
        if (
            type(c) not in (list, tuple)
            or len(c) not in (1, 3, 4)
            or min(c) < 0
            or max(c) > 1
        ):
            raise ValueError("need 1, 3 or 4 color components in range 0 to 1")


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


def CheckParent(o: typing.Any):
    return
    if not hasattr(o, "parent") or o.parent is None:
        #jlib.log(jlib.exception_info())
        raise ValueError(f"orphaned object type(o)={type(o)}: parent is None")


def CheckQuad(q: typing.Any) -> bool:
    """Check whether an object is convex, not empty  quad-like.

    It must be a sequence of 4 number pairs.
    """
    try:
        q0 = Quad(q)
    except:
        return False
    return q0.isConvex


def CheckRect(r: typing.Any) -> bool:
    """Check whether an object is non-degenerate rect-like.

    It must be a sequence of 4 numbers.
    """
    try:
        r = Rect(r)
    except:
        return False
    return not (r.is_empty or r.isInfinite)


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


def Page__add_text_marker(self, quads, annot_type):
    pdfpage = self._pdf_page()
    rotation = JM_page_rotation(pdfpage)
    def final():
        if rotation != 0:
            mupdf.mpdf_dict_put_int(pdfpage.obj(), PDF_NAME('Rotate'), rotation)
    try:
        if rotation != 0:
            mupdf.mpdf_dict_put_int(pdfpage.obj(), PDF_NAME('Rotate'), 0)
        annot = mupdf.mpdf_create_annot(pdfpage, annot_type)
        len_ = len(quads)
        for item in quads:
            q = JM_quad_from_py(item);
            mupdf.mpdf_add_annot_quad_point(annot, q)
        JM_add_annot_id(annot, "A")
        mupdf.mpdf_update_annot(annot)
    except Exception as e:
        #jlib.log('{e=}')
        final()
        return
    final()
    #annot = mupdf.mpdf_keep_annot(annot)
    return Annot(annot)


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
    #page.obj().dict_put_drop( mupdf.PDF_ENUM_NAME_Contents, contents)
    page.obj().dict_put( mupdf.PDF_ENUM_NAME_Contents, contents)
    # fixme: page.this.dirty = 1
    return


def PaperRect(s: str) -> Rect:
    """Return a Rect for the paper size indicated in string 's'. Must conform to the argument of method 'PaperSize', which will be invoked.
    """
    width, height = PaperSize(s)
    return Rect(0.0, 0.0, width, height)


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


def PDF_NAME(x):
    assert isinstance(x, str)
    return getattr(mupdf, f'PDF_ENUM_NAME_{x}')


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


def args_match(args, *types):
    '''
    Returns true if <args> matches <types>.

    Each item in <types> is a type or tuple of types. Any of these types will
    match an item in <args>. None will match anything in <args>.
    '''
    j = 0
    for i in range(len(types)):
        type_ = types[i]
        #jlib.log('{type_=}')
        if j >= len(args):
            if isinstance(type_, tuple) and None in type_:
                # arg is missing but has default value.
                continue
            else:
                #jlib.log('returning false: {type=} {i=}>{len(args)=}')
                return False
        if type_ is not None and not isinstance(args[j], type_):
            #jlib.log('returning false: {type=} does not match {type(args[i])=}')
            return False
        j += 1
    if j != len(args):
        #jlib.log('returning false: have only matched first {j} args in {args}')
        return False
    #jlib.log('returning true: {args=} match {types=}')
    return True


def calc_image_matrix(width, height, tr, rotate, keep):
    '''
    # compute image isnertion matrix
    '''
    trect = JM_rect_from_py(tr);
    rot = mupdf.mfz_rotate(rotate)
    trw = trect.x1 - trect.x0
    trh = trect.y1 - trect.y0
    w = trw
    h = trh
    if keep:
        large = max(width, height)
        fw = width / large
        fh = height / large
    else:
        fw = fh = 1
    small = min(fw, fh)
    if rotate != 0 and rotate != 180:
        f = fw
        fw = fh
        fh = f
    if fw < 1:
        if trw / fw > trh / fh:
            w = trh * small
            h = trh
        else:
            w = trw
            h = trw / small
    elif fw != fh:
        if trw / fw > trh / fh:
            w = trh / small
            h = trh
        else:
            w = trw
            h = trw * small
    else:
        w = trw
        h = trh
    tmp = mupdf.mfz_make_point(
            (trect.x0 + trect.x1) / 2,
            (trect.y0 + trect.y1) / 2,
            )
    mat = mupdf.mfz_make_matrix(1, 0, 0, 1, -0.5, -0.5)
    mat = mupdf.mfz_concat(mat, rot)
    mat = mupdf.mfz_concat(mat, mupdf.mfz_scale(w, h))
    mat = mupdf.mfz_concat(mat, mupdf.mfz_translate(tmp.x, tmp.y))
    return mat


def detect_super_script(line, ch):
    if line.m_internal.wmode == 0 and line.m_internal.dir.x == 1 and line.m_internal.dir.y == 0:
        return ch.m_internal.origin.y < line.m_internal.first_char.origin.y - ch.m_internal.size * 0.1
    return 0


def dir_str(x):
    ret = f'{x} {type(x)} ({len(dir(x))}):\n'
    for i in dir(x):
        ret += f'    {i}\n'
    return ret


def getLinkDict(ln) -> dict:
    nl = {"kind": ln.dest.kind, "xref": 0}
    try:
        nl["from"] = ln.rect
    except:
        pass
    pnt = Point(0, 0)
    if ln.dest.flags & LINK_FLAG_L_VALID:
        pnt.x = ln.dest.lt.x
    if ln.dest.flags & LINK_FLAG_T_VALID:
        pnt.y = ln.dest.lt.y

    if ln.dest.kind == LINK_URI:
        nl["uri"] = ln.dest.uri

    elif ln.dest.kind == LINK_GOTO:
        nl["page"] = ln.dest.page
        nl["to"] = pnt
        if ln.dest.flags & LINK_FLAG_R_IS_ZOOM:
            nl["zoom"] = ln.dest.rb.x
        else:
            nl["zoom"] = 0.0

    elif ln.dest.kind == LINK_GOTOR:
        nl["file"] = ln.dest.fileSpec.replace("\\", "/")
        nl["page"] = ln.dest.page
        if ln.dest.page < 0:
            nl["to"] = ln.dest.dest
        else:
            nl["to"] = pnt
            if ln.dest.flags & LINK_FLAG_R_IS_ZOOM:
                nl["zoom"] = ln.dest.rb.x
            else:
                nl["zoom"] = 0.0

    elif ln.dest.kind == LINK_LAUNCH:
        nl["file"] = ln.dest.fileSpec.replace("\\", "/")

    elif ln.dest.kind == LINK_NAMED:
        nl["name"] = ln.dest.named

    else:
        nl["page"] = ln.dest.page
    return nl


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


def get_text_blocks(
        page: Page,
        clip: rect_like = None,
        flags: OptInt = None,
        textpage: TextPage = None,
        sort: bool = False,
        ) -> list:
    """Return the text blocks on a page.

    Notes:
        Lines in a block are concatenated with line breaks.
    Args:
        flags: (int) control the amount of data parsed into the textpage.
    Returns:
        A list of the blocks. Each item contains the containing rectangle
        coordinates, text lines, block type and running block number.
    """
    CheckParent(page)
    if flags is None:
        flags = (
            TEXT_PRESERVE_WHITESPACE | TEXT_PRESERVE_IMAGES | TEXT_PRESERVE_LIGATURES
        )
    tp = textpage
    if tp is None:
        tp = page.get_textpage(clip=clip, flags=flags)
    elif getattr(tp, "parent") != page:
        raise ValueError("not a textpage of this page")

    blocks = tp.extractBLOCKS()
    if textpage is None:
        del tp
    if sort is True:
        blocks.sort(key=lambda b: (b[3], b[0]))
    return blocks


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


def get_text_words(
        page: Page,
        clip: rect_like = None,
        flags: OptInt = None,
        textpage: TextPage = None,
        sort: bool = False,
        ) -> list:
    """Return the text words as a list with the bbox for each word.

    Args:
        flags: (int) control the amount of data parsed into the textpage.
    """
    CheckParent(page)
    if flags is None:
        flags = TEXT_PRESERVE_WHITESPACE | TEXT_PRESERVE_LIGATURES
    tp = textpage
    if tp is None:
        tp = page.get_textpage(clip=clip, flags=flags)
    elif getattr(tp, "parent") != page:
        raise ValueError("not a textpage of this page")
    words = tp.extractWORDS()
    if textpage is None:
        del tp
    if sort is True:
        words.sort(key=lambda w: (w[3], w[0]))
    return words


def image_properties(img: typing.ByteString) -> dict:
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


def pdf_dict_getl(doc, obj, *keys):
    assert 0, 'is this fn defunct?'
    #jlib.log('{obj=} {len(keys)=}: {keys}')
    #jlib.log('{doc.this.count_pages()=}')
    for i in range(len(keys)):
        if not obj:
            break
        obj = obj.dict_get(keys[i])
        #jlib.log('{i=} {keys[i]=} {obj=} {doc.this.count_pages()=}')
    #jlib.log('{obj=} {doc.this.count_pages()=}')
    return obj


def planish_line(p1: point_like, p2: point_like) -> Matrix:
    """Compute matrix which maps line from p1 to p2 to the x-axis, such that it
    maintains its length and p1 * matrix = Point(0, 0).

    Args:
        p1, p2: point_like
    Returns:
        Matrix which maps p1 to Point(0, 0) and p2 to a point on the x axis at
        the same distance to Point(0,0). Will always combine a rotation and a
        transformation.
    """
    p1 = Point(p1)
    p2 = Point(p2)
    return Matrix(TOOLS._hor_matrix(p1, p2))




    # Information taken from the following web sites:
    # www.din-formate.de
    # www.din-formate.info/amerikanische-formate.html
    # www.directtools.de/wissen/normen/iso.htm

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


def _get_glyph_text() -> bytes:
    '''
    Adobe Glyph List function
    '''
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


def CheckRect(r: typing.Any) -> bool:
    """Check whether an object is non-degenerate rect-like.

    It must be a sequence of 4 numbers.
    """
    try:
        r = Rect(r)
    except:
        return False
    return not (r.is_empty or r.isInfinite)


def DUMMY(*args, **kw):
    return


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
    html = textwrap.dedent("""
            <!DOCTYPE html>
            <html>
            <head>
            <style>
            body{background-color:gray}
            div{position:relative;background-color:white;margin:1em auto}
            p{position:absolute;margin:0}
            img{position:absolute}
            </style>
            </head>
            <body>
            """)

    xml = textwrap.dedent("""
            <?xml version="1.0"?>
            <document name="%s">
            """
            % filename
            )

    xhtml = textwrap.dedent("""
            <?xml version="1.0"?>
            <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
            <html xmlns="http://www.w3.org/1999/xhtml">
            <head>
            <style>
            body{background-color:gray}
            div{background-color:white;margin:1em;padding:1em}
            p{white-space:pre-wrap}
            </style>
            </head>
            <body>
            """)

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


def adobe_glyph_names() -> tuple:
    '''
    Adobe Glyph List function
    '''
    if _adobe_unicodes == {}:
        for line in _get_glyph_text():
            if line.startswith("#"):
                continue
            gname, unc = line.split(";")
            c = int("0x" + unc[:4], base=16)
            _adobe_unicodes[gname] = c
    return tuple(_adobe_unicodes.keys())


def adobe_glyph_unicodes() -> tuple:
    '''
    Adobe Glyph List function
    '''
    if _adobe_unicodes == {}:
        for line in _get_glyph_text():
            if line.startswith("#"):
                continue
            gname, unc = line.split(";")
            c = int("0x" + unc[:4], base=16)
            _adobe_unicodes[gname] = c
    return tuple(_adobe_unicodes.values())


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
    #annot.parent = weakref.proxy(page)
    page._annot_refs[id(annot)] = annot
    annot.thisown = True


def canon(c):
    assert isinstance(c, int)
    # TODO: proper unicode case folding
    # TODO: character equivalence (a matches ä, etc)
    if c == 0xA0 or c == 0x2028 or c == 0x2029:
        return ord(' ')
    if c == ord('\r') or c == ord('\n') or c == ord('\t'):
        return ord(' ')
    if c >= ord('A') and c <= ord('Z'):
        return c - ord('A') + ord('a')
    return c


def chartocanon(s):
    assert isinstance(s, str)
    n, c = mupdf.mfz_chartorune(s)
    c = canon(c);
    #jlib.log('{s!r=} => {n=} {c=}')
    return n, c;


def dest_is_valid_page(obj, page_object_nums, pagecount):
    num = mupdf.mpdf_to_num(obj)

    if num == 0:
        return 0
    for i in range(pagecount):
        if mupdf.mpage_object_nums[i] == num:
            return 1
    return 0


def find_string(s, needle):
    assert isinstance(s, str)
    for i in range(len(s)):
        end = match_string(s[i:], needle);
        if end is not None:
            end += i
            return i, end
    return None, None


def getPDFnow() -> str:
    '''
    "Now" timestamp in PDF Format
    '''
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
    if clip.is_empty or clip.is_infinite:
        return []

    # extract text of page, clip only, no images, expand ligatures
    blocks = page.get_text(
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
        if not (r.is_empty or r.is_infinite):
            lines.insert(0, r)  # insert again if not empty
    else:
        lines.insert(0, bboxf)  # insert again

    if lines == []:  # the list might have been emptied
        return lines

    # cut off suffix from last line if stop point is close to its bottom
    bboxl = lines.pop()
    if stop.y - bboxl.y1 <= 0.1 * bboxl.height:  # close enough?
        r = Rect(bboxl.tl, stop.x, bboxl.y1)  # intersection rectangle
        if not (r.is_empty or r.is_infinite):
            lines.append(r)  # append if not empty
    else:
        lines.append(bboxl)  # append again

    return lines


def glyph_name_to_unicode(name: str) -> int:
    '''
    Adobe Glyph List function
    '''
    if _adobe_unicodes == {}:
        for line in _get_glyph_text():
            if line.startswith("#"):
                continue
            gname, unc = line.split(";")
            c = int(unc[:4], base=16)
            _adobe_unicodes[gname] = c
    return _adobe_unicodes.get(name, 65533)


def hdist(dir, a, b):
    dx = b.x - a.x;
    dy = b.y - a.y;
    return mupdf.mfz_abs(dx * dir.x + dy * dir.y)


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
    if rect.is_empty or rect.isInfinite:
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


def match_string(h0, n0):
    h = 0
    n = 0
    #int hc, nc;
    e = h
    delta_h, hc = chartocanon(h0[h:])
    h += delta_h
    delta_n, nc = chartocanon(n0[n:])
    n += delta_n
    while hc == nc:
        e = h
        if hc == ord(' '):
            while 1:
                delta_h, hc = chartocanon(h0[h:])
                h += delta_h
                if hc != ord(' '):
                    break
        else:
            delta_h, hc = chartocanon(h0[h:])
            h += delta_h
        if nc == ord(' '):
            while 1:
                delta_n, nc = chartocanon(n0[n:])
                n += delta_n
                if nc != ord(' '):
                    break
        else:
            delta_n, nc = chartocanon(n0[n:])
            n += delta_n
    return None if nc != 0 else e


def on_highlight_char(hits, line, ch):
    #jlib.log('{hits=}')
    assert hits
    assert isinstance(line, mupdf.StextLine)
    assert isinstance(ch, mupdf.StextChar)
    #struct highlight *hits = arg;
    vfuzz = ch.m_internal.size * hits.vfuzz
    hfuzz = ch.m_internal.size * hits.hfuzz
    ch_quad = JM_char_quad(line, ch)
    #jlib.log('{hits.len=} {len(hits.quads)=} {vfuzz=} {hfuzz=} {ch_quad=}')
    if hits.len > 0:
        # fixme: end = hits.quads[-1]
        quad = hits.quads[hits.len - 1]
        end = JM_quad_from_py(quad)
        #jlib.log('{line.m_internal.dir=} {quad=} {end=} {ch_quad=}')
        if ( 1
                and hdist(line.m_internal.dir, end.lr, ch_quad.ll) < hfuzz
                and vdist(line.m_internal.dir, end.lr, ch_quad.ll) < vfuzz
                and hdist(line.m_internal.dir, end.ur, ch_quad.ul) < hfuzz
                and vdist(line.m_internal.dir, end.ur, ch_quad.ul) < vfuzz
                ):
            #jlib.log('extending')
            end.ur = ch_quad.ur
            end.lr = ch_quad.lr
            #quad = JM_py_from_quad(end)
            #hits.quads[hits.len - 1] = quad
            assert hits.quads[-1] == end
            return
    #hits.quads.append(JM_py_from_quad(ch_quad))
    hits.quads.append(ch_quad)
    hits.len += 1


def page_merge(doc_des, doc_src, page_from, page_to, rotate, links, copy_annots, graft_map):
    '''
    Deep-copies a specified source page to the target location.
    Modified copy of function of pdfmerge.c: we also copy annotations, but
    we skip **link** annotations. In addition we rotate output.
    '''
    #pdf_obj *page_ref = NULL;
    #pdf_obj *page_dict = NULL;
    #pdf_obj *obj = NULL, *ref = NULL;

    # list of object types (per page) we want to copy
    known_page_objs = [
        PDF_NAME('Contents'),
        PDF_NAME('Resources'),
        PDF_NAME('MediaBox'),
        PDF_NAME('CropBox'),
        PDF_NAME('BleedBox'),
        PDF_NAME('TrimBox'),
        PDF_NAME('ArtBox'),
        PDF_NAME('Rotate'),
        PDF_NAME('UserUnit'),
        ]
    n = len(known_page_objs)  # number of list elements
    page_ref = mupdf.mpdf_lookup_page_obj(doc_src, page_from)
    mupdf.mpdf_flatten_inheritable_page_items(page_ref)

    # make new page dict in dest doc
    page_dict = mupdf.mpdf_new_dict(doc_des, 4)
    mupdf.mpdf_dict_put(page_dict, PDF_NAME('Type'), PDF_NAME('Page'))

    # copy objects of source page into it
    for i in range(n):
        obj = mupdf.mpdf_dict_get(page_ref, known_page_objs[i])
        if obj.m_internal:
            mupdf.mpdf_dict_put( page_dict, known_page_objs[i], mupdf.mpdf_graft_mapped_object(graft_map, obj))

    # Copy the annotations, but skip types Link, Popup, IRT.
    # Remove dict keys P (parent) and Popup from copied annot.
    if copy_annots:
        old_annots = mupdf.mpdf_dict_get( page_ref, PDF_NAME('Annots'))
        if old_annots.m_internal:
            n = mupdf.mpdf_array_len( old_annots)
            new_annots = mupdf.mpdf_dict_put_array( page_dict, PDF_NAME('Annots'), n)
            for i in range(n):
                o = mupdf.mpdf_array_get( old_annots, i)
                if mupdf.mpdf_dict_gets( o, "IRT").m_internal:
                    continue
                subtype = mupdf.mpdf_dict_get( o, PDF_NAME('Subtype'))
                if mupdf.mpdf_name_eq( subtype, PDF_NAME('Link')):
                    continue
                if mupdf.mpdf_name_eq( subtype, PDF_NAME('Popup')):
                    continue
                mupdf.mpdf_dict_del( o, PDF_NAME('Popup'))
                mupdf.mpdf_dict_del( o, PDF_NAME('P'))
                copy_o = mupdf.mpdf_graft_mapped_object( graft_map, o)
                annot = mupdf.mpdf_new_indirect( doc_des, mupdf.mpdf_to_num( copy_o), 0)
                mupdf.mpdf_array_push( new_annots, annot)

    # rotate the page
    if rotate != -1:
        mupdf.mpdf_dict_put_int( page_dict, PDF_NAME('Rotate'), rotate)
    # Now add the page dictionary to dest PDF
    ref = mupdf.mpdf_add_object( doc_des, page_dict)

    # Insert new page at specified location
    mupdf.mpdf_insert_page( doc_des, page_to, ref)


def pdfobj_string(o, prefix=''):
    '''
    Returns description of mupdf.PdfObj (wrapper for pdf_obj) <o>.
    '''
    assert 0, 'use mupdf.mpdf_debug_obj() ?'
    ret = ''
    if o.is_array:
        l = o.array_len()
        ret += f'array {l}\n'
        for i in range(l):
            oo = o.array_get(i)
            ret += pdfobj_string(oo, prefix + '    ')
            ret += '\n'
    elif o.is_bool():
        ret += f'bool: {o.array_get_bool()}\n'
    elif o.is_dict():
        l = o.dict_len()
        ret += f'dict {l}\n'
        for i in range(l):
            key = o.dict_get_key(i)
            value = o.dict_get( key)
            ret += f'{prefix} {key}: '
            ret += pdfobj_string( value, prefix + '    ')
            ret += '\n'
    elif o.is_embedded_file():
        ret += f'embedded_file: {o.embedded_file_name()}\n'
    elif o.is_indirect():
        ret += f'indirect: ...\n'
    elif o.is_int():
        ret += f'int: {o.to_int()}\n'
    elif o.is_jpx_image():
        ret += f'jpx_image:\n'
    elif o.is_name():
        ret += f'name: {o.to_name()}\n'
    elif o.is_null:
        ret += f'null\n'
    #elif o.is_number:
    #    ret += f'number\n'
    elif o.is_real:
        ret += f'real: {o.to_real()}\n'
    elif o.is_stream():
        ret += f'stream\n'
    elif o.is_string():
        ret += f'string: {o.to_string()}\n'
    else:
        ret += '<>\n'

    return ret


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


def recover_line_quad(line, spans=None):
    return utils.recover_line_quad(line, spans)


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

def retainpage(doc, parent, kids, page):
    '''
    Recreate page tree to only retain specified pages.
    '''
    pageref = mupdf.mpdf_lookup_page_obj(doc, page)
    mupdf.mpdf_flatten_inheritable_page_items(pageref)
    mupdf.mpdf_dict_put(pageref, PDF_NAME('Parent'), parent)
    # Store page object in new kids array
    mupdf.mpdf_array_push(kids, pageref)


def retainpages(doc, liste):
    '''
    This is called by PyMuPDF:
    liste = page numbers to retain
    '''
    #pdf_obj *oldroot, *root, *pages, *kids, *countobj, *olddests;
    argc = len(liste)
    #pdf_obj *names_list = NULL;
    #pdf_obj *outlines;
    #pdf_obj *ocproperties;
    pagecount = mupdf.mpdf_count_pages(doc)

    # Keep only pages/type and (reduced) dest entries to avoid
    # references to dropped pages
    oldroot = mupdf.mpdf_dict_get(mupdf.mpdf_trailer(doc), PDF_NAME('Root'))
    pages = mupdf.mpdf_dict_get(oldroot, PDF_NAME('Pages'))
    olddests = mupdf.mpdf_load_name_tree(doc, PDF_NAME('Dests'))
    outlines = mupdf.mpdf_dict_get(oldroot, PDF_NAME('Outlines'))
    ocproperties = mupdf.mpdf_dict_get(oldroot, PDF_NAME('OCProperties'))

    root = mupdf.mpdf_new_dict(doc, 3)
    mupdf.mpdf_dict_put(root, PDF_NAME('Type'), mupdf.mpdf_dict_get(oldroot, PDF_NAME('Type')))
    mupdf.mpdf_dict_put(root, PDF_NAME('Pages'), mupdf.mpdf_dict_get(oldroot, PDF_NAME('Pages')))
    if outlines.m_internal:
        mupdf.mpdf_dict_put(root, PDF_NAME('Outlines'), outlines)
    if ocproperties.m_internal:
        mupdf.mpdf_dict_put(root, PDF_NAME('OCProperties'), ocproperties)

    mupdf.mpdf_update_object(doc, mupdf.mpdf_to_num(oldroot), root)

    # Create a new kids array with only the pages we want to keep
    kids = mupdf.mpdf_new_array(doc, 1)

    # Retain pages specified
    for page in range(argc):
        i = liste[page]
        if i < 0 or i >= pagecount:
            THROWMSG("invalid page number(s)")
        retainpage(doc, pages, kids, i)

    # Update page count and kids array
    countobj = mupdf.mpdf_new_int(mupdf.mpdf_array_len(kids))
    mupdf.mpdf_dict_put(pages, PDF_NAME('Count'), countobj)
    mupdf.mpdf_dict_put(pages, PDF_NAME('Kids'), kids)

    pagecount = mupdf.mpdf_count_pages(doc)
    #page_object_nums = fz_calloc(ctx, pagecount, sizeof(*page_object_nums));
    page_object_nums = []
    for i in range(pagecount):
        pageref = mupdf.mpdf_lookup_page_obj(doc, i)
        page_object_nums.append(mupdf.mpdf_to_num(pageref))

    # If we had an old Dests tree (now reformed as an olddests dictionary),
    # keep any entries in there that point to valid pages.
    # This may mean we keep more than we need, but it is safe at least.
    if olddests:
        names = mupdf.mpdf_new_dict(doc, 1)
        dests = mupdf.mpdf_new_dict(doc, 1)
        len = mupdf.mpdf_dict_len(olddests)

        names_list = mupdf.mpdf_new_array(doc, 32)

        for i in range(len):
            key = mupdf.mpdf_dict_get_key(olddests, i)
            val = mupdf.mpdf_dict_get_val(olddests, i)
            dest = mupdf.mpdf_dict_get(val, PDF_NAME('D'))

            dest = mupdf.mpdf_array_get(dest if dest.m_internal else val, 0)
            # fixme: need dest_is_valid_page.
            if dest_is_valid_page(dest, page_object_nums, pagecount):
                key_str = mupdf.mpdf_new_string(mupdf.mpdf_to_name(key), len(mupdf.mpdf_to_name(key)))
                mupdf.mpdf_array_push(names_list, key_str)
                mupdf.mpdf_array_push(names_list, val)

        mupdf.mpdf_dict_put(dests, PDF_NAME('Names'), names_list)
        mupdf.mpdf_dict_put(names, PDF_NAME('Dests'), dests)
        mupdf.mpdf_dict_put(root, PDF_NAME('Names'), names)

        #mupdf.mpdf_drop_obj(names)
        #mupdf.mpdf_drop_obj(dests)
        #mupdf.mpdf_drop_obj(olddests)

    # Edit each pages /Annot list to remove any links pointing to nowhere.
    for i in range(pagecount):
        pageref = mupdf.mpdf_lookup_page_obj(doc, i)
        annots = mupdf.mpdf_dict_get(pageref, PDF_NAME('Annots'))
        len = mupdf.mpdf_array_len(annots)
        j = 0
        while 1:
            if j >= len:
                break
            o = mupdf.mpdf_array_get(annots, j)

            if not mupdf.mpdf_name_eq(mupdf.mpdf_dict_get(o, PDF_NAME('Subtype')), PDF_NAME('Link')):
                continue

            if not dest_is_valid(o, pagecount, page_object_nums, names_list):
                # Remove this annotation
                mupdf.mpdf_array_delete(annots, j)
                len -= 1
                j -= 1
            j += 1

    if (strip_outlines(ctx, doc, outlines, pagecount, page_object_nums, names_list) == 0):
        mupdf.mpdf_dict_del(root, PDF_NAME('Outlines'))


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


def strip_outline(doc, outlines, page_count, page_object_nums, names_list):
    '''
    Returns (count, first, prev).
    '''
    current = outlines
    while current.m_internal:
        # Strip any children to start with. This takes care of
        # First / Last / Count for us.
        nc = strip_outlines(doc, current, page_count, page_object_nums, names_list)

        if not dest_is_valid(current, page_count, page_object_nums, names_list):
            if nc == 0:
                # Outline with invalid dest and no children. Drop it by
                # pulling the next one in here.
                next = mupdf.mpdf_dict_get(current, PDF_NAME('Next'))
                if not next.m_internal:
                    # There is no next one to pull in
                    if prev.m_internal:
                        mupdf.mpdf_dict_del(prev, PDF_NAME('Next'))
                elif prev.m_internal:
                    mupdf.mpdf_dict_put(prev, PDF_NAME('Next'), next)
                    mupdf.mpdf_dict_put(next, PDF_NAME('Prev'), prev)
                else:
                    mupdf.mpdf_dict_del(next, PDF_NAME('Prev'))
                current = next
            else:
                # Outline with invalid dest, but children. Just drop the dest.
                mupdf.mpdf_dict_del(current, PDF_NAME('Dest'));
                mupdf.mpdf_dict_del(current, PDF_NAME('A'));
                current = mupdf.mpdf_dict_get(current, PDF_NAME('Next'))
        else:
            # Keep this one
            if not first.m_internal:
                first = current
            prev = current
            current = mupdf.mpdf_dict_get(current, PDF_NAME('Next'))
            count += 1

    return count, first, prev


def strip_outlines(pdoc, outlines, page_count, page_object_nums, names_list):
    if not outlines.m_internal:
        return 0

    first = mupdf.mpdf_dict_get(outlines, PDF_NAME('First'))
    if not first.m_internal:
        nc = 0
    else:
        nc, first, last = strip_outline(doc, first, page_count, page_object_nums, names_list)

    if nc == 0:
        mupdf.mpdf_dict_del(outlines, PDF_NAME('First'))
        mupdf.mpdf_dict_del(outlines, PDF_NAME('Last'))
        mupdf.mpdf_dict_del(outlines, PDF_NAME('Count'))
    else:
        old_count = mupdf.mpdf_to_int(mupdf.mpdf_dict_get(outlines, PDF_NAME('Count')))
        mupdf.mpdf_dict_put(outlines, PDF_NAME('First'), first);
        mupdf.mpdf_dict_put(outlines, PDF_NAME('Last'), last);
        mupdf.mpdf_dict_put(outlines, PDF_NAME('Count'), nc if mupdf.mpdf_new_int(old_count) > 0 else -nc)
    return nc


def unicode_to_glyph_name(ch: int) -> str:
    '''
    Adobe Glyph List function
    '''
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


def vdist(dir, a, b):
    dx = b.x - a.x
    dy = b.y - a.y
    return mupdf.mfz_abs(dx * dir.y + dy * dir.x)

class TOOLS:
    @staticmethod
    def _get_all_contents(page):
        page = page.this.page_from_fz_page()
        res = JM_read_contents(page.obj())
        result = JM_BinFromBuffer( res)
        return result

    JM_annot_id_stem = 'fitz'

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

    @staticmethod
    def mupdf_warnings(reset=1):
        pass

    @staticmethod
    def set_annot_stem( stem=None):
        if stem is None:
            return JM_annot_id_stem
        len_ = len(stem) + 1
        if len_ > 50:
            len_ = 50
        JM_annot_id_stem = stem[:50]
        return JM_annot_id_stem

    @staticmethod
    def _concat_matrix(m1, m2):
        #return _fitz.Tools__concat_matrix(m1, m2)
        a = JM_matrix_from_py(m1)
        b = JM_matrix_from_py(m2)
        ret = JM_py_from_matrix(mupdf.mfz_concat(a, b))
        return ret

    @staticmethod
    def _fill_widget(annot, widget):
        #val = _fitz.Tools__fill_widget(self, annot, widget)
        #jlib.log('{type(widget)=}')
        #jlib.log('{widget=}')
        val = JM_get_widget_properties(annot, widget)
        #jlib.log('{type(widget)=}')
        #jlib.log('{widget=}')

        widget.rect = Rect(annot.rect)
        #jlib.log('{type(widget)=}')
        #jlib.log('{widget=}')
        widget.xref = annot.xref
        widget.parent = annot.parent
        widget._annot = annot  # backpointer to annot object
        #jlib.log('{widget=} {type(widget)=}')
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

    @staticmethod
    def _hor_matrix(C, P):
        #return _fitz.Tools__hor_matrix(self, C, P)
        # calculate matrix m that maps line CP to the x-axis,
        # such that C * m = (0, 0), and target line has same length.
        c = JM_point_from_py(C)
        p = JM_point_from_py(P)
        s = mupdf.mfz_normalize_vector(mupdf.mfz_make_point(p.x - c.x, p.y - c.y))
        m1 = mupdf.mfz_make_matrix(1, 0, 0, 1, -c.x, -c.y)
        m2 = mupdf.mfz_make_matrix(s.x, -s.y, s.y, s.x, 0, 0)
        return JM_py_from_matrix(mupdf.mfz_concat(m1, m2))

    @staticmethod
    def _include_point_in_rect(r, p):
        #return _fitz.Tools__include_point_in_rect(self, r, p)
        r2 = mupdf.mfz_include_point_in_rect(
                JM_rect_from_py(r),
                JM_point_from_py(p),
                )
        r3 = JM_py_from_rect( r2)
        return JM_py_from_rect(
                mupdf.mfz_include_point_in_rect(
                    JM_rect_from_py(r),
                    JM_point_from_py(p),
                    )
                )

    @staticmethod
    def _insert_contents(page, newcont, overlay=1):
        """Add bytes as a new /Contents object for a page, and return its xref."""
        #return _fitz.Tools__insert_contents(self, page, newcont, overlay)
        #fz_buffer *contbuf = NULL;
        #int xref = 0;
        pdfpage = page._pdf_page()
        ASSERT_PDF(pdfpage)
        contbuf = JM_BufferFromBytes(newcont)
        xref = JM_insert_contents(pdfpage.doc(), pdfpage.obj(), contbuf, overlay)
        #fixme: pdfpage->doc->dirty = 1;
        return xref

    @staticmethod
    def _intersect_rect(r1, r2):
        #return _fitz.Tools__intersect_rect(self, r1, r2)
        return JM_py_from_rect(
                mupdf.mfz_intersect_rect(
                    JM_rect_from_py(r1),
                    JM_rect_from_py(r2),
                    )
                )

    @staticmethod
    def _invert_matrix(matrix):
        try:
            src = mupdf.Matrix(
                    matrix[0],
                    matrix[1],
                    matrix[2],
                    matrix[3],
                    matrix[4],
                    matrix[5],
                    )
        except Exception:
            src = matrix
        a = src.a
        det = a * src.d - src.b * src.c;
        if det < -sys.float_info.epsilon or det > sys.float_info.epsilon:
            dst = mupdf.Matrix()
            rdet = 1 / det
            dst.a = src.d * rdet
            dst.b = -src.b * rdet
            dst.c = -src.c * rdet
            dst.d = a * rdet
            a = -src.e * dst.a - src.f * dst.c
            dst.f = -src.e * dst.b - src.f * dst.d
            dst.e = a
            return 0, (dst.a, dst.b, dst.c, dst.d, dst.e, dst.f)

        return 1, ()

    @staticmethod
    def _le_annot_parms(annot, p1, p2, fill_color):
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
        m = Matrix(TOOLS._hor_matrix(np1, np2))  # matrix makes the line horizontal
        im = ~m                            # inverted matrix
        L = np1 * m                        # converted start (left) point
        R = np2 * m                        # converted end (right) point
        if 0 <= annot.opacity < 1:
            opacity = "/H gs\n"
        else:
            opacity = ""
        return m, im, L, R, w, scol, fcol, opacity

    @staticmethod
    def _le_diamond(annot, p1, p2, lr, fill_color):
        """Make stream commands for diamond line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = TOOLS._le_annot_parms(annot, p1, p2, fill_color)
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

    @staticmethod
    def _le_square(annot, p1, p2, lr, fill_color):
        """Make stream commands for square line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = TOOLS._le_annot_parms(annot, p1, p2, fill_color)
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

    @staticmethod
    def _le_circle(annot, p1, p2, lr, fill_color):
        """Make stream commands for circle line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = TOOLS._le_annot_parms(annot, p1, p2, fill_color)
        shift = 2.5             # 2*shift*width = length of square edge
        d = shift * max(1, w)
        M = R - (d/2., 0) if lr else L + (d/2., 0)
        r = Rect(M, M) + (-d, -d, d, d)         # the square
        ap = "q\n" + opacity + TOOLS._oval_string(r.tl * im, r.tr * im, r.br * im, r.bl * im)
        ap += "%g w\n" % w
        ap += scol + fcol + "b\nQ\n"
        return ap

    @staticmethod
    def _le_butt(annot, p1, p2, lr, fill_color):
        """Make stream commands for butt line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = TOOLS._le_annot_parms(annot, p1, p2, fill_color)
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

    @staticmethod
    def _le_slash(annot, p1, p2, lr, fill_color):
        """Make stream commands for slash line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = TOOLS._le_annot_parms(annot, p1, p2, fill_color)
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

    @staticmethod
    def _le_openarrow(annot, p1, p2, lr, fill_color):
        """Make stream commands for open arrow line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = TOOLS._le_annot_parms(annot, p1, p2, fill_color)
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

    @staticmethod
    def _le_closedarrow(annot, p1, p2, lr, fill_color):
        """Make stream commands for closed arrow line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = TOOLS._le_annot_parms(annot, p1, p2, fill_color)
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

    @staticmethod
    def _le_ropenarrow(annot, p1, p2, lr, fill_color):
        """Make stream commands for right open arrow line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = TOOLS._le_annot_parms(annot, p1, p2, fill_color)
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

    @staticmethod
    def _le_rclosedarrow(annot, p1, p2, lr, fill_color):
        """Make stream commands for right closed arrow line end symbol. "lr" denotes left (False) or right point.
        """
        m, im, L, R, w, scol, fcol, opacity = TOOLS._le_annot_parms(annot, p1, p2, fill_color)
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

    @staticmethod
    def _measure_string(text, fontname, fontsize, encoding=0):
        #return _fitz.Tools__measure_string(self, text, fontname, fontsize, encoding)
        font = mupdf.mfz_new_base14_font(fontname)
        w = 0;
        pos = 0
        while pos < len(text):
            t, c = mupdf.mfz_chartorune(text)
            pos += t
            if encoding == mupdf.PDF_SIMPLE_ENCODING_GREEK:
                c = mupdf.mfz_iso8859_7_from_unicode(c)
            elif encoding == mupdf.PDF_SIMPLE_ENCODING_CYRILLIC:
                c = mupdf.mfz_windows_1251_from_unicode(c)
            else:
                c = mupdf.mfz_windows_1252_from_unicode(c)
            if c < 0:
                c = 0xB7
            g = mupdf.mfz_encode_character(font, c)
            w += mupdf.mfz_advance_glyph(font, g, 0)
        return w * fontsize

    @staticmethod
    def _oval_string(p1, p2, p3, p4):
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

    @staticmethod
    def _parse_da(annot):
        def Tools__parse_da(annot):
            this_annot = annot.this
            assert isinstance(this_annot, mupdf.PdfAnnot)
            try:
                da = mupdf.mpdf_dict_get_inheritable(this_annot.obj(), PDF_NAME('DA'))
                if not da.m_internal:
                    trailer = mupdf.mpdf_trailer(this_annot.page().doc())
                    da = mupdf.ppdf_dict_getl(trailer,
                            PDF_NAME('Root'),
                            PDF_NAME('AcroForm'),
                            PDF_NAME('DA'),
                            )
                da_str = mupdf.mpdf_to_text_string(da)
            except Exception:
                return
            #return JM_UnicodeFromStr(da_str);
            return da_str

        val = Tools__parse_da(annot)

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

    @staticmethod
    def _save_widget(annot, widget):
        #return _fitz.Tools__save_widget(self, annot, widget)
        JM_set_widget_properties(annot, widget);

    @staticmethod
    def _sine_between(C, P, Q):
        # for points C, P, Q compute the sine between lines CP and QP
        c = JM_point_from_py(C)
        p = JM_point_from_py(P)
        q = JM_point_from_py(Q)
        s = mupdf.mfz_normalize_vector(mupdf.mfz_make_point(q.x - p.x, q.y - p.y))
        m1 = mupdf.mfz_make_matrix(1, 0, 0, 1, -p.x, -p.y)
        m2 = mupdf.mfz_make_matrix(s.x, -s.y, s.y, s.x, 0, 0)
        m1 = mupdf.mfz_concat(m1, m2)
        c = mupdf.mfz_transform_point(c, m1)
        c = mupdf.mfz_normalize_vector(c)
        return c.y

    def _transform_point(point, matrix):
        #return _fitz.Tools__transform_point(self, point, matrix)
        return JM_py_from_point(
                mupdf.mfz_transform_point(
                    JM_point_from_py(point),
                    JM_matrix_from_py(matrix),
                    )
                )

    def _transform_rect(rect, matrix):
        #return _fitz.Tools__transform_rect(self, rect, matrix)
        return JM_py_from_rect(
                mupdf.mfz_transform_rect(
                    JM_rect_from_py(rect),
                    JM_matrix_from_py(matrix),
                    )
                )

    def _union_rect(r1, r2):
        #return _fitz.Tools__union_rect(self, r1, r2)
        # fz_union_rect() doesn't ignore empty rectangles like it says it
        # should, so we need to do our own checks first.
        a = JM_rect_from_py(r1)
        b = JM_rect_from_py(r2)
        if a.is_empty_rect():
            ret = b
        elif b.is_empty_rect():
            ret = a
        else:
            ret = mupdf.mfz_union_rect(a, b)
        return JM_py_from_rect(ret)

    def _update_da(annot, da_str):
        #return _fitz.Tools__update_da(self, annot, da_str)
        try:
            this_annot = annot.this
            assert isinstance(this_annot, mupdf.PdfAnnot)
            mupdf.mpdf_dict_put_text_string(this_annot.obj(), PDF_NAME('DA'), da_str)
            mupdf.mpdf_dict_del(this_annot.obj(), PDF_NAME('DS'))    # /* not supported */
            mupdf.mpdf_dict_del(this_annot.obj(), PDF_NAME('RC'))    # /* not supported */
            mupdf.mpdf_dirty_annot(this_annot)
        except Exception:
            return
        return

    @staticmethod
    def gen_id():
        global TOOLS_JM_UNIQUE_ID
        TOOLS_JM_UNIQUE_ID += 1
        return TOOLS_JM_UNIQUE_ID

    @staticmethod
    def set_font_width(doc, xref, width):
        #return _fitz.Tools_set_font_width(self, doc, xref, width)
        pdf = doc._pdf_document()
        if not pdf.m_internal:
            return False
        font = mupdf.mpdf_load_object(pdf, xref)
        dfonts = mupdf.mpdf_dict_get(font, PDF_NAME('DescendantFonts'))
        if mupdf.mpdf_is_array(dfonts):
            n = mupdf.mpdf_array_len(dfonts)
            for i in range(n):
                dfont = mupdf.mpdf_array_get(dfonts, i)
                warray = mupdf.mpdf_new_array(pdf, 3)
                mupdf.mpdf_array_push(warray, mupdf.mpdf_new_int(0))
                mupdf.mpdf_array_push(warray, mupdf.mpdf_new_int(65535))
                mupdf.mpdf_array_push(warray, mupdf.mpdf_new_int(width))
                #mupdf.mpdf_dict_put_drop(dfont, PDF_NAME('W'), warray)
                mupdf.mpdf_dict_put(dfont, PDF_NAME('W'), warray)
        return True

    @staticmethod
    def set_small_glyph_heights(on=None):
        """Set / unset small glyph heights."""
        #return _fitz.Tools_set_small_glyph_heights(self, on)
        global small_glyph_heights
        if on is None:
            return small_glyph_heights
        if on:
            small_glyph_heights = 1
        else:
            small_glyph_heights = 0
        return small_glyph_heights

import utils
if 1:
    pass
elif 0:
    mupdf.mfz_set_warning_callback(JM_mupdf_warning, 0)
else:
    import ctypes
    ctypes_mupdf = ctypes.CDLL('libmupdf.so')
    fn = ctypes.CFUNCTYPE(
        ctypes.c_int,                   # return
        ctypes.c_void_p,                # caller_handle
        ctypes.POINTER(ctypes.c_char),  # str
        )(JM_mupdf_warning)
    ctypes_mupdf.fz_set_warning_callback(0, fn, 0)
