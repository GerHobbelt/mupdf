using System;
using mupdf;

public class HelloWorld
{
    public static void Main(string[] args)
    {
        Console.WriteLine("Hello Mono World");
        mupdf.Document document = new mupdf.Document("zlib.clean.pdf");
        Console.WriteLine("num chapters: " + document.count_chapters());
        mupdf.Page page = document.load_page(0);
        mupdf.Rect rect = page.bound_page();
        Console.WriteLine("rect: " + rect.to_string());

        mupdf.Matrix matrix = new mupdf.Matrix(1, 0, 0, 1, 0, 0);
        mupdf.Colorspace colorspace = new mupdf.Colorspace(mupdf.Colorspace.Fixed.Fixed_RGB);
        mupdf.Pixmap pixmap = page.new_pixmap_from_page_contents(
                matrix,
                colorspace,
                0
                );
    }
}
