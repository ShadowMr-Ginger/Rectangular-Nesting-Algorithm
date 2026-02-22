#include "RectangleNestTask.h"


RectangleNestTask::RectangleNestTask(list<RectanglePart> partList_toNest, double plateLength, double plateWidth)
{
	this->plateLength = plateLength;
	this->plateWidth = plateWidth;
	this->partList_toNest = partList_toNest;
}

void RectangleNestTask::addPart(string materialType, int partNumber, double partLength, double partWidth)
{
    RectanglePart rectangle = RectanglePart::RectanglePart(materialType, partNumber, partLength, partWidth);
	this->partList_toNest.push_back(rectangle);
}

void RectangleNestTask::addPart(RectanglePart rectanglePart)
{
	this->partList_toNest.push_back(rectanglePart);
}

void RectangleNestTask::load_parts_from_CSV(string filename)
{
    list<RectanglePart> rectlist1;
    double a = 0;
    double b = 0;
    int d = 0;
    string c;
    num_product = 0;
	jay::util::CSVread csv_read(filename,
		jay::util::CSVread::strict_mode
		//| jay::util::CSVread::text_mode
		| jay::util::CSVread::process_empty_records
		//| jay::util::CSVread::skip_utf8_bom_check
	);
    if (csv_read.error) {
        cerr << "CSVread failed: " << csv_read.error_msg << endl;
        exit(1);
    }

    while (csv_read.ReadRecord()) {
        // cout << endl << "Record #" << csv_read.record_num << endl;
        if (csv_read.record_num == 1) {
            continue;
        }
        ++num_product;
        for (int i = 0; i < stod(csv_read.fields[2]); ++i) {
            d = stoi(csv_read.fields[0]);
            c = csv_read.fields[1];
            a = stod(csv_read.fields[3]);
            b = stod(csv_read.fields[4]);
            RectanglePart rect1 = RectanglePart::RectanglePart(c,d,a,b);
            rectlist1.push_back(rect1);
        }
    }

    if (csv_read.eof && (csv_read.record_num == csv_read.end_record_num)) {
        cout << "All records read successfully. (" << csv_read.end_record_num << ")"
            << endl;
        if (csv_read.end_record_not_terminated) {
            cout << "WARNING: End record not terminated!" << endl;
        }
    }
    else if (csv_read.error) {
        cerr << "Error: " << csv_read.error_msg << endl;
    }
    this->partList_toNest = rectlist1;
}

void RectangleNestTask::PerformNestingTask()
{
    this->readINI();

    // 初始化vector数组来存储products信息
    vector<double> products;
    vector<double> materials;
    int num_material = 0;
    // 转换product格式 
    emxArray_real_T* partsSize;
    partsSize = emxCreate_real_T(num_product, 2);
    RectanglePart part1;
    list<RectanglePart>::iterator rect0 = this->partList_toNest.begin();
    for (int i = 0; i < num_product - 1; ++i) {
        part1 = *rect0;
        partsSize->data[i] = part1.partLength;
        partsSize->data[i + num_product] = part1.partWidth;
        advance(rect0, 1);
    };
    part1 = *rect0;
    partsSize->data[num_product - 1] = part1.partLength;
    partsSize->data[num_product * 2 - 1] = part1.partWidth;


    double plateLength_Width[2];
    plateLength_Width[0] = this->plateLength;
    plateLength_Width[1] = this->plateWidth;

    emxInitArray_real_T(&layouts, 2);
    emxInitArray_real_T(&sheetDetails, 2);
    emxInitArray_real_T(&partIsNested, 1);

    /* Call the entry-point 'nest_and_calc_Ver3'. */
    Nest_and_reNest(partsSize, plateLength_Width, tightnessCoef, interval_distance_parts,
        interval_distance_plate, S_coef, reNestStepLength,
        this->layouts, this->sheetDetails, &this->num_plate, &this->lastPlateSurplusLength,
        &this->utilization, this->partIsNested);
    int cantNestNumber = 0;
    double *partIsNestedData = partIsNested->data;
    for (int i = 0; i < num_product; ++i) {
        if (partIsNestedData[i] < 1) {
            cantNestNumber = cantNestNumber + 1;
        }
    }
    if (cantNestNumber > 0) {
        cout << "以下方形件尺寸过大，无法完成套料：" << endl;
        for (int i = 0; i < num_product; ++i) {
            if (partIsNestedData[i] < 1) {
                cout << "第" << i + 1 << "个零件" << endl;
            }
        }
    }
    else {
        //cout << "所有方形件都已成功套料" << endl;
    }
}

