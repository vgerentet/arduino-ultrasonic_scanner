clear;
clc;
close all;

% =========================================================
% LOAD CSV DATA
% =========================================================

% Read CSV file
data = readtable('scan.csv');

% Extract angle (degrees) and distance (cm)
angleDeg = data.angle;
distance = data.distance;

% =========================================================
% FILTER DISTANCES > 750 cm
% =========================================================

valid = distance <= 500;

angleDeg = angleDeg(valid);
distance = distance(valid);

% =========================================================
% CONVERT ANGLES TO RADIANS
% =========================================================

% Polar functions in MATLAB use radians
angleRad = deg2rad(angleDeg);

% =========================================================
% POLAR PLOT
% =========================================================

figure;

polarplot(angleRad, distance, '.-');

title('Ultrasonic Scanner - Polar Projection');

% =========================================================
% CONVERT POLAR COORDINATES TO CARTESIAN
% =========================================================

% x = r*cos(theta)
% y = r*sin(theta)

x = distance .* cos(angleRad);
y = distance .* sin(angleRad);

% =========================================================
% CARTESIAN RECONSTRUCTION
% =========================================================

figure;

plot(x, y, '.-');

% Keep equal scaling on both axes
axis equal;

% Add grid
grid on;

% Labels
xlabel('X Position (cm)');
ylabel('Y Position (cm)');

title('2D Shape Reconstruction');