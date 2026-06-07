clear;
clc;

% =========================
% SERIAL PORT
% =========================

arduino = serialport("COM3",9600);

% Waiting time
configureTerminator(arduino,"LF");

pause(2);

% =========================
% CSV CREATION
% =========================

fileID = fopen('scan.csv','w');

disp("Acquisition in progress...");

% =========================
% ACQUISITION
% =========================

while true

    % Reading Arduino line
    line = readline(arduino);

    % Matlab display
    disp(line);

    % CSV save
    fprintf(fileID,'%s\n',line);

end