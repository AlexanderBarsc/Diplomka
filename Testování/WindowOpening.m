clear all
close all

M = csvread('WindowOpeningEdited.csv');

data = M(:,[2 3]);

data(:,1) = data(:,1) - 3

for i = 1:length(data)
      data(i,3) = (i - 1)*20;
end

data = data(1:30,:);

figure();
sgtitle('Testovaci udalost: Otevreni okna'); 
subplot(2,1,1);
plot(data(:,3),data(:,1));
title('Hodnoty z teplomeru');
xlabel('Cas t(s)');
ylabel('Teplota (°C)');  

hold on;
xline(20, 'k--', 'LineWidth', 1.5, 'Color', 'r');
legend('Namerene data', 'Otevreni okna', 'Location', 'northwest'); 

subplot(2,1,2);
plot(data(:,3),data(:,2))
title('Hodnoty z vlkhomeru');
xlabel('Cas t(s)');
ylabel('Relativni vlkhost (%)');

xline(20, 'k--', 'LineWidth', 1.5, 'Color', 'r');
legend('Namerene data', 'Otevreni okna', 'Location', 'northwest');