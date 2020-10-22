function	integer clog2;
	input	integer value;
			integer result;
	begin
		value = value - 1;
		for (result = 0; value > 0; result = result + 1)
			value = value >> 1;
		clog2 = result;
	end
endfunction
