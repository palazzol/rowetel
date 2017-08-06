% dcdc_sim_plot.m
% David Rowe 
% Feb 24 2011
%
% Plots ouput from dcdc_sim C simulation, compare to Octave dcdc.m version

load steady.txt
load ring.txt

% these must match same constants C simulation

Fs = 1000;
N = 0.125*Fs;

figure(1)
stem((1/Fs)*(1:N),steady)
grid on
xlabel("Time (s)");
ylabel("Vbatt (V)");
title("Steady State - Fixed Point")
st = floor(0.02*Fs)
en = length(steady)
std(steady(st:en)) % compare this variance to dcdc.m, 
print("dcdc_fixed_steady.png", '-dpng', "-S500,500");

figure(2)
stem((1/Fs)*(1:N),ring)
grid on
xlabel("Time (s)");
ylabel("Vbatt (V)");
title("Ringing - Fixed Point")
print("dcdc_fixed_ringing.png", '-dpng', "-S500,500");

figure(3)
plot((1/Fs)*(st:en),steady(st:en))
grid on
xlabel("Time (s)");
ylabel("Vbatt (V)");
title("Steady State Noise - Fixed Point")
print("dcdc_fixed_noise.png", '-dpng', "-S500,500");
