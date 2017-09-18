% go to this link to learn more
% https://www.dsprelated.com/freebooks/pasp/Lagrange_Interpolation.html

% order
N = 2
% desired fractional sample delay amount
delay = .5

n = 0:N;
h = ones(1,N+1);
for k = 0:N
    index = find(n ~= k);
    h(index) = h(index) *  (delay-k)./ (n(index)-k);
end

% h will be the coefficients
h