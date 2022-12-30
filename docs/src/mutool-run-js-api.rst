.. Copyright (C) 2001-2022 Artifex Software, Inc.
.. All Rights Reserved.


.. title:: mutool JavaScript API

.. include:: header.rst

.. meta::
   :description: MuPDF documentation
   :keywords: MuPDF, pdf, epub


.. _mutool_run_javascript_api:







Base :title:`MuPDF` properties & methods
------------------------------------------



.. method:: setUserCSS(userStylesheet, usePublisherStyles)

    Set user styles and whether to use publisher styles when laying out reflowable documents.

    :arg userStylesheet: Link to CSS stylesheet file.
    :arg usePublisherStyles: ``Boolean``.








Matrices and Rectangles
----------------------------

These are not classes as such but array definitions. All dimensions are in points unless otherwise specified.



.. _mutool_run_js_api_matrix:

Matrices
~~~~~~~~~~~~

Matrices are simply 6-element arrays representing a 3-by-3 transformation matrix as:


.. code-block:: bash

    / a b 0 \
    | c d 0 |
    \ e f 1 /

This matrix is represented in :title:`JavaScript` as ``[a,b,c,d,e,f]``.

.. property:: Identity

    The identity matrix, short hand for ``[1,0,0,1,0,0]``.

.. method:: Scale(sx, sy)

    Returns a scaling matrix, short hand for ``[sx,0,0,sy,0,0]``.

.. method:: Translate(tx, ty)

    Return a translation matrix, short hand for ``[1,0,0,1,tx,ty]``.

.. method:: Concat(a, b)

    Concatenate matrices ``a`` and ``b``. Bear in mind that matrix multiplication is not commutative.

.. _mutool_run_js_api_rectangle:

Rectangles
~~~~~~~~~~~~

Rectangles are 4-element arrays, specifying the minimum and maximum corners (typically upper left and lower right, in a coordinate space with the origin at the top left with descending y): ``[ulx,uly,lrx,lry]``.

If the minimum x coordinate is bigger than the maximum x coordinate, :title:`MuPDF` treats the rectangle as infinite in size.



.. _mutool_run_js_api_colors:

Colors
----------

Colors are specified as arrays with the appropriate number of components for the color space. Each number is a floating point between ``0`` and ``1`` for the color value.

For example:

- In the ``DeviceRGB`` color space a color would be ``[Red,Green,Blue]``. A full green color would therefore be ``[0,1,0]``.
- In the ``DeviceGray`` color space a color would be ``[Black]``. A full black color would therefore be ``[1]``.



.. _mutool_run_js_api_alpha:

Alpha
~~~~~~~~~~~~

Alpha values are floats between ``0`` and ``1``, whereby ``0`` denotes full transparency & ``1`` denotes full opacity.


Buffer
-----------

The ``Buffer`` objects are used for working with binary data. They can be used much like arrays, but are much more efficient since they only store bytes.



.. method:: new()

    *Constructor method*.

    Create a new empty buffer.

    :return: ``Buffer``.

    **Example**

    .. code-block:: javascript

        var buffer = new Buffer();


.. method:: readFile(fileName)

    *Constructor method*.

    Create a new buffer with the contents of a file.

    :arg fileName: The path to the file to read.

    :return: ``Buffer``.

    **Example**

    .. code-block:: javascript

        var buffer = readFile("my_file.pdf");

----

**Instance properties**



.. property:: length

   The number of bytes in the buffer. `Read-only`.


.. property:: [n]

    Read/write the byte at index 'n'. Will throw exceptions on out of bounds accesses.


    **Example**

    .. code-block:: javascript

        var byte = buffer[0];




----

**Instance methods**

.. method:: writeByte(b)

    Append a single byte to the end of the buffer.

    :arg b: The byte value.


.. method:: writeRune(c)

    Encode a unicode character as UTF-8 and append to the end of the buffer.

    :arg c: The character value.

.. method:: writeLine(...)

    Append arguments to the end of the buffer, separated by spaces, ending with a newline.

    :arg ...: List of arguments.

.. method:: write(...)

    Append arguments to the end of the buffer, separated by spaces.

    :arg ...: List of arguments.

.. method:: writeBuffer(data)

    Append the contents of the 'data' buffer to the end of the buffer.

    :arg data: Data buffer.

.. method:: save(fileName)

    Write the contents of the buffer to a file.

    :arg fileName: Filename to save to.




Document
--------------------

:title:`MuPDF` can open many document types (:title:`PDF`, :title:`XPS`, :title:`CBZ`, :title:`EPUB`, :title:`FB2` and a handful of image formats).




.. method:: new(fileName)

    *Constructor method*.

    Open the named document.

    :arg fileName: Filename to open.


    :return: ``Document``.


    **Example**

    .. code-block:: javascript

        var document = new Document("my_pdf.pdf");



.. method:: needsPassword()

    Returns *true* if a password is required to open this password protected PDF.

    :return: ``Boolean``.


.. method:: authenticatePassword(password)

    Returns *true* if the password matches.

    :arg password: The password to attempt authentication with.
    :return: ``Boolean``.


.. method:: getMetaData(key)

    Return various meta data information. The common keys are: "format", "encryption", "info:Author", and "info:Title".

    :arg key: ``String``.
    :return: ``String``.

.. method:: layout(pageWidth, pageHeight, fontSize)

    Layout a reflowable document (:title:`EPUB`, :title:`FB2`, or :title:`XHTML`) to fit the specified page and font size.

    :arg pageWidth: ``Int``.
    :arg pageHeight: ``Int``.
    :arg fontSize: ``Int``.

.. method:: countPages()

    Count the number of pages in the document. This may change if you call the layout function with different parameters.

    :return: ``Int``.

.. method:: loadPage(number)

    Returns a ``Page`` object for the given page number. Page number zero (0) is the first page in the document.

    :return: ``Page``.

.. method:: loadOutline()

    Returns an array with the outline (also known as "table of contents" or "bookmarks"). In the array is an object for each heading with the property 'title', and a property 'page' containing the page number. If the object has a 'down' property, it contains an array with all the sub-headings for that entry.

    :return: ``[]``.

.. method:: isPDF()

    Returns *true* if the document is a :title:`PDF` document.

    :return: ``Boolean``.



.. _mutool_run_js_api_page:

Page
--------

.. method:: bound()

    Returns a :ref:`rectangle<mutool_run_js_api_rectangle>` containing the page dimensions.

    :return: ``[ulx,uly,lrx,lry]``.

