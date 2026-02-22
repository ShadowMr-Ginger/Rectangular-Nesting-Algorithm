function [allocation,direction]=Mutate(Known_Data,allocation,direction)
%% 变异算子
DecisionLimit=Known_Data.DecisionLimit;
[a]=size(allocation,1);

group_number=10;
group_size=floor(a/group_number);
group_index=zeros(1,group_number);
group_index(1,group_number)=a;
for i=1:group_number-1
    group_index(1,i)=group_size*i;
end
sequence1=zeros(a,1);
for i=1:group_number
    if i==1
        randlist=randperm(group_index(1,1));
        randlist=randlist';
        sequence1(1:group_index(1,i),1)=randlist;
    else
        randlist=randperm(group_index(1,i)-group_index(1,i-1))+group_index(1,i-1);
        randlist=randlist';
        sequence1(group_index(1,i-1)+1:group_index(1,i),1)=randlist;
    end
end


for i=1:a-1
    if allocation(sequence1(i,1),1)~=0 && allocation(sequence1(i+1,1),1)~=0
        if rand<0.1
            t=allocation(sequence1(i,1),1);
            allocation(sequence1(i,1),1)=allocation(sequence1(i+1,1),1);
            allocation(sequence1(i+1,1),1)=t;
        end
    end
end

rate=a/100;
for i=1:a
    if DecisionLimit(i)==0
        if rand<rate
            direction(i)=1-direction(i);
        end
    end
end




        