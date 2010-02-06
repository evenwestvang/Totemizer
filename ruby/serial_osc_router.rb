
require 'rubygems'
require 'osc'

Host = 'localhost'
Port = 6449

c = OSC::UDPSocket.new

while true
  # m = OSC::Message.new('/t/v', 'i', 0x1, 0x2, 0x3, 0x4)
  m = OSC::Message.new('/t', 'iiii', 0x1, 0x2, 0x3, 0x4)
  c.send m, 0, Host, Port
  puts "ping"
  sleep 0.05
end