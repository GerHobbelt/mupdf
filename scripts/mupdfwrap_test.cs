public class HelloWorld
{
    struct Buffer
    {
        unsafe public fixed byte buffer[32];
    }

    unsafe public class FzStream3 : mupdf.FzStream2
    {
        unsafe Buffer m_buffer;
        //public fixed byte buffer[32];
        //unsafe public byte* m_buffer;
        int m_buffer_length;
        public int m_i;

        public FzStream3()
        {
            System.Console.WriteLine("## FzStream3().");
            //m_buffer0 = new Buffer();
            //m_buffer_length = 32;
            //m_buffer = new byte[ m_buffer_length];

            m_i = 0;
            use_virtual_next();
            System.Console.WriteLine("## FzStream3() have called use_virtual_next().");
        }

        public override int next(mupdf.fz_context ctx, uint max)
        {
            System.Console.WriteLine("## next(): max={0}", max);
            unsafe
            {
                m_buffer.buffer[0] = (byte) (65 + m_i);
                m_i += 1;
                //m_internal.rp = buffer;
                //m_internal.wp = buffer + 1;
            }
            return 1;
        }
    }
    public static void Main(string[] args)
    {
        System.Console.WriteLine("MuPDF C# test starting.");

        // Test FzStream2
        var stream = new FzStream3();
        mupdf.FzDocument document0 = new mupdf.FzDocument("pdf", stream);

        // Check we can load a document.
        mupdf.FzDocument document = new mupdf.FzDocument("zlib.3.pdf");
        System.Console.WriteLine("document: " + document);
        System.Console.WriteLine("num chapters: " + document.fz_count_chapters());
        mupdf.FzPage page = document.fz_load_page(0);
        mupdf.FzRect rect = page.fz_bound_page();
        System.Console.WriteLine("rect: " + rect);
        if ("" + rect != rect.to_string())
        {
            throw new System.Exception("rect ToString() is broken: '" + rect + "' != '" + rect.to_string() + "'");
        }

        // Check we can load a document with utf filename. This will have been
        // set up by scripts/wrap/__main__.py.
        //byte[] infix_utf8 = {0xef, 0xbf, 0xbd, 0xef, 0xbf, 0xbd, 0xef, 0xbf, 0xbd, 0xef, 0xbf, 0xbd, 0xef, 0xbf, 0xbd};
        byte[] infix_utf8 = {0xc3, 0xaa, 0xc3, 0x9f, 0xc3, 0xb6, 0xc3, 0xa4, 0xc3, 0xbc};

        string infix = System.Text.Encoding.UTF8.GetString(infix_utf8);
        string path = "zlib.3." + infix + ".pdf";

        System.Console.OutputEncoding = System.Text.Encoding.UTF8;
        System.Console.WriteLine("Opening non-ascii path: " + path);

        //byte[] path3 = System.Text.Encoding.UTF8.GetBytes(path);
        //System.Console.WriteLine("Opening with utf8 bytes: " + path3);
        //mupdf.FzDocument document3 = new mupdf.FzDocument(path3);

        System.Console.WriteLine("Opening with c# string: " + path);
        mupdf.FzDocument document2 = new mupdf.FzDocument(path);

        // Test conversion to html using docx device.
        var buffer = page.fz_new_buffer_from_page_with_format(
                "docx",
                "html",
                new mupdf.FzMatrix(1, 0, 0, 1, 0, 0),
                new mupdf.FzCookie()
                );
        var data = buffer.fz_buffer_extract();
        var s = System.Text.Encoding.UTF8.GetString(data, 0, data.Length);
        if (s.Length < 100) {
            throw new System.Exception("HTML text is too short");
        }
        System.Console.WriteLine("s=" + s);

        // Check that previous buffer.fz_buffer_extract() cleared the buffer.
        data = buffer.fz_buffer_extract();
        s = System.Text.Encoding.UTF8.GetString(data, 0, data.Length);
        if (s.Length > 0) {
            throw new System.Exception("Buffer was not cleared.");
        }

        // Check we can create pixmap from page.
        var pixmap = page.fz_new_pixmap_from_page_contents(
                new mupdf.FzMatrix(1, 0, 0, 1, 0, 0),
                new mupdf.FzColorspace(mupdf.FzColorspace.Fixed.Fixed_RGB),
                0 /*alpha*/
                );

        // Check returned tuple from bitmap.fz_bitmap_details().
        var w = 100;
        var h = 200;
        var n = 4;
        var xres = 300;
        var yres = 300;
        var bitmap = new mupdf.FzBitmap(w, h, n, xres, yres);
        (var w2, var h2, var n2, var stride) = bitmap.fz_bitmap_details();
        System.Console.WriteLine("bitmap.fz_bitmap_details() returned:"
                + " " + w2 + " " + h2 + " " + n2 + " " + stride);
        if (w2 != w || h2 != h) {
            throw new System.Exception("Unexpected tuple values from bitmap.fz_bitmap_details().");
        }

        // Check we get exception from MuPDF. As of 2024-06-14 this exception
        // does not contain the original MuPDF exception text, it just says
        // "External component has thrown an exception."
        //
        // We only do this test if not running on Mono - Mono fails
        // with:
        //
        // > terminate called after throwing an instance of
        // > 'mupdf::FzErrorSystem'
        //
        if (System.Type.GetType("Mono.Runtime") == null)
        {
            int received_exception = 0;
            try
            {
                mupdf.FzDocument document2 = new mupdf.FzDocument("does not exist");
                System.Console.WriteLine("*** Error, did not get expected exception.");
            }
            catch (System.Exception e)
            {
                received_exception = 1;
                System.Console.WriteLine("Received exception: " + e.Message);
            }
            if (received_exception != 1)
            {
                throw new System.Exception("Did not receive expected exception");
            }
        }
        else
        {
            System.Console.WriteLine("Not checking handling of exceptions because running on Mono.");
        }

        // Check we can make MuPDF open filename containing 4-byte unicode
        // character.
        byte[] text_utf8 =
        {
                0xf0,
                0x90,
                0x90,
                0xb7,
        };
        string testfile2 = "zlib.3.pdf"
                + System.Text.Encoding.UTF8.GetString(text_utf8)
                + ".pdf";
        System.Console.WriteLine("Opening testfile2: " + testfile2);
        try
        {
            mupdf.FzDocument document2 = new mupdf.FzDocument(testfile2);
            System.Console.WriteLine("new mupdf.FzDocument succeeded");
        }
        catch (System.Exception e)
        {
            System.Console.WriteLine("Exception: " + e.Message);
            throw new System.Exception("Failed to open filename containing 4-byte unicode character");
        }

        System.Console.WriteLine("MuPDF C# test finished.");
    }
}