.. method:: run(device, transform, skipAnnotations)

    Calls device functions for all the contents on the page, using the specified ``transform`` `matrix<mutool_run_js_api_matrix>`. The ``device`` can be one of the built-in devices or a :title:`JavaScript` object with methods for the device calls. The ``transform`` maps from user space points to device space pixels. If ``skipAnnotations`` is *true* then annotations are ignored.

    :arg device: The device object.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg skipAnnotations: ``Boolean``.

.. method:: toPixmap(transform, colorspace, alpha, skipAnnotations)

    Render the page into a :ref:`Pixmap<mutool_run_js_api_pixmap>`, using the ``transform`` and ``colorspace``. If ``alpha`` is *true*, the page will be drawn on a transparent background, otherwise white.

    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg colorspace: ``ColorSpace``.
    :arg alpha: ``Boolean``.
    :arg skipAnnotations: ``Boolean``.

.. method:: toDisplayList(skipAnnotations)

    Record the contents on the page into a ``DisplayList``.

    :arg skipAnnotations: ``Boolean``.

.. method:: toStructuredText(options)

    Extract the text on the page into a ``StructuredText`` object. The options argument is a comma separated list of flags: "preserve-ligatures", "preserve-whitespace", "preserve-spans", and "preserve-images".

    :arg options: ``String``.



.. method:: search(needle)

    Search for ``needle`` text on the page, and return an array with :ref:`rectangles<mutool_run_js_api_rectangle>` of all matches found.

    :arg needle: ``String``.
    :return: ``[]``.

.. method:: getAnnotations()

    Return array of all ``Annotations`` on the page.

    :return: ``[]``.

.. method:: getLinks()

    Return an array of all the links on the page. Each link is an object with a 'bounds' property, and either a 'page' or 'uri' property, depending on whether it's an internal or external link.

    :return: ``[]``.

.. method:: isPDF()

    Returns *true* if the page is from a :title:`PDF` document.

    :return: ``Boolean``.


Annotation
--------------

.. method:: bound()

    Returns a :ref:`rectangle<mutool_run_js_api_rectangle>` containing the location and dimension of the annotation.


.. method:: run(device, transform)

    Calls device functions to draw the annotation.

    :arg device: The device to use.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.

.. method:: toPixmap(transform, colorspace, alpha)

    Render the annotation into a :ref:`Pixmap<mutool_run_js_api_pixmap>`, using the transform and colorspace.

    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg colorspace: ``ColorSpace``.
    :arg alpha: ``Boolean``. If alpha is *true*, the annotation will be drawn on a transparent background, otherwise white.


.. method:: toDisplayList()

    Record the contents of the annotation into a ``DisplayList``.

.. method:: isPDF()

    Returns *true* if the annotation is from a :title:`PDF` document.

    :return: ``Boolean``.



.. _mutool_run_js_api_structured_text:


StructuredText
----------------------------

``StructuredText`` objects hold text from a page that has been analyzed and grouped into blocks, lines and spans.

.. method:: search(needle)

    Search the text for all instances of ``needle``, and return an array with :ref:`rectangles<mutool_run_js_api_rectangle>` of all matches found.

    :arg needle: ``String``.
    :return: ``[]``.

.. method:: highlight(p, q)

    Return an array with :ref:`rectangles<mutool_run_js_api_rectangle>` needed to highlight a selection defined by the start and end points.

    :arg p: Start point in format ``{x:y}``.
    :arg q: End point in format ``{x:y}``.

.. method:: copy(p, q)

    Return the text from the selection defined by the start and end points.

    :arg p: Start point in format ``{x:y}``.
    :arg q: End point in format ``{x:y}``.



.. _mutool_run_javascript_api_colorspace:


ColorSpace
----------------------------




.. property:: DeviceGray

    The default grayscale colorspace.

.. property:: DeviceRGB

    The default RGB colorspace.

.. property:: DeviceBGR

    The default RGB colorspace, but with components in reverse order.

.. property:: DeviceCMYK

    The default CMYK colorspace.



.. method:: getNumberOfComponents()

    A grayscale colorspace has one component, RGB has 3, CMYK has 4, and DeviceN may have any number of components.


    **Example**

    .. code-block:: javascript

        var cs = DeviceRGB;
        var num = cs.getNumberOfComponents();
        print(num);  //3




.. _mutool_run_js_api_pixmap:

Pixmap
----------------------------

A ``Pixmap`` object contains a color raster image (short for pixel map). The components in a pixel in the ``Pixmap`` are all byte values, with the transparency as the last component. A ``Pixmap`` also has a location (x, y) in addition to its size; so that they can easily be used to represent tiles of a page.




.. method:: new(colorspace, bounds, alpha)

    *Constructor method*.

    Create a new pixmap. The pixel data is **not** initialized; and will contain garbage.

    :arg colorspace: ``Colorspace``.
    :arg bounds: ``[ulx,uly,lrx,lry]`` :ref:`Rectangle<mutool_run_js_api_rectangle>`.
    :arg alpha: ``Boolean``.

    :return: ``Pixmap``.

    **Example**

    .. code-block:: javascript

        var pixmap = new Pixmap(DeviceRGB, [0,0,100,100], true);


.. method:: clear(value)

    Clear the pixels to the specified value. Pass ``255`` for white, or ``undefined`` for transparent.

    :arg value: Pixel value.

.. method:: bound()

    Return the pixmap bounds.

    :return: ``[ulx,uly,lrx,lry]`` :ref:`Rectangle<mutool_run_js_api_rectangle>`.

.. method:: getWidth()

    :return: The width value.

.. method:: getHeight()

    :return: The height value.

.. method:: getNumberOfComponents()

    Number of colors; plus one if an alpha channel is present.

    :return: Number of color components.

.. method:: getAlpha()

    *True* if alpha channel is present.

    :return: ``Boolean``.

.. method:: getStride()

    Number of bytes per row.

    :return: ``Int``.

.. method:: getColorSpace()

    Returns the ``ColorSpace`` for the ``Pixmap``.

    :return: ``ColorSpace``.

.. method:: getXResolution()

    Returns the x resolution for the ``Pixmap``.

    :return: ``Int`` Image resolution in dots per inch.


.. method:: getYResolution()

    Returns the x resolution for the ``Pixmap``.

    :return: ``Int`` Image resolution in dots per inch.



