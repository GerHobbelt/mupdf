package example;

import com.artifex.mupdf.fitz.*;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Date;

class TestAPI
{
	public void wontThrow(Runnable runnable) {
		boolean success = false;
		boolean exception = false;
		Exception ex = null;
		try {
			runnable.run();
			success = true;
		} catch (Exception e) {
			exception = true;
			ex = e;
		}
		if (!success || exception)
		{
			StringWriter sw = new StringWriter();
			ex.printStackTrace(new PrintWriter(sw));
			throw new RuntimeException("Unexpected exception: [" + sw.toString() + "]");
		}
	}

	public void willThrow(Runnable runnable) {
		boolean success = false;
		boolean exception = false;
		try {
			runnable.run();
			success = true;
		} catch (Exception e) {
			exception = true;
		}
		if (success || !exception)
			throw new RuntimeException("Unexpected success");
	}

	public void loadPageFromDocumentWithoutPages() {
		final PDFDocument pdf = new PDFDocument();
		willThrow(new Runnable() {
			public void run() {
				Page page = pdf.loadPage(0);
			}
		});
	}

	public void interactWithDeletedAnnotation() {
		PDFDocument pdf = new PDFDocument();

		PDFObject pageobj = pdf.addPage(new Rect(0, 0, 421, 595), 0, null, "");
		pdf.insertPage(-1, pageobj);

		PDFPage page = (PDFPage) pdf.loadPage(0);

		final PDFAnnotation annot = page.createAnnotation(PDFAnnotation.TYPE_SQUARE);

		page.deleteAnnotation(annot);

		willThrow(new Runnable() { public void run() {
			Pixmap p = new Pixmap(ColorSpace.DeviceRGB, new Rect(0, 0, 100, 100), true);
			DrawDevice d = new DrawDevice(p);
			annot.run(d, Matrix.Identity(), null);
		}});
		willThrow(new Runnable() { public void run() {
			annot.toPixmap(Matrix.Identity(), ColorSpace.DeviceRGB, false);
		}});
		willThrow(new Runnable() { public void run() {
			annot.getBounds();
		}});
		willThrow(new Runnable() { public void run() {
			annot.toDisplayList();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getType();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getFlags();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setFlags(0);
		}});
		willThrow(new Runnable() { public void run() {
			annot.getContents();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setContents("");
		}});
		willThrow(new Runnable() { public void run() {
			annot.getRect();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setRect(new Rect(0, 0, 100, 100));
		}});
		willThrow(new Runnable() { public void run() {
			annot.hasBorder();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getBorder();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setBorder(1.0f);
		}});
		willThrow(new Runnable() { public void run() {
			annot.getColor();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setColor(new float[]{ 1, 0, 0 });
		}});
		willThrow(new Runnable() { public void run() {
			annot.hasInteriorColor();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getInteriorColor();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setInteriorColor(new float[]{ 1, 0, 0 });
		}});
		willThrow(new Runnable() { public void run() {
			annot.getOpacity();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setOpacity(0.5f);
		}});
		willThrow(new Runnable() { public void run() {
			annot.hasAuthor();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getAuthor();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setAuthor("Dummy");
		}});
		willThrow(new Runnable() { public void run() {
			annot.getCreationDate();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setCreationDate(new Date());
		}});
		willThrow(new Runnable() { public void run() {
			annot.getModificationDate();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setModificationDate(new Date());
		}});
		willThrow(new Runnable() { public void run() {
			annot.hasLineEndingStyles();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getLineEndingStyles();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setLineEndingStyles(PDFAnnotation.LINE_ENDING_NONE, PDFAnnotation.LINE_ENDING_NONE);
		}});
		willThrow(new Runnable() { public void run() {
			annot.getBorderStyle();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setBorderStyle(PDFAnnotation.BORDER_STYLE_SOLID);
		}});
		willThrow(new Runnable() { public void run() {
			annot.getBorderWidth();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setBorderWidth(0);
		}});
		willThrow(new Runnable() { public void run() {
			annot.getBorderDashCount();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getBorderDashItem(0);
		}});
		willThrow(new Runnable() { public void run() {
			annot.clearBorderDash();
		}});
		willThrow(new Runnable() { public void run() {
			annot.addBorderDashItem(1.0f);
		}});
		willThrow(new Runnable() { public void run() {
			annot.setBorderDashPattern(new float[] { 1, 0 });
		}});
		willThrow(new Runnable() { public void run() {
			annot.hasBorderEffect();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getBorderEffect();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setBorderEffect(PDFAnnotation.BORDER_EFFECT_NONE);
		}});
		willThrow(new Runnable() { public void run() {
			annot.getBorderEffectIntensity();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setBorderEffectIntensity(0.0f);
		}});
		willThrow(new Runnable() { public void run() {
			annot.hasQuadPoints();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getQuadPointCount();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getQuadPoint(0);
		}});
		willThrow(new Runnable() { public void run() {
			annot.clearQuadPoints();
		}});
		willThrow(new Runnable() { public void run() {
			annot.addQuadPoint(new Quad(0, 0, 0, 0, 1, 1, 1, 1));
		}});
		willThrow(new Runnable() { public void run() {
			annot.hasVertices();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getVertexCount();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getVertex(0);
		}});
		willThrow(new Runnable() { public void run() {
			annot.clearVertices();
		}});
		willThrow(new Runnable() { public void run() {
			annot.addVertex(0, 0);
		}});
		willThrow(new Runnable() { public void run() {
			annot.hasInkList();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getInkListCount();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getInkListStrokeCount(0);
		}});
		willThrow(new Runnable() { public void run() {
			annot.clearInkList();
		}});
		willThrow(new Runnable() { public void run() {
			annot.addInkListStroke();
		}});
		willThrow(new Runnable() { public void run() {
			annot.addInkListStrokeVertex(0, 0);
		}});
		willThrow(new Runnable() { public void run() {
			annot.hasIcon();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getIcon();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setIcon("");
		}});
		willThrow(new Runnable() { public void run() {
			annot.hasOpen();
		}});
		willThrow(new Runnable() { public void run() {
			annot.isOpen();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setIsOpen(false);
		}});
		willThrow(new Runnable() { public void run() {
			annot.eventEnter();
		}});
		willThrow(new Runnable() { public void run() {
			annot.eventExit();
		}});
		willThrow(new Runnable() { public void run() {
			annot.eventDown();
		}});
		//eventUp() doesn't throw, why?
		wontThrow(new Runnable() { public void run() {
			annot.eventUp();
		}});
		willThrow(new Runnable() { public void run() {
			annot.eventEnter();
			annot.eventDown();
			annot.eventUp();
		}});
		willThrow(new Runnable() { public void run() {
			annot.eventFocus();
		}});
		willThrow(new Runnable() { public void run() {
			annot.eventBlur();
		}});
		willThrow(new Runnable() { public void run() {
			annot.update();
		}});
		//getObject() doesn't throw, why?
		wontThrow(new Runnable() { public void run() {
			annot.getObject();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getLanguage();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setLanguage(PDFDocument.LANGUAGE_UNSET);
		}});
		willThrow(new Runnable() { public void run() {
			annot.getQuadding();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setQuadding(0);
		}});
		willThrow(new Runnable() { public void run() {
			annot.hasLine();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getLine();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setLine(new Point(0, 0), new Point(1, 1));
		}});
		willThrow(new Runnable() { public void run() {
			annot.getDefaultAppearance();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setDefaultAppearance("font", 12, new float[]{ 1, 0, 0 });
		}});
		willThrow(new Runnable() { public void run() {
			annot.setAppearance(new DisplayList(new Rect(0, 0, 100, 100)));
		}});
		willThrow(new Runnable() { public void run() {
			annot.setAppearance(new Image(new Pixmap(ColorSpace.DeviceRGB, new Rect(0, 0, 100, 100), true)));
		}});
		willThrow(new Runnable() { public void run() {
			annot.hasFileSpecification();
		}});
		willThrow(new Runnable() { public void run() {
			annot.getFileSpecification();
		}});
		willThrow(new Runnable() { public void run() {
			annot.setFileSpecification(null);
		}});
		//getHiddenForEditing() doesn't throw, why?
		wontThrow(new Runnable() { public void run() {
			annot.getHiddenForEditing();
		}});
		//getHiddenForEditing() doesn't throw, why?
		wontThrow(new Runnable() { public void run() {
			annot.setHiddenForEditing(false);
		}});
	}

