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

        System.Drawing.Bitmap bitmap = new System.Drawing.Bitmap(
                pixmap.pixmap_width(),
                pixmap.pixmap_height(),
                pixmap.pixmap_stride(),
                System.Drawing.Imaging.PixelFormat.Format24bppRgb,
                (System.IntPtr) pixmap.pixmap_samples_int()
                );

        System.Windows.Forms.PictureBox P = new System.Windows.Forms.PictureBox();
        System.Windows.Forms.Form form = new System.Windows.Forms.Form
        {
            Name = "Screenshot Displayer", Size = new System.Drawing.Size(800, 800), Location = new System.Drawing.Point(140, 170), Visible = true
        };
        P.Image = bitmap;
        //P.Dock = DockStyle.Fill;
        form.Controls.Add(P);
        form.Show();
    }
}
