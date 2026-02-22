function draw_rectangle(location,L,W,color,pencolor)
x=location(1);
y=location(2);
points=[x,y;x+L,y;x+L,y+W;x,y+W;x,y];
hold on
fill(points(:,1),points(:,2),color)
plot(points(:,1),points(:,2),"LineWidth",1.5,"Color",pencolor)
hold off