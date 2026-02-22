function overlapped=isRectangleOverlapped(rectangle1,rectangle2)
%% 判断两个矩形是否相互相交
% rectangle1\rectangle2 格式：[默认旋转角0°的情况下左下角顶点x，左下角顶点y，x方向长度，y方向长度]
% rec1\rec2格式：[左上角顶点x，左上角顶点y；右下角顶点x，右下角顶点y]

rec1=[rectangle1(1),rectangle1(2)+rectangle1(4);rectangle1(1)+rectangle1(3),rectangle1(2)];
rec2=[rectangle2(1),rectangle2(2)+rectangle2(4);rectangle2(1)+rectangle2(3),rectangle2(2)];
if rec1(1,1)+0.0001>rec2(2,1) || rec1(2,1)-0.0001<rec2(1,1) || rec1(2,2)+0.0001>rec2(1,2) || rec1(1,2)-0.0001<rec2(2,2)
    overlapped=0;
else
    overlapped=1;
end
end


