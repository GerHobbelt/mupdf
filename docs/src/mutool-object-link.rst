.. Copyright (C) 2001-2023 Artifex Software, Inc.
.. All Rights Reserved.


.. default-domain:: js

.. include:: html_tags.rst

.. _mutool_object_link:



.. _mutool_run_js_api_link:




`Link`
------------

`Link` objects contain information about page links.


.. method:: getBounds()

    Returns a :ref:`rectangle<mutool_run_js_api_rectangle>` describing the link's location on the page.

    :return: `[ulx,uly,lrx,lry]`.

    **Example**

    .. code-block:: javascript

        var rect = link.getBounds();


.. method:: getURI()

    Returns a string describing the link's URI.

    :return: `String`.

    **Example**

    .. code-block:: javascript

        var uri = link.getURI();



.. method:: isExternal()

    |wasm_tag|

    Returns a boolean indicating if the link is external or not.

    :return: `Boolean`.

    **Example**

    .. code-block:: javascript

        var isExternal = link.isExternal();