.. method:: getSample(x, y, k)

    Get the value of component ``k`` at position ``x``, ``y`` (relative to the image origin: 0, 0 is the top left pixel).

    :arg x: X co-ordinate.
    :arg y: Y co-ordinate.
    :arg k: Component value.


.. method:: saveAsPNG(fileName, saveAlpha)

    Save the ``Pixmap`` as a :title:`PNG`. Only works for Gray and RGB images.



DrawDevice
----------------------------

The ``DrawDevice`` can be used to render to a :ref:`Pixmap<mutool_run_js_api_pixmap>`; either by running a :ref:`Page<mutool_run_js_api_page>` with it or by calling its methods directly.




.. method:: new(transform, pixmap)

    *Constructor method*.

    Create a device for drawing into a ``Pixmap``. The ``Pixmap`` bounds used should match the transformed page bounds, or you can adjust them to only draw a part of the page.

    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg pixmap: ``Pixmap``.

    **Example**

    .. code-block:: javascript

        var pixmap = new DrawDevice(Identity, pixmap);



.. _mutool_run_js_api_display_list:


DisplayList
------------------------

A display list records all the device calls for playback later. If you want to run a page through several devices, or run it multiple times for any other reason, recording the page to a display list and replaying the display list may be a performance gain since then you can avoid reinterpreting the page each time. Be aware though, that a display list will keep all the graphics required in memory, so will increase the amount of memory required.




.. method:: new(mediabox)

    *Constructor method*.

    Create an empty display list. The mediabox rectangle has the bounds of the page in points.

    :arg mediabox: ``[ulx,uly,lrx,lry]`` :ref:`Rectangle<mutool_run_js_api_rectangle>`.

    :return: ``DisplayList``.

    **Example**

    .. code-block:: javascript

        var displayList = new DisplayList([0,0,100,100]);



.. method:: run(device, transform)

    Play back the recorded device calls onto the device.

    :arg device: ``Device``.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.


.. method:: bound()

    Returns a rectangle containing the dimensions of the display list contents.

    :return: ``[ulx,uly,lrx,lry]`` :ref:`Rectangle<mutool_run_js_api_rectangle>`.


.. method:: toPixmap(transform, colorspace, alpha)

    Render display list to a ``Pixmap``.

    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg colorspace: ``ColorSpace``.
    :arg alpha: ``Boolean``. If alpha is *true*, the annotation will be drawn on a transparent background, otherwise white.



.. method:: toStructuredText(options)


    Extract the text on the page into a ``StructuredText`` object. The options argument is a comma separated list of flags: "preserve-ligatures", "preserve-whitespace", "preserve-spans", and "preserve-images".

    :arg options: ``String``.


.. method:: search(needle)

    Search the display list text for all instances of 'needle', and return an array with :ref:`rectangles<mutool_run_js_api_rectangle>` of all matches found.

    :arg needle: ``String``.
    :return: ``[]``.



.. _mutool_run_js_api_display_list_device:

DisplayListDevice
--------------------------------------------------------



.. method:: new DisplayListDevice(displayList)

    *Constructor method*.

    Create a device for recording onto a display list.

    :arg displayList: ``DisplayList``.

    :return: ``DisplayListDevice``.

    **Example**

    .. code-block:: javascript

        var displayListDevice = new DisplayListDevice(my_display_list);



.. _mutool_run_js_api_device:

Device
------------------

All built-in devices have the methods listed below. Any function that accepts a device will also accept a :title:`JavaScript` object with the same methods. Any missing methods are simply ignored, so you only need to create methods for the device calls you care about.

Many of the methods take graphics objects as arguments: ``Path``, ``Text``, ``Image`` and ``Shade``.

Colors are specified as arrays with the appropriate number of components for the color space.

The methods that clip graphics must be balanced with a corresponding ``popClip``.

.. method:: fillPath(path, evenOdd, transform, colorspace, color, alpha)

    Fill a path.

    :arg path: ``Path`` object.
    :arg evenOdd: The `even odd rule`_ to use.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg colorspace: The :ref:`ColorSpace<mutool_run_javascript_api_colorspace>`.
    :arg color: The :ref:`color value<mutool_run_js_api_colors>`.
    :arg alpha: The :ref:`alpha value<mutool_run_js_api_alpha>`.


    **Example**

    .. code-block:: bash

        device.fillPath(path, false, Identity, DeviceRGB, [1,0,0], 1);

.. method:: strokePath(path, stroke, transform)

    Stroke a path.

    :arg path: ``Path`` object.
    :arg stroke: The :ref:`stroke dictionary<mutool_run_js_api_stroke_dictionary>`.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg colorspace: The :ref:`ColorSpace<mutool_run_javascript_api_colorspace>`.
    :arg color: The :ref:`color value<mutool_run_js_api_colors>`.
    :arg alpha: The :ref:`alpha value<mutool_run_js_api_alpha>`.

    **Example**

    .. code-block:: bash

        device.strokePath(path, {dashes:[5,10], lineWidth:3, lineCap:'Round'}, Identity, DeviceRGB, [0,1,0], 0.5);

.. method:: clipPath(path, evenOdd, transform, colorspace, color, alpha)

    Clip a path.

    :arg path: ``Path`` object.
    :arg evenOdd: The `even odd rule`_ to use.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.


.. method:: clipStrokePath(path, stroke, transform)

    Clip & stroke a path.

    :arg path: ``Path`` object.
    :arg stroke: The :ref:`stroke dictionary<mutool_run_js_api_stroke_dictionary>`.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.



.. method:: fillText(text, transform, colorspace, color, alpha)

    Fill a text object.

    :arg text: ``Text`` object.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg colorspace: The :ref:`ColorSpace<mutool_run_javascript_api_colorspace>`.
    :arg color: The :ref:`color value<mutool_run_js_api_colors>`.
    :arg alpha: The :ref:`alpha value<mutool_run_js_api_alpha>`.

.. method:: strokeText(text, stroke, transform, colorspace, color, alpha)

    Stroke a text object.

    :arg text: ``Text`` object.
    :arg stroke: The :ref:`stroke dictionary<mutool_run_js_api_stroke_dictionary>`.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg colorspace: The :ref:`ColorSpace<mutool_run_javascript_api_colorspace>`.
    :arg color: The :ref:`color value<mutool_run_js_api_colors>`.
    :arg alpha: The :ref:`alpha value<mutool_run_js_api_alpha>`.


.. method:: clipText(text, transform)

    Clip a text object.

    :arg text: ``Text`` object.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.

