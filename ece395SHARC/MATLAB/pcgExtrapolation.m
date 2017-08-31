clear all; close all; clc;

% trying to extrapolate the max speed of the sharc's PCG
% y axis is resultant frequency (function of clock divisor)
% x axis is input value for clock divisor

% some sample y values input to code
clkDivisor = [64 48 32 24 16 12 8 4 2 1];

% resultant frequencies determined with scope
freq = [384e3 512e3 767e3 1.02e6 1.54e6 2.05e6 3.07e6 6.15e6 12.3e6 24.5e6];

stem(clkDivisor,freq);

F = 0:0.01:clkDivisor;

%{
% get the coefficients for a polynomial approximation
coefficients = polyfit(clkDivisor,freq,3);


y = polyval(coefficients,F);

plot(F,y);
%}

hold on;

maxFreq = 24.5e6;

Y = maxFreq./F;

plot(F(100:end),Y(100:end));