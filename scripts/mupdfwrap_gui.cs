// Basic PDF viewer using MuPDF C# bindings.
//

public class MuPDFGui : System.Windows.Forms.Form
{
    // We use static pixmap to ensure it isn't garbage-collected.
    static mupdf.Pixmap pixmap;

    private System.Windows.Forms.MainMenu menu;
    private System.Windows.Forms.MenuItem menu_item_file;

    // Throws exception if pixmap and bitmap differ.
    void Check(mupdf.Pixmap pixmap, System.Drawing.Bitmap bitmap, int pixmap_bytes_per_pixel)
    {
        long samples = pixmap.pixmap_samples_int();
        if (pixmap.pixmap_width() != bitmap.Width || pixmap.pixmap_height() != bitmap.Height)
        {
            throw new System.Exception("Inconsistent sizes:"
                    + " pixmap=(" + pixmap.pixmap_width() + " " + pixmap.pixmap_height()
                    + " bitmap=(" + bitmap.Width + " " + bitmap.Height
                    );
        }
        int stride = pixmap.pixmap_stride();
        for (int x=0; x<bitmap.Width; x+=1)
        {
            for (int y=0; y<bitmap.Height; y+=1)
            {
                unsafe
                {
                    byte* sample = (byte*) samples + stride * y + pixmap_bytes_per_pixel * x;
                    System.Drawing.Color color = bitmap.GetPixel( x, y);
                    if (color.R != sample[0] || color.G != sample[1] || color.B != sample[2])
                    {
                        string pixmap_pixel_text = "";
                        for (int i=0; i<pixmap_bytes_per_pixel; ++i)
                        {
                            if (i > 0) pixmap_pixel_text += " ";
                            pixmap_pixel_text += sample[i];
                        }
                        throw new System.Exception("Pixels differ: (" + x + " " + y + "):"
                                + " pixmap: (" + pixmap_pixel_text + ")"
                                + " bitmap: " + color);
                    }
                }
            }
        }
    }

    public MuPDFGui()
    {

        menu_item_file = new System.Windows.Forms.MenuItem("File",
                new System.Windows.Forms.MenuItem[]
                {
                    new System.Windows.Forms.MenuItem("&Open..."),
                    new System.Windows.Forms.MenuItem("&Show html"),
                    new System.Windows.Forms.MenuItem("&Quit"),
                }
                );
        menu = new System.Windows.Forms.MainMenu(new System.Windows.Forms.MenuItem [] {menu_item_file});
        this.Menu = menu;

        System.Windows.Forms.PictureBox picture_box = new System.Windows.Forms.PictureBox();
        picture_box.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;

        // Load pdf document using mupdf.
        mupdf.Document document = new mupdf.Document("zlib.3.pdf");
        mupdf.Page page = document.load_page(0);

        mupdf.Rect rect = page.bound_page();
        System.Console.WriteLine("rect: " + rect);

        System.Drawing.Bitmap bitmap;

        if (System.Type.GetType("Mono.Runtime") != null)
        {
            /* Use pixmap data without copying. This does not work on
            Windows.

            It looks like it's important to use MuPDF Fixed_RGB with
            alpha=1, and C#'s Format32bppRgb. Other combinations,
            e.g. (Fixed_RGB with alpha=0) and Format24bppRgb, result in a
            blank display. */
            pixmap = page.new_pixmap_from_page_contents(
                    new mupdf.Matrix(1, 0, 0, 1, 0, 0),
                    new mupdf.Colorspace(mupdf.Colorspace.Fixed.Fixed_RGB),
                    1 /*alpha*/
                    );

            bitmap = new System.Drawing.Bitmap(
                    pixmap.pixmap_width(),
                    pixmap.pixmap_height(),
                    pixmap.pixmap_stride(),
                    System.Drawing.Imaging.PixelFormat.Format32bppRgb,
                    (System.IntPtr) pixmap.pixmap_samples_int()
                    );

            if (true)
            {
                Check(pixmap, bitmap, 4);
            }
        }
        else
        {
            /* Copy pixmap's pixels into bitmap. This works on both Linux
            (Mono) and Windows.

            Unlike above, it seems that we need to use MuPDF Fixed_RGB with
            alpha=0, and C#'s Format32bppRgb. Other combinations give a
            blank display (possibly with alpha=0 for each pixel). */
            pixmap = page.new_pixmap_from_page_contents(
                    new mupdf.Matrix(1, 0, 0, 1, 0, 0),
                    new mupdf.Colorspace(mupdf.Colorspace.Fixed.Fixed_RGB),
                    0 /*alpha*/
                    );

            bitmap = new System.Drawing.Bitmap(
                    pixmap.pixmap_width(),
                    pixmap.pixmap_height(),
                    System.Drawing.Imaging.PixelFormat.Format32bppRgb
                    );
            long samples = pixmap.pixmap_samples_int();
            int stride = pixmap.pixmap_stride();
            for (int x=0; x<bitmap.Width; x+=1)
            {
                for (int y=0; y<bitmap.Height; y+=1)
                {
                    unsafe
                    {
                        byte* sample = (byte*) samples + stride * y + 3 * x;
                        var color = System.Drawing.Color.FromArgb(sample[0], sample[1], sample[2]);
                        bitmap.SetPixel( x, y, color);
                    }
                }
            }

            if (true)
            {
                Check(pixmap, bitmap, 3);
            }
        }

        // Show bitmap in our window.
        picture_box.Image = bitmap;
        Controls.Add(picture_box);
        Width = picture_box.Right;
        Height = picture_box.Bottom;
    }

    public static void Main()
    {
        System.Console.WriteLine("MuPDF C# gui test starting.");
        System.Threading.Thread.CurrentThread.Name = "Main thread";
        System.Windows.Forms.Application.Run(new MuPDFGui());
        System.Console.WriteLine("MuPDF C# gui test finished.");
    }
}
