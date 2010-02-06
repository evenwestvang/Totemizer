SinOsc s1 => dac;
SinOsc s2 => dac;

for(0 => int i; i < 2; i++)
{
  440.0 => s1.sfreq;
  480.0 => s2.sfreq;
  1.0 => s1.gain;
  1.0 => s2.gain;
  1000::ms => now;
  0.0 => s1.gain;
  0.0 => s2.gain;
  2000::ms => now;
}

1.0 => float modulate;
while(true)
{
  440.0 => s1.sfreq;
  480.0 => s2.sfreq;
  1.0 => s1.gain;
  1.0 => s2.gain;
  500::ms => now;
  0.0 => float t;
  while(10::ms => now)
  {
    440.0 + Math.sin(t * (modulate/1.2) ) * (100.0 * modulate) => s1.sfreq;
    480.0 - Math.sin(t * (modulate/1.2)) * (100.0 * modulate) => s2.sfreq;
    t + 0.2 => t;
    if(t > 30.0 * modulate / 2.0)
      break;
  }
  0.5 + modulate => modulate;
  0.0 => s1.gain;
  0.0 => s2.gain;
  2000::ms => now;
}