clear
clc

data_products=readmatrix("dataA1.csv");
products_list={};
for i=1:length(data_products(:,1))
    products_list{i,1}=strcat(['H',num2str(i)]);
    products_list{i,2}=1;
    products_list{i,3}=strcat(['PL',num2str(10),'*',num2str(data_products(i,5))]);
    products_list{i,4}=data_products(i,4);
    products_list{i,5}=7850*data_products(i,4)*data_products(i,5)*10/1000000000;
    products_list{i,6}='Q345B';
end
writecell(products_list,"out.xls")
    
