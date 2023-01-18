.. Copyright (C) 2001-2023 Artifex Software, Inc.
.. All Rights Reserved.

.. _mutool_object_dom:

.. _mutool_run_js_api_object_dom:





``DOM``
-------------

DOM#body()
Return a DOM for the body element.
DOM#documentElement()
Return a DOM for the top level element.
DOM#createElement(tag)
Create an element with the given tag type, but do not link it into the DOM yet.
DOM#createTextMode(text)
Create a text node with the given text contents, but do not link it into the DOM yet.
DOM#find(tag, attribute, value)
Find the element matching the tag, attribute and value. Set either of those to null to match anything.
DOM#findNext(tag, attribute, value)
Find the next element matching the tag, attribute and value. Set either of those to null to match anything.
DOM#appendChild(dom, childdom)
Insert an element as the last child of a parent, unlinking the child from its current position if required.
DOM#insertBefore(dom, elementDom)
Insert an element before this element, unlinking the new element from its current position if required.
DOM#insertAfter(dom, elementDom)
Insert an element after this element, unlinking the new element from its current position if required.
DOM#remove()
Remove an element from the DOM. The element can be added back elsewhere if required.
DOM#clone()
Clone an element (and its children). The clone is not yet linked into the DOM.
DOM#firstChild()
Return the first child of the element as a DOM, or null if no child exist.
DOM#parent()
Return the parent of the element as a DOM, or null if no parent exists.
DOM#next()
Return the next element as a DOM, or null if no such element exists.
DOM#previous()
Return the previous element as a DOM, or null if no such element exists.
DOM#addAttribute(attribute, value)
Add attribute with the given value, returns the updated element as a DOM.
DOM#removeAttribute(attribute)
Remove the specified attribute from the element.
DOM#attribute(attribute)
Return the element's attribute value as a string, or null if no such attribute exists.
DOM#getAttributes()
Returns a dictionary object with properties and their values corresponding to the element's attributes and their values.
