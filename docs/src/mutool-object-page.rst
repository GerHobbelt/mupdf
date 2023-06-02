.. Copyright (C) 2001-2023 Artifex Software, Inc.
.. All Rights Reserved.


.. default-domain:: js

.. include:: html_tags.rst

.. _mutool_object_page:

.. _mutool_run_js_api_page:

`Page`
-------------


**Instance methods**

.. method:: bound()

    |mutool_tag|

    Returns a :ref:`rectangle<mutool_run_js_api_rectangle>` containing the page dimensions.

    :return: `[ulx,uly,lrx,lry]`.

    **Example**

    .. code-block:: javascript

        let rect = page.bound();

.. method:: getBounds()

    |wasm_tag|

    Returns a :ref:`rectangle<mutool_run_js_api_rectangle>` containing the page dimensions.

    :return: `[ulx,uly,lrx,lry]`.

    **Example**

    .. code-block:: javascript

        let rect = page.getBounds();


.. _Page_run:


.. method:: run(device, matrix)

    |wasm_tag|

    Calls device functions for all the contents on the page, using the specified transform :ref:`matrix<mutool_run_js_api_matrix>`. The `device` can be one of the built-in devices or a :title:`JavaScript` object with methods for the device calls. The `matrix` maps from user space points to device space pixels.

    :arg device: The device object.
    :arg matrix: `[a,b,c,d,e,f]`. The transform :ref:`matrix<mutool_run_js_api_matrix>`.

    **Example**

    .. code-block:: javascript

        let rect = page.run(obj, mupdf.Matrix.identity);

    |tor_todo| Make mutool run method match this.


.. method:: runPageContents(device, matrix)

    |wasm_tag|

    This is the same as the :ref:`run<Page_run>` method above but it only considers the page itself and omits annotations and widgets.

    :arg device: The device object.
    :arg matrix: `[a,b,c,d,e,f]`. The transform :ref:`matrix<mutool_run_js_api_matrix>`.

    **Example**

    .. code-block:: javascript

        let rect = page.runPageContents(obj, mupdf.Matrix.identity);

    |tor_todo| Make mutool run method match this.


.. method:: runPageAnnots(device, matrix)

    |wasm_tag|

    This is the same as the :ref:`run<Page_run>` method above but it only considers the page annotations.

    :arg device: The device object.
    :arg matrix: `[a,b,c,d,e,f]`. The transform :ref:`matrix<mutool_run_js_api_matrix>`.

    **Example**

    .. code-block:: javascript

        let rect = page.runPageAnnots(obj, mupdf.Matrix.identity);

    |tor_todo| Make mutool run method match this.


.. method:: runPageWidgets(device, matrix)

    |wasm_tag|

    This is the same as the :ref:`run<Page_run>` method above but it only considers the page widgets.

    :arg device: The device object.
    :arg matrix: `[a,b,c,d,e,f]`. The transform :ref:`matrix<mutool_run_js_api_matrix>`.

    **Example**

    .. code-block:: javascript

        let rect = page.runPageWidgets(obj, mupdf.Matrix.identity);

    |tor_todo| Make mutool run method match this.

.. method:: toPixmap(matrix, colorspace, alpha, showExtras)

    Render the page into a :ref:`Pixmap<mutool_run_js_api_pixmap>`, using the specified transform :ref:`matrix<mutool_run_js_api_matrix>` and `colorspace`. If `alpha` is *true*, the page will be drawn on a transparent background, otherwise white. If `showExtras` is *true* then the operation will include any page annotations and/or widgets.

    :arg matrix: `[a,b,c,d,e,f]`. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg colorspace: `ColorSpace`.
    :arg alpha: `Boolean`.
    :arg showExtras: `Boolean`.

    :return: `Pixmap`.

    .. note::

        In :title:`MuPDF WASM` `alpha` & `showExtras` default to *true* unless otherwise specified.

    **Example**

    .. code-block:: javascript

        let pixmap = page.toPixmap(mupdf.Martrix.identity, mupdf.ColorSpace.DeviceRGB, true, true);

.. method:: toDisplayList(showExtras)

    Record the contents on the page into a `DisplayList`_. If `showExtras` is *true* then the operation will include any page annotations and/or widgets.


    :arg showExtras: `Boolean`.

    :return: `DisplayList`.

    .. note::

        In :title:`MuPDF WASM` `showExtras` defaults to *true* unless otherwise specified.

    **Example**

    .. code-block:: javascript

        let displayList = page.toDisplayList(true);


.. method:: toStructuredText(options)

    Extract the text on the page into a `StructuredText` object. The options argument is a comma separated list of flags: "preserve-ligatures", "preserve-whitespace", "preserve-spans", and "preserve-images".

    :arg options: `String`.
    :return: `StructuredText`.

    **Example**

    .. code-block:: javascript

        let sText = page.toStructuredText("preserve-whitespace");


    |jamie_todo| Pick up from here .......


.. method:: search(needle)

    Search for `needle` text on the page, and return an array with :ref:`rectangles<mutool_run_js_api_rectangle>` of all matches found.

    :arg needle: `String`.
    :return: `[]`.



.. method:: getLinks()

    Return an array of all the links on the page. Each link is an object with a 'bounds' property, and either a 'page' or 'uri' property, depending on whether it's an internal or external link. See: :ref:`Links<mutool_run_js_api_links>`.

    :return: `[]`.


.. _mutool_run_js_api_page_create_link:


.. method:: createLink(rect, destinationUri)

    Create a new link within the rectangle on the page, linking to the destination URI string.

    :arg rect: :ref:`Rectangle<mutool_run_js_api_rectangle>` for the link.
    :arg destinationUri: `String`.
    :return: `Object` :ref:`Link dictionary<mutool_run_js_api_link_dict>`.

    **Example**

    .. code-block:: javascript

        var link = page.createLink([0,0,100,100],"http://mupdf.com");


.. method:: deleteLink(link)

    Delete the link from the page.

    :arg link: `Object` :ref:`Link dictionary<mutool_run_js_api_link_dict>`.


.. method:: getLabel()

    Returns the page number as a string using the numbering scheme of the document.

    :return: `String`.


.. method:: isPDF()

    Returns *true* if the page is from a :title:`PDF` document.

    :return: `Boolean`.
