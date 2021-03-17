time_interval = 2;
freq = 1 / time_interval;
angular_displacement = 10*pi;
angular_velocity = 2*pi*freq;
amp1 = 1;
amp2 = 0.1;

offset = 1;

t = linspace(0, angular_displacement, angular_displacement / angular_velocity + 1);
x1 = amp1 / 2 * square(t) + amp1 / 2 + offset;
x2 = amp2 / 2 * square(t) + amp2 / 2 + offset;
x = cat(2, x1, x2);

% ������
title={'ref'};

%���ɱ�񣬰�������
% VariableNames ��������������ͷ
result_table = table(x', 'VariableNames', title);

% ��������
writetable(result_table, 'squarewave.csv');

% csvwrite('squarewave.csv', x');