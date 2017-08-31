%{
clc; clear all; close all;

FILTER_LENGTH = 32;

coeffs = (1/FILTER_LENGTH)*ones(FILTER_LENGTH,1);

%}


clc, clear all, close all

FILTER_LENGTH = 64;
Fs = 48000;
fc = 400;
cutoff = fc/Fs;

coeffs = fir1(FILTER_LENGTH, cutoff, 'low');
figure(1);
freqz(coeffs);
title('test');

theFileName = 'C:\wudtke_sconza_nunes\ece395SHARC\coeffs.h';
theFile = fopen(theFileName,'wt');




fprintf(theFile,'#define FILTER_LENGTH %d\n\n',FILTER_LENGTH);
fprintf(theFile,'// %f\n\n', cutoff);
fprintf(theFile,'double coeffs[%d] = {\n\n',FILTER_LENGTH);
fprintf(theFile,'\t%6.6f,\n',coeffs(1:end-2));
fprintf(theFile,'\t%6.6f',coeffs(end-1));
fprintf(theFile,'\n\n};\n\n');




