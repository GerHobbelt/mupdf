.. Copyright (C) 2001-2023 Artifex Software, Inc.
.. All Rights Reserved.


.. default-domain:: js

.. include:: html_tags.rst

.. _mutool_object_document:


.. _mutool_run_js_api_document:


`Document`
--------------------

:title:`MuPDF` can open many document types (:title:`PDF`, :title:`XPS`, :title:`CBZ`, :title:`EPUB`, :title:`FB2` and a handful of image formats).


.. method:: new Document.openDocument(fileName, fileType)

    *Constructor method*.

    Open the named document.

    :arg fileName: File name to open.
    :arg fileType: File type.

    :return: `Document`.


    **Example**

    .. code-block:: javascript

        var document = new mupdf.Document.openDocument("my_pdf.pdf", "application/pdf");


----

**Instance methods**

.. method:: needsPassword()

    Returns *true* if a password is required to open a password protected PDF.

    :return: `Boolean`.

    **Example**

    .. code-block:: javascript

        var needsPassword = document.needsPassword();


.. method:: authenticatePassword(password)

    Returns *true* if the password matches.

    :arg password: The password to attempt authentication with.
    :return: `Boolean`.

    **Example**

    .. code-block:: javascript

        var auth = document.authenticatePassword("abracadabra");


.. method:: hasPermission(permission)

    Returns *true* if the document has permission for "print", "annotate", "edit", "copy", "form", "accessbility", "assemble" or "print-hq".

    :arg permission: `String` The permission to seek for, e.g. "edit".
    :return: `Boolean`.

    **Example**

    .. code-block:: javascript

        var canEdit = document.hasPermission("edit");


.. method:: getMetaData(key)

    Return various meta data information. The common keys are: `format`, `encryption`, `info:Author`, and `info:Title`.

    :arg key: `String`.
    :return: `String`.

    **Example**

    .. code-block:: javascript

        var metaData = document.getMetaData("format");


.. method:: setMetaData(key, value)

    Set document meta data information field to a new value.

    :arg key: `String`.
    :arg value: `String`.

    **Example**

    .. code-block:: javascript

        document.setMetaData("info:Author", "My Name");


.. method:: isReflowable()

    Returns true if the document is reflowable, such as :title:`EPUB`, :title:`FB2` or :title:`XHTML`.

    :return: `Boolean`.

    **Example**

    .. code-block:: javascript

        var isReflowable = document.isReflowable();

    .. note::

        This will always return `false` in the :title:`WASM` context as there is no :title:`HTML`/:title:`EPUB` support in :title:`WASM`.


.. method:: layout(pageWidth, pageHeight, fontSize)

    Layout a reflowable document (:title:`EPUB`, :title:`FB2`, or :title:`XHTML`) to fit the specified page and font size.

    :arg pageWidth: `Int`.
    :arg pageHeight: `Int`.
    :arg fontSize: `Int`.

    **Example**

    .. code-block:: javascript

        document.layout(300,300,16);


.. method:: countPages()

    Count the number of pages in the document. This may change if you call the layout function with different parameters.

    :return: `Int`.

    **Example**

    .. code-block:: javascript

        var numPages = document.countPages();


.. method:: loadPage(number)

    Returns a `Page`_ (or `PDFPage`_) object for the given page number. Page number zero (0) is the first page in the document.

    :return: `Page` or `PDFPage`.

    **Example**

    .. code-block:: javascript

        var page = document.loadPage(0); // loads the 1st page of the document

.. method:: loadOutline()

    Returns an array with the outline (also known as "table of contents" or "bookmarks"). In the array is an object for each heading with the property 'title', and a property 'page' containing the page number. If the object has a 'down' property, it contains an array with all the sub-headings for that entry.

    :return: `[]`.


    **Example**

    .. code-block:: javascript

        var outline = document.loadOutline();


.. method:: outlineIterator()

    |mutool_tag|

    Returns an :ref:`OutlineIterator<mutool_object_outline_iterator>` for the document outline.

    :return: `OutlineIterator`.

    **Example**

    .. code-block:: javascript

        var obj = document.outlineIterator();


.. _mutool_run_js_api_document_resolveLink:


.. method:: resolveLink(uri)

    Resolve a document internal link :title:`URI` to a link destination.

    :arg uri: `String`.
    :return: :ref:`Link destination<mutool_run_js_api_link_dest>`.

    **Example**

    .. code-block:: javascript

        var linkDestination = document.resolveLink(my_link);



.. method:: isPDF()

    Returns *true* if the document is a :title:`PDF` document.

    :return: `Boolean`.

    **Example**

    .. code-block:: javascript

        var isPDF = document.isPDF();


.. method:: formatLinkURI(linkDestination)

    |mutool_tag|

    Format a document internal link destination object to a :title:`URI` string suitable for :ref:`createLink()<mutool_run_js_api_page_create_link>`.

    :arg linkDestination: :ref:`Link destination<mutool_run_js_api_link_dest>`.
    :return: `String`.


    **Example**

    .. code-block:: javascript

        var str = document.formatLinkURI(linkDestination);


