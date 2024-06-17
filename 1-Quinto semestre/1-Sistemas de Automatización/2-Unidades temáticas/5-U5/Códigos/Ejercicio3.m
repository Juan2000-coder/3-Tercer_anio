1;
k = 4;
tol = 1e-6;
maxiter = 10000;
den = conv([2 1],[5 1]);
den = conv(den, [0.5 1]);
Go = tf(3, den);

format;

W = [0.1, 0.5, 1, 2];
P = (2*pi)./W;

N = 200000;
p = 10;
T = linspace(0, p*P(k), N);
U = sin(W(k)*T);

Y = lsim(Go, U, T);
M = max(Y(round((p-1)*N/p):end));



hf = figure(1);hold on; grid on;
ti = title(["RESPUESTA PARA:  w = 0.1 rad/s; T = ", num2str(P(k))]);

set(ti, "fontname","Times New Roman","fontsize",15);
set(gca,"fontsize",12,"linewidth",0.5);
xlabel("Tiempo(s)","fontsize",12,"fontname","Times New Roman","fontweight","bold");
ylabel("y(t)","fontsize",12,"fontname","Times New Roman","fontweight","bold");


plot(T, M*ones(1,length(T)),'--k');
plot(T, U, '-b');
plot(T, Y, '-r', "linewidth", 1.3);

#3ticksX = xticks();
##ticksX = ([P(k)*(1:1:p), ticksX]);
##xticks(ticksX);
##xlabels = xticklabels();
xlim([0,p*P(k)]);

yticks([-3:0.5:3, M]);


##for i = 1:p
	#3xlabels{1,i} = strcat(num2str(i), 'T');
#3endfor
##xticklabels(xlabels);

legend("Yss","u(t)","y(t)");