.. method:: clipStrokeText(text, stroke, transform)

    Clip & stroke a text object.

    :arg text: ``Text`` object.
    :arg stroke: The :ref:`stroke dictionary<mutool_run_js_api_stroke_dictionary>`.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.

.. method:: ignoreText(text, transform)

    Invisible text that can be searched but should not be visible, such as for overlaying a scanned OCR image.

    :arg text: ``Text`` object.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.



.. method:: fillShade(shade, transform, alpha)

    Fill a shade (a.k.a. gradient).

    .. note::

        TODO: the details of gradient fills are not exposed to :title:`JavaScript` yet.


    :arg shade: The gradient.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg alpha: The :ref:`alpha value<mutool_run_js_api_alpha>`.

.. method:: fillImage(image, transform, alpha)

    Draw an image. An image always fills a unit rectangle ``[0,0,1,1]``, so must be transformed to be placed and drawn at the appropriate size.

    :arg image: ``Image`` object.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg alpha: The :ref:`alpha value<mutool_run_js_api_alpha>`.

.. method:: fillImageMask(image, transform, colorspace, color, alpha)

    An image mask is an image without color. Fill with the color where the image is opaque.

    :arg image: ``Image`` object.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg colorspace: The :ref:`ColorSpace<mutool_run_javascript_api_colorspace>`.
    :arg color: The :ref:`color value<mutool_run_js_api_colors>`.
    :arg alpha: The :ref:`alpha value<mutool_run_js_api_alpha>`.

.. method:: clipImageMask(image, transform)

    Clip graphics using the image to mask the areas to be drawn.

    :arg image: ``Image`` object.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.


.. method:: popClip()

    Pop the clip mask installed by the last clipping operation.


.. method:: beginMask(area, luminosity, backdropColorspace, backdropColor)

    Create a soft mask. Any drawing commands between ``beginMask`` and ``endMask`` are grouped and used as a clip mask.

    :arg area: ``Path`` Mask area.
    :arg luminosity: ``Boolean`` If luminosity is *true*, the mask is derived from the luminosity (grayscale value) of the graphics drawn; otherwise the color is ignored completely and the mask is derived from the alpha of the group.
    :arg backdropColorspace: The :ref:`ColorSpace<mutool_run_javascript_api_colorspace>`.
    :arg backdropColor: The :ref:`color value<mutool_run_js_api_colors>`.


.. method:: endMask()

    Ends the mask.






.. method:: beginGroup(area, isolated, knockout, blendmode, alpha)

    Push/pop a transparency blending group. See the PDF reference for details on ``isolated`` and ``knockout``.

    :arg area: ``Path`` Blend area.
    :arg isolated:
    :arg knockout:
    :arg blendmode: Blendmode is one of the standard :title:`PDF` blend modes: "Normal", "Multiply", "Screen", etc.
    :arg alpha: The :ref:`alpha value<mutool_run_js_api_alpha>`.


.. method:: endGroup()

    Ends the blending group.


.. method:: beginTile(areaRect, viewRect, xStep, yStep, transform)

    Draw a tiling pattern. Any drawing commands between ``beginTile`` and ``endTile`` are grouped and then repeated across the whole page. Apply a clip mask to restrict the pattern to the desired shape.

    :arg areaRect: ``[ulx,uly,lrx,lry]`` :ref:`Rectangle<mutool_run_js_api_rectangle>`.
    :arg viewRect: ``[ulx,uly,lrx,lry]`` :ref:`Rectangle<mutool_run_js_api_rectangle>`.
    :arg xStep: Integer representing ``x`` step.
    :arg yStep: Integer representing ``y`` step.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.


.. method:: endTile()

    Ends the tiling pattern.



.. method:: close()

    Tell the device that we are done, and flush any pending output.




----

.. _mutool_run_js_api_stroke_dictionary:

**Stroke**

The stroking state is a dictionary with keys for:

    - ``startCap``, ``dashCap``, ``endCap``
        "Butt", "Round", "Square", or "Triangle".

    - ``lineCap``
        Set ``startCap``, ``dashCap``, and ``endCap`` all at once.

    - ``lineJoin``
        "Miter", "Round", "Bevel", or "MiterXPS".

    - ``lineWidth``
        Thickness of the line.

    - ``miterLimit``
        Maximum ratio of the miter length to line width, before beveling the join instead.

    - ``dashPhase``
        Starting offset for dash pattern.

    - ``dashes``
        Array of on/off dash lengths.


**Example:**

    ``{dashes:[5,10], lineWidth:3, lineCap:'Round'}``



.. _mutool_run_js_api_path:


Path
--------

A ``Path`` object represents vector graphics as drawn by a pen. A path can be either stroked or filled, or used as a clip mask.


.. method:: new()

    *Constructor method*.

    Create a new empty path.

    :return: ``Path``.

    **Example**

    .. code-block:: javascript

        var path = new Path();


.. method:: moveTo(x, y)

    Lift and move the pen to the coordinate.

    :arg x: X coordinate.
    :arg y: Y coordinate.

.. method:: lineTo(x, y)

    Draw a line to the coordinate.

    :arg x: X coordinate.
    :arg y: Y coordinate.

.. method:: curveTo(x1, y1, x2, y2, x3, y3)

    Draw a cubic bezier curve to ``(x3,y3)`` using ``(x1,y1)`` and ``(x2,y2)`` as control points.

    :arg x1: X1 coordinate.
    :arg y1: Y1 coordinate.
    :arg x2: X2 coordinate.
    :arg y2: Y2 coordinate.
    :arg x3: X3 coordinate.
    :arg y3: Y3 coordinate.

.. method:: closePath()

    Close the path by drawing a line to the last moveTo.

.. method:: rect(x1, y1, x2, y2)

    Shorthand for ``moveTo``, ``lineTo``, ``lineTo``, ``lineTo``, ``closePath`` to draw a rectangle.

    :arg x1: X1 coordinate.
    :arg y1: Y1 coordinate.
    :arg x2: X2 coordinate.
    :arg y2: Y2 coordinate.


.. method:: walk(pathWalker)

    Call ``moveTo``, ``lineTo``, ``curveTo`` and ``closePath`` methods on the ``pathWalker`` object to replay the path.


    :arg pathWalker: The path walker object. A user definable :title:`JavaScript` object which can be used to trigger your own functions on the path methods.


    **Example**

    .. code-block:: javascript

        var myPathWalker = {
            moveTo: function (x, y) { ... do whatever ... },
            lineTo: function (x, y) { ... do whatever ... },
        }

        path.walk(myPathWalker);


