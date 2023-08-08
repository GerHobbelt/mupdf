.. Copyright (C) 2001-2023 Artifex Software, Inc.
.. All Rights Reserved.

----

.. default-domain:: js

.. include:: html_tags.rst

.. _mutool_object_image:

.. _mutool_run_js_api_image:

`Image`
------------

`Image` objects are similar to `Pixmaps`, but can contain compressed data.


.. method:: new Image(ref)

    *Constructor method*.

    Create a new image from a `Pixmap` data, or load an image file data.

    :return: `Image`.

    |example_tag|

    .. code-block:: javascript

        var imageFromPixmap = new mupdf.Image(pixmap);
        var imageFromBuffer = new mupdf.Image(buffer);


|instance_methods|


.. method:: getWidth()

    Get the image width in pixels.

    :return: The width value.

    |example_tag|

    .. code-block:: javascript

        var width = image.getWidth();


.. method:: getHeight()

    Get the image height in pixels.

    :return: The height value.

    |example_tag|

    .. code-block:: javascript

        var height = image.getHeight();

.. method:: getXResolution()

    Returns the x resolution for the `Image`.

    :return: `Int` Image resolution in dots per inch.

    |example_tag|

    .. code-block:: javascript

        var xRes = image.getXResolution();


.. method:: getYResolution()

    Returns the y resolution for the `Image`.

    :return: `Int` Image resolution in dots per inch.

    |example_tag|

    .. code-block:: javascript

        var yRes = image.getYResolution();


.. method:: getColorSpace()

    Returns the `ColorSpace` for the `Image`.

    :return: `ColorSpace`.

    |example_tag|

    .. code-block:: javascript

        var cs = image.getColorSpace();


.. method:: getNumberOfComponents()

    Number of colors; plus one if an alpha channel is present.

    :return: `Integer`.

    |example_tag|

    .. code-block:: javascript

        var num = image.getNumberOfComponents();


.. method:: getBitsPerComponent()

    Returns the number of bits per component.

    :return: `Integer`.

    |example_tag|

    .. code-block:: javascript

        var bits = image.getBitsPerComponent();


.. method:: getInterpolate()

    Returns *true* if interpolated was used during decoding.

    :return: `Boolean`.

    |example_tag|

    .. code-block:: javascript

        var interpolate = image.getInterpolate();


.. method:: getColorKey()

    Returns an array with 2 * N integers for an N component image with color key masking, or `null` if masking is not used. Each pair of integers define an interval, and component values within that interval are not painted.

    :return: `[...]` or `null`.


    |example_tag|

    .. code-block:: javascript

        var result = image.getColorKey();

.. method:: getDecode()

    Returns an array with 2 * N numbers for an N component image with color mapping, or `null` if mapping is not used. Each pair of numbers define the lower and upper values to which the component values are mapped linearly.

    :return: `[...]` or `null`.

    |example_tag|

    .. code-block:: javascript

        var arr = image.getDecode();


.. method:: getOrientation()

    Returns the orientation of the image.

    :return: `Integer`.

    |example_tag|

    .. code-block:: javascript

        var orientation = image.getOrientation();

.. method:: setOrientation(orientation)

    Set the image orientation to the given orientation.

    :arg orientation: `Integer` Orientation value from the table below:


** Orientation values **

    .. list-table::
       :header-rows: 0

       * - **0**
         - Undefined
       * - **1**
         - 0 degree ccw rotation. (Exif = 1)
       * - **2**
         - 90 degree ccw rotation. (Exif = 8)
       * - **3**
         - 180 degree ccw rotation. (Exif = 3)
       * - **4**
         - 270 degree ccw rotation. (Exif = 6)
       * - **5**
         - flip on X. (Exif = 2)
       * - **6**
         - flip on X, then rotate ccw by 90 degrees. (Exif = 5)
       * - **7**
         - flip on X, then rotate ccw by 180 degrees. (Exif = 4)
       * - **8**
         - flip on X, then rotate ccw by 270 degrees. (Exif = 7)

    |example_tag|

    .. code-block:: javascript

        var orientation = image.setOrientation(4);


