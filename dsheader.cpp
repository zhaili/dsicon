#include "stdafx.h"

#include "dsheader.h"


#define LO(b) (b & 0xF)
#define HI(b) ((u8)(b) >> 4)

void ReadTile(char* romname, u8* tile, u16* palette)
{
    FILE* fp = fopen(romname, "rb");

    NDS_header header;
    Banner     banner;

    fread(&header, sizeof(header), 1, fp);

    fseek(fp, header.IconOff, SEEK_SET);
    fread(&banner, sizeof(banner), 1, fp);

    u8* src = banner.tile;
    for (int i=0; i<32; ++i)
    {
        for (int j=0; j<32; j+=2)
        {
            tile[i*32+j]   = LO(*src);
            tile[i*32+j+1] = HI(*src);
            ++src;
        }
    }

    memcpy(palette, banner.palette, sizeof(banner.palette));

    fclose(fp);
}

void CopyBlock(u8* dst, int sz, int col, u8* src)
{
    for (int i=0; i<sz; ++i)
    {
        for (int j=0; j<sz; ++j)
        {
            *dst = *src;
            ++dst;
            ++src;
        }
        dst += col-sz;
    }
}

void Tile2Image(u8 tile[1024], u8 image[1024])
{
    u8* dst = image;
    u8* src = tile;

    for (int i=0; i<4; ++i)
    {
        for (int j=0; j<4; ++j)
        {
            CopyBlock(&dst[i*32*8+j*8], 8, 32, src);
            src += 64;
        }
    }
}

void ReadDSImage(char* filename, u8* image, u16* pal)
{
    u8 tile[32*32];

    ReadTile(filename, tile, pal);
    Tile2Image(tile, image);
}



///// Class CDSImage

CDSImage::CDSImage(char* dsrom)
{
	m_pbmi = NULL;
    Load(dsrom);
}

CDSImage::~CDSImage()
{
    Release();
}

BOOL CDSImage::Load(char* dsrom)
{
    Release();

	PBITMAPINFO pbmi = (PBITMAPINFO)malloc(
        sizeof(BITMAPINFO)+15*sizeof(RGBQUAD));

	PBITMAPINFOHEADER pbmih = &pbmi->bmiHeader;

	pbmih->biSize   = sizeof(BITMAPINFOHEADER);
	pbmih->biWidth  = 32;
	pbmih->biHeight = -32;

    pbmih->biPlanes        = 1;
    pbmih->biBitCount      = 8; /* bit depth */
    pbmih->biCompression   = BI_RGB;
    pbmih->biSizeImage     = 0; /* set to 0 for BI_RGB */
    pbmih->biXPelsPerMeter = 0;
    pbmih->biYPelsPerMeter = 0;
    pbmih->biClrUsed       = 16 /* color used */;
    pbmih->biClrImportant  = 0;

    u16 pal[16];
    ReadDSImage(dsrom, m_image, pal);

    for (int i=0; i<16; ++i)
    {
        pbmi->bmiColors[i].rgbRed   = 255 * (pal[i] & 31) / 31;
        pbmi->bmiColors[i].rgbGreen = 255 * ((pal[i] >> 5) & 31) / 31;
        pbmi->bmiColors[i].rgbBlue  = 255 * ((pal[i] >> 10) & 31) / 31;
    }

    m_pbmi = pbmi;

    return TRUE;
}

void CDSImage::Release()
{
    if (m_pbmi != NULL)
    {
        free(m_pbmi);
        m_pbmi = NULL;
    }
}
