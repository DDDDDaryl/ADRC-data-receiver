T = 1 / 100; %50Hz
t = linspace(0, (length(ref) - 1) * T, length(ref));
figure;
xlabel("Time/s");
ylabel("Voltage/V");
title("Title");
legend;
hold on;
% plot(t,control_signal,'DisplayName','control_signal');hold on;plot(t,ESO1storder,'DisplayName','ESO1storder');plot(t,ref,'DisplayName','ref');plot(t,sensor_data,'DisplayName','sensor_data');plot(t,transientprofile,'DisplayName','transientprofile');hold off;

