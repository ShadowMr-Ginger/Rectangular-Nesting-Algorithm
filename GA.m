function [layouts,sheetDetails]=GA(AlgorithmParameters,choose_material,products)
%% 算法Packcing：产品(或栈)打包成 Stripe条带 
% 输入参数：
% AlgorithmParameters  遗传算法的参数[遗传代数，种群大小，变异概率，重生成比例]
% choose_materials  输入原料的信息 [编号，数量，长，宽，优先级]
% products  产品的信息[编号，长，宽]
% 输出参数：
% products_allocation 产品-条带的分配 [分配到条带的编号，摆放方向]
% stripe_details 条带的信息[条带余料长度，条带高度，利用率]
%% 算法部分
LM=choose_material(3);%原料长
WM=choose_material(4);%原料宽

%% 



%% 传入算法参数信息
GA_Generation_Num=AlgorithmParameters(1);
GA_Population_Size=AlgorithmParameters(2);
GA_Mutation_Rate=AlgorithmParameters(3);
GA_Proportion_Random_Individual=AlgorithmParameters(4);
%输入产品信息
DecisionLimit=zeros(length(products(:,1)),1);
%第三列为0-1-2变量。首先判断该矩形件能否放置为横和纵。
%    如果横纵皆可，则为0，只能横则为1，只能纵则为2。
%    如果横纵都放不下，或该产品已经分配了生产，则为-1

%首先将第三列约束值算出来。
for i=1:length(products(:,1))
    if products(i,2)>LM || products(i,3)>LM 
        DecisionLimit(i,1)=-1;
    elseif products(i,2)<=WM && products(i,3)<=WM
        DecisionLimit(i,1)=0;
    elseif products(i,2)>WM
        DecisionLimit(i,1)=1;
    elseif products(i,3)>WM
        DecisionLimit(i,1)=2;
    end
end


%% Step Of Packing 遗传算法进行矩形件打包优化模型求解

Known_Data=struct();%将已知数据存入这个数组中，[LM,WM,products,DecesionLimit]
Known_Data.products=products;
Known_Data.DecisionLimit=DecisionLimit;
Known_Data.LM=LM;
Known_Data.WM=WM;

%% 遗传算法参数设置

num_generation=GA_Generation_Num; % 遗传代数
population_size=GA_Population_Size; % 种群大小
% rate_tilt=0.3; % 交配参数——概率倾斜
% rate_mutation=GA_Mutation_Rate; % 变异参数——变异概率
% rate_mutation_plus=0.5; %变异参数——已经收敛后启用的变异概率
% subrate_mutation=0.1; % 变异参数——变异算子内部变异概率
% renew_population=1; % 是否将迭代结束后的最终种群保留。0，代表否，1代表是。
% runtime_statistic=0; % 是否运行时计时。0，代表否，1代表是。
proportion_random_individual=GA_Proportion_Random_Individual; % 每次新生代种群中随机个体的比例。会向下取整。
%KillMode=1; % 淘汰规则。1为直接淘汰法。目前只有此方法。

%% 遗传算法主体


p_number=length(products(:,1));
group_sequence=zeros(p_number,population_size);
group_direction=zeros(p_number,population_size);
group_adaptation=zeros(population_size,1);
tic
for i=1:population_size
    %STEP_1.生成初始种群 Initial.m
    [group_sequence(:,i),group_direction(:,i)]=Initial(Known_Data);
    %STEP_2.计算初始种群适应度 AdaptationCalc.m
    [A,B,group_adaptation(i)] = nest_and_calc(group_sequence(:,i),group_direction(:,1),products(:,4:5),[LM,WM]);
end
toc
%遗传大循环
for gene=1:num_generation

    
    %STEP_3.种群交配操作 Copulate.m
    [new_group_allocation,new_group_direction]=Copulate(group_sequence,group_direction);
  
    %STEP_4.种群变异操作 Mutate.m
    for i=1:population_size
        if rand<GA_Mutation_Rate
            [new_group_allocation(:,i),new_group_direction(:,i)]=Mutate(Known_Data,new_group_allocation(:,i),new_group_direction(:,i));
        end
    end
    
    %STEP_5.新种群适应化操作 Adapt.m
    for i=1:population_size
        [new_group_allocation(:,i)]=Adapt(Known_Data,new_group_allocation(:,i),new_group_direction(:,i));
        %操作意义在于使新诞生的无效个体转换为有效个体。
    end
    

    %STEP_6.新种群优化计算 Optimize.m
    for i=1:population_size
        [new_group_allocation(:,i),new_group_direction(:,i)]=Optimize(Known_Data,new_group_allocation(:,i),new_group_direction(:,i));
        %操作意义在于使新诞生的有效个体定向优化。
    end
    
    
    %STEP_7.新种群适应度计算
    new_group_adaptation=zeros(population_size,1);
    for i=1:population_size
        new_group_adaptation(i)=AdaptationCalc(Known_Data,new_group_allocation(:,i),new_group_direction(:,i));
    end
    
    %STEP_8.适应度比较
    adaptation_list=zeros(2*population_size,1);%先旧后新
    adaptation_list(1:population_size,1)=group_adaptation;
    adaptation_list(population_size+1:2*population_size,1)=new_group_adaptation;
    [~,sort_sequence]=sort(adaptation_list,"descend");
    
    group_allocation_final=group_sequence;
    group_direction_final=group_direction;
    
    %重生成个体的数量
    random_individual_num=floor(population_size*proportion_random_individual);
    
    for i=1:population_size-random_individual_num
        choose_num=sort_sequence(i);
        if choose_num>population_size
            group_allocation_final(:,i)=new_group_allocation(:,choose_num-population_size);
            group_direction_final(:,i)=new_group_direction(:,choose_num-population_size);
        else
            group_allocation_final(:,i)=group_sequence(:,choose_num);
            group_direction_final(:,i)=group_direction(:,choose_num);
        end
    end
    for i=population_size-random_individual_num+1:population_size
        [group_allocation_final(:,i),group_direction_final(:,i)]=Initial(Known_Data);
    end

    %Step_9.进入下一循环，更新种群
    group_sequence=group_allocation_final;
    group_direction=group_direction_final;
end









