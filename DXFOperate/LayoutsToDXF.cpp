/*
 * @file main.cpp
 */

/*****************************************************************************
**  $Id: main.cpp 3591 2006-10-18 21:23:25Z andrew $
**
**  This is part of the dxflib library
**  Copyright (C) 2000-2001 Andrew Mustun
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU Library General Public License as
**  published by the Free Software Foundation.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Library General Public License for more details.
**
**  You should have received a copy of the GNU Library General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
******************************************************************************/

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "src\dl_dxf.h"
#include "src\dl_creationadapter.h"

#include "test_creationclass.h"
#include <list>
//#include "LayoutRecord.h"
#include "Layout.h"
#include "..\LayoutsToDXF.h"
#include <LayoutRecord.h>

using namespace std;

//void usage();
//void testReading(char* file);
//void testWriting();


void LayoutsToDXF(list<Layout>* Layouts, int sheet_num, double utilization)
{
    string a;
    a = "Sheet" + to_string(sheet_num) + ".dxf";
    const char* b;
    b = a.c_str();
    DL_Dxf* dxf = new DL_Dxf();
    DL_Codes::version exportVersion = DL_Codes::AC1015;
    DL_WriterA* dw = dxf->out(b, exportVersion);
    if (dw == NULL) {
        printf("Cannot open dxf file ");
        // abort function e.g. with return
    }

    dxf->writeHeader(*dw);
    /*
    // int variable:
    dw->dxfString(9, "$INSUNITS");
    dw->dxfInt(70, 4);
    // real (double, float) variable:
    dw->dxfString(9, "$DIMEXE");
    dw->dxfReal(40, 1.25);
    // string variable:
    dw->dxfString(9, "$TEXTSTYLE");
    dw->dxfString(7, "Standard");
    // vector variable:
    dw->dxfString(9, "$LIMMIN");
    dw->dxfReal(10, 0.0);
    dw->dxfReal(20, 0.0);
    */
    dw->sectionEnd();
    dw->sectionTables();
    dxf->writeVPort(*dw);

    dw->tableLinetypes(3);
    dxf->writeLinetype(*dw, DL_LinetypeData("BYBLOCK", "BYBLOCK", 0, 0, 0.0));
    dxf->writeLinetype(*dw, DL_LinetypeData("BYLAYER", "BYLAYER", 0, 0, 0.0));
    dxf->writeLinetype(*dw,
        DL_LinetypeData("CONTINUOUS", "Continuous", 0, 0, 0.0));
    dw->tableEnd();

    int numberOfLayers = 3;
    dw->tableLayers(numberOfLayers);

    dxf->writeLayer(*dw, DL_LayerData("0", 0),
        DL_Attributes(std::string(""),     // leave empty
            DL_Codes::black,     // default color
            100,                 // default width
            "CONTINUOUS", 1.0)); // default line style

    dxf->writeLayer(
        *dw, DL_LayerData("platelayer", 0),
        DL_Attributes(std::string(""), DL_Codes::red, 100, "CONTINUOUS", 1.0));

    dxf->writeLayer(
        *dw, DL_LayerData("partlayer", 0),
        DL_Attributes(std::string(""), DL_Codes::blue, 100, "CONTINUOUS", 1.0));

    dw->tableEnd();

    dw->tableStyle(1);
    dxf->writeStyle(
        *dw, DL_StyleData("standard", 0, 2.5, 1.0, 0.0, 0, 2.5, "txt", ""));
    dw->tableEnd();

    dxf->writeView(*dw);
    dxf->writeUcs(*dw);

    dw->tableAppid(1);
    dxf->writeAppid(*dw, "ACAD");
    dw->tableEnd();

    dxf->writeDimStyle(*dw, 1, 1, 1, 1, 1);

    dxf->writeBlockRecord(*dw);
    dxf->writeBlockRecord(*dw, "myblock1");
    dxf->writeBlockRecord(*dw, "myblock2");
    dw->tableEnd();

    dw->sectionEnd();

    dw->sectionBlocks();
    dxf->writeBlock(*dw, DL_BlockData("*Model_Space", 0, 0.0, 0.0, 0.0));
    dxf->writeEndBlock(*dw, "*Model_Space");
    dxf->writeBlock(*dw, DL_BlockData("*Paper_Space", 0, 0.0, 0.0, 0.0));
    dxf->writeEndBlock(*dw, "*Paper_Space");
    dxf->writeBlock(*dw, DL_BlockData("*Paper_Space0", 0, 0.0, 0.0, 0.0));
    dxf->writeEndBlock(*dw, "*Paper_Space0");

    dxf->writeBlock(*dw, DL_BlockData("myblock1", 0, 0.0, 0.0, 0.0));
    // ...
    // write block entities e.g. with dxf->writeLine(), ..
    // ...
    dxf->writeEndBlock(*dw, "myblock1");

    dxf->writeBlock(*dw, DL_BlockData("myblock2", 0, 0.0, 0.0, 0.0));
    // ...
    // write block entities e.g. with dxf->writeLine(), ..
    // ...
    dxf->writeEndBlock(*dw, "myblock2");

    dw->sectionEnd();
    dw->sectionEntities();

    //// write all entities in model space:
    // dxf->writePoint(
    //     *dw,
    //     DL_PointData(10.0,
    //         45.0,
    //         0.0),
    //     DL_Attributes("platelayer", 256, -1, "BYLAYER", 1.0));

    // dxf->writeLine(
    //     *dw,
    //     DL_LineData(25.0,   // start point
    //         30.0,
    //         0.0,
    //         100.0,   // end point
    //         120.0,
    //         0.0),
    //     DL_Attributes("platelayer", 256, -1, "BYLAYER", 1.0));

    list<Layout>::iterator layout;

    layout = Layouts->begin();
    dxf->writeText(*dw,
        DL_TextData(0, -100, 0,       // text location
            0, 0, 0.0, // 对齐坐标，
            50, 1.5,        /// 文本高度和scale
            0, 0, 0, "Utilization: " + to_string(utilization * 100) + "%",
            "Standard", // 字体
            0),         // 旋转角度
        DL_Attributes("mainlayer", 255, 0, "BYLAYER", 1.0));


    while (layout != Layouts->end()) {
        dxf->writeLine(*dw,
            DL_LineData(0.0, // start point
                0.0, 0.0,
                layout->plate.Length, // end point
                0.0, 0.0),
            DL_Attributes("platelayer", 256, -1, "BYLAYER", 1.0));

        dxf->writeLine(*dw,
            DL_LineData(layout->plate.Length, // start point
                0.0, 0.0,
                layout->plate.Length, // end point
                layout->plate.Height, 0.0),
            DL_Attributes("platelayer", 256, -1, "BYLAYER", 1.0));

        dxf->writeLine(*dw,
            DL_LineData(layout->plate.Length, // end point
                layout->plate.Height, 0.0,
                0.0, // end point
                layout->plate.Height, 0.0),
            DL_Attributes("platelayer", 256, -1, "BYLAYER", 1.0));

        dxf->writeLine(*dw,
            DL_LineData(0.0, // start point
                layout->plate.Height, 0.0,
                0.0, // end point
                0.0, 0.0),
            DL_Attributes("platelayer", 256, -1, "BYLAYER", 1.0));

        list<NestedPart>::iterator nestedPart;
        // list<NestedPart> list = layout->nestedPart;

        nestedPart = (&layout->nestedPart)->begin();

        while (nestedPart != (&layout->nestedPart)->end()) {
            dxf->writeLine(*dw,
                DL_LineData(nestedPart->PartPositionX, // start point
                    nestedPart->PartPositionY, 0.0,
                    nestedPart->PartPositionX +
                    nestedPart->Length, // end point
                    nestedPart->PartPositionY, 0.0),
                DL_Attributes("partlayer", 256, -1, "BYLAYER", 1.0));

            dxf->writeLine(
                *dw,
                DL_LineData(
                    nestedPart->PartPositionX + nestedPart->Length, // end point
                    nestedPart->PartPositionY, 0.0,
                    nestedPart->PartPositionX + nestedPart->Length, // end point
                    nestedPart->PartPositionY + nestedPart->Height, 0.0),
                DL_Attributes("partlayer", 256, -1, "BYLAYER", 1.0));

            dxf->writeLine(
                *dw,
                DL_LineData(nestedPart->PartPositionX +
                    nestedPart->Length, // end point
                    nestedPart->PartPositionY + nestedPart->Height, 0.0,
                    nestedPart->PartPositionX, // end point
                    nestedPart->PartPositionY + nestedPart->Height, 0.0),
                DL_Attributes("partlayer", 256, -1, "BYLAYER", 1.0));

            dxf->writeLine(*dw,
                DL_LineData(nestedPart->PartPositionX, // end point
                    nestedPart->PartPositionY + nestedPart->Height,
                    0.0,
                    nestedPart->PartPositionX, // end point
                    nestedPart->PartPositionY, 0.0),
                DL_Attributes("partlayer", 256, -1, "BYLAYER", 1.0));

            cout << nestedPart->Code << " ";
            // cout << nestedPart->Material << " ";
            nestedPart++;
        }

        cout << layout->plate.Code << " ";
        cout << layout->plate.Material << " ";
        layout++;
    }
    cout << endl;

    dw->sectionEnd();

    dxf->writeObjects(*dw);
    dxf->writeObjectsEnd(*dw);


    dw->dxfEOF();
    dw->close();
    delete dw;
    delete dxf;
}