Text
--------

A ``Text`` object contains text.


.. method:: new()

    *Constructor method*.

    Create a new empty text object.

    :return: ``Text``.

    **Example**

    .. code-block:: javascript

        var text = new Text();



.. method:: showGlyph(font, transform, glyph, unicode, wmode)

    Add a glyph to the text object.

    Transform is the text matrix, specifying font size and glyph location. For example: ``[size,0,0,-size,x,y]``.

    Glyph and unicode may be ``-1`` for n-to-m cluster mappings. For example, the "fi" ligature would be added in two steps: first the glyph for the 'fi' ligature and the unicode value for 'f'; then glyph ``-1`` and the unicode value for 'i'.

    :arg font: ``Font`` object.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg glyph:
    :arg unicode:
    :arg wmode: ``0`` for horizontal writing, and ``1`` for vertical writing.


.. method:: showString(font, transform, string)

    Add a simple string to the ``Text`` object. Will do font substitution if the font does not have all the unicode characters required.

    :arg font: ``Font`` object.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg string: String content for ``Text`` object.

.. method:: walk(textWalker)

    Call the ``showGlyph`` method on the ``textWalker`` object for each glyph in the text object.

    :arg textWalker: The text walker object. A user definable :title:`JavaScript` object which can be used to trigger your own functions on the text methods.


    **Example**

    .. code-block:: javascript

        var myTextWalker = {
            showGlyph: function (font, transform, glyph, unicode, wmode) { ... do whatever ... },
        }

        text.walk(myTextWalker);


Font
--------

``Font`` objects can be created from :title:`TrueType`, :title:`OpenType`, :title:`Type1` or :title:`CFF` fonts. In :title:`PDF` there are also special :title:`Type3` fonts.


.. method:: new(ref)

    *Constructor method*.

    Create a new font, either using a built-in font name or a file name.

    The built-in standard :title:`PDF` fonts are:

    - :title:`Times-Roman`.
    - :title:`Times-Italic`.
    - :title:`Times-Bold`.
    - :title:`Times-BoldItalic`.
    - :title:`Helvetica`.
    - :title:`Helvetica-Oblique`.
    - :title:`Helvetica-Bold`.
    - :title:`Helvetica-BoldOblique`.
    - :title:`Courier`.
    - :title:`Courier-Oblique`.
    - :title:`Courier-Bold`.
    - :title:`Courier-BoldOblique`.
    - :title:`Symbol`.
    - :title:`ZapfDingbats`.

    The built-in CJK fonts are referenced by language code: ``zh-Hant``, ``zh-Hans``, ``ja``, ``ko``.

    :arg ref: Font name or file name.

    :return: ``Font``.

    **Example**

    .. code-block:: javascript

        var font = new Font("Times-Roman");



.. method:: getName()

    Get the font name.

    :return: ``String``.


.. method:: encodeCharacter(unicode)

    Get the glyph index for a unicode character. Glyph zero (.notdef) is returned if the font does not have a glyph for the character.

    :arg unicode: The unicode character.

    :return: Glyph index.


.. method:: advanceGlyph(glyph, wmode)

    Return advance width for a glyph in either horizontal or vertical writing mode.

    :arg glyph: The glyph.
    :arg wmode: ``0`` for horizontal writing, and ``1`` for vertical writing.

    :return: Width for a glyph.





Image
------------

``Image`` objects are similar to ``Pixmaps``, but can contain compressed data.


.. method:: new(ref)

    *Constructor method*.

    Create a new image from a ``Pixmap`` data, or load an image from a file.

    :return: ``Image``.

    **Example**

    .. code-block:: javascript

        var image = new Image("my-image-file.png");


.. method:: getWidth()

    Get the image width in pixels.

    :return: The width value.

.. method:: getHeight()

    Get the image height in pixels.

    :return: The height value.

.. method:: getXResolution()

    Returns the x resolution for the ``Image``.

    :return: ``Int`` Image resolution in dots per inch.


.. method:: getYResolution()

    Returns the y resolution for the ``Image``.

    :return: ``Int`` Image resolution in dots per inch.

.. method:: getColorSpace()

    Returns the ``ColorSpace`` for the ``Image``.

    :return: ``ColorSpace``.


.. method:: toPixmap(scaledWidth, scaledHeight)

    Create a ``Pixmap`` from the image. The ``scaledWidth`` and ``scaledHeight`` arguments are optional, but may be used to decode a down-scaled ``Pixmap``.

    :arg scaledWidth: Width value.
    :arg scaledHeight: Height value.

    :return: ``Pixmap``.



DocumentWriter
---------------------

``DocumentWriter`` objects are used to create new documents in several formats.


.. method:: new(filename, format, options)

    *Constructor method*.

    Create a new document writer to create a document with the specified format and output options. If format is ``null`` it is inferred from the ``filename`` extension. The ``options`` argument is a comma separated list of flags and key-value pairs.

    The output ``format`` & ``options`` are the same as in the :ref:`mutool convert<mutool_convert>` command.


    :arg filename: The file name to output to.
    :arg format: The file format.
    :arg options: The options as key-value pairs.
    :return: ``DocumentWriter``.

    **Example**

    .. code-block:: javascript

        var writer = new DocumentWriter("out.pdf", "PDF", "");



.. method:: beginPage(mediabox)

    Begin rendering a new page. Returns a ``Device`` that can be used to render the page graphics.

    :arg mediabox: ``[ulx,uly,lrx,lry]`` :ref:`Rectangle<mutool_run_js_api_rectangle>`.

    :return: ``Device``.


.. method:: endPage(device)

    Finish the page rendering. The argument must be the same ``Device`` object that was returned by the ``beginPage`` method.

    :arg mediabox: ``Device``.

.. method:: close()

    Finish the document and flush any pending output.



PDFDocument
-----------------

With :title:`MuPDF` it is also possible to create, edit and manipulate :title:`PDF` documents using low level access to the objects and streams contained in a :title:`PDF` file. A ``PDFDocument`` object is also a ``Document`` object. You can test a ``Document`` object to see if it is safe to use as a ``PDFDocument`` by calling ``document.isPDF()``.


.. method:: new()

    *Constructor method*.

    Create a new empty :title:`PDF` document.

    :return: ``PDFDocument``.

    **Example**

    .. code-block:: javascript

        var pdfDocument = new PDFDocument();


