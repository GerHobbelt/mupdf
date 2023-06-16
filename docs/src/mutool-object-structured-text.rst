.. Copyright (C) 2001-2023 Artifex Software, Inc.
.. All Rights Reserved.

----

.. default-domain:: js

.. include:: html_tags.rst

.. _mutool_object_structured_text:




.. _mutool_run_js_api_structured_text:


`StructuredText`
----------------------------

`StructuredText` objects hold text from a page that has been analyzed and grouped into blocks, lines and spans. To obtain a `StructuredText` instance use :ref:`Page toStructuredText()<mutool_page_toStructuredText>`.


|instance_methods|

.. method:: search(needle)

    Search the text for all instances of `needle`, and return an array with :ref:`rectangles<mutool_run_js_api_rectangle>` of all matches found.

    :arg needle: `String`.
    :return: `[...]`.

    |example_tag|

    .. code-block:: javascript

        var result = sText.search("Hello World!");

    |tor_todo| WASM, Even says "TODO" in the mupdf.js source file :)



.. method:: highlight(p, q)

    Return an array with :ref:`rectangles<mutool_run_js_api_rectangle>` needed to highlight a selection defined by the start and end points.

    :arg p: Start point in format `{x:y}`.
    :arg q: End point in format `{x:y}`.

    :return: `[...]`.

    |example_tag|

    .. code-block:: javascript

        var result = sText.highlight({100:100}, {200:100});

    |tor_todo| WASM, Even says "TODO" in the mupdf.js source file :)


.. method:: copy(p, q)

    Return the text from the selection defined by the start and end points.

    :arg p: Start point in format `{x:y}`.
    :arg q: End point in format `{x:y}`.

    :return: `String`.


    |example_tag|

    .. code-block:: javascript

        var result = sText.highlight({100:100}, {200:100});


    |tor_todo| WASM, Even says "TODO" in the mupdf.js source file :)




.. method:: walk(walker)

    |wasm_tag|

    |tor_todo| I don't know if this is an internal function, in any case I don't know what the `walker` parameter is


.. method:: asJSON()

    |wasm_tag|

    Returns the instance in :title:`JSON` format.

    :return: `String`.

    |example_tag|

    .. code-block:: javascript

        var json = sText.asJSON();




