.. Copyright (C) 2001-2023 Artifex Software, Inc.
.. All Rights Reserved.




.. _mutool_object_device:



.. _mutool_run_js_api_device:


``Device``
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

.. method:: clipPath(path, evenOdd, transform)

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


.. method:: beginMask(area, luminosity, backdropColorspace, backdropColor, backdropAlpha)

    Create a soft mask. Any drawing commands between ``beginMask`` and ``endMask`` are grouped and used as a clip mask.

    :arg area: ``Path`` Mask area.
    :arg luminosity: ``Boolean`` If luminosity is *true*, the mask is derived from the luminosity (grayscale value) of the graphics drawn; otherwise the color is ignored completely and the mask is derived from the alpha of the group.
    :arg backdropColorspace: The :ref:`ColorSpace<mutool_run_javascript_api_colorspace>`.
    :arg backdropColor: The :ref:`color value<mutool_run_js_api_colors>`.
    :arg backdropAlpha: The  :ref:`alpha value<mutool_run_js_api_alpha>`.


.. method:: endMask()

    Ends the mask.



.. method:: beginGroup(area, isolated, knockout, blendmode, alpha)

    Push/pop a transparency blending group. See the PDF reference for details on ``isolated`` and ``knockout``.

    :arg area: ``Path`` Blend area.
    :arg isolated: ``Boolean``.
    :arg knockout: ``Boolean``.
    :arg blendmode: Blendmode is one of the standard :title:`PDF` blend modes: "Normal", "Multiply", "Screen", etc.
    :arg alpha: The :ref:`alpha value<mutool_run_js_api_alpha>`.


    .. image:: images/isolated-and-knockout.png
       :align: center
       :scale: 50%


.. method:: endGroup()

    Ends the blending group.


.. method:: beginTile(areaRect, viewRect, xStep, yStep, transform, id)

    Draw a tiling pattern. Any drawing commands between ``beginTile`` and ``endTile`` are grouped and then repeated across the whole page. Apply a clip mask to restrict the pattern to the desired shape.

    :arg areaRect: ``[ulx,uly,lrx,lry]`` :ref:`Rectangle<mutool_run_js_api_rectangle>`.
    :arg viewRect: ``[ulx,uly,lrx,lry]`` :ref:`Rectangle<mutool_run_js_api_rectangle>`.
    :arg xStep: ``Integer`` representing ``x`` step.
    :arg yStep: ``Integer`` representing ``y`` step.
    :arg transform: ``[a,b,c,d,e,f]``. The transform :ref:`matrix<mutool_run_js_api_matrix>`.
    :arg id: ``Integer`` The purpose of ``id`` is to allow for efficient caching of rendered tiles. If ``id`` is ``0``, then no caching is performed. If it is non-zero, then it assumed to uniquely identify this tile.


.. method:: endTile()

    Ends the tiling pattern.



.. method:: close()

    Tell the device that we are done, and flush any pending output.


.. method:: beginLayer(tag)

    Begin a marked-content layer with the given tag.

    :arg tag: ``String``.

.. method:: endLayer()

    End a marked-content layer.



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




.. External links:

.. _even odd rule: https://en.wikipedia.org/wiki/Even–odd_rule





