


load data.mat

n1 = length(d1);
n2 = length(d2);

freq1 = fs1*(0:(n1-1))/(n1);
freq2 = fs2*(0:(n2-1))/(n2);



plot(freq1,abs(fft(d1)),'b.-')
hold on
plot(freq2,abs(fft(d2)),'r.-')
%


xlim([1000,1400])
legend('APRSDroid','Arduino')