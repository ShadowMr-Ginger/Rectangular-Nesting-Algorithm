#ifndef LayoutsToDXF_H
#define LayoutsToDXF_H
#include "Layout.h"
#include "LayoutRecord.h"
void LayoutsToDXF(list<Layout>* Layouts, int sheet_num, double utilization);
void LayoutRecordsToDXF(list<LayoutRecord>* Layouts);

#endif;