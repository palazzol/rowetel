% dc-dc_sim.m
% David Rowe 
% Feb 21 2011
%
% DC-DC converter simulation for $10 ATA project.

% Stuff to test
%   [ ] different Rload
%   [ ] sinusoidal 20Hz ringing

% constant mapping PWM value to Vbatt voltage output,
% calculated from measured impulse response

k = -2.7/127;

% Measured time constant, roughly RC (33ms)

tau = 0.028;

% Design filter constant c based on sample rate and tau.  Note
% tau of discrete time filter should not change with changing Fs,
% when measured in ms.

Fs = 4000;
N  = Fs*0.125;
f  = 1/(2*pi*tau);
w  = 2*pi*f/Fs;
c  = w;

% Control loop gain

gain  = 8;

% Maps voltage to pwm

kv    = -5.58;

% Closed loop response to a step (steady state) input

Vset  = -48;
Vbatt = zeros(N,1);
Vdiff = Vset;
pwm   = Vdiff*kv;

for n=2:N
  Vdiff = Vset - Vbatt(n-1);
  pwm = Vdiff*kv*gain;
  if (pwm < 0)
     pwm = 0;
  endif
  if (pwm > 255)
     pwm = 255;
  endif
  Vbatt(n) = Vbatt(n-1) + k * pwm - c * Vbatt(n-1);
end

figure(1)
stem((1/Fs)*(1:N),Vbatt)
grid on
xlabel("Time (s)");
ylabel("Vbatt (V)");
title("Steady State")
st = floor(0.02*Fs)
en = length(Vbatt)
std(Vbatt(st:en))
print("dcdc_sim_steady.png", '-dpng', "-S500,500");

% Closed loop response to -48V DC + 50Vp 20Hz sinusoidal input

kv    = -5.58;
Vset  = -48 + 50*sin(2*pi*(20/Fs)*(1:N));
Vbatt = zeros(N,1);
Vdiff = Vset(1);
pwm   = Vdiff*kv;

for n=2:N
  Vdiff = Vset(n) - Vbatt(n-1);
  pwm = Vdiff*kv*gain;
  if (pwm < 0)
     pwm = 0;
  endif
  if (pwm > 255)
     pwm = 255;
  endif
  Vbatt(n) = Vbatt(n-1) + k * pwm - c * Vbatt(n-1);
end

figure(2)
stem((1/Fs)*(1:N),Vbatt)
grid on
xlabel("Time (s)");
ylabel("Vbatt (V)");
title("Ringing")
print("dcdc_sim_ring.png", '-dpng', "-S500,500");
