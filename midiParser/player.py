import mido
import serial
import threading
import time

mid = mido.MidiFile('midi/marioOverworld.mid')
time_div = 300
inst_offset = {1: 0, 2: 1, 3: 2, 4: 3, 5: 4, 6: 5}

# mid = mido.MidiFile('midi/marioGustyGarden.mid')
# time_div = 250
# inst_offset = {4: 0, 13: 1}

ser = serial.Serial('/dev/cu.usbmodem14301')


msgs = {}

for i, track in enumerate(mid.tracks):
    for m in track:
      if i in inst_offset:
        if i in msgs and len(msgs[i]) > 0 and m.time == 0:
          msgs[i][len(msgs[i])-1].append(m)
        elif i in msgs:
          msgs[i].append([m])
        else:
          msgs[i] = [[m]]


# print(msgs)
def send_note(arg):
  idx = arg
  if len(msgs[idx]) > 0:
    msg_arr = msgs[idx].pop(0)
    msg_arr_len = 0
    string = ""
    for msg in msg_arr:
      if msg.type == "note_on" or msg.type == "note_off" or msg.type == "program_change":
        string += " " + str(len(msg.bytes())) + " " + " ".join([str(x) for x in msg.bytes()])
        msg_arr_len += 1
    if msg_arr_len > 0:
      string = "raw " + str(inst_offset[arg]) + " " + str(msg_arr_len) + string + " "
      print(string)
      ser.write(string.encode())
  if len(msgs[idx]) > 0:
    timer_inner = threading.Timer(msgs[idx][0][0].time/time_div, send_note, [idx])
    timer_inner.start()

ser.write("zero ".encode())
time.sleep(1)
ser.write("zero ".encode())

for g in inst_offset:
  timer = threading.Timer(msgs[g][0][0].time/time_div, send_note, [g])
  timer.start()
