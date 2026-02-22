function [layouts,sheetDetails,num_plate] = nest_and_calc(partsSequence,partsDirection,partsSize,plateLength_Width)
%% 排料与成材率计算函数
% 用于指定板材顺序的情况下进行排料并计算成材率
% 输入参数：
%    partsSequence：板件的排样顺序
%    partsDirection：板件纵横
%    partsSize：板件尺寸(长，宽)
%    plateLength_Width：原料板的长宽
% 输出参数：
%    layouts：板件的布局（材料类型，sheet编号，左下顶点坐标x，y，x方向长度，y方向长度）
%    sheetDetails：每张原料的细节（余料，成材率）
%    utilization：整体成材率
%% 具体实现：
% 初始化数据
plateLength=plateLength_Width(1);
plateWidth=plateLength_Width(2);
% 初始化输出结果
num_parts=size(partsSequence,1);
layouts=zeros(num_parts,6);
sheetDetails=zeros(num_parts,4);
% 初始化中间变量
num_plate=0;
partIsNested=zeros(num_parts,1);
%partNest_pointer=1;
base_point_list=zeros(num_parts*2,6);%%x,y,是否可以放置，紧密度，对应的current_part，左上角(0)或右下角(1)
%base_point_pointer=0;

%中间变量，选择的
while ~all(partIsNested)
    num_plate=num_plate+1;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
    currentNestedParts=zeros(num_parts,6);%x,y,xLength,yLength,左上角是否被堆叠，右下角是否杯堆叠
    currentNestedParts_pointer=0;
    S_efficient=0;%用于计算成材率
    for i=1:num_parts
        part_number=partsSequence(i);
        if partIsNested(part_number)==1
            continue
        end
        base_point_list=zeros(num_parts*2,6);
        base_point_pointer=0;
        %base_point_list(:,3)=base_point_list(:,3)*0;
        %base_point_list(:,4)=base_point_list(:,4)*0;
        %记录长宽
        if partsDirection(part_number)==0
            partLength=partsSize(part_number,1);
            partWidth=partsSize(part_number,2);
        else
            partLength=partsSize(part_number,2);
            partWidth=partsSize(part_number,1);
        end
        %左底原则，第一顶点为方形件左下角
        if currentNestedParts_pointer==0
            base_point_pointer=base_point_pointer+1;
        end
        %其他板件边角
        if currentNestedParts_pointer~=0
            for j=1:currentNestedParts_pointer
                if currentNestedParts(j,5)==0
                    base_point_pointer=base_point_pointer+1;
                    base_point_list(base_point_pointer,1)=currentNestedParts(j,1);
                    base_point_list(base_point_pointer,2)=currentNestedParts(j,2)+currentNestedParts(j,4);
                    base_point_list(base_point_pointer,5)=j;
                    base_point_list(base_point_pointer,6)=0;
                end
                if currentNestedParts(j,6)==0
                    base_point_pointer=base_point_pointer+1;
                    base_point_list(base_point_pointer,1)=currentNestedParts(j,1)+currentNestedParts(j,3);
                    base_point_list(base_point_pointer,2)=currentNestedParts(j,2);
                    base_point_list(base_point_pointer,5)=j;
                    base_point_list(base_point_pointer,6)=1;
                end
            end
            %判断所有的边点是否可以放置矩形
            for j=1:base_point_pointer
                base_point_list(j,3)=1;
                if base_point_list(j,1)+partLength>plateLength || base_point_list(j,2)+partWidth>plateWidth
                    base_point_list(j,3)=0;
                    continue
                end

                if currentNestedParts_pointer==0
                    base_point_list(j,3)=0;
                    break
                end
                rectangle_current=[base_point_list(j,1),base_point_list(j,2),partLength,partWidth];
                for k=1:currentNestedParts_pointer
                    %检查矩形是否相互交叠
                    rectangle_detect=currentNestedParts(k,1:4);
                    overlapped=isRectangleOverlapped(rectangle_current,rectangle_detect);
                    if overlapped==1
                        base_point_list(j,3)=0;
                        break
                    end
                end
                if base_point_list(j,3)==1
                    % 计算紧密度
                    base_point_list(j,4)=tightnessCalc(rectangle_current,currentNestedParts,currentNestedParts_pointer);
                else
                    base_point_list(j,4)=0;
                end
            end
            [bestTightness,bestNdx]=max(base_point_list(1:base_point_pointer,4));
            if bestTightness==0
                continue
            end
            currentNestedParts_pointer=currentNestedParts_pointer+1;
            currentNestedParts(currentNestedParts_pointer,1)=base_point_list(bestNdx,1);
            currentNestedParts(currentNestedParts_pointer,2)=base_point_list(bestNdx,2);
            currentNestedParts(currentNestedParts_pointer,3)=partLength;
            currentNestedParts(currentNestedParts_pointer,4)=partWidth;
            currentNestedParts(base_point_list(bestNdx,5),5+base_point_list(bestNdx,6))=1;
            partIsNested(part_number)=1;
            layouts(part_number,1)=0;%材料类型
            layouts(part_number,2)=num_plate;%sheet编号
            layouts(part_number,3)=currentNestedParts(currentNestedParts_pointer,1);%产品左下角x
            layouts(part_number,4)=currentNestedParts(currentNestedParts_pointer,2);%产品左下角y
            layouts(part_number,5)=currentNestedParts(currentNestedParts_pointer,3);%产品x长度
            layouts(part_number,6)=currentNestedParts(currentNestedParts_pointer,4);%产品y长度
            S_efficient=S_efficient+partLength*partWidth;
            %partNest_pointer=partNest_pointer+1;
        else
            currentNestedParts_pointer=currentNestedParts_pointer+1;
            currentNestedParts(currentNestedParts_pointer,3)=partLength;
            currentNestedParts(currentNestedParts_pointer,4)=partWidth;
            partIsNested(part_number)=1;
            layouts(part_number,1)=0;%材料类型
            layouts(part_number,2)=num_plate;%sheet编号
            layouts(part_number,3)=currentNestedParts(currentNestedParts_pointer,1);%产品左下角x
            layouts(part_number,4)=currentNestedParts(currentNestedParts_pointer,2);%产品左下角y
            layouts(part_number,5)=currentNestedParts(currentNestedParts_pointer,3);%产品x长度
            layouts(part_number,6)=currentNestedParts(currentNestedParts_pointer,4);%产品y长度
            %partNest_pointer=partNest_pointer+1;
            S_efficient=S_efficient+partLength*partWidth;
        end
    end
    surplusLength=plateLength;
    surplusWidth=plateWidth;
    for i=1:currentNestedParts_pointer
        if plateLength-currentNestedParts(i,1)-currentNestedParts(i,3)<surplusLength
            surplusLength=plateLength-currentNestedParts(i,1)-currentNestedParts(i,3);
        end
        if plateWidth-currentNestedParts(i,2)-currentNestedParts(i,4)<surplusWidth
            surplusWidth=plateWidth-currentNestedParts(i,2)-currentNestedParts(i,4);
        end
    end
    plateUtilization=S_efficient/plateWidth/plateLength;
    sheetDetails(num_plate,1)=num_plate;
    sheetDetails(num_plate,2)=surplusLength;
    sheetDetails(num_plate,3)=surplusWidth;
    sheetDetails(num_plate,4)=plateUtilization;
end





