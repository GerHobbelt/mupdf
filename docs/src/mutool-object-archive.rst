.. Copyright (C) 2001-2023 Artifex Software, Inc.
.. All Rights Reserved.

----

.. default-domain:: js

.. include:: html_tags.rst

.. _mutool_object_archive:



.. _mutool_run_js_api_object_archive:



`Archive`
------------------------

|mutool_tag|

.. method:: new Archive(path)

    *Constructor method*.

    Create a new archive based either on a :title:`tar` or :title:`zip` file or the contents of a directory.

    :arg path: `String`.

    :return: `Archive`.

    |example_tag|

    .. code-block:: javascript

        var archive = new mupdf.Archive(<path>);


|instance_methods|

.. method:: getFormat()

    Returns a string describing the archive format.

    :return: `String`.

    |example_tag|

    .. code-block:: javascript

        var format = archive.getFormat();

.. method:: countEntries()

    Returns the number of entries in the archive.

    :return: `Integer`.

    |example_tag|

    .. code-block:: javascript

        var numEntries = archive.countEntries();

.. method:: listEntry(idx)

    Returns the name of entry number `idx` in the archive.

    :arg idx: `Integer`.

    :return: `String`.

    |example_tag|

    .. code-block:: javascript

        var entry = archive.listEntry(0);


.. method:: hasEntry(name)

    Returns :title:`true` if an entry of the given name exists in the archive.

    :arg name: `String`.

    :return: `Boolean`.

    |example_tag|

    .. code-block:: javascript

        var hasEntry = archive.hasEntry("Hello");


.. method:: readEntry(name)

    Returns the contents of the entry of the given name.

    :arg name: `String`.

    :return: `String`.

    |example_tag|

    .. code-block:: javascript

        var value = archive.readEntry("Hello");


`MultiArchive`
------------------------

|mutool_tag|

.. method:: new MultiArchive()

    *Constructor method*.

    Create a new empty multi archive.

    :return: `MultiArchive`.

    |example_tag|

    .. code-block:: javascript

        var multiArchive = new mupdf.MultiArchive();

|instance_methods|


.. method:: mountArchive(subArchive, path)

    Add an archive to the set of archives handled by a multi archive. If `path` is `null`, the `subArchive` contents appear at the top-level, otherwise they will appear prefixed by the string `path`.

    :arg subArchive: `Archive`.
    :arg path: `String`.

    |example_tag|

    .. code-block:: javascript

        archive.mountArchive(subArchive, null);



`TreeArchive`
------------------------

|mutool_tag|

.. method:: new TreeArchive()

    *Constructor method*.

    Create a new empty tree archive.

    :return: `TreeArchive`.

    |example_tag|

    .. code-block:: javascript

        var treeArchive = new mupdf.TreeArchive();

|instance_methods|


.. method:: add(name, buffer)

    Add a named buffer to a tree archive.

    :arg name: `String`.
    :arg buffer: `Buffer`.

    |example_tag|

    .. code-block:: javascript

        archive.add("name", buffer);