	public void interactWithAnnotationOnDeletedPage(final PDFAnnotation annot, final PDFObject fs)
	{
		wontThrow(new Runnable() { public void run() {
			Pixmap p = new Pixmap(ColorSpace.DeviceRGB, new Rect(0, 0, 100, 100), true);
			DrawDevice d = new DrawDevice(p);
			annot.run(d, Matrix.Identity(), null);
		}});
		wontThrow(new Runnable() { public void run() {
			annot.toPixmap(Matrix.Identity(), ColorSpace.DeviceRGB, false);
		}});
		wontThrow(new Runnable() { public void run() {
			annot.getBounds();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.toDisplayList();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.getType();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.getFlags();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.getFlags();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.setFlags(0);
		}});
		wontThrow(new Runnable() { public void run() {
			annot.getContents();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.setContents("");
		}});
		wontThrow(new Runnable() { public void run() {
			annot.hasRect();
		}});
		if (annot.hasRect())
		{
			wontThrow(new Runnable() { public void run() {
				annot.getRect();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.setRect(new Rect(0, 0, 100, 100));
			}});
		}
		else
		{
			willThrow(new Runnable() { public void run() {
				annot.getRect();
			}});
			willThrow(new Runnable() { public void run() {
				annot.setRect(new Rect(0, 0, 100, 100));
			}});
		}
		wontThrow(new Runnable() { public void run() {
			annot.hasBorder();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.getBorder();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.setBorder(1.0f);
		}});
		wontThrow(new Runnable() { public void run() {
			annot.getColor();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.setColor(new float[]{ 1, 0, 0 });
		}});
		wontThrow(new Runnable() { public void run() {
			annot.hasInteriorColor();
		}});
		if (annot.hasInteriorColor())
		{
			wontThrow(new Runnable() { public void run() {
				annot.getInteriorColor();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.setInteriorColor(new float[]{ 1, 0, 0 });
			}});
		}
		else
		{
			willThrow(new Runnable() { public void run() {
				annot.getInteriorColor();
			}});
			willThrow(new Runnable() { public void run() {
				annot.setInteriorColor(new float[]{ 1, 0, 0 });
			}});
		}
		wontThrow(new Runnable() { public void run() {
			annot.getOpacity();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.setOpacity(0.5f);
		}});
		wontThrow(new Runnable() { public void run() {
			annot.hasAuthor();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.getAuthor();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.setAuthor("Dummy");
		}});
		wontThrow(new Runnable() { public void run() {
			annot.getCreationDate();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.setCreationDate(new Date());
		}});
		wontThrow(new Runnable() { public void run() {
			annot.getModificationDate();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.setModificationDate(new Date());
		}});
		wontThrow(new Runnable() { public void run() {
			annot.hasLineEndingStyles();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.hasLineEndingStyles();
		}});
		if (annot.hasLineEndingStyles())
		{
			wontThrow(new Runnable() { public void run() {
				annot.getLineEndingStyles();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.setLineEndingStyles(PDFAnnotation.LINE_ENDING_NONE, PDFAnnotation.LINE_ENDING_NONE);
			}});
		}
		else
		{
			willThrow(new Runnable() { public void run() {
				annot.getLineEndingStyles();
			}});
			willThrow(new Runnable() { public void run() {
				annot.setLineEndingStyles(PDFAnnotation.LINE_ENDING_NONE, PDFAnnotation.LINE_ENDING_NONE);
			}});
		}
		if (annot.hasBorder())
		{
			wontThrow(new Runnable() { public void run() {
				annot.getBorderStyle();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.setBorderStyle(PDFAnnotation.BORDER_STYLE_SOLID);
			}});
			wontThrow(new Runnable() { public void run() {
				annot.getBorderWidth();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.setBorderWidth(0);
			}});
			wontThrow(new Runnable() { public void run() {
				annot.getBorderDashCount();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.getBorderDashItem(0);
			}});
			wontThrow(new Runnable() { public void run() {
				annot.clearBorderDash();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.addBorderDashItem(1.0f);
			}});
			wontThrow(new Runnable() { public void run() {
				annot.setBorderDashPattern(new float[] { 1, 0 });
			}});
		}
		else
		{
			willThrow(new Runnable() { public void run() {
				annot.getBorderStyle();
			}});
			willThrow(new Runnable() { public void run() {
				annot.setBorderStyle(PDFAnnotation.BORDER_STYLE_SOLID);
			}});
			willThrow(new Runnable() { public void run() {
				annot.getBorderWidth();
			}});
			willThrow(new Runnable() { public void run() {
				annot.setBorderWidth(0);
			}});
			willThrow(new Runnable() { public void run() {
				annot.getBorderDashCount();
			}});
			willThrow(new Runnable() { public void run() {
				annot.getBorderDashItem(0);
			}});
			willThrow(new Runnable() { public void run() {
				annot.clearBorderDash();
			}});
			willThrow(new Runnable() { public void run() {
				annot.addBorderDashItem(1.0f);
			}});
			willThrow(new Runnable() { public void run() {
				annot.setBorderDashPattern(new float[] { 1, 0 });
			}});
		}
		wontThrow(new Runnable() { public void run() {
			annot.hasBorderEffect();
		}});
		if (annot.hasBorderEffect())
		{
			wontThrow(new Runnable() { public void run() {
				annot.getBorderEffect();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.setBorderEffect(PDFAnnotation.BORDER_EFFECT_NONE);
			}});
			wontThrow(new Runnable() { public void run() {
				annot.getBorderEffectIntensity();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.setBorderEffectIntensity(0.0f);
			}});
		}
		else
		{
			willThrow(new Runnable() { public void run() {
				annot.getBorderEffect();
			}});
			willThrow(new Runnable() { public void run() {
				annot.setBorderEffect(PDFAnnotation.BORDER_EFFECT_NONE);
			}});
			willThrow(new Runnable() { public void run() {
				annot.getBorderEffectIntensity();
			}});
			willThrow(new Runnable() { public void run() {
				annot.setBorderEffectIntensity(0.0f);
			}});
		}
		wontThrow(new Runnable() { public void run() {
			annot.hasQuadPoints();
		}});

		if (annot.hasQuadPoints())
		{
			wontThrow(new Runnable() { public void run() {
				annot.getQuadPointCount();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.getQuadPoint(0);
			}});
			wontThrow(new Runnable() { public void run() {
				annot.clearQuadPoints();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.addQuadPoint(new Quad(0, 0, 0, 0, 1, 1, 1, 1));
			}});
		}
		else
		{
			willThrow(new Runnable() { public void run() {
				annot.getQuadPointCount();
			}});
			willThrow(new Runnable() { public void run() {
				annot.getQuadPoint(0);
			}});
			willThrow(new Runnable() { public void run() {
				annot.clearQuadPoints();
			}});
			willThrow(new Runnable() { public void run() {
				annot.addQuadPoint(new Quad(0, 0, 0, 0, 1, 1, 1, 1));
			}});
		}
		wontThrow(new Runnable() { public void run() {
			annot.hasVertices();
		}});
		if (annot.hasVertices())
		{
			wontThrow(new Runnable() { public void run() {
				annot.getVertexCount();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.getVertex(0);
			}});
			wontThrow(new Runnable() { public void run() {
				annot.clearVertices();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.addVertex(0, 0);
			}});
		}
		else
		{
			willThrow(new Runnable() { public void run() {
				annot.getVertexCount();
			}});
			willThrow(new Runnable() { public void run() {
				annot.getVertex(0);
			}});
			willThrow(new Runnable() { public void run() {
				annot.clearVertices();
			}});
			willThrow(new Runnable() { public void run() {
				annot.addVertex(0, 0);
			}});
		}
		wontThrow(new Runnable() { public void run() {
			annot.hasInkList();
		}});
		if (annot.hasInkList())
		{
			wontThrow(new Runnable() { public void run() {
				annot.getInkListCount();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.getInkListStrokeCount(0);
			}});
			wontThrow(new Runnable() { public void run() {
				annot.clearInkList();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.addInkListStroke();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.addInkListStrokeVertex(0, 0);
			}});
		}
		else
		{
			willThrow(new Runnable() { public void run() {
				annot.getInkListCount();
			}});
			willThrow(new Runnable() { public void run() {
				annot.getInkListStrokeCount(0);
			}});
			willThrow(new Runnable() { public void run() {
				annot.clearInkList();
			}});
			willThrow(new Runnable() { public void run() {
				annot.addInkListStroke();
			}});
			willThrow(new Runnable() { public void run() {
				annot.addInkListStrokeVertex(0, 0);
			}});
		}
		wontThrow(new Runnable() { public void run() {
			annot.hasIcon();
		}});
		if (annot.hasIcon())
		{
			wontThrow(new Runnable() { public void run() {
				annot.getIcon();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.setIcon("");
			}});
		}
		else
		{
			willThrow(new Runnable() { public void run() {
				annot.getIcon();
			}});
			willThrow(new Runnable() { public void run() {
				annot.setIcon("");
			}});
		}
		wontThrow(new Runnable() { public void run() {
			annot.hasOpen();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.isOpen();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.setIsOpen(false);
		}});
		wontThrow(new Runnable() { public void run() {
			annot.eventEnter();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.eventExit();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.eventDown();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.eventUp();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.eventEnter();
			annot.eventDown();
			annot.eventUp();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.eventFocus();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.eventBlur();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.update();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.getObject();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.getLanguage();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.setLanguage(PDFDocument.LANGUAGE_UNSET);
		}});
		if (annot.hasQuadding())
		{
			wontThrow(new Runnable() { public void run() {
				annot.getQuadding();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.setQuadding(0);
			}});
		}
		else
		{
			willThrow(new Runnable() { public void run() {
				annot.getQuadding();
			}});
			willThrow(new Runnable() { public void run() {
				annot.setQuadding(0);
			}});
		}
		wontThrow(new Runnable() { public void run() {
			annot.hasLine();
		}});
		if (annot.hasLine())
		{
			wontThrow(new Runnable() { public void run() {
				annot.getLine();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.setLine(new Point(0, 0), new Point(1, 1));
			}});
		}
		else
		{
			willThrow(new Runnable() { public void run() {
				annot.getLine();
			}});
			willThrow(new Runnable() { public void run() {
				annot.setLine(new Point(0, 0), new Point(1, 1));
			}});
		}
		wontThrow(new Runnable() { public void run() {
			annot.getDefaultAppearance();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.setDefaultAppearance("font", 12, new float[]{ 1, 0, 0 });
		}});
		wontThrow(new Runnable() { public void run() {
			annot.setAppearance(new DisplayList(new Rect(0, 0, 100, 100)));
		}});
		if (annot.hasRect())
		{
			wontThrow(new Runnable() { public void run() {
				annot.setAppearance(new Image(new Pixmap(ColorSpace.DeviceRGB, new Rect(0, 0, 100, 100), true)));
			}});
		}
		else
		{
			willThrow(new Runnable() { public void run() {
				annot.setAppearance(new Image(new Pixmap(ColorSpace.DeviceRGB, new Rect(0, 0, 100, 100), true)));
			}});
		}
		wontThrow(new Runnable() { public void run() {
			annot.hasFileSpecification();
		}});
		if (annot.hasFileSpecification())
		{
			wontThrow(new Runnable() { public void run() {
				annot.getFileSpecification();
			}});
			wontThrow(new Runnable() { public void run() {
				annot.setFileSpecification(fs);
			}});
		}
		else
		{
			willThrow(new Runnable() { public void run() {
				annot.getFileSpecification();
			}});
			willThrow(new Runnable() { public void run() {
				annot.setFileSpecification(null);
			}});
		}
		wontThrow(new Runnable() { public void run() {
			annot.getHiddenForEditing();
		}});
		wontThrow(new Runnable() { public void run() {
			annot.setHiddenForEditing(false);
		}});
	}

