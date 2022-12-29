.. Copyright (C) 2001-2022 Artifex Software, Inc.
.. All Rights Reserved.


.. title:: mutool JavaScript API

.. include:: header.rst

.. meta::
   :description: MuPDF documentation
   :keywords: MuPDF, pdf, epub


.. _mutool_run_javascript_api:







Base :title:`MuPDF` methods
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

Constructors
~~~~~~~~~~~~~~~~

.. method:: new Buffer()

    Create a new empty buffer.

.. method:: readFile(fileName)

    Create a new buffer with the contents of a file.

    :arg fileName: The path to the file to read.


Buffer properties
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. property:: length

   The number of bytes in the buffer. `Read-only`.


.. property:: [n]

    Read/write the byte at index 'n'. Will throw exceptions on out of bounds accesses.


Buffer methods
~~~~~~~~~~~~~~~~~~~~~~~~~~

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


Constructors
~~~~~~~~~~~~~~~~

.. method:: new Document(fileName)

    Open the named document.

    :arg fileName: Filename to open.



Document methods
~~~~~~~~~~~~~~~~~~~~~~~~~~

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


Page
--------

.. method:: bound()

    Returns a :ref:`rectangle<mutool_run_js_api_rectangle>` containing the page dimensions.

    :return: ``[ulx,uly,lrx,lry]``.

.. method:: run(device, transform, skipAnnotations)

    Calls device functions for all the contents on the page, using the specified transform `matrix<mutool_run_js_api_matrix>`. The device can be one of the built-in devices or a :title:`JavaScript` object with methods for the device calls. The transform maps from user space points to device space pixels. If ``skipAnnotations`` is *true* then annotations are ignored.

    :arg device: The device object.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg skipAnnotations: ``Boolean``.

.. method:: toPixmap(transform, colorspace, alpha, skipAnnotations)

    Render the page into a ``Pixmap``, using the transform and colorspace. If alpha is true, the page will be drawn on a transparent background, otherwise white.

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

    Search for 'needle' text on the page, and return an array with :ref:`rectangles<mutool_run_js_api_rectangle>` of all matches found.

    :arg needle: ``String``.
    :return: ``[]``.

.. method:: getAnnotations()

    Return array of all ``Annotations`` on the page.

    :return: ``[]``.

.. method:: getLinks()

    Return an array of all the links on the page. Each link is an object with a 'bounds' property, and either a 'page' or 'uri' property, depending on whether it's an internal or external link.

    :return: ``[]``.

.. method:: isPDF()

    Returns true if the page is from a :title:`PDF` document.

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

    Render the annotation into a ``Pixmap``, using the transform and colorspace.

    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg colorspace: ``ColorSpace``.
    :arg alpha: ``Boolean``. If alpha is true, the annotation will be drawn on a transparent background, otherwise white.


.. method:: toDisplayList()

    Record the contents of the annotation into a ``DisplayList``.

.. method:: isPDF()

    Returns true if the annotation is from a :title:`PDF` document.

    :return: ``Boolean``.


.. include:: footer.rst