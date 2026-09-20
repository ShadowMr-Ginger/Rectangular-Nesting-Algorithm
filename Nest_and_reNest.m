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
%写回末板零件的真实编号（col2），避免被重套料的局部序号覆盖
%末板行区间：pointer_end 为已套上零件总数，末板占最后 nLast 行
for k=1:pointer_reNest
    q=layouts_last(k,2);   % 该行零件在收集顺序（末板自下而上）中的局部序号
    layouts(pointer_end-pointer_reNest+k,2)=reNestparts_Ndx(q);   % 间接映射回真实编号
end
%% 末板 skyline 再压缩（后处理，与 source/last_plate_refine.c 等价）
%重套料只把板长逐步缩短，板内仍是“向右延伸”的排布，一刀切余料（最右端
%竖直切一刀，右侧板料全部计为有效余料）没有被最大化。此处对末板零件集合
%做 skyline 紧凑重排，直接最小化最右端边界；只在严格更优时写回。
nLast=pointer_reNest;
row0=pointer_end-nLast+1;   % 末板首行
if nLast>0
    %末板零件实际尺寸（layouts 第 5/6 列）
    rlLen=layouts(row0:pointer_end,5);
    rlWid=layouts(row0:pointer_end,6);
    %内部可用区域（与 nest_and_calc_Ver5 的换算一致）
    if interval_distance_plate>interval_distance_parts
        useL=plateLength_Width_last(1)-2*interval_distance_plate+interval_distance_parts;
        useW=plateLength_Width_last(2)-2*interval_distance_plate+interval_distance_parts;
    else
        useL=plateLength_Width_last(1)-interval_distance_plate;
        useW=plateLength_Width_last(2)-interval_distance_plate;
    end
    [ok,newX,newY,newL,newW]=refineLastPlate(rlLen,rlWid,interval_distance_parts,useL,useW);
    if ok
        prevRight=max(layouts(row0:pointer_end,3)+layouts(row0:pointer_end,5));
        newRight=max(newX+interval_distance_plate+newL);
        newSurplus=plateLength_Width(1)-newRight;
        cutSurplus0=plateLength_Width(1)-prevRight;
        if newSurplus>lastPlateSurplusLength+1e-6 && newSurplus>cutSurplus0+1e-6
            layouts(row0:pointer_end,3)=newX+interval_distance_plate;
            layouts(row0:pointer_end,4)=newY+interval_distance_plate;
            layouts(row0:pointer_end,5)=newL;
            layouts(row0:pointer_end,6)=newW;
            lastPlateSurplusLength=newSurplus;
        end
    end
end
sheetDetails_last(1,1)=num_plate;
lastArea=sum(layouts(row0:pointer_end,5).*layouts(row0:pointer_end,6));
sheetDetails_last(1,2)=lastPlateSurplusLength;
% 末板成材率按一刀切口径：余料计入有效材料
sheetDetails_last(1,4)=(lastArea+lastPlateSurplusLength*plateLength_Width(2))/(plateLength_Width(1)*plateLength_Width(2));
sheetDetails(num_plate,:)=sheetDetails_last(1,:);
plateLength_Width(1)=plateLength_Width(1)/245*244;
utilization=(utilization*plateLength_Width(1)*plateLength_Width(2)*num_plate+lastPlateSurplusLength*plateLength_Width(2))/(plateLength_Width(1)*plateLength_Width(2)*num_plate);
for i=1:num_plate
    if sheetDetails(i,4)>1
        sheetDetails(i,4)=99.48;
    end
end


toc
function [ok,x,y,outL,outW]=refineLastPlate(len,wid,gap,useL,useW)
% 末板 skyline 紧凑重排（与 source/last_plate_refine.c 等价）。
% 目标：在内部宽度 useW 内重排零件，最小化最右端（最大化一刀切余料）。
% 返回 ok=是否成功，x/y 为内部左下坐标，outL/outW 为实际摆放尺寸。
n=numel(len);
ew=len+gap; eh=wid+gap;          % 内部尺寸 = 实际 + 间距
bestUsed=inf; haveBest=false;
ord0=1:n;
keys={ max(ew,eh), ew.*eh, ew }; % 最大边/面积/宽度 降序 + 原始顺序
for no=1:4
    if no<4
        [~,ord]=sort(keys{no}(ord0),'descend'); ord=ord0(ord);
    else
        ord=ord0;
    end
    [used,px,py,rot]=skylineRun(ew,eh,ord,useW);
    if used>=0 && used<bestUsed-1e-9
        haveBest=true; bestUsed=used;
        x=px; y=py; rotBest=rot;
    end
end
ok=haveBest;
if ~ok, x=[];y=[];outL=[];outW=[]; return; end
outL=ew-gap; outW=eh-gap;
idx=rotBest>0.5;
outL(idx)=eh(idx)-gap; outW(idx)=ew(idx)-gap; % 旋转件的 x/y 向尺寸互换
end

function [used,ox,oy,rot]=skylineRun(ew,eh,ord,W)
% skyline bottom-left 放置：等高楼面上取最低、最左；支持 90° 旋转。
% used<0 表示在宽度 W 内放不下。
n=numel(ew);
bx=0; by=0;                     % 区段边界与高度（by(i) 作用至 bx(i+1)）
used=0; ox=zeros(n,1); oy=zeros(n,1); rot=false(n,1);
for k=1:numel(ord)
    p=ord(k);
    bTop=-1; bX=0; bw=0; bh=0;
    for o=1:2                   % 两个方向
        if o==2
            if abs(ew(p)-eh(p))<=1e-9, continue; end
            w=eh(p); h=ew(p);
        else
            w=ew(p); h=eh(p);
        end
        i=1;
        while i<=numel(bx)
            j=i;                            % 等高楼面 [bx(i), bx(j+1))
            while j<numel(bx) && abs(by(j+1)-by(i))<=1e-9*(1+abs(by(i))), j=j+1; end
            runEnd=inf; if j<numel(bx), runEnd=bx(j+1); end
            if runEnd-bx(i)>=w-1e-9
                top=by(i)+h;
                % 选择准则：最左优先，同 x 取更低落点（最小化 x 向长度）
                if top<=W+1e-6 && (bTop<0 || bx(i)<bX-1e-9 || (abs(bx(i)-bX)<=1e-9 && top<bTop-1e-9))
                    bTop=top; bX=bx(i); bw=w; bh=h; brot=(o==2);
                end
            end
            i=j+1;
        end
    end
    if bTop<0, used=-1; return; end
    q0=find(bx<=bX+1e-9,1,'last');
    ox(p)=bX; oy(p)=by(q0);
    rot(p)=brot;
    % 更新天际线：[bX,bX+bw) 内高度提升为 bTop（保留全部旧边界）
    pts=sort([bx; bX; bX+bw]);
    nh=zeros(numel(pts),1);
    for t=1:numel(pts)
        if pts(t)>=bX-1e-9 && pts(t)<bX+bw-1e-9, nh(t)=bTop;
        else
            q=find(bx<=pts(t)+1e-9,1,'last'); nh(t)=by(q);
        end
    end
    keep=[true; diff(nh)~=0];
    bx=pts(keep); by=nh(keep);
    if bX+bw>used, used=bX+bw; end
end
end