	public void interactWithAnnotationOnDeletedPage() {
		PDFDocument pdf = new PDFDocument();

		PDFObject pageobj = pdf.addPage(new Rect(0, 0, 421, 595), 0, null, "");
		pdf.insertPage(-1, pageobj);

		PDFPage page = (PDFPage) pdf.loadPage(0);

		final PDFAnnotation annot_square = page.createAnnotation(PDFAnnotation.TYPE_SQUARE);
		final PDFAnnotation annot_polyline = page.createAnnotation(PDFAnnotation.TYPE_POLY_LINE);
		final PDFAnnotation annot_line = page.createAnnotation(PDFAnnotation.TYPE_LINE);
		final PDFAnnotation annot_text = page.createAnnotation(PDFAnnotation.TYPE_TEXT);
		final PDFAnnotation annot_highlight = page.createAnnotation(PDFAnnotation.TYPE_HIGHLIGHT);
		final PDFAnnotation annot_ink = page.createAnnotation(PDFAnnotation.TYPE_INK);
		final PDFAnnotation annot_fileattachment = page.createAnnotation(PDFAnnotation.TYPE_FILE_ATTACHMENT);

		final PDFObject fs = pdf.addEmbeddedFile("", "", new Buffer(), -1, -1, false);

		pdf.deletePage(0);

		interactWithAnnotationOnDeletedPage(annot_square, fs);
		//interactWithAnnotationOnDeletedPage(annot_polyline, fs);
		interactWithAnnotationOnDeletedPage(annot_line, fs);
		interactWithAnnotationOnDeletedPage(annot_text, fs);
		//interactWithAnnotationOnDeletedPage(annot_highlight, fs);
		//interactWithAnnotationOnDeletedPage(annot_ink, fs);
		interactWithAnnotationOnDeletedPage(annot_fileattachment, fs);
	}

