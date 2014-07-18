/*
_______________________________________________________________________________

 Copyright (c) 2012 TU Dresden, Chair for Embedded Systems
 (http://www.mr.inf.tu-dresden.de) All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 3. All advertising materials mentioning features or use of this software
    must display the following acknowledgement: "This product includes
    software developed by the TU Dresden Chair for Embedded Systems and
    its contributors."

 4. Neither the name of the TU Dresden Chair for Embedded Systems nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY TU DRESDEN CHAIR FOR EMBEDDED SYSTEMS AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
_______________________________________________________________________________
*/
/**
 * Funktionen zur automatischen Berechnung der Teiler zur Erzeugung der Baudrate
 * fuer die UART. Diese erzeugt somit unabhaengig vom Eingangstakt immer die 
 * gleichen Baudraten.
*/

function signed integer abs;

	input integer value;
	 
	begin
		abs = value < 0 ? -value : value;
	end

endfunction

function integer log2;

	input integer value;
	
	for (log2=0; value>0; log2=log2+1)
		value = value>>1;
		
endfunction

/**
 * Diese Function gibt keine eigentliche Samplingsfrequenz aus, sondern die 10 fache Frequenz.
 * Dadurch kann das Problem der Rundung von Baud-Teiler (z.B. 1,9 zu 2) zusammen mit der folgenden 
 * Function gelöst werden. Sonst bekommt man beispielersweise 1 aus 1,9. 
 */
function integer ovs_freq_10x; 

	input  integer clock_rate;
	input  integer ovs_rate;

	begin
		ovs_freq_10x = (clock_rate*10)/ovs_rate;
	end

endfunction

function integer baud_divisor;

	input  integer ovs_freq;
	input  integer baud_rate;

	begin
		baud_divisor = (ovs_freq/baud_rate+5)/10;//hierbei muss Baud-Teiler wieder durch 10 dividiert werden. 
	end

endfunction

function integer baud_error;
	input integer clock_rate;
	input integer ovs_rate;
	input integer baud_rate;

	integer ovs_freq;
	integer divisor;

	begin
		ovs_freq   = ovs_freq_10x(clock_rate,ovs_rate);
		divisor	   = baud_divisor(ovs_freq,baud_rate);
		baud_error = abs(((ovs_freq / 10) / divisor) - baud_rate) *10000 / baud_rate ;
	end

endfunction

function integer find_best_ovs_rate;

	input integer clock_rate;
	input integer baud_rate;

	integer best_baud_error;
	integer best_ovs_rate;
	
	integer err;
	integer baud_divisor_rx;
	integer probe_baud_rate;

	begin
		
		best_baud_error = baud_error(clock_rate, 9, baud_rate);
		best_ovs_rate   = 9;
		
		for(probe_baud_rate = 10; probe_baud_rate <= 31; probe_baud_rate = probe_baud_rate + 1) begin
			err             = baud_error(clock_rate, probe_baud_rate, baud_rate);
			baud_divisor_rx = baud_divisor(ovs_freq_10x(clock_rate, probe_baud_rate), baud_rate);
			if (err < best_baud_error && baud_divisor_rx > 1)
			begin
				best_baud_error = err;
				best_ovs_rate   = probe_baud_rate;
			end
		end

		find_best_ovs_rate = best_ovs_rate;
		
	end
	
endfunction
