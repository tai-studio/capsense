// serial port connection to the
//		CY8CKIT-49-42xx PSoC 4
// programmed with jeff snyders workshop patch 2014

/*
	A VECTOR VISUALISATION
*/

// close all serial connections,
// only needed if developing on the serial port
SerialPort.closeAll;
Server.killAll;
s.latency = nil;

(
// config
q = q ? ();
q.numVals = 6;


/* check if this is the device
SerialPort.devices; // */
q.device = "/dev/tty.usbmodem*".pathMatch[0];
// initial minMax values, find out below on how to adjust.
//
q.minmaxes = [ [ 0, 10827 ], [ 0, 11609 ], [ 1604, 10955 ], [ 1252, 13049 ], [ 812, 8298 ], [ 1355, 9906 ] ];
q.isAdjust = false;


q.trace = false;

s.latency = nil;
s.boot;
q.capVals = 0!q.numVals;


q.adjustMinMax = {|q, val, i|
	var min, max;

	#min, max = q.minmaxes[i];

	(val < min).if{
		q.minmaxes[i][0] = val;
	};
	(val > max).if{
		q.minmaxes[i][1] = val;
	};
};



q.normVals = {
	q.capVals.collect { |val, i|
		q.isAdjust.if{
			q.adjustMinMax(val, i);
		};
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

	q.allVectors = normVals;

	Ndef(\myCapSound).setUni(
		\freq, normVals[0],
		\width,  normVals[1],
		\fFreq,  normVals[2],
		\fRq,  normVals[3],
		\distort,  normVals[4],
		\amp, normVals[5]
	)

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
				(tmpArr.size == q.numVals).if{
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


q.setAdjust = {|q, isOn = true|
	isOn.if({
		q.minmaxes = {[inf, -inf]}! q.numVals;
		q.isAdjust = true;
	}, {
		q.isAdjust = false;
		"q.minmaxes = %;\n".postf(q.minmaxes.asCompileString);
	});
}
)



/////////////////// adjust settings below //////////////////////////


q.allVectors
(

q.win = Window.new.decorate.front;
q.barview = MultiSliderView(q.win, q.win.bounds.extent * [0.2, 0.4]).size_(q.numVals);

q.sliderview = Slider2D(q.win, q.win.bounds.extent * [0.4, 0.4]);




q.skippy = SkipJack({
	q.barview.valueAction = q.allVectors;
	q.sliderview.setXYActive(*(q.xy));

}, 0.1);
)

q.setAdjust(true);
q.setAdjust(false);


q.skippy.play
q.skippy.stop


Tdef(\acloop).play;
Tdef(\acloop).reset;
Tdef(\acloop).stop;

// to find out about the minmax values, post the arrays:
q.trace =  true; // on
q.trace = false; // off


// turn sound on / off
Ndef(\myCapSound).play; // on
Ndef(\myCapSound).stop; // off


(
Ndef(\myCapSound).addSpec(\freq, [200, 1600, \exp]);
Ndef(\myCapSound).addSpec(\width, [0.1, 0.5]);
Ndef(\myCapSound).addSpec(\fFreq, [100, 1000, \exp].asSpec);
Ndef(\myCapSound).addSpec(\fRq, [1, 4]);
Ndef(\myCapSound).addSpec(\distort, [0, 10]);
Ndef(\myCapSound).addSpec(\amp, [0, 1]);

Ndef(\myCapSound, {

	(
		RLPF.ar(
		in:
			  Pulse.ar(\freq.kr(100, 0.1), \width.kr(0.5)),
		freq: \fFreq.kr(100),
		rq:   \fRq.kr(1)
		)
		* (1 + \distort.kr(0))
	).tanh * \amp.kr(1);

})
)

NdefMixer(s)