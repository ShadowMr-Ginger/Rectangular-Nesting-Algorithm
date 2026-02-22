function [new_group_allocation,new_group_direction]=Copulate(group_allocation,group_direction)
%% 遗传算法 交配函数
[a,b]=size(group_allocation);
randlist=randperm(b);
group_allocation=group_allocation(:,randlist);
group_direction=group_direction(:,randlist);
double_group_allocation=[group_allocation,group_allocation];
double_group_direction=[group_direction,group_direction];
new_group_allocation=group_allocation;
new_group_direction=group_direction;

for i=1:b
    father1=double_group_allocation(:,2*i-1);
    mother1=double_group_allocation(:,2*i);
    father2=double_group_direction(:,2*i-1);
    mother2=double_group_direction(:,2*i);
    kid1=father1;
    kid2=father2;
    for j=1:a
        if rand<0.5
            kid1(j)=mother1(j);
        end
        if rand<0.5
            kid2(j)=mother2(j);
        end
    end
    new_group_allocation(:,i)=kid1;
    new_group_direction(:,i)=kid2;
end