.. method:: new(fileName)

    *Constructor method*.

    Load a :title:`PDF` document from file.

    :return: ``PDFDocument``.

    **Example**

    .. code-block:: javascript

        var pdfDocument = new PDFDocument("my-file.pdf");




.. method:: save(fileName, options)

    Write the ``PDFDocument`` to file. The write options are a string of comma separated options (see the :ref:`mutool convert options<mutool_convert>`).

    :arg fileName: The name of the file to save to.
    :options: The options as key-value pairs.


.. method:: getTrailer()

    The trailer dictionary. This contains indirect references to the "Root" and "Info" dictionaries. See: :ref:`PDF object access<mutool_run_js_api_pdf_object_access>`.

    :return: The trailer dictionary.

.. method:: countObjects()

    Return the number of objects in the :title:`PDF`. Object number ``0`` is reserved, and may not be used for anything. See: :ref:`PDF object access<mutool_run_js_api_pdf_object_access>`.

    :return: Object count.

.. method:: createObject()

    Allocate a new numbered object in the :title:`PDF`, and return an indirect reference to it. The object itself is uninitialized.

    :return: The new object.


.. method:: deleteObject(obj)

    Delete the object referred to by the indirect reference.

    :arg obj: The object to delete.


----

.. _mutool_run_js_api_pdf_object_access:

**PDF Object Access**

A :title:`PDF` document contains objects, similar to those in :title:`JavaScript`: arrays, dictionaries, strings, booleans, and numbers. At the root of the :title:`PDF` document is the trailer object; which contains pointers to the meta data dictionary and the catalog object which contains the pages and other information.

Pointers in :title:`PDF` are also called indirect references, and are of the form "32 0 R" (where 32 is the object number, 0 is the generation, and R is magic syntax). All functions in :title:`MuPDF` dereference indirect references automatically.

:title:`PDF` has two types of strings: ``/Names`` and ``(Strings)``. All dictionary keys are names.

Some dictionaries in :title:`PDF` also have attached binary data. These are called streams, and may be compressed.


.. note::

    ``PDFObjects`` are always bound to the document that created them. Do **NOT** mix and match objects from one document with another document!




----

.. method:: addObject(obj)

    Add 'obj' to the :title:`PDF` as a numbered object, and return an indirect reference to it.

    :arg obj: Object to add.


.. method:: addStream(buffer, object)

    Create a stream object with the contents of ``buffer``, add it to the :title:`PDF`, and return an indirect reference to it. If ``object`` is defined, it will be used as the stream object dictionary.

    :arg buffer: ``Buffer`` object.
    :arg object: The object to stream to.




.. method:: addRawStream(buffer, object)

    Create a stream object with the contents of ``buffer``, add it to the :title:`PDF`, and return an indirect reference to it. If ``object`` is defined, it will be used as the stream object dictionary. The ``buffer`` must contain already compressed data that matches the "Filter" and "DecodeParms".

    :arg buffer: ``Buffer`` object.
    :arg object: The object to stream to.




.. method:: newNull()

    Create a new null object.

    :return: Object.

.. method:: newBoolean(boolean)

    Create a new boolean object.

    :arg boolean: The boolean value.

    :return: Object.

.. method:: newInteger(number)

    Create a new integer object.

    :arg number: The number value.

    :return: Object.

.. method:: newReal(number)

    Create a new real number object.

    :arg number: The number value.

    :return: Object.


.. method:: newString(string)

    Create a new string object.

    :arg string: The string value.

    :return: Object.

.. method:: newName(string)

    Create a new name object.

    :arg string: The string value.

    :return: Object.

.. method:: newIndirect(objectNumber, generation)

    Create a new indirect object.

    :arg objectNumber: The string value.
    :arg generation: The string value.

    :return: Object.

.. method:: newArray()

    Create a new array object.

    :return: Object.

.. method:: newDictionary()

    Create a new dictionary object.

    :return: Object.



----

**PDF Page Access**

All page objects are structured into a page tree, which defines the order the pages appear in.

.. method:: countPages()

    Number of pages in the document.

    :return: Page number.

.. method:: findPage(number)

    Return the ``Page`` object for a page number.

    :arg number: The page number, the first page is number zero.

    :return: ``Page``.

.. method:: deletePage(number)

    Delete the numbered ``Page``.

    :arg number: The page number, the first page is number zero.


.. method:: insertPage(at, page)

    Insert the ``Page`` object in the page tree at the location. If 'at' is -1, at the end of the document.

    Pages consist of a content stream, and a resource dictionary containing all of the fonts and images used.

    :arg at: The index to insert at.
    :arg page: The ``Page`` to insert.


.. method:: addPage(mediabox, rotate, resources, contents)

    Create a new page object. Note: this function does NOT add it to the page tree.

    :arg mediabox: ``[ulx,uly,lrx,lry]`` :ref:`Rectangle<mutool_run_js_api_rectangle>`.
    :arg rotate: Rotation value.
    :arg resources: Resources object.
    :arg contents: Contents string.

    :return: ``Page``.


    **Example**

    .. literalinclude:: ../examples/pdf-create.js
       :caption: docs/examples/pdf-create.js
       :language: javascript


.. method:: addSimpleFont(font, encoding)

    Create a PDF object from the ``Font`` object as a simple font.

    :arg font: ``Font``.
    :arg encoding: The encoding to use. Encoding is either "Latin" (CP-1252), "Greek" (ISO-8859-7), or "Cyrillic" (KOI-8U). The default is "Latin".


.. method:: addCJKFont(font, language, wmode, style)

    Create a PDF object from the Font object as a UTF-16 encoded CID font for the given language ("zh-Hant", "zh-Hans", "ko", or "ja"), writing mode ("H" or "V"), and style ("serif" or "sans-serif").

    :arg font: ``Font``.
    :arg language: ``String``.
    :arg wmode: ``0`` for horizontal writing, and ``1`` for vertical writing.
    :arg style: ``String``.

.. method:: addFont(font)

    Create a :title:`PDF` object from the ``Font`` object as an Identity-H encoded CID font.

    :arg font: ``Font``.


.. method:: addImage(image)

    Create a :title:`PDF` object from the ``Image`` object.

    :arg image: ``Image``.

.. method:: loadImage(obj)

    Load an ``Image`` from a :title:`PDF` object (typically an indirect reference to an image resource).

    :arg obj: ``PDFObject``.



----


The following functions can be used to copy objects from one document to another:



.. method:: graftObject(object)

    Deep copy an object into the destination document. This function will not remember previously copied objects. If you are copying several objects from the same source document using multiple calls, you should use a graft map instead.

    :arg object: Object to graft.


