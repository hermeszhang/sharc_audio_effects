clear all; close all; clc;

Fs = 48000;
% samples taken in
N = 256;
% digital time
n = 1:N;

% frequencies in Hz
f1 = 21600;
f2 = 500;

% digital signals
x1 = sin((2*pi/Fs)*f1*n);
x2 = sin((2*pi/Fs)*f2*n);

% digital signal
x = x1 + x2;

%{
    set up the filter
    get zeros, poles and gain
    then convert to 2nd order sections
    
    # of rows in sos matrix 
    is # of 2nd order sections
    it's = max of #p/2 and #z/2
%}

ORDER = 3;
fc = 5000;
cutoff = fc/Fs;

[z,p,k] = butter(ORDER,cutoff);
[b,a] = butter(ORDER,cutoff);
% including 'down' and 'two' causes b0 != 1
% so iir loop below will have a third output_acc line
[sos,g] = zp2sos(z,p,k);

%{
    see note about embedding gain on mathworks page
    about zp2sos.  they say use ss2sos with two
    outputs instead.
%}

freqz(b,a);
hold on;
refline([0 -3]);

% --------- TEST MATLAB GENERATED FILTER -------- %
%{
% plot filter for reference
% w/ ORDER = 2, response is kinda weird but 
% actual output is best so go figure...
%fvtool(sos);

figure;
plot(n,x);


xFiltered = filter(b,a,x);
hold on;
plot(n, xFiltered,'LineWidth',2);

%}


% ----- CREATE COEFFS MATRIX FROM SOS MATRIX ------ %

% matlab puts b's before a's in the rows
% but want it other way around
middle = ceil(size(sos,2)/2);
coeffs = sos;
% swap left for right blocks of matrix
coeffs(:,1:middle) = sos(:,(middle+1):end);
coeffs(:,(middle+1):end) = sos(:,1:middle);
% get rid of 1st col of 1's, a0
coeffs = coeffs(:,2:end);

% get rid of col of 1's for b0 if b0 is 1
% don't get rid of b0 if != 1 due to parameters in zp2sos
%coeffs = [coeffs(:,1:(middle-1)) coeffs(:,(middle+1):end)];

stages = size(sos,1);
history = zeros(1,2*stages);

% ------------ C-STYLE FILTER TEST ---------------- %

input_acc = 0;
output_acc = 0;
stages = size(sos,1);
history = zeros(1,2*stages);
output = zeros(1,length(x));

for i = 1:length(x)
    
    input = x(i);
    
    % gain factor before entering the stages
    % g from zp2sos isn't quite right
    % see note about embedding gain on zp2sos page
    input_acc = input*g;
    
    % for stepping through history buffer
    index = 1;

    for j = 1:stages
        
        % poles 
        input_acc = input_acc - coeffs(j,1)*history(index);
        input_acc = input_acc - coeffs(j,2)*history(index+1);
        
        %zeros
        % if don't pass params to zp2sos, then b0's = 1 and so
        % don't need the first multiply here which is for b0
        % so would change coeffs(j,4) to coeffs(j,3) and
        % coeffs(j,5) to coeffs(j,4) and get rid of first multiply
        % but multiplying by 1 is fine so just leaving it for now
        output_acc = input_acc*coeffs(j,3);
        output_acc = output_acc + coeffs(j,4)*history(index);
        output_acc = output_acc + coeffs(j,5)*history(index+1);
        
        % shift the history buffer
        history(index+1) = history(index);
        history(index) = input_acc;
        
        index = index + 2;
        
    end
    
    output(i) = output_acc;
    
end


figure;
plot(n,x);
hold on;
plot(n,output,'LineWidth',2);


% -------- END C-STYLE FILTER TEST ---------------- %



% reformat coeffs matris into one c-style array
coeff_array = reshape(coeffs.',1,[]);


theFileName = 'C:\wudtke_sconza_nunes\ece395SHARC\coeffsIIR.h';
theFile = fopen(theFileName,'wt');

%fprintf(theFile,'#define N %d\n\n',N);
%fprintf(theFile,'#define ORDER %d\n',ORDER);
%{
    num coeffs is either stages * 4 or stages * 5
    it's stages * 4 in the usual case because each stage
    has two delay elements and each delay element
    has 1 feedback + 1 feedforward coefficient.
    but it's stages * 5 if 
    b0 is not equal to 1, which happens if 
    extra params are passed to zp2sos call above
%}
fprintf(theFile,'double coeffsIIR[%d] = {\n\n',stages*5);
fprintf(theFile,'\t%6.6f,\n',coeff_array(1:end-1));
fprintf(theFile,'\t%6.6f',coeff_array(end));
fprintf(theFile,'\n\n};\n\n');

fprintf(theFile,'//cutoff = %d\n', cutoff*Fs);
fprintf(theFile,'double gain = %6.20f;\n', g);
fprintf(theFile,'int stages = %d;\n',stages);

fprintf(theFile,'double history[%d] = {0.0};\n\n', length(history));

%{
fprintf(theFile,'double x[%d] = {\n\n',N);
fprintf(theFile,'\t%6.6f,\n',x(1:end-1));
fprintf(theFile,'\t%6.6f',x(end));
fprintf(theFile,'\n\n};');
%}



