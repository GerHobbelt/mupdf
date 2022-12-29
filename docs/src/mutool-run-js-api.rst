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


Colors are specified as arrays with the appropriate number of components for the color space.

The methods that clip graphics must be balanced with a corresponding ``popClip``.

.. method:: fillPath(path, evenOdd, transform, colorspace, color, alpha)

    Fill a path.

    :arg path: ``Path``.
    :arg evenOdd:
    :arg transform:
    :arg colorspace: The :ref:`ColorSpace<mutool_run_javascript_api_colorspace>`.
    :arg color:
    :arg alpha:

.. method:: strokePath(path, stroke, transform)

    Stroke a path.

    :arg path: ``Path``.
    :arg stroke:
    :arg transform:

.. method:: clipPath(path, evenOdd, transform, colorspace, color, alpha)

    Clip a path.

    :arg path: ``Path``.
    :arg evenOdd:
    :arg transform:
    :arg colorspace: The :ref:`ColorSpace<mutool_run_javascript_api_colorspace>`.
    :arg color:
    :arg alpha:

.. method:: clipStrokePath(path, stroke, transform)

    Clip & stroke a path.

    :arg path: ``Path``.
    :arg stroke:
    :arg transform:













.. include:: footer.rst







