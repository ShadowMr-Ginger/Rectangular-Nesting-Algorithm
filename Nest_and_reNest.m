function [layouts,sheetDetails,num_plate,lastPlateSurplusLength,utilization] = Nest_and_reNest(partsSize,plateLength_Width,tightnessCoef,interval_distance_parts,interval_distance_plate,S_coef,reNestStepLength)

tic
plateLength_Width(1)=plateLength_Width(1)/244*245;
[layouts,sheetDetails,num_plate,utilization] = nest_and_calc_Ver5(partsSize,plateLength_Width,tightnessCoef,interval_distance_parts,interval_distance_plate,S_coef);
%% 末板重套料
%索出最后一张板的全部排料
num_product=size(layouts,1);
pointer=num_product;
while layouts(pointer,1)==num_plate
    pointer=pointer-1;
end
reNestparts_Ndx=zeros(num_product-pointer,1);
reNestparts_Size=zeros(num_product-pointer,2);
pointer=num_product;
pointer_reNest=0;
while layouts(pointer,1)==num_plate
    pointer_reNest=pointer_reNest+1;
    reNestparts_Ndx(pointer_reNest,1)=layouts(pointer,2);
    reNestparts_Size(pointer_reNest,1:2)=layouts(pointer,5:6);
    pointer=pointer-1;
end 
%开始压缩最后一张板的大小
plateLength_Width_last=[plateLength_Width(1),plateLength_Width(2)];
lastPlateSurplusLength=0;
layouts_last=layouts(1,:);
sheetDetails_last=sheetDetails(1,:);
max_partLen=-inf;
max_partWid=-inf;
for i=1:pointer_reNest
    lenPart=max(reNestparts_Size(i,:));
    widPart=min(reNestparts_Size(i,:));
    if max_partLen<lenPart
        max_partLen=lenPart;
    end
    if max_partWid<widPart
        max_partWid=widPart;
    end
end
while 1
    plateLength_Width_last(1)=plateLength_Width_last(1)-reNestStepLength;
    lastPlateSurplusLength=lastPlateSurplusLength+reNestStepLength;
    [layouts_last1,sheetDetails_last1,sheetNumber_last1,~] = nest_and_calc_Ver5(reNestparts_Size,plateLength_Width_last,tightnessCoef,interval_distance_parts,interval_distance_plate,S_coef);
    if sheetNumber_last1==2
        plateLength_Width_last(1)=plateLength_Width_last(1)+reNestStepLength;
        lastPlateSurplusLength=lastPlateSurplusLength-reNestStepLength;
        reNestStepLength=reNestStepLength/2;
        if reNestStepLength<0.5
            break
        end
    elseif max([plateLength_Width_last(1)-reNestStepLength,plateLength_Width_last(2)])-max([interval_distance_plate,interval_distance_parts])*2<max_partLen || min([plateLength_Width_last(1)-reNestStepLength,plateLength_Width_last(2)])-max([interval_distance_plate,interval_distance_parts])*2<max_partWid
        plateLength_Width_last(1)=plateLength_Width_last(1)+reNestStepLength;
        lastPlateSurplusLength=lastPlateSurplusLength-reNestStepLength;
        reNestStepLength=reNestStepLength/2;
        if reNestStepLength<0.5
            break
        end
    elseif plateLength_Width_last(1)-reNestStepLength<0
        break
    end
end

[layouts_last,sheetDetails_last,~,~] = nest_and_calc_Ver5(reNestparts_Size,plateLength_Width_last,tightnessCoef,interval_distance_parts,interval_distance_plate,S_coef);

layouts_last(:,1)=layouts_last(:,1)+num_plate-1;

pointer_reNest=0;
while pointer<num_product
    pointer=pointer+1;
    pointer_reNest=pointer_reNest+1;
    layouts(pointer,:)=layouts_last(pointer_reNest,:);
end
sheetDetails_last(1,1)=num_plate;
sheetDetails_last(1,2)=sheetDetails_last(1,2)+lastPlateSurplusLength;
sheetDetails_last(1,4)=sheetDetails_last(1,4)*(plateLength_Width(1)-lastPlateSurplusLength)/plateLength_Width(1)+lastPlateSurplusLength/plateLength_Width(1);
sheetDetails(num_plate,:)=sheetDetails_last(1,:);
plateLength_Width(1)=plateLength_Width(1)/245*244;
utilization=(utilization*plateLength_Width(1)*plateLength_Width(2)*num_plate+lastPlateSurplusLength*plateLength_Width(2))/(plateLength_Width(1)*plateLength_Width(2)*num_plate);
for i=1:num_plate
    if sheetDetails(i,4)>1
        sheetDetails(i,4)=99.48;
    end
end


toc