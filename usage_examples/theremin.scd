// serial port connection to the
//		CY8CKIT-49-42xx PSoC 4
// programmed with jeff snyders workshop patch 2014

/*
	A THEREMIN
*/

// close all serial connections,
// only needed if developing on the serial port
SerialPort.closeAll;
Server.killAll;

(
// config
q = q ? ();

/* check if this is the device
SerialPort.devices; // */
q.device = "/dev/tty.usbmodem*".pathMatch[0];
// initial minMax values, find out below on how to adjust.
q.minmaxes = [
	[0, 16383],
	[0, 16383],
	[0, 16383],
	[0, 16383],
];
q.trace = false;

s.latency = nil;
s.boot;
q.capVals = 0!4;
q.normVals = {
	q.capVals.collect { |val, i|
		val.linlin(q.minmaxes[i][0], q.minmaxes[i][1], 0, 1);
	};
};
)

(
// program internals
q.serial = SerialPort(q.device,
	baudrate: 115200,
	databits: 8,
	stopbit: 1,
	parity: nil,
);


(
q.action = {|q, arr|
	var normVals = q.normVals;
	q.trace.if{
		"caps: \t".post;
		arr.postcs;
	};
	// normVals.round(0.0001).postln;
	Ndef(\myCapSound).set(
		\a, normVals[0],
		\b, normVals[1],
		\c, normVals[2],
		\d, normVals[3]
	);

};
);

(
Tdef(\acloop, {
	var char;
	var raw = "";
	var tmpArr;

	inf.do{
		while({
			char= q.serial.next; char.isNil
		}, { 0.01.wait });

		char = char.asAscii;
		(char != 10.asAscii).if({
			raw = raw ++ char;
			//raw.postcs;
		},{
			try({
				tmpArr = raw.split($ ).asInteger;
				(tmpArr.size == 4).if{
					q.capVals = tmpArr;
				};
				//raw.postln;
			}, {"failed to acquire data, raw array is:%".format(raw).warn});

			// evaluate action
			q.action(q.capVals);

			raw = "";
		});
	}
}).play;
);

)



/////////////////// adjust settings below //////////////////////////

/*
   Your sound, it has four parameters,
   each connected to one of the capacitive sensing items.
   You can adjust settings in the sound be re-evaluating the Ndef
   definition. For more info, see
		Ndef.openHelpFile;
*/

Ndef(\myCapSound).edit

(
// theremin, sortof

Ndef(\myCapSound, { |a, b, c, d|
	var amp, freq, width, fFreq;

	freq  = a.linexp(0, 1, 100   , 1600  );
	amp   = b.linlin(0, 1,   0   ,    1  );
	width = c.linlin(0, 1,   0.01,    0.5);
	fFreq = d.linexp(0, 1, 5000,    400);
	// b.poll;

	MoogFF.ar(
		in: LFPulse.ar(freq: freq, mul: amp, width: width),
		freq: fFreq
	);
}).play;
)



/*
Ndef(\myCapSound).set(\a, 1.0.rand); // set first value
*/

Tdef(\acloop).play;
Tdef(\acloop).reset;
Tdef(\acloop).stop;

// to find out about the minmax values, post the arrays:
q.trace =  true; // on
q.trace = false; // off


// turn sound on / off
Ndef(\myCapSound).play; // on
Ndef(\myCapSound).stop; // off