clear all
close all

M = csvread('roomEntryData.csv')

data = M(:,end-1:end);

for i = 1:length(data)
      data(i,3) = (i - 1)*20;
end

figure();
sgtitle('Testovaci udalost: Prichod do mistnosti a rozsviceni svetla'); 
subplot(2,1,1);
plot(data(:,3),data(:,1));
title('Hodnoty z fototranzistoru');
xlabel('Cas t(s)');
ylabel({'Zprumerovana hodnota z mereni'; 'na A/D prevodniku na fototranzistoru'}, 'Interpreter', 'tex');  % Use tex interpreter for line break


hold on;
xline(220, 'k--', 'LineWidth', 1.5, 'Color', 'r');
legend('Namerene data', 'Detekce rozsviceni svetla', 'Location', 'northwest'); 


subplot(2,1,2);
plot(data(:,3),data(:,2))
title('Hodnoty z PIR cidla');
xlabel('Cas t(s)');
ylabel('Detekovan pohyb');

xline(180, 'k--', 'LineWidth', 1.5, 'Color', 'r');
legend('Namerene data', 'Detekce pohybu', 'Location', 'northwest'); 