.. method:: newGraftMap()

    Create a graft map on the destination document, so that objects that have already been copied can be found again. Each graft map should only be used with one source document! Make sure to create a new graft map for each source document used.

    :return: ``PDFGraftMap``.





PDFGraftMap
----------------

.. method:: graftObject(object)

    Use the graft map to copy objects, with the ability to remember previously copied objects.

    :arg object: Object to graft.



PDFObject
--------------


All functions that take ``PDFObjects``, do automatic translation between :title:`JavaScript` objects and ``PDFObjects`` using a few basic rules:


- Null, booleans, and numbers are translated directly.
- :title:`JavaScript` strings are translated to :title:`PDF` names, unless they are surrounded by parentheses: "Foo" becomes the :title:`PDF` name /Foo and "(Foo)" becomes the :title:`PDF` string (Foo).
- Arrays and dictionaries are recursively translated to :title:`PDF` arrays and dictionaries. Be aware of cycles though! The translation does NOT cope with cyclic references!
- The translation goes both ways: :title:`PDF` dictionaries and arrays can be accessed similarly to :title:`JavaScript` objects and arrays by getting and setting their properties.


.. property:: length

    Length of the array.


.. method:: get(ref)

    :arg ref: Key or index.

    :return: The value for the key or index.

.. method:: put(key or index, value)

    :arg ref: Key or index.
    :arg value: The value for the key or index.


.. method:: delete(ref)

    Access dictionaries and arrays. Dictionaries and arrays can also be accessed using normal property syntax: ``obj.Foo = 42; delete obj.Foo; x = obj[5]``.

    :arg ref: Key or index.

.. method:: resolve()

    If the object is an indirect reference, return the object it points to; otherwise return the object itself.

    :return: Object.

.. method:: isArray()

    :return: ``Boolean``.

.. method:: isDictionary()

    :return: ``Boolean``.

.. method:: forEach(fun)

    Iterate over all the entries in a dictionary or array and call ``fun`` for each key-value pair.

    :arg fun: Function in the format ``function(key,value){...}``.



.. method:: push(item)

    Append ``item`` to the end of the array.

    :arg item: Item to add.


----

**PDF streams**

The only way to access a stream is via an indirect object, since all streams are numbered objects.


.. method:: isIndirect()

    Is the object an indirect reference.

    :return: ``Boolean``.

.. method:: asIndirect()

    Return the object number the indirect reference points to.

    :return: ``Boolean``.


.. method:: isStream()

    *True* if the object is an indirect reference pointing to a stream.

    :return: ``Boolean``.


.. method:: readStream()

    Read the contents of the stream object into a ``Buffer``.

    :return: ``Buffer``.

.. method:: readRawStream()

    Read the raw, uncompressed, contents of the stream object into a ``Buffer``.

    :return: ``Buffer``.

.. method:: writeObject(obj)

    Update the object the indirect reference points to.

    :arg obj: Object to update.

.. method:: writeStream(buffer)

    Update the contents of the stream the indirect reference points to. This will update the "Length", "Filter" and "DecodeParms" automatically.

    :arg buffer: ``Buffer``.

.. method:: writeRawStream(buffer)

    Update the contents of the stream the indirect reference points to. The buffer must contain already compressed data that matches the "Filter" and "DecodeParms". This will update the "Length" automatically, but leave the "Filter" and "DecodeParms" untouched.


    :arg buffer: ``Buffer``.


----


Primitive :title:`PDF` objects such as booleans, names, and numbers can usually be treated like :title:`JavaScript` values. When that is not sufficient use these functions:


.. method:: isNull()

    Is the object the 'null' object?

    :return: ``Boolean``.

.. method:: isBoolean()

    Is the object a boolean?

    :return: ``Boolean``.

.. method:: asBoolean()

    Get the boolean primitive value.

    :return: ``Boolean``.

.. method:: isNumber()

    Is the object a number?

    :return: ``Boolean``.

.. method:: asNumber()

    Get the number primitive value.

    :return: ``Integer``.

.. method:: isName()

    Is the object a name?

    :return: ``Boolean``.

.. method:: asName()

    Get the name as a string.

    :return: ``String``.

.. method:: isString()

    Is the object a string?

    :return: ``Boolean``.

.. method:: asString()

    Convert a "text string" to a :title:`JavaScript` unicode string.

    :return: ``String``.

.. method:: asByteString()

    Convert a string to an array of byte values.

    :return: ``[...]``.



PDFPage
---------------

.. method:: getAnnotations()

    Return array of all annotations on the page.

    :return: ``[]``.


.. method:: createAnnotation(type)

    Create a new blank annotation of a given type. The type must be one of the annotation subtypes: "Text", "Link", "FreeText", "Line", "Square", "Circle", "Polygon", "PolyLine", "Highlight", "Underline", "Squiggly", "StrikeOut", "Redact", "Stamp", "Caret", "Ink", "Popup", "FileAttachment", "Sound", "Movie", "RichMedia", "Widget", "Screen", "PrinterMark", "TrapNet", "Watermark", "3D" or "Projection".

    :arg type: ``String`` representing annotation type.
    :return: ``PDFAnnotation``.

.. method:: deleteAnnotation(annot)

    Delete the annotation from the page.

    :arg annot: ``PDFAnnotation``.

.. method:: update()

    Loop through all annotations of the page and update them. Returns true if re-rendering is needed because at least one annotation was changed (due to either events or :title:`JavaScript` actions or annotation editing).

.. method:: applyRedactions(blackboxes, imageMethod)

    Apply redaction annotations to the page. Should black boxes be drawn at each redaction or not? Should affected images be ignored, entirely redacted or should just the covered part of the image be redacted?

    :arg blackboxes:
    :arg imageMethod:





PDFAnnotation
----------------------

:title:`PDF` Annotations belong to a specific ``PDFPage`` and may be created/changed/removed. Because annotation appearances may change (for several reasons) it is possible to scan through the annotations on a page and query them whether a re-render is necessary. Finally redaction annotations can be applied to a ``PDFPage``, destructively removing content from the page.


.. method:: bound()

    Returns a rectangle containing the location and dimension of the annotation.

    :return: ``[ulx,uly,lrx,lry]`` :ref:`Rectangle<mutool_run_js_api_rectangle>`.


.. method:: run(device, transform)

    Calls the device functions to draw the annotation.

    :arg device: ``Device``.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.