.. method:: getImageMask()

    Returns *true* if this image is an image mask.

    :return: `Boolean`.

    |example_tag|

    .. code-block:: javascript

        var mask = image.getImageMask();


.. method:: getMask()

    Get another `Image` used as a mask for this one.

    :return: `Image` (or `null`).

    |example_tag|

    .. code-block:: javascript

        var img = image.getMask();



.. method:: toPixmap(scaledWidth, scaledHeight)

    Create a `Pixmap` from the image. The `scaledWidth` and `scaledHeight` arguments are optional, but may be used to decode a down-scaled `Pixmap`.

    :arg scaledWidth: `Float`.
    :arg scaledHeight: `Float`.

    :return: `Pixmap`.


    |example_tag|

    .. code-block:: javascript

        var pixmap = image.toPixmap();
        var scaledPixmap = image.toPixmap(100, 100);

.. method:: getImageData()

    Return the image data for the image.

    :return: `ImageData`.

`ImageData`
------------------------------

`ImageData` is an object with keys for:

`buffer`
    The buffer supplied when creating the ImageData.

`type`
    The type from the image data parameters.

`params`
    An `ImageDataParams` dictionary containting the image data parameters.


`ImageDataParams`
------------------------------

`ImageDataParams` is an object with keys for:

`type`
    Either of "raw", "fax", "flate", "lzw", "rld", "bmp", "gif", "jbig2", "jpeg", "jpx", "jxr", "png", "pnm", "tiff" or "unknown".

`columns`
    Width of the image in pixels (can be specified for fax, flate and lzw).

`rows`
    Height of the image in scanlines (can be specified for fax).

`k`
    Encoding scheme: < 0 for pure 2D encoding (Group 4), 0 for pure 1D encoding (Group 3, 1D), > 0 for mixed 1D and 2D encoding (Group3, 2D) where each 1D line is at most followed by <em>k - 1</em> 2D lines (can be specified for fax).

`endOfLine`
    Boolean flag indicating whether end-of-line bit patterns are required in the encoding (can be specified for fax).

`encodedByteAlign`
    Boolean flag indicating whether extar 0 bits before each encoded line should be expected (can be specified for fax).

`endOfBlock`
    Boolean flag indicating whether the encoded data is terminated by an end-of-block pattern (can be specified for fax).

`blackIs1`
    Boolean flag indicating whether 1 bits are to be interpreted as 1 and 0 bits as white, or the reverse (can be specified for fax).

`damagedRowsBeforeError`
    Number of damaged rows of data before an error occurs. (can be specified for fax).

`predictor`
    Predictor algorithm to decode the image, either of (can be specified for flate or lzw) from the table below:

.. list-table::
   :header-rows: 0

   * - **1**
     - None
   * - **2**
     - TIFF predictor 2 (only valid for lzw)
   * - **10**
     - PNG None
   * - **11**
     - PNG Sub
   * - **12**
     - PNG Up
   * - **13**
     - PNG Average
   * - **14**
     - PNG Paeth
   * - **15**
     - Optimal PNG predictor per scanline.


`colors`
    Number of interleaved color components per sample if predictor is > 1 (can be specified for flate or lzw).

`bitsPerComponent`
    Number of bits used to represent each color component if predictor is > 1 (can be specified for flate or lzw). Valid values are 1, 2, 4, 8, or 16.

`earlyChange`
    Code length increases are postponed as long as possible if set 0, or occurs one code early if set to 1 (can be specified for lzw).

`embedded`
    Boolean flag indicating whether the image data uses embedded organisation (can be specified for jbig2).

`globals`
    A buffer containing encoded global jbig2 segments (can be specified for jbig2)

`colorTransform`
    The color transform to use, 0 for no transform or 1 for YUV-to-RGB transform for 3 component images, or YUVK-to-CMYK for 4 compnent images, after decoding (can be specified for jpeg)

`sMaskInData`
    If 0 encoded soft-mask data is ignored, if 1 the image's data stream includes soft-mask data, if 2 the image's components contain colors that have been pre-blended with a background and an opacity channel (can be specified for jpx).
