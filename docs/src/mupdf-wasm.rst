.. Copyright (C) 2001-2023 Artifex Software, Inc.
.. All Rights Reserved.


.. include:: header.rst

.. meta::
   :description: MuPDF documentation
   :keywords: MuPDF, pdf, epub


Using :title:`MuPDF WASM`
==========================================


`npm install mupdf`

const fs = require("fs")
const mupdf = require("mupdf")
mupdf.ready.then(function () {
        var input = fs.readFileSync("pdfref17.pdf")
        var doc = mupdf.Document.openDocument(input, "application/pdf")
        console.log(doc.countPages())
})


.. include:: footer.rst



.. External links


