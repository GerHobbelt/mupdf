.. Copyright (C) 2001-2023 Artifex Software, Inc.
.. All Rights Reserved.




.. _mutool_object_archive:



.. _mutool_run_js_api_object_archive:



``Archive``
------------------------


.. method:: new(path)

    Create a new archive based either on a :title:`tar` or :title:`zip` file or the contents of a directory.

    :arg path: ``String``.

    :return: ``Archive``.

    **Example**

    .. code-block:: javascript

        let archive = new Archive(<path>);

.. method:: getFormat()

    Returns a string describing the archive format.

    :return: ``String``.

.. method:: countEntries()

    Returns the number of entries in the archive.

    :return: ``Integer``.

.. method:: listEntry(idx)

    Returns the name of entry number ``idx`` in the archive.

    :arg idx: ``Integer``.

    :return: ``String``.

.. method:: hasEntry(name)

    Returns :title:`true` if an entry of the given name exists in the archive.

    :arg name: ``String``.

    :return: ``Boolean``.

.. method:: readEntry(name)

    Returns the contents of the entry of the given name.

    :arg name: ``String``.

    :return: ``String``.


``MultiArchive``
------------------------


.. method:: new()

    Create a new empty multi archive.

    :return: ``MultiArchive``.

    **Example**

    .. code-block:: javascript

        let multiArchive = new MultiArchive();


.. method:: mountArchive(subArchive, path)

    Add an archive to the set of archives handled by a multi archive. If ``path`` is ``null``, the ``subArchive`` contents appear at the top-level, otherwise they will appear prefixed by the string ``path``.

    :arg subArchive:
    :arg path: ``String``.



``TreeArchive``
------------------------

.. method:: new()

    Create a new empty tree archive.

    :return: ``TreeArchive``.

    **Example**

    .. code-block:: javascript

        let treeArchive = new TreeArchive();


.. method:: add(name, buffer)

    Add a named buffer to a tree archive.

    :arg name: ``String``.
    :arg buffer: ``Buffer``.



