//using System;
//using System.Drawing;
//using mupdf;

public class HelloWorld
{
    public static void Main(string[] args)
    {
        System.Console.WriteLine("Hello Mono World");
        mupdf.Document document = new mupdf.Document("zlib.clean.pdf");
        System.Console.WriteLine("num chapters: " + document.count_chapters());
        mupdf.Page page = document.load_page(0);
        mupdf.Rect rect = page.bound_page();
        System.Console.WriteLine("rect: " + rect.to_string());

        mupdf.Matrix matrix = new mupdf.Matrix(1, 0, 0, 1, 0, 0);
        mupdf.Colorspace colorspace = new mupdf.Colorspace(mupdf.Colorspace.Fixed.Fixed_RGB);
        mupdf.Pixmap pixmap = page.new_pixmap_from_page_contents(
                matrix,
                colorspace,
                0
                );
        /*System.Drawing.Bitmap bmp = new System.Drawing.Bitmap(
                pixmap.pixmap_width(),
                pixmap.pixmap_height(),
                System.Drawing.Imaging.PixelFormat.Format24bppRgb
                );*/
        //Bitmap^ bmp = gcnew System.Drawing.Bitmap(pixmap.pixmap_width(), pixmap.pixmap_height(), System.Drawing.Imaging.PixelFormat.Format24bppRgb);
        //System.Drawing.Rectangle rect2 = new System.Drawing.Rectangle(0, 0, pixmap.pixmap_width(), pixmap.pixmap_height());
        //System.Drawing.Imaging.BitmapData bmpData = bmp.LockBits(
        //        rect2,
        //        System.Drawing.Imaging.ImageLockMode.ReadWrite,
        //        bmp.PixelFormat
        //        );

        System.Drawing.Bitmap bmp = new System.Drawing.Bitmap(
                pixmap.pixmap_width(),
                pixmap.pixmap_height(),
                pixmap.pixmap_stride(),
                System.Drawing.Imaging.PixelFormat.Format24bppRgb,
                (System.IntPtr) pixmap.pixmap_samples_int()
                );
    }
}
