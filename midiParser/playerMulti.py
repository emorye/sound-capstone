import mido
import serial
import threading
import time
import sys

mid = mido.MidiFile('midi/marioOverworld.mid')
time_div = 300
inst_offset = {1: 0, 2: 1, 3: 2, 4: 3, 5: 4, 6: 5}

# mid = mido.MidiFile('midi/marioGustyGarden.mid')
# time_div = 250
# inst_offset = {4: 0, 13: 1}

ser = serial.Serial('/dev/cu.usbmodem14301')

msgs = []

msgs_tmp = {}

offsets = {}

total_msgs = 0

for i, track in enumerate(mid.tracks):
    if i > 0:
      msgs_track = []
      for m in track:
        msgs_track.append(m)
        total_msgs += 1
      msgs_tmp[i] = msgs_track
      offsets[i] = 0


while(total_msgs > 0):
  pass

print("\n".join([str(x) for x in msgs]))

ser.write("zero ".encode())
time.sleep(1)
ser.write("zero ".encode())

def send_note():
  if len(msgs) > 0:
    msg_arr = msgs.pop(0)
    msg_arr_len = 0
    string = ""
    for message in msg_arr:
      msg = message[2]
      if msg.type == "note_on" or msg.type == "note_off" or msg.type == "program_change":
        string += " " + str(len(msg.bytes())) + " " + " ".join([str(x) for x in msg.bytes()])
        msg_arr_len += 1
    if msg_arr_len > 0:
      string = "raw " + str(inst_offset[msgs[0][0][1]]) + " " + str(msg_arr_len) + string + " "
      print(string)
      ser.write(string.encode())
  if len(msgs) > 0:
    timer_inner = threading.Timer(msgs[0][0][0]/time_div, send_note)
    timer_inner.start()


timer = threading.Timer(msgs[0][0][0]/time_div, send_note)
timer.start()