	public void createAnnotationOnDeletedPage() {
		PDFDocument pdf = new PDFDocument();

		PDFObject pageobj = pdf.addPage(new Rect(0, 0, 421, 595), 0, null, "");
		pdf.insertPage(-1, pageobj);

		final PDFPage page = (PDFPage) pdf.loadPage(0);

		pdf.deletePage(0);

		wontThrow(new Runnable() { public void run() {
			page.createAnnotation(PDFAnnotation.TYPE_SQUARE);
		}});
	}

	public void readStreamFromDeletedObject() {
		PDFDocument pdf = new PDFDocument();

		PDFObject pageobj = pdf.addPage(new Rect(0, 0, 421, 595), 0, null, "");
		pdf.insertPage(-1, pageobj);

		PDFPage page = (PDFPage) pdf.loadPage(0);
		PDFAnnotation annot = page.createAnnotation(PDFAnnotation.TYPE_SQUARE);

		final PDFObject obj = annot.getObject();

		pdf.deleteObject(obj);

		wontThrow(new Runnable() { public void run() {
			obj.isIndirect();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.isBoolean();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.isInteger();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.isReal();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.isNumber();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.isString();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.isName();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.isArray();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.isDictionary();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.isStream();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.asBoolean();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.asInteger();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.asFloat();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.asIndirect();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.asName();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.asString();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.asByteString();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.toString();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.resolve();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.equals(null);
		}});
		willThrow(new Runnable() { public void run() {
			obj.readStream();
		}});
		willThrow(new Runnable() { public void run() {
			obj.readRawStream();
		}});
		wontThrow(new Runnable() { public void run() {
			obj.writeObject(null);
		}});
		willThrow(new Runnable() { public void run() {
			obj.writeStream(new Buffer());
		}});
		willThrow(new Runnable() { public void run() {
			obj.writeRawStream(new Buffer());
		}});
		wontThrow(new Runnable() { public void run() {
			obj.get("key");
		}});
		willThrow(new Runnable() { public void run() {
			obj.put("key", "value");
		}});
		willThrow(new Runnable() { public void run() {
			obj.delete("key");
		}});
		wontThrow(new Runnable() { public void run() {
			obj.size();
		}});
		willThrow(new Runnable() { public void run() {
			obj.push(true);
		}});
	}

	public void interactWithDeletedLink() {
		PDFDocument pdf = new PDFDocument();

		PDFObject pageobj = pdf.addPage(new Rect(0, 0, 421, 595), 0, null, "");
		pdf.insertPage(-1, pageobj);

		PDFPage page = (PDFPage) pdf.loadPage(0);

		final Link link = page.createLink(new Rect(0, 0, 100, 100), "http://example.com");

		page.deleteLink(link);

		wontThrow(new Runnable() { public void run() {
			link.getBounds();
		}});
		wontThrow(new Runnable() { public void run() {
			link.setBounds(new Rect(0, 0, 1, 1));
		}});
		wontThrow(new Runnable() { public void run() {
			link.getURI();
		}});
		wontThrow(new Runnable() { public void run() {
			link.setURI("http://subdomain.example.com");
		}});
	}

	public static void main(String[] args)
	{
		TestAPI t = new TestAPI();
		t.loadPageFromDocumentWithoutPages();
		t.interactWithDeletedAnnotation();
		t.interactWithAnnotationOnDeletedPage();
		t.createAnnotationOnDeletedPage();
		t.readStreamFromDeletedObject();
		t.interactWithDeletedLink();
	}
}
