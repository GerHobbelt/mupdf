.. Copyright (C) 2001-2023 Artifex Software, Inc.
.. All Rights Reserved.


.. default-domain:: js

.. _mutool_object_dom:

.. _mutool_run_js_api_object_dom:


``DOM``
-------------

.. method:: body()

    Return a ``DOM`` for the body element.

    :return: ``DOM``.

.. method:: documentElement()

    Return a ``DOM`` for the top level element.

    :return: ``DOM``.

.. method:: createElement(tag)

    Create an element with the given tag type, but do not link it into the ``DOM`` yet.

    :arg tag: ``String``.

    :return: ``DOM``.


.. method:: createTextNode(text)

    Create a text node with the given text contents, but do not link it into the ``DOM`` yet.

    :arg text: ``String``.

    :return: ``DOM``.

.. method:: find(tag, attribute, value)

    Find the element matching the ``tag``, ``attribute`` and ``value``. Set either of those to ``null`` to match anything.

    :arg tag: ``String``.
    :arg attribute: ``String``.
    :arg value: ``String``.

    :return: ``DOM``.

.. method:: findNext(tag, attribute, value)

    Find the next element matching the ``tag``, ``attribute`` and ``value``. Set either of those to ``null`` to match anything.

    :arg tag: ``String``.
    :arg attribute: ``String``.
    :arg value: ``String``.

    :return: ``DOM``.


.. method:: appendChild(dom, childDom)

    Insert an element as the last child of a parent, unlinking the child from its current position if required.

    :arg dom: ``DOM``.
    :arg childDom: ``DOM``.


.. method:: insertBefore(dom, elementDom)

    Insert an element before this element, unlinking the new element from its current position if required.

    :arg dom: ``DOM``.
    :arg elementDom: ``DOM``.

.. method:: insertAfter(dom, elementDom)

    Insert an element after this element, unlinking the new element from its current position if required.

    :arg dom: ``DOM``.
    :arg elementDom: ``DOM``.


.. method:: remove()

    Remove this element from the ``DOM``. The element can be added back elsewhere if required.

.. method:: clone()

    Clone this element (and its children). The clone is not yet linked into the ``DOM``.

    :return: ``DOM``.

.. method:: firstChild()

    Return the first child of the element as a ``DOM``, or ``null`` if no child exist.

    :return: ``DOM`` \| ``null``.

.. method:: parent()

    Return the parent of the element as a ``DOM``, or ``null`` if no parent exists.

    :return: ``DOM`` \| ``null``.

.. method:: next()

    Return the next element as a ``DOM``, or ``null`` if no such element exists.

    :return: ``DOM`` \| ``null``.

.. method:: previous()

    Return the previous element as a ``DOM``, or ``null`` if no such element exists.

    :return: ``DOM`` \| ``null``.

.. method:: addAttribute(attribute, value)

    Add attribute with the given value, returns the updated element as a ``DOM``.

    :arg attribute: ``String``.
    :arg value: ``String``.

    :return: ``DOM``.

.. method:: removeAttribute(attribute)

    Remove the specified attribute from the element.

    :arg attribute: ``String``.

.. method:: attribute(attribute)

    Return the element's attribute value as a ``String``, or ``null`` if no such attribute exists.

    :arg attribute: ``String``.

    :return: ``String`` \| ``null``.

.. method:: getAttributes()

    Returns a dictionary object with properties and their values corresponding to the element's attributes and their values.

    :return: ``{}``.
