// https://www.mono-project.com/docs/gui/winforms/

namespace System.Windows.Forms {

    public class PictureBoxDemo : System.Windows.Forms.Form {

        public PictureBoxDemo()
        {
            System.Windows.Forms.PictureBox box = new System.Windows.Forms.PictureBox();
            box.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;

            mupdf.Document document = new mupdf.Document("zlib.3.pdf");
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

            /* this doesn't seem to work... presuambly the samples are not accessible?

            System.Drawing.Bitmap bitmap = new System.Drawing.Bitmap(
                    pixmap.pixmap_width(),
                    pixmap.pixmap_height(),
                    pixmap.pixmap_stride(),
                    System.Drawing.Imaging.PixelFormat.Format24bppRgb,
                    (System.IntPtr) pixmap.pixmap_samples_int()
                    );*/

            // Create bitmap same size as pixmap.
            System.Drawing.Bitmap bitmap = new System.Drawing.Bitmap(
                    pixmap.pixmap_width(),
                    pixmap.pixmap_height()
                    );

            // Copy pixmap data into bitmap.
            long samples = pixmap.pixmap_samples_int();
            int x;
            for (x=0; x<bitmap.Width; x+=1)
            {
                int y;
                for (y=0; y<bitmap.Height; y+=1)
                {
                    unsafe
                    {
                        byte* sample = (byte*) samples + pixmap.pixmap_stride() * y + 3 * x;
                        bitmap.SetPixel( x, y,
                                System.Drawing.Color.FromArgb(
                                        sample[0],
                                        sample[1],
                                        sample[2]
                                        )
                                );
                    }
                }
            }

            box.Image = bitmap;
            Controls.Add(box);
            Width = box.Right;
            Height = box.Bottom;
        }

        public static void Main()
        {
            System.Threading.Thread.CurrentThread.Name = "Main Thread";
            Application.Run (new PictureBoxDemo());
        }
    }
}
