#include "stdafx.h"

#include "dsheader.h"


#define LO(b) (b & 0xF)
#define HI(b) ((u8)(b) >> 4)

void ReadTile(char* romname, u8* tile, u16* palette)
{
    FILE* fp = fopen(romname, "rb");

	if (fp == NULL)
		return;

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
CDSImage::CDSImage()
{
    m_pref  = NULL;
    m_pbmi  = NULL;
    m_image = NULL;
}

CDSImage::CDSImage(char* dsrom)
{
    m_pref  = NULL;
	m_pbmi  = NULL;
    m_image = NULL;
    Init(dsrom);
}

CDSImage::CDSImage(const CDSImage& img)
{
    assign(img);
}

CDSImage::~CDSImage()
{
    Release();
}

void CDSImage::Release()
{
    if (!m_pref)
        return;
    
    (*m_pref)--;

    if (!*m_pref)
    {
        free(m_pref);
        free(m_pbmi);
        free(m_image);
    }

    m_pref  = NULL;
    m_pbmi  = NULL;
    m_image = NULL;
}

void CDSImage::assign(const CDSImage& img)
{
    this->m_pbmi  = img.m_pbmi;
    this->m_image = img.m_image;
    this->m_pref  = img.m_pref;

	if (m_pref)
        (*m_pref)++;
}

CDSImage& CDSImage::operator=(const CDSImage& img)
{
    Release();

    assign(img);
	return (*this);
}

BOOL CDSImage::Load(char* dsrom)
{
    Release();
    return Init(dsrom);
}

BOOL CDSImage::Init(char* dsrom)
{
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
    m_image = (u8*)malloc(sizeof(u8)*32*32);
    ReadDSImage(dsrom, m_image, pal);

    for (int i=0; i<16; ++i)
    {
        pbmi->bmiColors[i].rgbRed   = 255 * (pal[i] & 31) / 31;
        pbmi->bmiColors[i].rgbGreen = 255 * ((pal[i] >> 5) & 31) / 31;
        pbmi->bmiColors[i].rgbBlue  = 255 * ((pal[i] >> 10) & 31) / 31;
    }

    m_pbmi  = pbmi;
    m_pref  = (int*)malloc(sizeof(int));
    *m_pref = 1;

    return TRUE;
}