.. method:: toPixmap(transform, colorspace, alpha)

    Render the annotation into a ``Pixmap``, using the transform and colorspace.

    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg colorspace: ``ColorSpace``.
    :arg alpha: ``Boolean``.


.. method:: toDisplayList()

    Record the contents of the annotation into a ``DisplayList``.

    :return: ``DisplayList``.


.. method:: getObject()

    Get the underlying ``PDFObject`` for an annotation.

    :return: ``PDFObject``.


.. method:: process(processor)

    Run through the annotation appearance stream and call methods on the supplied :title:`PDF` processor.

    :arg processor: User defined function.


.. method:: setAppearance(appearance, state, transform, displayList)

    Set the annotation appearance stream for the given appearance. The desired appearance is given as a transform along with a display list.

    :arg appearance: Appearance stream ("N", "R" or "D").
    :arg state: "On" or "Off".
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg displayList: ``DisplayList``.

.. method:: setAppearance(appearance, state, transform, bbox, resources, contents)

    Set the annotation appearance stream for the given appearance. The desired appearance is given as a transform along with a bounding box, a :title:`PDF` dictionary of resources and a content stream.

    :arg appearance: Appearance stream ("N", "R" or "D").
    :arg state: "On" or "Off".
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg bbox: ``[ulx,uly,lrx,lry]`` :ref:`Rectangle<mutool_run_js_api_rectangle>`.
    :arg resources: Resources object.
    :arg contents: Contents string.


.. method:: update()

    Update the appearance stream to account for changes in the annotation.


.. method:: getHot()

    Get the annotation as being hot, *i.e.* that the pointer is hovering over the annotation.

    :return: ``Boolean``.

.. method:: setHot(hot)

    Set the annotation as being hot, *i.e.* that the pointer is hovering over the annotation.

    :arg hot: ``Boolean``.


----

These properties are available for all annotation types.

.. method:: getType()

    Return the annotation subtype.

    :return: Annotation type.

.. method:: getFlags()

    Get the annotation flags.

    :return: Annotsation flags.

.. method:: setFlags(flags)

    Set the annotation flags.

    :arg flags:


.. method:: getContents()

    Get the annotation contents.

    :return: ``String``.

.. method:: setContents(text)

    Set the annotation contents.

    :arg text: ``String``.


.. method:: getBorder()

    Get the annotation border line width in points.

    :return: Border width.

.. method:: setBorder(width)

    Set the annotation border line width in points. Use ``setBorderWidth()`` to avoid removing the border effect.

    :arg width: Border width.


.. method:: getColor()

    Get the annotation color, represented as an array of up to 4 component values.

    :return: The :ref:`color value<mutool_run_js_api_colors>`.

.. method:: setColor(color)

    Set the annotation color, represented as an array of up to 4 component values.

    :arg color: The :ref:`color value<mutool_run_js_api_colors>`.

.. method:: getOpacity()

    Get the annotation opacity.

    :return: The :ref:`opacity<mutool_run_js_api_alpha>` value.

.. method:: setOpacity(opacity)

    Set the annotation opacity.

    :arg opacity: The :ref:`opacity<mutool_run_js_api_alpha>` value.

.. method:: getCreationDate()

    Get the annotation creation date as a :title:`JavaScript` ``Date`` object.

    :return: ``Date``.

.. method:: setCreationDate(milliseconds)

    Set the creation date to the number of milliseconds since the epoch.

    :arg milliseconds: Milliseconds value.



.. method:: getModificationDate()

    Get the annotation modification date as a :title:`JavaScript` ``Date`` object.

    :return: ``Date``.


.. method:: setModificationDate(milliseconds)

    Set the annotation modification date to the number of milliseconds since the epoch.

    :arg milliseconds: Milliseconds value.


.. method:: getQuadding()

    Get the annotation quadding (justification).

    :return: Quadding value, ``0`` for left-justified, ``1`` for centered, ``2`` for right-justified.


.. method:: setQuadding(value)

    Set the annotation quadding (justification).

    :arg value: Quadding value, ``0`` for left-justified, ``1`` for centered, ``2`` for right-justified.


.. method:: getLanguage()

    Get the annotation language (or get inherit the document language).

    :return: ``String``.


.. method:: setLanguage(language)

    Set the annotation language.

    :arg language: ``String``.



----


These properties are only present for some annotation types, so support for them must be checked before use.

PDFAnnotation#getRect(), #setRect(rect)
Get/set the annotation bounding box.
PDFAnnotation#getDefaultAppearance(), #setDefaultAppearance(font, size, color)
Get/Set the default text appearance used for free text annotations.
PDFAnnotation#hasInteriorColor(), #getInteriorColor(), #setInteriorColor(color)
Check for support/get/set the annotation interior color, represented as an array of up to 4 component values.
PDFAnnotation#hasAuthor(), #getAuthor(), #setAuthor(author)
Check for support/get/set the annotation author.
PDFAnnotation#hasLineEndingStyles(), #getLineEndingStyles(), #setLineEndingStyles(start, end)
Check for support/get/set the annotation line ending styles, either of "None", "Square", "Circle", "Diamond", "OpenArrow", "ClosedArrow", "Butt", "ROpenArrow", "RClosedArrow" or "Slash".
PDFAnnotation#hasIcon(), #getIcon(), #setIcon(iconname)
Check for support/get/set annotation icon. Standard icons names for:
File attachment annotations:
"Graph", "PaperClip", "PushPin" and "Tag".
Sound annotations:
"Mic" and "Speaker".
Stamp annotations:
"Approved", "AsIs", "Confidential", "Departmental", "Draft", "Experimental", "Expired", "Final", "ForComment", "ForPublicRelease", "NotApproved", "NorForPublicRelease", "Sold" and "TopSecret".
Text annotations:
"Comment", "Help", "Insert", "Key", "NewParagraph", "Note" and "Paragraph".
PDFAnnotation#hasLine(), #getLine(), #setLine(endpoints)
Check for support/get/set line end points, represented by an array of to points, each represented as an [x, y] array.
PDFAnnotation#hasOpen(), #isOpen(), #setIsOpen(state)
Check for support/get/set annotation open state, represented as a boolean.
PDFAnnotation#hasFilespec(), #getFilespec(), #setFilespec(filespecpdfobject)
Check for support/get/set annotation file specification, represented by a PDF object.

----



.. include:: footer.rst



.. External links:

.. _even odd rule: https://en.wikipedia.org/wiki/Even–odd_rule





