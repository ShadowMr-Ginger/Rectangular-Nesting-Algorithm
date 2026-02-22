function tightness=tightnessCalc(rectangle_current,currentNestedParts,currentNestedParts_pointer)
%% 紧密度计算函数
tightness=0;
rectangle_target=[rectangle_current(1),rectangle_current(2)+rectangle_current(4);rectangle_current(1)+rectangle_current(3),rectangle_current(2)];
for i=1:currentNestedParts_pointer
    relativePosition=[0,0,0,0];%[上，下，左，右]
    %记录左上和右下两个点
    rectangle_nested=[currentNestedParts(i,1),currentNestedParts(i,2)+currentNestedParts(i,4);currentNestedParts(i,1)+currentNestedParts(i,3),currentNestedParts(i,2)];
    if rectangle_nested(2,2)+0.0001>rectangle_target(1,2)
        relativePosition(1)=1;
    end
    if rectangle_nested(1,2)-0.0001<rectangle_target(2,2)
        relativePosition(2)=1;
    end
    if rectangle_nested(2,1)-0.0001<rectangle_target(1,1)
        relativePosition(3)=1;
    end
    if rectangle_nested(1,1)+0.0001>rectangle_target(2,1)
        relativePosition(4)=1;
    end
    if sum(relativePosition)>=2
        continue
    end
    if sum(relativePosition)==1
        rectangle_target1=rectangle_target;
        if relativePosition(1)==1
            rectangle_target1(1,2)=rectangle_target1(1,2)+10;
        elseif relativePosition(2)==1
            rectangle_target1(2,2)=rectangle_target1(2,2)-10;
        elseif relativePosition(3)==1
            rectangle_target1(1,1)=rectangle_target1(1,1)-10;
        else
            rectangle_target1(2,1)=rectangle_target1(2,1)+10;
        end
        overlapArea=rectangleOverlapAreaCalc(rectangle_target1,rectangle_nested);
        tightness=tightness+overlapArea;
    end
end

