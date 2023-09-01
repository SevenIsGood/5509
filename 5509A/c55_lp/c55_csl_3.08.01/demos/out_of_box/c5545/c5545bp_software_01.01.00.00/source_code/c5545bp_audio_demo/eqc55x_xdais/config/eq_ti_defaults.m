%*******************************************************************************************
%
%	This Matlab file calculates the required coefficients for the Texas Instruments
%	5 Band Graphic EQ used on TI's portable audio player.
%
%  Stephen Handley - 5th April 1999
%
%*******************************************************************************************

eq_ti_coeffs( ...
   [60, 250, 1000, 4000, 12000], ...
   [0.75, 0.75, 0.75, 0.75, 0.75], ...
   [6, 3, 1, 3, 6]...
);
