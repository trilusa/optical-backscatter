tiledlayout('flow');
title("Raw RX'd signals")
% plot all the waves for visual inspection
for i=1:10%length(distance_signals)
nexttile
t=distance_signals{i}.t_ms/1000;
y=((distance_signals{i}.v));
y=y(100:end); %remove flash of light
t=t(100:end);
y=smooth(y,21); %21 sample boxcar

hold on
plot(y)
xlabel([num2str(i) 'm']);
ylabel("V (adc counts)")
ylim([0,2^10])
hold off
end

%%
figure
semilogy(1:10,BER)
ylim([10e-3 100])
xlim([1 10]);
xlabel("distance (m)")
ylabel(")



