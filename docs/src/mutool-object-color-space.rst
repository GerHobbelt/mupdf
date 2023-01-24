.. Copyright (C) 2001-2023 Artifex Software, Inc.
.. All Rights Reserved.

.. default-domain:: js


.. _mutool_object_color_space:

.. _mutool_run_javascript_api_colorspace:


.. _mutool_run_js_api_colorspace:


``ColorSpace``
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



