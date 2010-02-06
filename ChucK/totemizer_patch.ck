// -------- <3 global vars
int x_accel,y_accel,z_accel, brightness;

// -------- Sample sets

0 => int environment_select;

[ ["ChucK/sample_data/City_Ambience_4.wav", "ChucK/sample_data/Dog_Barks_5.wav", "ChucK/sample_data/siren.wav"] ] @=> string environments[][];

// -------- Setup OSC

OscRecv recv;
6449 => recv.port;
recv.listen();
recv.event( "/t, iiii" ) @=> OscEvent oe;

// -------- Setup drone

SndBuf buf_ambience => Envelope buf_ambience_easing => dac;
environments[environment_select][0] => buf_ambience.read;
1 => buf_ambience.play; 
1 => buf_ambience.loop; 
0 => buf_ambience.gain;
1000::ms => buf_ambience_easing.duration;

// -------- Setup incident

SndBuf buf_incident => dac;
environments[environment_select][1] => buf_incident.read;
0 => buf_incident.play; 

// -------- Setup incident

SndBuf buf_motion_wail => ADSR wail_envelope => dac;
environments[environment_select][2] => buf_motion_wail.read;
1 => buf_motion_wail.play; 
1 => buf_motion_wail.loop; 
1 => buf_motion_wail.gain;

// Where our hero relates to the light level and encounters a yapping dog or two

fun void control_ambient_level()
{
  // Calibration
  0.3 => float cal_zero_level;
  2 => float cal_multiplier;

  float delta_intensity;
  float last_intensity;

  while(true) {
    // Normalize me
    ((brightness / 1023.0) - cal_zero_level) * cal_multiplier => float intensity;

    // Don't wrap
    if (intensity < 0 ) { 0 => intensity; } 
    
    // Don't blow my eardrums out if the hardware transmits a bogus value
    Math.min(intensity, 1) => buf_ambience.gain;

    // First run sanity
    if (last_intensity == 0) { intensity => last_intensity; }

    // Rate of change
    delta_intensity + last_intensity - intensity => delta_intensity;
    delta_intensity * 0.70 => delta_intensity;
    intensity => last_intensity;

    // Bump
    if (delta_intensity < -0.10) {
      1 => buf_incident.play;
      0 => buf_incident.pos;
      intensity => buf_incident.rate;
    }

    // The time is now
    1::ms => now;
  }
}

// Houston, we are in motion

fun int calc_abs(int val) {
  if (val < 32768) return val;
  return 65536 - val; 
}

fun int calc_int(int val) {
  if (val < 32768) return val;
  return val - 65536; 
}


fun void control_motion_level() {
  int abs_x, abs_y, abs_z, abs_total;
  float abs_normalized;

  wail_envelope.set(1500::ms, 2000::ms, 0.4, 1200::ms );
  wail_envelope.keyOff();

  // Calibration
  1680 => int acc_zero_level;

  while (true) {
    calc_abs(x_accel) => abs_x;
    calc_abs(y_accel) => abs_y;
    calc_abs(z_accel) => abs_z;
    
    abs_x + abs_y + abs_z => abs_total;
    Math.max((abs_total - acc_zero_level)/3500.1, 0) => abs_normalized;

    if (abs_normalized > 0.2) { wail_envelope.keyOn(); }
    if (wail_envelope.state() == 2 && abs_normalized < 0.2 ) { wail_envelope.keyOff(); }
    <<< abs_z / 1100.1 >>>;

    // The time is now
    1::ms => now;
  }
}

spork ~ control_ambient_level();
spork ~ control_motion_level();

// infinite event loop
while ( true ) {
    // wait for events
    oe => now;

    // grab the next message from the queue. 
    while ( oe.nextMsg() != 0 ) { 
        oe.getInt() => brightness;
        oe.getInt() => x_accel;
        oe.getInt() => y_accel;
        oe.getInt() => z_accel;
    }
}
