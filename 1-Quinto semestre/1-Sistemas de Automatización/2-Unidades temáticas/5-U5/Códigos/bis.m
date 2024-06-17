function retval = bis(Y, tol, maxiter)
	k = 0;
	a = 1; b = length(Y);
	retval = a;
	while (abs(Y(retval))> tol)&&(k <= maxiter)
		k++;
		c = round((a+b)/2);
		if (Y(c)*Y(a)) < 0
			b = c;
		else
			a = c;
		endif
		retval = c;
	endwhile
endfunction