void LayoutRecordsToDXF(list<LayoutRecord>* Layouts)
{

    DL_Dxf* dxf = new DL_Dxf();
    DL_Codes::version exportVersion = DL_Codes::AC1015;
    DL_WriterA* dw = dxf->out("myfile.dxf", exportVersion);
    if (dw == NULL) {
        printf("Cannot open file 'myfile.dxf' \
               for writing.");
        // abort function e.g. with return
    }

    dxf->writeHeader(*dw);
    /*
    // int variable:
    dw->dxfString(9, "$INSUNITS");
    dw->dxfInt(70, 4);
    // real (double, float) variable:
    dw->dxfString(9, "$DIMEXE");
    dw->dxfReal(40, 1.25);
    // string variable:
    dw->dxfString(9, "$TEXTSTYLE");
    dw->dxfString(7, "Standard");
    // vector variable:
    dw->dxfString(9, "$LIMMIN");
    dw->dxfReal(10, 0.0);
    dw->dxfReal(20, 0.0);
    */
    dw->sectionEnd();
    dw->sectionTables();
    dxf->writeVPort(*dw);

    dw->tableLinetypes(3);
    dxf->writeLinetype(*dw, DL_LinetypeData("BYBLOCK", "BYBLOCK", 0, 0, 0.0));
    dxf->writeLinetype(*dw, DL_LinetypeData("BYLAYER", "BYLAYER", 0, 0, 0.0));
    dxf->writeLinetype(*dw, DL_LinetypeData("CONTINUOUS", "Continuous", 0, 0, 0.0));
    dw->tableEnd();

    int numberOfLayers = 3;
    dw->tableLayers(numberOfLayers);

    dxf->writeLayer(*dw,
        DL_LayerData("0", 0),
        DL_Attributes(
            std::string(""),      // leave empty
            DL_Codes::black,        // default color
            100,                  // default width
            "CONTINUOUS", 1.0));       // default line style

    dxf->writeLayer(*dw,
        DL_LayerData("platelayer", 0),
        DL_Attributes(
            std::string(""),
            DL_Codes::red,
            100,
            "CONTINUOUS", 1.0));

    dxf->writeLayer(*dw,
        DL_LayerData("partlayer", 0),
        DL_Attributes(
            std::string(""),
            DL_Codes::black,
            100,
            "CONTINUOUS", 1.0));

    dw->tableEnd();

    dw->tableStyle(1);
    dxf->writeStyle(*dw, DL_StyleData("standard", 0, 2.5, 1.0, 0.0, 0, 2.5, "txt", ""));
    dw->tableEnd();

    dxf->writeView(*dw);
    dxf->writeUcs(*dw);

    dw->tableAppid(1);
    dxf->writeAppid(*dw, "ACAD");
    dw->tableEnd();

    dxf->writeDimStyle(*dw, 1, 1, 1, 1, 1);

    dxf->writeBlockRecord(*dw);
    dxf->writeBlockRecord(*dw, "myblock1");
    dxf->writeBlockRecord(*dw, "myblock2");
    dw->tableEnd();

    dw->sectionEnd();

    dw->sectionBlocks();
    dxf->writeBlock(*dw, DL_BlockData("*Model_Space", 0, 0.0, 0.0, 0.0));
    dxf->writeEndBlock(*dw, "*Model_Space");
    dxf->writeBlock(*dw, DL_BlockData("*Paper_Space", 0, 0.0, 0.0, 0.0));
    dxf->writeEndBlock(*dw, "*Paper_Space");
    dxf->writeBlock(*dw, DL_BlockData("*Paper_Space0", 0, 0.0, 0.0, 0.0));
    dxf->writeEndBlock(*dw, "*Paper_Space0");

    dxf->writeBlock(*dw, DL_BlockData("myblock1", 0, 0.0, 0.0, 0.0));
    // ...
    // write block entities e.g. with dxf->writeLine(), ..
    // ...
    dxf->writeEndBlock(*dw, "myblock1");

    dxf->writeBlock(*dw, DL_BlockData("myblock2", 0, 0.0, 0.0, 0.0));
    // ...
    // write block entities e.g. with dxf->writeLine(), ..
    // ...
    dxf->writeEndBlock(*dw, "myblock2");

    dw->sectionEnd();
    dw->sectionEntities();

    //// write all entities in model space:
    //dxf->writePoint(
    //    *dw,
    //    DL_PointData(10.0,
    //        45.0,
    //        0.0),
    //    DL_Attributes("platelayer", 256, -1, "BYLAYER", 1.0));

    //dxf->writeLine(
    //    *dw,
    //    DL_LineData(25.0,   // start point
    //        30.0,
    //        0.0,
    //        100.0,   // end point
    //        120.0,
    //        0.0),
    //    DL_Attributes("platelayer", 256, -1, "BYLAYER", 1.0));

    list<LayoutRecord>::iterator record;

    record = Layouts->begin();

    while (record != Layouts->end())
    {
        dxf->writeLine(
            *dw,
            DL_LineData(25.0,   // start point
                30.0,
                0.0,
                100.0,   // end point
                120.0,
                0.0),
            DL_Attributes("platelayer", 256, -1, "BYLAYER", 1.0));

        cout << record->plate.Code << " ";
        cout << record->Material << " ";
        record++;
    }
    cout << endl;

    dw->sectionEnd();

    dxf->writeObjects(*dw);
    dxf->writeObjectsEnd(*dw);

    dw->dxfEOF();
    dw->close();
    delete dw;
    delete dxf;
}