void RectangleNestTask::OutputCSV()
{
    try
    {
        double* layouts_data = this->layouts->data;
        double* sheetDetails_data = this->sheetDetails->data;
        /*结果输出部分（包含dxf结果和csv结果）*/
          // 写入csv文件，输出结果
        jay::util::CSVwrite csv_write("result.csv",
            jay::util::CSVwrite::text_mode
            //| jay::util::CSVread::text_mode
            | jay::util::CSVwrite::process_empty_records
            //| jay::util::CSVread::skip_utf8_bom_check
        );
        // csv_write.truncate;
        double plateLength_Width[2];
        plateLength_Width[0] = this->plateLength;
        plateLength_Width[1] = this->plateWidth;
        vector<string> out = { "原片材质",     "原片序号",  "产品id",
                              "产品x坐标",    "产品y坐标", "产品x方向长度",
                              "产品y方向长度" };
        int x_location = 0;
        int y_location = 0;

        ////////////////////////////// 以下开始输出
        csv_write.WriteRecord(out);
        for (int i = 0; i < num_plate; ++i) {
            for (int j = 0; j < num_product; ++j) {
                if (i + 1 == layouts_data[j]) {
                    out[0] = "默认";
                    out[1] = to_string(layouts_data[j]);
                    out[2] = to_string(layouts_data[j + num_product]);
                    out[3] = to_string(layouts_data[j + num_product * 2]);
                    out[4] = to_string(layouts_data[j + num_product * 3]);
                    out[5] = to_string(layouts_data[j + num_product * 4]);
                    out[6] = to_string(layouts_data[j + num_product * 5]);
                    csv_write.WriteRecord(out);
                }
            }
        }
        cout << "结果生成至result.csv文件中" << endl;
    }
    catch (const std::exception&)
    {
        cout << "请先调用PerformNestingTask方法完成矩形排样！" << endl;
    }
}

void RectangleNestTask::OutputDXF()
{
    try
    {
        double* layouts_data = this->layouts->data;
        double* sheetDetails_data = this->sheetDetails->data;
        double plateLength_Width[2];
        plateLength_Width[0] = this->plateLength;
        plateLength_Width[1] = this->plateWidth;
        list<Layout>* Layouts;
        Layouts = new list<Layout>();
        Layout* layout;
        Plate* plate;
        Part* part;
        NestedPart* nestedPart;
        list<NestedPart>* nestedParts;

        nestedParts = new list<NestedPart>();
        for (int i = 0; i < num_plate; ++i) {
            plate = new Plate("Q345", i + 1, plateLength_Width[0], plateLength_Width[1]);
            for (int j = 0; j < num_product; ++j) {
                if (i + 1 == layouts_data[j]) {
                    int a = int(layouts_data[j + num_product] + 0.001);
                    part = new Part("Q345", a,
                        layouts_data[j + num_product * 4],
                        layouts_data[j + num_product * 5]);
                    nestedPart = new NestedPart(*part, layouts_data[j + num_product * 2],
                        layouts_data[j + num_product * 3]);
                    nestedParts->push_back(*nestedPart);
                }
            }
            layout = new Layout(*plate, *nestedParts);
            Layouts->push_back(*layout);
            list<Layout>::iterator layoutItem = Layouts->begin();
            cout << "Layouts:" << std::endl;
            while (layoutItem != Layouts->end()) {
                cout << layoutItem->plate.Material << " ";
                layoutItem++;
            }
            LayoutsToDXF(Layouts, i + 1, sheetDetails_data[3 * num_product + i]);
            Layouts = new list<Layout>();
            nestedParts = new list<NestedPart>();
        }
    }
    catch (const std::exception&)
    {
        cout << "请先调用PerformNestingTask方法完成矩形排样！" << endl;
    }
}

void RectangleNestTask::writeINI(double tightnessCoef, double S_coef, double reNestStepLength, double interval_distance_parts, double interval_distance_plate)
{
    // Creating ini file object
    INI::File ini;
    ini.GetSection("AlgorithmParameters")->SetValue("tightnessCoef", tightnessCoef);
    ini.GetSection("AlgorithmParameters")->SetComment("tightnessCoef", "紧密度系数。建议不低于最小方形件边长");
    ini.GetSection("AlgorithmParameters")->SetValue("S_coef", S_coef);
    ini.GetSection("AlgorithmParameters")->SetComment("S_coef", "面积系数，不建议太大，不建议低于0");
    ini.GetSection("AlgorithmParameters")->SetValue("reNestStepLength", reNestStepLength);
    ini.GetSection("AlgorithmParameters")->SetComment("reNestStepLength", "重套料步长参数，影响最后一张布局的成材率");
    ini.GetSection("NestingParameters")->SetValue("interval_distance_parts", interval_distance_parts);
    ini.GetSection("NestingParameters")->SetComment("interval_distance_parts", "零件间距，量纲为单位长度");
    ini.GetSection("NestingParameters")->SetValue("interval_distance_plate", interval_distance_plate);
    ini.GetSection("NestingParameters")->SetComment("interval_distance_plate", "板间距，量纲为单位长度");
    ini.Save("parametersSetting.ini");
}

void RectangleNestTask::readINI()
{
    try
    {
        // Getting ini parameters from .ini file
        INI::File ini;
        ini.Load("parametersSetting.ini");
        this->tightnessCoef = ini.GetSection("AlgorithmParameters")->GetValue("tightnessCoef").AsDouble();
        this->S_coef = ini.GetSection("AlgorithmParameters")->GetValue("S_coef").AsDouble();
        this->reNestStepLength = ini.GetSection("AlgorithmParameters")->GetValue("reNestStepLength").AsDouble();

        this->interval_distance_parts = ini.GetSection("NestingParameters")->GetValue("interval_distance_parts").AsDouble();
        this->interval_distance_plate = ini.GetSection("NestingParameters")->GetValue("interval_distance_plate").AsDouble();
    }
    catch (const std::exception&)
    {
        cout << "找不到parametersSetting.ini文件" << endl;
    }
}

