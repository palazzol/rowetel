% test_dcdc_model.m
% David Rowe 22 Feb 2011
%
% Discrete time DC-DC converter model
% tau should be roughly RC

% calculated from impulse response measurements

k = -0.021;    
tau = 0.028;

% Design filter constant c based on sample rate and tau.  Note
% tau of discrete time filter should not change with changing Fs,
% when measured in ms.

Fs = 1000;
f = 1/(2*pi*tau)
w = 2*pi*f/Fs
c = w

N=Fs*4*tau;

% Impulse response ---------------------------------------

% Measured impulse response with a 3300 Rload, 10uF capacitor
% was a time constant (tau) around 28ms.  Check plot too see
% if time constant (time to decay to 37% original) is 28ms
% Vbatt should peak at -5.6V

y=zeros(N,1);
x=zeros(N,1);
x(1)=255;

y(1) = k * x(1);
for n=2:N
  y(n) = y(n-1) + k * x(n) - c * y(n-1);
end

figure(1)
stem((1/Fs)*(1:N),y)
grid on
xlabel("Time (s)");
ylabel("Vbatt (V)");
title("Impulse Response")

% Steady State (step response) -----------------------------

% Following table was measured:
%
% Measured Vbatt  PWM (OCR1A)
%
%          -78    191
%          -66    127
%          -34    63
%          -17    31
 
pwm = 63;

y=zeros(N,1);
x=zeros(N,1);
x=pwm*ones(N,1);

y(1) = k * x(1);
for n=2:N
  y(n) = y(n-1) + k * x(n) - c * y(n-1);
end
print("dcdc_model_impulse.png", '-dpng', "-S500,500");

figure(2)
stem((1/Fs)*(1:N),y)
grid on
xlabel("Time (s)");
ylabel("Vbatt (V)");
title("Steady State")
print("dcdc_model_steady.png", '-dpng', "-S500,500");
