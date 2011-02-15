/* cl /Od /MTd /Zi /EHsc testnds.cpp dsheader.cpp */


#include "stdafx.h"

#include "..\vld\include\vld.h"

#include "dsheader.h"

int main()
{
    __asm int 3;

    CDSImage img1("e:\\test\\xpa-msog.nds");
    CDSImage img2("e:\\test\\xpa-msog.nds");

    CDSImage img3(img1);

    img1 = img2;

    img2 = img1;

	img3 = img2;

    img2.Release();

    img3.Release();

    img3 = img1;

    img1.Release();

    img3 = img1;

    //printf("%d", img1.GetImageData()[0]);

    return 0;
}
