using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace DCTDenoiseTest
{
    public unsafe partial class FrmTest : Form
    {
        enum IP_DEPTH
        {
            IP_DEPTH_8U = 0,
            IP_DEPTH_8S = 1,
            IP_DEPTH_16S = 2,
            IP_DEPTH_32S = 3,
            IP_DEPTH_32F = 4,
            IP_DEPTH_64F = 5,
        };

        enum IP_RET
        {
            RET_OK,
            RET_ERR_OUTOFMEMORY,
            RET_ERR_NULLREFERENCE,
            RET_ERR_ARGUMENTOUTOFRANGE,
            RET_ERR_PARAMISMATCH,
            RET_ERR_DIVIDEBYZERO,
            RET_ERR_NOTSUPPORTED,
            RET_ERR_UNKNOWN
        };
  
        [StructLayout(LayoutKind.Sequential)]   
        public unsafe struct TMatrix
        {

            public int Width;               //	The width of the matrix
            public int Height;				//	The height of the matrix
	        public int Channel;             //	The number of matrix channels
            public int Depth;               //	The type of matrix element
            public int WidthStep;           //	The number of bytes occupied by a row of elements in the matrix
            public int RowAligned;			//	Whether to use four-byte alignment
            public byte* Data;	            //	Matrix data
            public TMatrix(int Width, int Height, int WidthStep, int Depth, int Channel, byte* Scan0)
            {
                this.Width = Width;
                this.Height = Height;
                this.WidthStep = WidthStep;
                this.Depth = Depth;
                this.Channel = Channel;
                this.Data = Scan0;
                this.RowAligned = 1;
            }
        };

        [DllImport("DCT_Denoising.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Unicode, ExactSpelling = true)]
        private static extern IP_RET DCT_Denoising(ref TMatrix Src, ref TMatrix Dest, float Sigma_C);


        Bitmap SrcBmp, DstBmp;
        TMatrix SrcImg, DestImg;
        private bool Busy = false;

        public FrmTest()
        {
            InitializeComponent();
        }   

        private void CmdOpen_Click(object sender, EventArgs e)
        {

            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "All files(*.*) |*.*|Bitmap files (*.Bitmap)|*.Bmp|Jpeg files (*.jpg)|*.jpg|Png files (*.png)|*.png";
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                Bitmap Bmp = (Bitmap)Bitmap.FromFile(openFileDialog.FileName);
                SrcBmp = (Bitmap)Bmp.Clone();
                DstBmp = (Bitmap)Bmp.Clone();
                Bmp.Dispose();
                PicSrc.Image = SrcBmp;
                PicDest.Image = DstBmp;
                GetImageData();
            }
        }

        private void FrmTest_Load(object sender, EventArgs e)
        {

            SrcBmp = (Bitmap)PicSrc.Image;
            DstBmp = (Bitmap)PicDest.Image;
            GetImageData();
        }

        private void CmdSave_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "Bitmap files (*.Bitmap)|*.Bmp|Jpeg files (*.jpg)|*.jpg|Png files (*.png)|*.png";
            saveFileDialog.FilterIndex = 3;
            saveFileDialog.RestoreDirectory = true;
            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                if (saveFileDialog.FilterIndex == 1)
                    DstBmp.Save(saveFileDialog.FileName, ImageFormat.Bmp);
                else if (saveFileDialog.FilterIndex == 2)
                    DstBmp.Save(saveFileDialog.FileName, ImageFormat.Jpeg);
                else if (saveFileDialog.FilterIndex == 3)
                    DstBmp.Save(saveFileDialog.FileName, ImageFormat.Png);
            }
        }

        void GetImageData()
        {
            BitmapData SrcBmpData = SrcBmp.LockBits(new Rectangle(0, 0, SrcBmp.Width, SrcBmp.Height), ImageLockMode.ReadWrite, SrcBmp.PixelFormat);
            BitmapData DstBmpData = DstBmp.LockBits(new Rectangle(0, 0, DstBmp.Width, DstBmp.Height), ImageLockMode.ReadWrite, DstBmp.PixelFormat);

            int ImgWidth = SrcBmp.Width;
            int ImgHeight = SrcBmp.Height;
            int ImgStride = SrcBmpData.Stride;
            byte * Src = (byte*)SrcBmpData.Scan0;
            byte * Dest = (byte*)DstBmpData.Scan0;

            if (SrcBmpData.PixelFormat == PixelFormat.Format8bppIndexed)
            {
                SrcImg = new TMatrix(ImgWidth, ImgHeight, ImgStride, (int)IP_DEPTH.IP_DEPTH_8U, 1, Src);
                DestImg = new TMatrix(ImgWidth, ImgHeight, ImgStride, (int)IP_DEPTH.IP_DEPTH_8U, 1, Dest);
            }
            else if (SrcBmpData.PixelFormat == PixelFormat.Format24bppRgb)
            {
                SrcImg = new TMatrix(ImgWidth, ImgHeight, ImgStride, (int)IP_DEPTH.IP_DEPTH_8U, 3, Src);
                DestImg = new TMatrix(ImgWidth, ImgHeight, ImgStride, (int)IP_DEPTH.IP_DEPTH_8U, 3, Dest);
            }
            else if (SrcBmpData.PixelFormat == PixelFormat.Format32bppArgb && DstBmpData.PixelFormat == PixelFormat.Format32bppArgb)
            {
                SrcImg = new TMatrix(ImgWidth, ImgHeight, ImgStride, (int)IP_DEPTH.IP_DEPTH_8U, 4, Src);
                DestImg = new TMatrix(ImgWidth, ImgHeight, ImgStride, (int)IP_DEPTH.IP_DEPTH_8U, 4, Dest);
            }
            SrcBmp.UnlockBits(SrcBmpData);
            DstBmp.UnlockBits(DstBmpData);
        }

        private void CmdProcess_Click(object sender, EventArgs e)
        {
            Stopwatch Sw = new Stopwatch();
            Sw.Start();
            DCT_Denoising(ref SrcImg, ref DestImg, SigmaC.Value * 0.1f);
            Sw.Stop();
            LblInfo.Text = "Algorithm processing time: " + Sw.ElapsedMilliseconds.ToString() + " ms";
            PicDest.Refresh();
        }

        private void SigmaC_Scroll(object sender, ScrollEventArgs e)
        {
            label2.Text = (SigmaC.Value * 0.1).ToString();
        }
    }
}
