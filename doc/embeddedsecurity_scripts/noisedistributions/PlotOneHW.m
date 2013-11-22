% -*- coding: utf-8 -*-
%
% Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
% All rights reserved.
%
% Find this and more at newae.com - this file is part of the chipwhisperer
% project, http://www.assembla.com/spaces/chipwhisperer
%
%    chipwhisperer is free software: you can redistribute it and/or modify
%    it under the terms of the GNU General Public License as published by
%    the Free Software Foundation, either version 3 of the License, or
%    (at your option) any later version.
%
%    chipwhisperer is distributed in the hope that it will be useful,
%    but WITHOUT ANY WARRANTY; without even the implied warranty of
%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%    GNU Lesser General Public License for more details.
%
%    You should have received a copy of the GNU General Public License
%    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
%=================================================



load('hwlist_bnum=5.mat')
close all
figure
hold on

mulist = [];

for i=1:9
    [muhat,sigmahat,muci,sigmaci] = normfit(hwlist{i});
    mulist = [mulist, muhat];
    mu = muhat;
    sd = sigmahat;
    ix = -3*sd:1e-4:3*sd; %covers more than 99% of the curve
    ix = ix + muhat;
    iy = normpdf(ix, mu, sd);
    plot(ix,iy);    
    xlabel('Shunt Current (linear in mA)')
end

figure
plot([0:8], mulist)

for i=1:9
    figure
    histfit(hwlist{i}, 22);
    title(sprintf('Hamming Weight = %d', i-1))
    xlabel('Shunt Current (linear in mA)')
    xlim([-0.14 -0.07])
end