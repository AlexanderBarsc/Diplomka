% Posle email pokud hodnota z kouroveho senzoru je vyssi nez 0
% Id kanalu
channelID = 2501251;

% Api klic
alertApiKey = 'TAKAdLdtiBzjI9PZzt5';

alertUrl="https://api.thingspeak.com/alerts/send";

options = weboptions("HeaderFields", ["ThingSpeak-Alerts-API-Key", alertApiKey ]);

% Predmet emailu
alertSubject = sprintf("Smoke sensor triggred");

% Precteni poslednich dat
data = thingSpeakRead(channelID);

smokeSensorValue = data(4);
alertBody = sprintf("Value from smoke sensor: %f\n", smokeSensorValue);
fprintf(alertBody);

if(smokeSensorValue > 0)
    try
        webwrite(alertUrl , "body", alertBody, "subject", alertSubject, options);
    catch someException
        fprintf("Failed to send alert: %s\n", someException.message);
    end
end



