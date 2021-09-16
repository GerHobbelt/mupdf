using System;
using System.Drawing;
using System.Windows.Forms;

// https://www.mono-project.com/docs/gui/winforms/

namespace System.Windows.Forms {

    public class PictureBoxDemo : Form {

        public PictureBoxDemo()
        {
            PictureBox box = new PictureBox();
            box.SizeMode = PictureBoxSizeMode.AutoSize;

            mupdf.Document document = new mupdf.Document("zlib.clean.pdf");
            mupdf.Page page = document.load_page(0);
            //mupdf.Rect rect = page.bound_page();
            mupdf.Matrix matrix = new mupdf.Matrix(1, 0, 0, 1, 0, 0);
            mupdf.Colorspace colorspace = new mupdf.Colorspace(mupdf.Colorspace.Fixed.Fixed_RGB);
            mupdf.Pixmap pixmap = page.new_pixmap_from_page_contents(
                    matrix,
                    colorspace,
                    0
                    );

            /* this doesn't seem to work... maybe the samples are no accessible?

            System.Drawing.Bitmap bitmap = new System.Drawing.Bitmap(
                    pixmap.pixmap_width(),
                    pixmap.pixmap_height(),
                    pixmap.pixmap_stride(),
                    System.Drawing.Imaging.PixelFormat.Format24bppRgb,
                    (System.IntPtr) pixmap.pixmap_samples_int()
                    );*/
            System.Drawing.Bitmap bitmap = new System.Drawing.Bitmap(
                    pixmap.pixmap_width(),
                    pixmap.pixmap_height()
                    );

            int x;
            int c = 0;
            for (x=0; x<bitmap.Width; x+=1)
            {
                int y;
                for (y=0; y<bitmap.Height; y+=1)
                {

                    if (c == 0)
                    {
                        System.Drawing.Color color = bitmap.GetPixel(x, y);
                        //System.Console.WriteLine(" color=" + color);
                        color = System.Drawing.Color.FromArgb(255, 0, 0);
                        bitmap.SetPixel(x, y, color);
                        //System.Console.WriteLine("*color=" + bitmap.GetPixel(x, y));
                    }
                    else if (c==4)
                    {
                        bitmap.SetPixel(x, y, System.Drawing.Color.FromArgb( 0, 255, 0));
                    }
                    else
                    {
                        unsafe
                        {
                            System.IntPtr data = (System.IntPtr) pixmap.pixmap_samples_int();
                            byte* data_b = (byte*) data;
                            data_b += pixmap.pixmap_stride() * y + x;
                            byte r = data_b[0];
                            bitmap.SetPixel(x, y,
                                    System.Drawing.Color.FromArgb(
                                            data_b[0],
                                            data_b[1],
                                            data_b[2]
                                            )
                                    );
                        }
                    }
                    c += 1;
                    if (c == 7) c = 0;
                }
            }

            //System.Drawing.Image image = bitmap;

            //image = Image.FromFile("zlib.png");
            box.Image = bitmap;

            //box.Image = Image.FromFile("zlib.png");
            //box.Left = 0 * box.Image.Width;
            //box.Top = 0 * box.Image.Height;
            Controls.Add(box);
            Width = box.Right + 10;
            Height = box.Bottom + 10;
        }

        public static void Main()
        {
            System.Threading.Thread.CurrentThread.Name = "Main Thread";
            Application.Run (new PictureBoxDemo());
        }
    }
}


/*namespace System.Windows.Forms {

        public class PictureBoxDemo : Form {

                public PictureBoxDemo ()
                {
                        PictureBox box = null;

                        for (int r = 0; r < 3; r++) {
                                for (int c = 0; c < 3; c++) {
                                        box = new PictureBox ();
                                        box.SizeMode = PictureBoxSizeMode.AutoSize;
                                        box.Image = Image.FromFile ("roxy.jpg");
                                        box.Left = 10 + c * box.Image.Width + (c * 10);
                                        box.Top = 10 + r * box.Image.Height + (r * 10);
                                        Controls.Add (box);
                                }
                        }
                        Width = box.Right + 10;
                        Height = box.Bottom + 10;
                }

                public static void Main ()
                {
                        System.Threading.Thread.CurrentThread.Name = "Main Thread";
                        Application.Run (new PictureBoxDemo ());
                }
        }
}
*/

/*
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

        int x;
        int c = 0;
        for (x=0; x<bitmap.Width; x+=1)
        {
            int y;
            for (y=0; y<bitmap.Height; y+=1)
            {
                if (c == 0)
                {
                    System.Drawing.Color color = bitmap.GetPixel(x, y);
                    //System.Console.WriteLine(" color=" + color);
                    color = System.Drawing.Color.FromArgb(255, 0, 1);
                    bitmap.SetPixel(x, y, color);
                    //System.Console.WriteLine("*color=" + bitmap.GetPixel(x, y));
                }
                c += 1;
                if (c == 3) c = 0;
            }
        }

        using (System.Windows.Forms.Form form = new System.Windows.Forms.Form())
        {
            form.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            form.Size = bitmap.Size;
            System.Console.WriteLine("form.Size=" + form.Size);
            System.Windows.Forms.PictureBox picturebox = new System.Windows.Forms.PictureBox();
            picturebox.Dock = System.Windows.Forms.DockStyle.Fill;
            picturebox.Image = bitmap;
            picturebox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            form.Controls.Add( picturebox);
            form.ShowDialog();
        }
    }
}
*/
