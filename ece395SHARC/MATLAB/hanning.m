clc; clear all; close all;

N = 256;

win = hann(N/2);
winShift = fftshift(win);

plot(win);
hold on;
plot(winShift);

theFileName = 'C:\wudtke_sconza_nunes\ece395SHARC\window.h';
theFile = fopen(theFileName,'wt');


fprintf(theFile, 'double hanning[%d] = {\n\n',N/2);
fprintf(theFile, '\t%6.6f,\n',win(1:end-1));
fprintf(theFile, '\t%6.6f',win(end));
fprintf(theFile,'\n\n};\n\n');

fprintf(theFile, 'double hanningShift[%d] = {\n\n',N/2);
fprintf(theFile, '\t%6.6f,\n',winShift(1:end-1));
fprintf(theFile, '\t%6.6f',winShift(end));
fprintf(theFile,'\n\n};\n\n');