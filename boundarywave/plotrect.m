% get intensity from time1
y = load("rect_z.txt");
% x axis range... can be found in the parameters of time1.sh
x = linspace(-0.005,0.025,4096);
% normalize y juse because
y = y./max(y);

% plot
[a,b]=peakdet(y,0.01);
c = x(a(:,1));


plot(x,y)
line([c(1) c(1)],[0 1]);
line([c(2) c(2)],[0 1]);
l = sprintf("%g",c(1));
text(c(1)+0.0001, 0.75, l);
l = sprintf("%g",c(2));
text(c(2)+0.0001, 0.75, l);

axis([-0.005 0.025 0 1], "normal")
xlabel("z axis (microns)")
ylabel("|E|^2 (normalized)")
title("rectangular aperture, t=53.333s")
print("time2.png", "-dpng", "-S1000,500")
