clear all
close all

M = csvread('songDataAdjusted.csv')

audio = M(:,2)

for i = 1:length(audio)
      audio(i,2) = (i - 1)*20;
end

figure()
plot(audio(:,2),audio(:,1));
title('Testovaci udalost: Spusteni hudby z reproduktoru');
xlabel('Cas t(s)');
ylabel({'Zprumerovana hodnota z mereni'; 'na A/D prevodniku z mikrofonu'}, 'Interpreter', 'tex');  % Use tex interpreter for line break

hold on;
xline(220, 'k--', 'LineWidth', 1.5, 'Color', 'r');
xline(700, 'k--', 'LineWidth', 1.5, 'Color', 'g');
legend('Namerene data', 'Spusteni hudby','Vypnuti hudby', 'Location', 'northwest'); 
