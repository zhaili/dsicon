#pragma once

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#pragma pack(push)
#pragma pack(1)

struct NDS_header
{
       char     gameTile[12];
       char     gameCode[4];
       u16      makerCode;
       u8       unitCode;
       u8       deviceCode;
       u8       cardSize;
       u8       cardInfo[9];
       u8       flags;
	   u8		romversion;
       
       u32      ARM9src;
       u32      ARM9exe;
       u32      ARM9cpy;
       u32      ARM9binSize;
       
       u32      ARM7src;
       u32      ARM7exe;
       u32      ARM7cpy;
       u32      ARM7binSize;
       
       u32      FNameTblOff;
       u32      FNameTblSize;
       
       u32      FATOff;
       u32      FATSize;
       
       u32     ARM9OverlayOff;
       u32     ARM9OverlaySize;
       u32     ARM7OverlayOff;
       u32     ARM7OverlaySize;
       
       u32     unknown2a;
       u32     unknown2b;
       
       u32     IconOff;
       u16     CRC16;
       u16     ROMtimeout;
       u32     ARM9unk;
       u32     ARM7unk;
       
       u8      unknown3c[8];
       u32     ROMSize;
       u32     HeaderSize;
       u8      unknown5[56];
       u8      logo[156];
       u16     logoCRC16;
       u16     headerCRC16;
       u8      zero[160];
};

struct Banner
{
    u16  version;
    u16  crc32;
    char reserved[28];
    u8   tile[512];
    u8   palette[32];
};

#pragma pack(pop)

class CDSImage{
public:
	CDSImage() { m_pbmi = NULL; }
	CDSImage(char* dsrom);
    ~CDSImage();

	BOOL Load(char* dsrom);
	void Release();

    const PBITMAPINFO GetBitmapInfo() const { return m_pbmi; }
    const u8* GetImageData() const { return m_image; }
private:
	PBITMAPINFO m_pbmi;
    u8          m_image[32*32];
};
